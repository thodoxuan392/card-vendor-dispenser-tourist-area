/*
 * statusreporter.c
 *
 *  Created on: Jun 3, 2023
 *      Author: xuanthodo
 */

#include "config.h"
#include "App/statusreporter.h"
#include "DeviceManager/billacceptormanager.h"
#include "DeviceManager/tcdmanager.h"
#include "scheduler/scheduler.h"

#define STATUSREPORT_INTERVAL		60 * 1000 	// 60s

static bool timeout_flag = true;

// Private function
static void STATUSREPORTER_report_status();
static void STATUSREPORTER_build_status_topic(char * buf, char * device_id);
static void STATUSREPORTER_build_status_payload(char * buf,
													CONFIG_t* config,
													uint8_t connection_type,
													TCDMNG_Status_t tcd_status,
													uint8_t billacepptor_status);
static void STATUSREPORTER_build_bill_accepted_topic(char * buf, char * device_id);
static void STATUSREPORTER_build_bill_accepted_payload(char * buf, uint32_t bill_value);
static void STATUSREPORTER_build_dispense_topic(char * buf, char * device_id);
static void STATUSREPORTER_build_dispense_payload(char * buf, uint32_t direction);
static void STATUSREPORTER_build_transaction_topic(char * buf, char * device_id);
static void STATUSREPORTER_build_transaction_payload(char * buf, uint32_t transaction_bill,
																uint32_t transaction_quantity);
static void STATUSREPORTER_timeout();

bool STATUSREPORTER_init(){

}

bool STATUSREPORTER_run(){
	if(timeout_flag){
		timeout_flag = false;
		// Publish status
		STATUSREPORTER_report_status();
		SCH_Add_Task(STATUSREPORTER_timeout, STATUSREPORT_INTERVAL, 0);
	}
}

void STATUSREPORTER_report_billaccepted(uint32_t bill_value){

}

void STATUSREPORTER_report_dispense(uint32_t direction){

}


void STATUSREPORTER_report_transaction(uint32_t card_price, uint32_t transaction_quantity){

}

static void STATUSREPORTER_report_status(){

}

static void STATUSREPORTER_timeout(){
	timeout_flag = true;
}
