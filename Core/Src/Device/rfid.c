/*
 * rfid.c
 *
 *  Created on: Dec 17, 2023
 *      Author: xuanthodo
 */

#include <Device/rfid.h>

#include <Tp/rs485.h>

#define RFID_485_NETWORK_ID	0x01	// RFID Network in RS485 is 0x01

#define RFID_485_MASTER_NODE_ID 0xFF

#define RFID_485_MESSAGE_ID_DETECTED 0x01
#define RFID_485_MESSAGE_ID_UPDATE 0x02
#define RFID_485_MESSAGE_ID_UPDATE_RESULT 0x03

typedef struct
{
	RFID_Error_t error;
	bool isPlaced;
	bool isDetected;
	RFID_t rfid;
} RFID_Handle;

// Validate Function
static bool RFID_Is485MessageValid(RS485_Message *message);

static bool RFID_handleDetected(RS485_Message* message);
static bool RFID_handleUpdateResult(RS485_Message* message);

static RFID_Handle RFID_handleTable[RFID_ID_MAX];
static RS485_Message RFID_rs485Message;
static RFID_UpdateResultCallback RFID_updateResultCallback;

void RFID_init(void)
{
}

void RFID_run(void)
{
	if(RS485_receive(&RFID_rs485Message))
	{
		if(!RFID_Is485MessageValid(&RFID_rs485Message)){
			return;
		}
		switch(RFID_rs485Message.messageId)
		{
			case RFID_485_MESSAGE_ID_DETECTED:
				RFID_handleDetected(&RFID_rs485Message);
				break;
			case RFID_485_MESSAGE_ID_UPDATE_RESULT:
				RFID_handleUpdateResult(&RFID_rs485Message);
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
	RFID_rs485Message.networkId = RFID_485_NETWORK_ID;
	RFID_rs485Message.srcNode = RFID_485_MASTER_NODE_ID;
	RFID_rs485Message.desNode = id;
	RFID_rs485Message.messageId = RFID_485_MESSAGE_ID_UPDATE;
	RFID_rs485Message.resultCode = RS485_RESULT_CODE_SUCCESS;
	RFID_rs485Message.dataLen = 0;
	RFID_rs485Message.data[RFID_rs485Message.dataLen++] = rfid->id_len;
	for(uint8_t i = 0; i < rfid->id_len; i++)
	{
		RFID_rs485Message.data[RFID_rs485Message.dataLen++] = rfid->id[i];
	}
	RFID_rs485Message.data[RFID_rs485Message.dataLen++] = rfid->code;
	RFID_rs485Message.data[RFID_rs485Message.dataLen++] = rfid->money >> 24;
	RFID_rs485Message.data[RFID_rs485Message.dataLen++] = rfid->money >> 16;
	RFID_rs485Message.data[RFID_rs485Message.dataLen++] = rfid->money >> 8;
	RFID_rs485Message.data[RFID_rs485Message.dataLen++] = rfid->money & 0xFF;
	RFID_rs485Message.data[RFID_rs485Message.dataLen++] = rfid->issueDate[0];
	RFID_rs485Message.data[RFID_rs485Message.dataLen++] = rfid->issueDate[1];
	RFID_rs485Message.data[RFID_rs485Message.dataLen++] = rfid->issueDate[2];
	if(!RS485_send(&RFID_rs485Message)){
		return false;
	}
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
		.money = 200000,
		.issueDate = {24, 1, 7},
	};
	while(1)
	{
		HAL_Delay(1000);
		RFID_set(RFID_ID_1, &rfid);
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
	if(!(message->srcNode >= RFID_ID_1 && message->srcNode <= RFID_ID_3))
	{
		return false;
	}
	return true;
}

static bool RFID_handleDetected(RS485_Message* message)
{
	if(message->desNode != RFID_485_MASTER_NODE_ID)
	{
		return false;
	}
	if(!(message->srcNode >= RFID_ID_1 && message->srcNode <= RFID_ID_3))
	{
		return false;
	}
	uint8_t srcNode = message->srcNode;

	RFID_handleTable[srcNode].isDetected = message->data[0];
	RFID_handleTable[srcNode].rfid.id_len = message->data[1];
	for(int var = 0; var < RFID_handleTable[srcNode].rfid.id_len; ++var)
	{
		RFID_handleTable[srcNode].rfid.id[var] = message->data[2 + var];
	}
	RFID_handleTable[srcNode].rfid.code = message->data[2 + RFID_handleTable[srcNode].rfid.id_len];
	RFID_handleTable[srcNode].rfid.money =
		((uint32_t)message->data[3 + RFID_handleTable[srcNode].rfid.id_len] << 24) |
		((uint32_t)message->data[4 + RFID_handleTable[srcNode].rfid.id_len] << 16) |
		((uint32_t)message->data[5 + RFID_handleTable[srcNode].rfid.id_len] << 8) |
		((uint32_t)message->data[6 + RFID_handleTable[srcNode].rfid.id_len]);
	RFID_handleTable[srcNode].rfid.issueDate[0] =
		message->data[7 + RFID_handleTable[srcNode].rfid.id_len];
	RFID_handleTable[srcNode].rfid.issueDate[1] =
		message->data[8 + RFID_handleTable[srcNode].rfid.id_len];
	RFID_handleTable[srcNode].rfid.issueDate[2] =
		message->data[9 + RFID_handleTable[srcNode].rfid.id_len];

	return true;
}


static bool RFID_handleUpdateResult(RS485_Message* message)
{
	uint8_t srcNode = message->srcNode;

	RFID_Error_t error = message->data[0];

	if(RFID_updateResultCallback){
		RFID_updateResultCallback(srcNode , error);
	}
}
