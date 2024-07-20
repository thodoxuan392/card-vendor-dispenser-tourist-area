/*
 * statemachine.c
 *
 *  Created on: Jul 10, 2024
 *      Author: xuanthodo
 */


#include "App/statemachine.h"

#include "App/protocol.h"
#include "App/statusreporter.h"
#include "DeviceManager/tcdmanager.h"
#include "DeviceManager/billacceptormanager.h"
#include "Device/rfid.h"
#include "Hal/timer.h"

#define STATEMACHINE_WAIT_FOR_RFID_DETECTED_TIMEOUT		20000
#define STATEMACHINE_WAIT_FOR_RFID_UPDATE_TIMEOUT		20000
#define STATEMACHINE_WAIT_FOR_DISPENSE_TIMEOUT			60000
#define STATEMACHINE_DISPENSE_INTERVAL_MIN				1000

typedef enum {
	STATEMACHINE_STATE_IDLE,
	STATEMACHINE_STATE_WAIT_FOR_RFID_DETECTED,
	STATEMACHINE_STATE_UPDATE_RFID,
	STATEMACHINE_STATE_WAIT_FOR_UPDATE_RFID_RESULT,
	STATEMACHINE_STATE_DISPENSE,
	STATEMACHINE_STATE_WAIT_FOR_DISPENSE_COMPLETE,
}STATEMACHINE_State;

static void STATEMACHINE_1msInterrupt(void);

static void STATEMACHINE_runBillAcceptorHandling(void);

static void STATEMACHINE_idle(void);
static void STATEMACHINE_waitForRFIDDetected(void);
static void STATEMACHINE_updateRFID(void);
static void STATEMACHINE_waitForUpdateRFIDResult(void);
static void STATEMACHINE_dispense(void);
static void STATEMACHINE_waitForDispenseComplete(void);

static void STATEMACHINE_tcdCompletedCallback(TCD_id_t id);
static void STATEMACHINE_tcdCorruptCallback(TCD_id_t id);
static void STATEMACHINE_rfidUpdateResultCallback(RFID_Id_t id, RFID_Error_t error);


static STATEMACHINE_State STATEMACHINE_state = STATEMACHINE_STATE_IDLE;
static STATEMACHINE_DispenseResultCallback STATEMACHINE_dispenseResultCallback;
static RFID_Id_t STATEMACHINE_tcdToRfid[] = {
	[TCD_1] = RFID_ID_1,
};

static bool STATEMACHINE_requestFlag = false;
static uint8_t STATEMACHINE_nbCardRequest;
static uint8_t STATEMACHINE_cardTypeRequest;
static TCD_id_t STATEMACHINE_usingTcdId;
static RFID_t STATEMACHINE_usingRfid;


static bool STATEMACHINE_dispenseCompleteFlag = false;
static bool STATEMACHINE_dispenseCorruptFlag = false;
static bool STATEMACHINE_updateRFIDResultIndicator = false;
static bool STATEMACHINE_updateRFIDResult = false;

// Timeout
static uint32_t STATEMACHINE_timeoutCnt = 0;
static bool STATEMACHINE_timeoutFlag = false;

// For Testing
//static uint32_t STATEMACHINE_lastTimeDispense = 0;

void STATEMACHINE_init(void){
	TIMER_attach_intr_1ms(STATEMACHINE_1msInterrupt);
	TCDMNG_set_take_card_cb(STATEMACHINE_tcdCompletedCallback);
	TCDMNG_set_callback_card_cb(STATEMACHINE_tcdCorruptCallback);
	RFID_setUpdateResultCallback(STATEMACHINE_rfidUpdateResultCallback);
}

void STATEMACHINE_run(void){
	switch (STATEMACHINE_state) {
		case STATEMACHINE_STATE_IDLE:
			STATEMACHINE_idle();
			break;
		case STATEMACHINE_STATE_WAIT_FOR_RFID_DETECTED:
			STATEMACHINE_waitForRFIDDetected();
			break;
		case STATEMACHINE_STATE_UPDATE_RFID:
			STATEMACHINE_updateRFID();
			break;
		case STATEMACHINE_STATE_WAIT_FOR_UPDATE_RFID_RESULT:
			STATEMACHINE_waitForUpdateRFIDResult();
			break;
		case STATEMACHINE_STATE_DISPENSE:
			STATEMACHINE_dispense();
			break;
		case STATEMACHINE_STATE_WAIT_FOR_DISPENSE_COMPLETE:
			STATEMACHINE_waitForDispenseComplete();
			break;
		default:
			break;
	}
	STATEMACHINE_runBillAcceptorHandling();
}

bool STATEMACHINE_requestDispense(uint8_t nbCard, uint8_t cardType){
	if(STATEMACHINE_state != STATEMACHINE_STATE_IDLE){
		return false;
	}
	if(!TCDMNG_is_available_for_use(&STATEMACHINE_usingTcdId)){
		return false;
	}
	STATEMACHINE_requestFlag = true;
	STATEMACHINE_nbCardRequest = nbCard;
	STATEMACHINE_cardTypeRequest = cardType;
	return true;
}

void STATEMACHINE_setDispenseResultCallback(STATEMACHINE_DispenseResultCallback callback){
	STATEMACHINE_dispenseResultCallback = callback;
}


static void STATEMACHINE_idle(void){
	if(TCDMNG_is_available_for_use(&STATEMACHINE_usingTcdId) && STATEMACHINE_requestFlag && STATEMACHINE_nbCardRequest > 0){
		STATEMACHINE_requestFlag = false;
		STATEMACHINE_timeoutCnt = STATEMACHINE_WAIT_FOR_RFID_DETECTED_TIMEOUT;
		STATEMACHINE_timeoutFlag = false;
		STATEMACHINE_state = STATEMACHINE_STATE_WAIT_FOR_RFID_DETECTED;
		return;
	}
}

static void STATEMACHINE_waitForRFIDDetected(void){
	RFID_Id_t usingRfidId = STATEMACHINE_tcdToRfid[STATEMACHINE_usingTcdId];
	if(RFID_isPlaced(usingRfidId) && RFID_isDetected(usingRfidId)){
		RFID_get(usingRfidId, &STATEMACHINE_usingRfid);
		RFID_clearDetected(usingRfidId);
		STATEMACHINE_state = STATEMACHINE_STATE_UPDATE_RFID;
		return;
	}
	if(STATEMACHINE_timeoutFlag){
		STATEMACHINE_dispenseResultCallback(PROTOCOL_RESULT_CMD_DISPENSE_CARD_TIMEOUT, STATEMACHINE_nbCardRequest);
		STATEMACHINE_state = STATEMACHINE_STATE_IDLE;
		return;
	}
}

static void STATEMACHINE_updateRFID(void){
	RFID_Id_t usingRfidId = STATEMACHINE_tcdToRfid[STATEMACHINE_usingTcdId];
	STATEMACHINE_usingRfid.isUsed = false;
	STATEMACHINE_usingRfid.type = STATEMACHINE_cardTypeRequest;
	if(!RFID_set(usingRfidId, &STATEMACHINE_usingRfid)){
		STATEMACHINE_dispenseResultCallback(PROTOCOL_RESULT_CMD_DISPENSE_CARD_UPDATE_RFID_FAILED, STATEMACHINE_nbCardRequest);
		STATEMACHINE_state = STATEMACHINE_STATE_IDLE;
		return;
	}
	STATEMACHINE_timeoutCnt = STATEMACHINE_WAIT_FOR_RFID_UPDATE_TIMEOUT;
	STATEMACHINE_timeoutFlag = false;
	STATEMACHINE_state = STATEMACHINE_STATE_WAIT_FOR_UPDATE_RFID_RESULT;
}

static void STATEMACHINE_waitForUpdateRFIDResult(void){
	if(STATEMACHINE_updateRFIDResultIndicator){
		STATEMACHINE_updateRFIDResultIndicator = false;
		if(STATEMACHINE_updateRFIDResult){
			STATEMACHINE_updateRFIDResult = false;
			STATEMACHINE_state = STATEMACHINE_STATE_DISPENSE;
			return;
		}else {
			STATEMACHINE_dispenseResultCallback(PROTOCOL_RESULT_CMD_DISPENSE_CARD_UPDATE_RFID_FAILED, STATEMACHINE_nbCardRequest);
			STATEMACHINE_state = STATEMACHINE_STATE_IDLE;
			return;
		}
	}

	if(STATEMACHINE_timeoutFlag){
		STATEMACHINE_dispenseResultCallback(PROTOCOL_RESULT_CMD_DISPENSE_CARD_UPDATE_RFID_FAILED, STATEMACHINE_nbCardRequest);
		STATEMACHINE_state = STATEMACHINE_STATE_IDLE;
		return;
	}
}
static void STATEMACHINE_dispense(void){
	if(!TCDMNG_payout(STATEMACHINE_usingTcdId)){
		STATEMACHINE_dispenseResultCallback(PROTOCOL_RESULT_CMD_DISPENSE_CARD_TCD_PAYOUT_FAILED, STATEMACHINE_nbCardRequest);
		STATEMACHINE_state = STATEMACHINE_STATE_IDLE;
		return;
	}
	STATEMACHINE_timeoutCnt = STATEMACHINE_WAIT_FOR_DISPENSE_TIMEOUT;
	STATEMACHINE_timeoutFlag = false;
	STATEMACHINE_state = STATEMACHINE_STATE_WAIT_FOR_DISPENSE_COMPLETE;
}

static void STATEMACHINE_waitForDispenseComplete(void){
	if(STATEMACHINE_dispenseCompleteFlag){
		STATEMACHINE_dispenseCompleteFlag = false;
		STATEMACHINE_nbCardRequest--;
		if(STATEMACHINE_nbCardRequest == 0){
			STATEMACHINE_state = STATEMACHINE_STATE_IDLE;
		}else {
			STATEMACHINE_timeoutCnt = STATEMACHINE_WAIT_FOR_RFID_DETECTED_TIMEOUT;
			STATEMACHINE_timeoutFlag = false;
			STATEMACHINE_state = STATEMACHINE_STATE_WAIT_FOR_RFID_DETECTED;
		}
		STATEMACHINE_dispenseResultCallback(PROTOCOL_RESULT_SUCCESS, STATEMACHINE_nbCardRequest);
		return;
	}

	if(STATEMACHINE_dispenseCorruptFlag){
		STATEMACHINE_dispenseCorruptFlag = false;
		STATEMACHINE_dispenseResultCallback(PROTOCOL_RESULT_CMD_DISPENSE_CARD_CORRUPT, STATEMACHINE_nbCardRequest);
		STATEMACHINE_state = STATEMACHINE_STATE_IDLE;
		return;
	}

	if(STATEMACHINE_timeoutFlag){
		STATEMACHINE_dispenseResultCallback(PROTOCOL_RESULT_CMD_DISPENSE_CARD_TIMEOUT, STATEMACHINE_nbCardRequest);
		STATEMACHINE_state = STATEMACHINE_STATE_IDLE;
		return;
	}
}

static void STATEMACHINE_runBillAcceptorHandling(void){
	if(BILLACCEPTORMNG_is_accepted()){
		uint32_t billValue = BILLACCEPTOR_get_last_bill_accepted();
		STATUSREPORTER_sendEvtBillAccepted(billValue);
		BILLACCEPTORMNG_clear_accepted();
	}
}

static void STATEMACHINE_1msInterrupt(void){
	if(STATEMACHINE_timeoutCnt > 0){
		STATEMACHINE_timeoutCnt--;
		if(STATEMACHINE_timeoutCnt == 0){
			STATEMACHINE_timeoutFlag = true;
		}
	}
}

static void STATEMACHINE_tcdCompletedCallback(TCD_id_t id){
	STATEMACHINE_dispenseCompleteFlag = true;
}


static void STATEMACHINE_tcdCorruptCallback(TCD_id_t id){
	STATEMACHINE_dispenseCorruptFlag = true;
}

static void STATEMACHINE_rfidUpdateResultCallback(RFID_Id_t id, RFID_Error_t error){
	STATEMACHINE_updateRFIDResultIndicator = true;
	STATEMACHINE_updateRFIDResult = (error == RFID_SUCCESS);
}
