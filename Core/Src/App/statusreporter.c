/*
 * statusreporter.c
 *
 *  Created on: Jun 3, 2023
 *      Author: xuanthodo
 */
#include "App/statusreporter.h"

#include "config.h"
#include "App/protocol.h"
#include "DeviceManager/billacceptormanager.h"
#include "DeviceManager/tcdmanager.h"
#include "Device/eeprom.h"
#include "scheduler/scheduler.h"

#define STATUSREPORT_INTERVAL		 500 	// 500ms


// Private function
static void STATUSREPORTER_buildStsDeviceErrProtocol(PROTOCOL_t *proto, uint16_t errorFlags);
static void STATUSREPORTER_buildStsBillAcceptor(PROTOCOL_t *proto, uint8_t billAcceptorStatus);
static void STATUSREPORTER_buildStsTcd(PROTOCOL_t *proto, TCD_id_t id, TCD_status_t tcdStatus);
static void STATUSREPORTER_buildEvtRfidDetected(PROTOCOL_t *proto, RFID_Id_t id, RFID_t *rfid);
static void STATUSREPORTER_buildEvtBillAccepted(PROTOCOL_t *proto, uint32_t billValue);
static void STATUSREPORTER_buildEvtCard(PROTOCOL_t *proto, TCD_id_t id, uint8_t cardDirection);

static uint16_t STATUSREPORTER_getDeviceErrorFlags(void);

static void STATUSREPORTER_timeout();

static bool STATUSREPORTER_timeout_flag = true;

bool STATUSREPORTER_init(){
	// Nothing to do
}

bool STATUSREPORTER_run(){
	if(STATUSREPORTER_timeout_flag){
		STATUSREPORTER_timeout_flag = false;

		STATUSREPORTER_sendAll();

		SCH_Add_Task(STATUSREPORTER_timeout, STATUSREPORT_INTERVAL, 0);
	}
}

void STATUSREPORTER_sendAll(void){
	uint16_t errorFlag = STATUSREPORTER_getDeviceErrorFlags();
	STATUSREPORTER_sendStsDeviceErr(errorFlag);

	uint8_t billAcceptorStatus = BILLACCEPTORMNG_get_status();
	STATUSREPORTER_sendStsBillAcceptor(billAcceptorStatus);

	for (int id = 0; id < TCD_MAX; ++id) {
		TCD_status_t tcdStatus = TCDMNG_get_status(id);
		STATUSREPORTER_sendStsTcd(id, tcdStatus);
	}
}

void STATUSREPORTER_sendStsDeviceErr(uint16_t errorFlags){
	PROTOCOL_t proto;

	STATUSREPORTER_buildStsDeviceErrProtocol(&proto, errorFlags);
	PROTOCOL_send(&proto);
}

void STATUSREPORTER_sendStsBillAcceptor(uint8_t billAcceptorStatus){
	PROTOCOL_t proto;

	STATUSREPORTER_buildStsBillAcceptor(&proto, billAcceptorStatus);
	PROTOCOL_send(&proto);
}

void STATUSREPORTER_sendStsTcd(TCD_id_t id, TCD_status_t tcdStatus){
	PROTOCOL_t proto;

	STATUSREPORTER_buildStsTcd(&proto, id, tcdStatus);
	PROTOCOL_send(&proto);
}

void STATUSREPORTER_sendEvtRfidDetected(RFID_Id_t id, RFID_t *rfid){
	PROTOCOL_t proto;

	STATUSREPORTER_buildEvtRfidDetected(&proto, id, rfid);
	PROTOCOL_send(&proto);
}

void STATUSREPORTER_sendEvtBillAccepted(uint32_t billValue){
	PROTOCOL_t proto;

	STATUSREPORTER_buildEvtBillAccepted(&proto, billValue);
	PROTOCOL_send(&proto);
}
void STATUSREPORTER_sendEvtCard(TCD_id_t id, uint8_t cardDirection){
	PROTOCOL_t proto;

	STATUSREPORTER_buildEvtCard(&proto, id, cardDirection);
	PROTOCOL_send(&proto);
}

static uint16_t STATUSREPORTER_getDeviceErrorFlags(void){

	uint16_t errorFlags = 0;
	errorFlags |= BILLACCEPTORMNG_is_error() << 0;
	errorFlags |= EEPROM_isError() << 1;
	errorFlags |= 0U << 2;	// Reserved for Keypad
	errorFlags |= 0U << 3;	// Reserved for LED

	bool rfidError = false;
	for (int id = 0; id < RFID_ID_MAX; ++id) {
		rfidError |= RFID_isError(id);
	}
	errorFlags |= (uint8_t)rfidError << 4;
	errorFlags |= 0U << 5; 	// Reserved for SIM
	errorFlags |= 0U << 6;  // Reserved for Sound

	bool tcdError = false;
	for (int id = 0; id < TCD_MAX; ++id) {
		tcdError |= TCD_is_error(id);
	}
	errorFlags |= (uint8_t)tcdError << 7;
	errorFlags |= 0U << 8; 	// Reserved for WIFI

	return errorFlags;
}

static void STATUSREPORTER_buildStsDeviceErrProtocol(PROTOCOL_t *proto, uint16_t errorFlags){
	proto->protocol_id = PROTOCOL_ID_STS_DEVICE_ERR;
	proto->data_len = 0;
	proto->data[proto->data_len++] = errorFlags >> 8;
	proto->data[proto->data_len++] = errorFlags & 0xFF;
}

static void STATUSREPORTER_buildStsBillAcceptor(PROTOCOL_t *proto, uint8_t billAcceptorStatus){
	proto->protocol_id = PROTOCOL_ID_STS_BILL_ACCEPTOR;
	proto->data_len = 0;
	proto->data[proto->data_len++] = billAcceptorStatus >> 8;
}

static void STATUSREPORTER_buildStsTcd(PROTOCOL_t *proto, TCD_id_t id, TCD_status_t tcdStatus){
	proto->protocol_id = PROTOCOL_ID_STS_TCD;
	proto->data_len = 0;
	proto->data[proto->data_len++] = id;
	proto->data[proto->data_len++] = tcdStatus.is_error;
	proto->data[proto->data_len++] = tcdStatus.is_lower;
	proto->data[proto->data_len++] = tcdStatus.is_empty;
}

static void STATUSREPORTER_buildEvtRfidDetected(PROTOCOL_t *proto, RFID_Id_t id, RFID_t *rfid){
	proto->protocol_id = PROTOCOL_ID_EVT_RFID_DETECTED;
	proto->data_len = 0;
	proto->data[proto->data_len++] = id;
	proto->data[proto->data_len++] = rfid->id_len;
	for (uint8_t i = 0; i < rfid->id_len; ++i) {
		proto->data[proto->data_len++] = rfid->id[i];
	}
	proto->data[proto->data_len++] = rfid->money >> 24;
	proto->data[proto->data_len++] = rfid->money >> 16;
	proto->data[proto->data_len++] = rfid->money >> 8;
	proto->data[proto->data_len++] = rfid->money & 0xFF;
	proto->data[proto->data_len++] = rfid->issueDate[2];
	proto->data[proto->data_len++] = rfid->issueDate[1];
	proto->data[proto->data_len++] = rfid->issueDate[0];
	proto->data[proto->data_len++] = rfid->code;
}

static void STATUSREPORTER_buildEvtBillAccepted(PROTOCOL_t *proto, uint32_t billValue){
	proto->protocol_id = PROTOCOL_ID_EVT_BILL_ACCEPTED;
	proto->data_len = 0;
	proto->data[proto->data_len++] = billValue >> 24;
	proto->data[proto->data_len++] = billValue >> 16;
	proto->data[proto->data_len++] = billValue >> 8;
	proto->data[proto->data_len++] = billValue & 0xFF;
}

static void STATUSREPORTER_buildEvtCard(PROTOCOL_t *proto, TCD_id_t id, uint8_t cardDirection){
	proto->protocol_id = PROTOCOL_ID_EVT_CARD_EVENT;
	proto->data_len = 0;
	proto->data[proto->data_len++] = cardDirection;
}

static void STATUSREPORTER_timeout(){
	STATUSREPORTER_timeout_flag = true;
}
