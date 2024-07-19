/*
 * rfid.c
 *
 *  Created on: Dec 17, 2023
 *      Author: xuanthodo
 */

#include <Device/rfid.h>

#include <utils/utils_logger.h>
#include <Tp/rs485.h>
#include <Hal/timer.h>

#define RFID_485_NETWORK_ID	0x01	// RFID Network in RS485 is 0x01

#define RFID_485_MASTER_NODE_ID 0xFF

#define RFID_485_MESSAGE_ID_REQUEST_STATUS 0x01
#define RFID_485_MESSAGE_ID_RESPONSE_STATUS 0x02
#define RFID_485_MESSAGE_ID_REQUEST_UPDATE 0x03
#define RFID_485_MESSAGE_ID_RESPONSE_UPDATE 0x04

#define RFID_485_MESSAGE_ID_REQUEST_STATUS_TIMEOUT	2000	// 500ms
#define RFID_485_MESSAGE_ID_REQUEST_UPDATE_TIMEOUT	2000	// 500ms

#define RFID_485_STATUS_POLLING_INTERVAL 	100	// 500ms


typedef enum {
	RFID_STATE_IDLE,
	RFID_STATE_WAIT_FOR_STATUS_RESPONSE,
	RFID_STATE_WAIT_FOR_UPDATE_RESPONSE,
}RFID_HandleState;

typedef struct
{
	RFID_Error_t error;
	bool isPlaced;
	bool isDetected;
	RFID_t rfid;

	// For RFID Update
	RS485_Message updateMessage;
	bool updatePending;
	bool updateResponseIndicator;
	bool updateResponseResult;

	// For RFID Status
	uint32_t statusPollingCnt;
	bool statusPollingPending;
	bool statusResponseIndicator;

	// Timeout
	uint32_t timeoutCnt;
	bool timeoutFlag;
} RFID_Handle;


static void RFID_interrupt1ms(void);

static void RFID_runIdle(void);
static void RFID_runWaitForStatusResponse(void);
static void RFID_runWaitForUpdateResponse(void);

// Validate Function
static bool RFID_Is485MessageValid(RS485_Message *message);
static RFID_Id_t RFID_getCurrHandleRfid(void);
static RFID_Id_t RFID_switchToNextRfid(void);

static bool RFID_requestStatus(RFID_Id_t id);
static bool RFID_handleResponseStatus(RS485_Message* message);
static bool RFID_handleResponseUpdate(RS485_Message* message);

static RFID_Handle RFID_handleTable[RFID_ID_MAX];
static RFID_HandleState RFID_handleState = RFID_STATE_IDLE;
static RFID_Id_t RFID_handleId = RFID_ID_1;
static RS485_Message RFID_rs485Message;
static RFID_UpdateResultCallback RFID_updateResultCallback;

void RFID_init(void)
{
	for (int id = 0; id < RFID_ID_MAX; ++id) {
		RFID_handleTable[id].statusPollingPending = true;
	}
	TIMER_attach_intr_1ms(RFID_interrupt1ms);
}

void RFID_run(void)
{
	switch (RFID_handleState) {
		case RFID_STATE_IDLE:
			RFID_runIdle();
			break;
		case RFID_STATE_WAIT_FOR_STATUS_RESPONSE:
			RFID_runWaitForStatusResponse();
			break;
		case RFID_STATE_WAIT_FOR_UPDATE_RESPONSE:
			RFID_runWaitForUpdateResponse();
			break;
		default:
			break;
	}
	// Polling for RFID
	if(RS485_receive(&RFID_rs485Message))
	{
		if(!RFID_Is485MessageValid(&RFID_rs485Message)){
			return;
		}
		switch(RFID_rs485Message.messageId)
		{
			case RFID_485_MESSAGE_ID_RESPONSE_STATUS:
				RFID_handleResponseStatus(&RFID_rs485Message);
				break;
			case RFID_485_MESSAGE_ID_RESPONSE_UPDATE:
				RFID_handleResponseUpdate(&RFID_rs485Message);
				break;
			default:
				break;
		}
	}
}

void RFID_setUpdateResultCallback(RFID_UpdateResultCallback callback){
	RFID_updateResultCallback = callback;
}

void RFID_get(RFID_Id_t id, RFID_t* rfid)
{
	memcpy(rfid, &RFID_handleTable[id].rfid, sizeof(RFID_t));
}

bool RFID_isPlaced(RFID_Id_t id)
{
	return RFID_handleTable[id].isPlaced;
}

bool RFID_isDetected(RFID_Id_t id)
{
	return RFID_handleTable[id].isDetected;
}

void RFID_clearDetected(RFID_Id_t id)
{
	RFID_handleTable[id].isDetected = false;
}

RFID_Error_t RFID_set(RFID_Id_t id, RFID_t* rfid)
{
	RFID_handleTable[id].updateMessage.networkId = RFID_485_NETWORK_ID;
	RFID_handleTable[id].updateMessage.srcNode = RFID_485_MASTER_NODE_ID;
	RFID_handleTable[id].updateMessage.desNode = id;
	RFID_handleTable[id].updateMessage.messageId = RFID_485_MESSAGE_ID_REQUEST_UPDATE;
	RFID_handleTable[id].updateMessage.resultCode = RS485_RESULT_CODE_SUCCESS;
	RFID_handleTable[id].updateMessage.dataLen = 0;
	RFID_handleTable[id].updateMessage.data[RFID_handleTable[id].updateMessage.dataLen++] = rfid->id_len;
	for(uint8_t i = 0; i < rfid->id_len; i++)
	{
		RFID_handleTable[id].updateMessage.data[RFID_handleTable[id].updateMessage.dataLen++] = rfid->id[i];
	}
	RFID_handleTable[id].updateMessage.data[RFID_handleTable[id].updateMessage.dataLen++] = rfid->code;
	RFID_handleTable[id].updateMessage.data[RFID_handleTable[id].updateMessage.dataLen++] = rfid->type;
	RFID_handleTable[id].updateMessage.data[RFID_handleTable[id].updateMessage.dataLen++] = rfid->isUsed;
	RFID_handleTable[id].updatePending = true;
	return true;
}

bool RFID_isError(RFID_Id_t id)
{
	return RFID_handleTable[id].error != RFID_SUCCESS;
}

RFID_Error_t RFID_getError(RFID_Id_t id)
{
	return RFID_handleTable[id].error;
}

void RFID_test(void)
{
	RFID_t rfid = {
		.id = {163, 52, 18, 8},
		.id_len = 4,
		.type = 0x00,
		.isUsed = 0
	};
	while(1)
	{
		HAL_Delay(1000);
		RFID_set(RFID_ID_1, &rfid);
	}
}

static void RFID_runIdle(){
	RFID_Id_t id = RFID_getCurrHandleRfid();
	if(RFID_handleTable[id].updatePending){
		RFID_handleTable[id].updatePending = false;
		RS485_send(&RFID_handleTable[id].updateMessage);
		RFID_handleTable[id].timeoutFlag = false;
		RFID_handleTable[id].timeoutCnt = RFID_485_MESSAGE_ID_REQUEST_UPDATE_TIMEOUT;
		RFID_handleState = RFID_STATE_WAIT_FOR_UPDATE_RESPONSE;
		return;
	}

	if(RFID_handleTable[id].statusPollingPending){
		RFID_requestStatus(id);
		RFID_handleTable[id].timeoutFlag = false;
		RFID_handleTable[id].timeoutCnt = RFID_485_MESSAGE_ID_REQUEST_STATUS_TIMEOUT;
		RFID_handleState = RFID_STATE_WAIT_FOR_STATUS_RESPONSE;
		return;
	}
}

static void RFID_runWaitForStatusResponse(){
	RFID_Id_t id = RFID_getCurrHandleRfid();
	// Success case
	if(RFID_handleTable[id].statusResponseIndicator){
		RFID_handleTable[id].statusResponseIndicator = false;
		RFID_handleState = RFID_STATE_IDLE;
		RFID_switchToNextRfid();
		return;
	}

	// Timeout case
	if(RFID_handleTable[id].timeoutFlag){
		utils_log_error("[RFID] Timeout to wait for status response from RFID id %d\r\n", id);
		RFID_handleState = RFID_STATE_IDLE;
		RFID_switchToNextRfid();
		return;
	}
}

static void RFID_runWaitForUpdateResponse(){
	RFID_Id_t id = RFID_getCurrHandleRfid();
	// Success case
	if(RFID_handleTable[id].updateResponseIndicator){
		RFID_handleTable[id].updateResponseIndicator = false;
		RFID_handleState = RFID_STATE_IDLE;
		RFID_switchToNextRfid();
		return;
	}

	// Timeout case
	if(RFID_handleTable[id].timeoutFlag){
		utils_log_error("[RFID] Timeout to wait for update response from RFID id %d\r\n", id);
		RFID_handleState = RFID_STATE_IDLE;
		return;
	}
}

static bool RFID_Is485MessageValid(RS485_Message *message){
	if(message->networkId != RFID_485_NETWORK_ID){
		return false;
	}
	if(message->desNode != RFID_485_MASTER_NODE_ID)
	{
		return false;
	}
	if(!(message->srcNode >= RFID_ID_1 && message->srcNode < RFID_ID_MAX))
	{
		return false;
	}
	return true;
}


static RFID_Id_t RFID_getCurrHandleRfid(void){
	return RFID_handleId;
}
static RFID_Id_t RFID_switchToNextRfid(void){
	RFID_handleId = (RFID_handleId + 1) % RFID_ID_MAX;
}

static bool RFID_requestStatus(RFID_Id_t id){
	RFID_rs485Message.networkId = RFID_485_NETWORK_ID;
	RFID_rs485Message.srcNode = RFID_485_MASTER_NODE_ID;
	RFID_rs485Message.desNode = id;
	RFID_rs485Message.messageId = RFID_485_MESSAGE_ID_REQUEST_STATUS;
	RFID_rs485Message.resultCode = RS485_RESULT_CODE_SUCCESS;
	RFID_rs485Message.dataLen = 0;
	RS485_send(&RFID_rs485Message);
}

static bool RFID_handleResponseStatus(RS485_Message* message)
{
	uint8_t srcNode = message->srcNode;

	if(message->resultCode == RS485_RESULT_CODE_SUCCESS){
		RFID_handleTable[srcNode].rfid.id_len = message->data[0];
		for(int var = 0; var < RFID_handleTable[srcNode].rfid.id_len; ++var)
		{
			RFID_handleTable[srcNode].rfid.id[var] = message->data[1 + var];
		}
		RFID_handleTable[srcNode].rfid.code = message->data[1 + RFID_handleTable[srcNode].rfid.id_len];
		RFID_handleTable[srcNode].rfid.type = message->data[2 + RFID_handleTable[srcNode].rfid.id_len];
		RFID_handleTable[srcNode].rfid.isUsed = message->data[3 + RFID_handleTable[srcNode].rfid.id_len];
		if(!RFID_handleTable[srcNode].isPlaced){
			RFID_handleTable[srcNode].isDetected = true;
		}
		RFID_handleTable[srcNode].isPlaced = true;

	}else {
		RFID_handleTable[srcNode].isPlaced = false;
	}
	RFID_handleTable[srcNode].statusResponseIndicator = true;
	return true;
}


static bool RFID_handleResponseUpdate(RS485_Message* message)
{
	uint8_t srcNode = message->srcNode;
	RFID_Error_t error = message->data[0];

	RFID_handleTable[srcNode].updateResponseIndicator = true;
	RFID_handleTable[srcNode].updateResponseResult = (error == RFID_SUCCESS);
	if(RFID_updateResultCallback)
		RFID_updateResultCallback(srcNode, RFID_handleTable[srcNode].updateResponseResult);

	if(RFID_updateResultCallback){
		RFID_updateResultCallback(srcNode , error);
	}
}

static void RFID_interrupt1ms(void){
	for (int id = 0; id < RFID_ID_MAX; ++id) {
		if(RFID_handleTable[id].timeoutCnt > 0){
			RFID_handleTable[id].timeoutCnt--;
			if(RFID_handleTable[id].timeoutCnt == 0){
				RFID_handleTable[id].timeoutFlag = true;
			}
		}
		if(RFID_handleTable[id].statusPollingCnt > 0){
			RFID_handleTable[id].statusPollingCnt--;
			if(RFID_handleTable[id].statusPollingCnt == 0){
				RFID_handleTable[id].statusPollingCnt = RFID_485_STATUS_POLLING_INTERVAL;
				RFID_handleTable[id].statusPollingPending = true;
			}
		}
	}
}
