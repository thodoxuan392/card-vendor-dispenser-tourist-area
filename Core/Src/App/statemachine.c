/*
 * statemachine.c
 *
 *  Created on: Jul 10, 2024
 *      Author: xuanthodo
 */


#include "App/statemachine.h"

#include "DeviceManager/tcdmanager.h"
#include "Device/rfid.h"

typedef enum {
	STATEMACHINE_STATE_IDLE,
	STATEMACHINE_STATE_WAIT_FOR_RFID_DETECTED,
	STATEMACHINE_STATE_UPDATE_RFID,
	STATEMACHINE_STATE_DISPENSE,
	STATEMACHINE_STATE_WAIT_FOR_DISPENSE_COMPLETE,
}STATEMACHINE_State;


static void STATEMACHINE_idle(void);
static void STATEMACHINE_waitForRFIDDetected(void);
static void STATEMACHINE_updateRFID(void);
static void STATEMACHINE_dispense(void);
static void STATEMACHINE_waitForDispenseComplete(void);

static void STATEMACHINE_tcdCompletedCallback(TCD_id_t id);
static void STATEMACHINE_tcdCorruptCallback(TCD_id_t id);


static STATEMACHINE_State STATEMACHINE_state = STATEMACHINE_STATE_IDLE;
static STATEMACHINE_DispenseResultCallback STATEMACHINE_dispenseResultCallback;
static RFID_Id_t STATEMACHINE_tcdToRfid[] = {
	[TCD_1] = RFID_ID_1,
	[TCD_2] = RFID_ID_2,
};

static bool STATEMACHINE_requestFlag = false;
static uint8_t STATEMACHINE_nbCardRequest;
static uint8_t STATEMACHINE_cardTypeRequest;
static TCD_id_t STATEMACHINE_usingTcdId;
static RFID_t STATEMACHINE_usingRfid;


static bool STATEMACHINE_dispenseCompleteFlag = false;
static bool STATEMACHINE_dispenseCorruptFlag = false;

void STATEMACHINE_init(void){
	TCDMNG_set_take_card_cb(STATEMACHINE_tcdCompletedCallback);
	TCDMNG_set_callback_card_cb(STATEMACHINE_tcdCorruptCallback);
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
		case STATEMACHINE_STATE_DISPENSE:
			STATEMACHINE_dispense();
			break;
		case STATEMACHINE_STATE_WAIT_FOR_DISPENSE_COMPLETE:
			STATEMACHINE_waitForDispenseComplete();
			break;
		default:
			break;
	}
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
	if(TCDMNG_is_available_for_use(&STATEMACHINE_usingTcdId) && STATEMACHINE_requestFlag){
		STATEMACHINE_requestFlag = false;
		STATEMACHINE_state = STATEMACHINE_STATE_WAIT_FOR_RFID_DETECTED;
		return;
	}
}

static void STATEMACHINE_waitForRFIDDetected(void){
	RFID_Id_t usingRfidId = STATEMACHINE_tcdToRfid[STATEMACHINE_usingTcdId];
	if(RFID_isDetected(usingRfidId)){
		RFID_get(usingRfidId, &STATEMACHINE_usingRfid);
		RFID_clearDetected(usingRfidId);
		STATEMACHINE_state = STATEMACHINE_STATE_UPDATE_RFID;
	}
}

static void STATEMACHINE_updateRFID(void){
	RFID_Id_t usingRfidId = STATEMACHINE_tcdToRfid[STATEMACHINE_usingTcdId];
	STATEMACHINE_usingRfid.isUsed = false;
	STATEMACHINE_usingRfid.type = STATEMACHINE_cardTypeRequest;
	if(!RFID_set(usingRfidId, &STATEMACHINE_usingRfid)){
		STATEMACHINE_dispenseResultCallback(STATEMACHINE_DISPENSE_RESULT_FAILED);
		STATEMACHINE_state = STATEMACHINE_STATE_IDLE;
		return;
	}
	STATEMACHINE_state = STATEMACHINE_STATE_DISPENSE;
}

static void STATEMACHINE_dispense(void){
	if(!TCDMNG_payout(STATEMACHINE_usingTcdId)){
		STATEMACHINE_dispenseResultCallback(STATEMACHINE_DISPENSE_RESULT_FAILED);
		STATEMACHINE_state = STATEMACHINE_STATE_IDLE;
		return;
	}
	STATEMACHINE_state = STATEMACHINE_STATE_WAIT_FOR_DISPENSE_COMPLETE;
}

static void STATEMACHINE_waitForDispenseComplete(void){
	if(STATEMACHINE_dispenseCompleteFlag){
		STATEMACHINE_dispenseCompleteFlag = false;
		STATEMACHINE_nbCardRequest--;
		if(STATEMACHINE_nbCardRequest == 0){
			STATEMACHINE_dispenseResultCallback(STATEMACHINE_DISPENSE_RESULT_SUCCESS);
			STATEMACHINE_state = STATEMACHINE_STATE_IDLE;
		}else {
			STATEMACHINE_state = STATEMACHINE_STATE_WAIT_FOR_RFID_DETECTED;
		}
	}

	if(STATEMACHINE_dispenseCorruptFlag){
		STATEMACHINE_dispenseCorruptFlag = false;
		STATEMACHINE_dispenseResultCallback(STATEMACHINE_DISPENSE_RESULT_FAILED);
		STATEMACHINE_state = STATEMACHINE_STATE_IDLE;
	}

}

static void STATEMACHINE_tcdCompletedCallback(TCD_id_t id){
	STATEMACHINE_dispenseCompleteFlag = true;
}


static void STATEMACHINE_tcdCorruptCallback(TCD_id_t id){
	STATEMACHINE_dispenseCorruptFlag = true;
}
