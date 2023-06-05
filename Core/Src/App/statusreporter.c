/*
 * statusreporter.c
 *
 *  Created on: Jun 3, 2023
 *      Author: xuanthodo
 */

#include <App/mqtt.h>
#include "App/statusreporter.h"
#include "DeviceManager/billacceptormanager.h"
#include "DeviceManager/tcdmanager.h"
#include "Lib/scheduler/scheduler.h"

#define STATUSREPORT_INTERVAL		5 * 60 * 1000 	// 5 minutes

enum {
	STATUSREPORTER_IDLE,
	STATUSREPORTER_REPORT
};


static uint8_t state = STATUSREPORTER_IDLE;
static bool timeout_flag = true;

// Private function
static void STATUSREPORTER_tcd_status();
static void STATUSREPORTER_billacceptor_status();
static void STATUSREPORTER_build_tcd_status_topic(char * buf, char * device_id);
static void STATUSREPORTER_build_tcd_status_payload(char * buf, TCDMNG_Status_t status);
static void STATUSREPORTER_build_billacceptor_status_topic(char * buf, char * device_id);
static void STATUSREPORTER_build_billacceptor_status_payload(char * buf, uint8_t status);
static void STATUSREPORTER_timeout();

bool STATUSREPORTER_init(){

}

bool STATUSREPORTER_run(){
	switch (state) {
		case STATUSREPORTER_IDLE:

			break;
		default:
			break;
	}
}

void STATUSREPORTER_amount_remain(uint32_t amount){
	MQTT_message_t message;
	// Build Topic
	STATUSREPORTER_build_tcd_status_topic(message.topic, "device_123");
	// Build Payload
	TCDMNG_Status_t status = TCDMNG_get_status();
	STATUSREPORTER_build_tcd_status_payload(message.payload, status);
	// Send message
	MQTT_sent_message(&message);
}

static void STATUSREPORTER_idle(){
	if(timeout_flag){
		timeout_flag = false;
		SCH_Add_Task(STATUSREPORTER_timeout, STATUSREPORT_INTERVAL, 0);
		state = STATUSREPORTER_REPORT;
	}
}

static void STATUSREPORTER_report(){
	STATUSREPORTER_tcd_status();
	STATUSREPORTER_billacceptor_status();
	state = STATUSREPORTER_IDLE;
}

static void STATUSREPORTER_tcd_status(){
	MQTT_message_t message;
	// Build Topic
	STATUSREPORTER_build_tcd_status_topic(message.topic, "device_123");
	// Build Payload
	TCDMNG_Status_t status = TCDMNG_get_status();
	STATUSREPORTER_build_tcd_status_payload(message.payload, status);
	// Send message
	MQTT_sent_message(&message);
}


static void STATUSREPORTER_billacceptor_status(){
	MQTT_message_t message;
	// Build Topic
	STATUSREPORTER_build_billacceptor_status_topic(message.topic, "device_123");
	// Build Payload
	uint8_t status = BILLACCEPTORMNG_get_status();
	STATUSREPORTER_build_billacceptor_status_payload(message.payload, status);
	// Send message
	MQTT_sent_message(&message);
}


static void STATUSREPORTER_build_tcd_status_topic(char * buf, char * device_id){
	snprintf(buf,
			TOPIC_MAX_LEN,
			"device/%d/report/tcd_status",
			device_id);
}

static void STATUSREPORTER_build_tcd_status_payload(char * buf, TCDMNG_Status_t status){
	snprintf(buf,
				PAYLOAD_MAX_LEN,
				"{\"tcd_1\":[%d,%d,%d],\"tcd_2\":[%d,%d,%d]}",
				status.TCD_1.is_empty,
				status.TCD_1.is_error,
				status.TCD_1.is_lower,
				status.TCD_2.is_empty,
				status.TCD_2.is_error,
				status.TCD_2.is_lower);
}


static void STATUSREPORTER_build_billacceptor_status_topic(char * buf, char * device_id){
	snprintf(buf,
				TOPIC_MAX_LEN,
				"device/%d/report/billacceptor_status",
				device_id);
}
static void STATUSREPORTER_build_billacceptor_status_payload(char * buf, uint8_t status){
	snprintf(buf,
				PAYLOAD_MAX_LEN,
				"%d",
				status);
}

static void STATUSREPORTER_build_billacceptor_status_topic(char * buf, char * device_id){
	snprintf(buf,
				TOPIC_MAX_LEN,
				"device/%d/report/amount",
				device_id);
}
static void STATUSREPORTER_build_billacceptor_status_payload(char * buf, uint32_t amount){
	snprintf(buf,
				PAYLOAD_MAX_LEN,
				"%d",
				amount);
}
