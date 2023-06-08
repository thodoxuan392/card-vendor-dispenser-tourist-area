/*
 * tcd_manager.c
 *
 *  Created on: Jun 2, 2023
 *      Author: xuanthodo
 */


#include "main.h"
#include "DeviceManager/tcdmanager.h"
#include "Device/tcd.h"
#include "Lib/scheduler/scheduler.h"

#define PAYOUT_DURATION			3000	//3s
#define CALLBACK_DURATION		3000	//3s

enum {
	TCDMNG_IDLE,
	TCDMNG_RESETING,
	TCDMNG_WAIT_FOR_RESETING,
	TCDMNG_PAYOUTING,
	TCDMNG_WAIT_FOR_PAYOUTING,
	TCDMNG_CALLBACKING,
	TCDMNG_WAIT_FOR_CALLBACKING,
	TCDMNG_ERROR
};

static uint8_t tcdmng_state = TCDMNG_IDLE;
static TCDMNG_Status_t status;
static TCD_id_t tcd_using;

// Action
static bool payout_enable = false;
static bool callback_enable = false;
static bool reset_enable = false;

// Timeout
static bool timeout_flag = false;

// Private function
static void TCDMNG_idle();
static void TCDMNG_reseting();
static void TCDMNG_wait_for_reseting();
static void TCDMNG_payouting();
static void TCDMNG_wait_for_payouting();
static void TCDMNG_callbacking();
static void TCDMNG_wait_for_callbacking();
static void TCDMNG_error();
static void TCDMNG_update_status();
static bool TCDMNG_is_error();
static bool TCDMNG_is_available(TCD_id_t tcd);
static void TCDMNG_timeout();

void TCDMNG_init(){
	// Do something
}


void TCDMNG_run(){
	switch (tcdmng_state) {
		case TCDMNG_IDLE:
			TCDMNG_idle();
			break;
		case TCDMNG_RESETING:
			TCDMNG_reseting();
			break;
		case TCDMNG_WAIT_FOR_RESETING:
			TCDMNG_wait_for_reseting();
			break;
		case TCDMNG_PAYOUTING:
			TCDMNG_payouting();
			break;
		case TCDMNG_WAIT_FOR_PAYOUTING:
			TCDMNG_wait_for_payouting();
			break;
		case TCDMNG_CALLBACKING:
			TCDMNG_callbacking();
			break;
		case TCDMNG_WAIT_FOR_CALLBACKING:
			TCDMNG_wait_for_callbacking();
			break;
		case TCDMNG_ERROR:
			TCDMNG_error();
			break;
		default:
			break;
	}
}

TCDMNG_Status_t TCDMNG_get_status(){
	return status;
}
uint8_t TCDMNG_get_state(){
	return tcdmng_state;
}


void TCDMNG_reset(){
	reset_enable = true;
}

void TCDMNG_payout(){
	payout_enable = true;
}

void TCDMNG_callback(){
	callback_enable = true;
}


static void TCDMNG_idle(){
	TCDMNG_update_status();
	if(reset_enable){
		reset_enable = false;
		tcdmng_state = TCDMNG_RESETING;
	}
	else if(payout_enable){
		payout_enable = false;
		tcdmng_state = TCDMNG_PAYOUTING;
	}
	else if (callback_enable){
		callback_enable = false;
		tcdmng_state = TCDMNG_CALLBACKING;
	}
	else if (TCDMNG_is_error()){
		tcdmng_state = TCDMNG_ERROR;
	}
}

static void TCDMNG_reseting(){
	TCD_reset(TCD_1, true);
	TCD_reset(TCD_2, true);
	// How long to enable payout signal
	timeout_flag = false;
	SCH_Add_Task(TCDMNG_timeout, PAYOUT_DURATION, 0);
	tcdmng_state = TCDMNG_WAIT_FOR_RESETING;
}

static void TCDMNG_wait_for_reseting(){
	if(timeout_flag){
		TCD_reset(TCD_1, false);
		TCD_reset(TCD_2, false);
	}
}

static void TCDMNG_payouting(){
	// Check what TCD is available for payout
	if(TCDMNG_is_available(TCD_1)){
		tcd_using = TCD_1;
		TCD_payout_card(TCD_1, true);
	}else if(TCDMNG_is_available(TCD_2)){
		tcd_using = TCD_2;
		TCD_payout_card(TCD_2, true);
	}else{
		tcdmng_state = TCDMNG_ERROR;
	}
	// How long to enable payout signal
	timeout_flag = false;
	SCH_Add_Task(TCDMNG_timeout, PAYOUT_DURATION, 0);
	// Switch State
	tcdmng_state = TCDMNG_WAIT_FOR_PAYOUTING;
}

static void TCDMNG_wait_for_payouting(){
	if(timeout_flag){
		TCD_payout_card(tcd_using, false);
		tcdmng_state = TCDMNG_IDLE;
	}
}

static void TCDMNG_callbacking(){
	// How long to enable payout signal
	// Check what TCD is available for payout
	if(TCDMNG_is_available(TCD_1)){
		tcd_using = TCD_1;
		TCD_callback(TCD_1, true);
	}else if(TCDMNG_is_available(TCD_2)){
		tcd_using = TCD_2;
		TCD_callback(TCD_2, true);
	}else{
		tcdmng_state = TCDMNG_ERROR;
	}
	// How long to enable payout signal
	timeout_flag = false;
	SCH_Add_Task(TCDMNG_timeout, PAYOUT_DURATION, 0);
	tcdmng_state = TCDMNG_WAIT_FOR_CALLBACKING;
}

static void TCDMNG_wait_for_callbacking(){
	if(timeout_flag){
		TCD_callback(tcd_using, false);
		tcdmng_state = TCDMNG_IDLE;
	}
}

static void TCDMNG_error(){
	tcdmng_state = TCDMNG_IDLE;
}

static bool TCDMNG_is_error(){
	return status.TCD_1.is_error && status.TCD_2.is_error;
}

static void TCDMNG_update_status(){
	// Get status of 2 TCD
	status.TCD_1.is_empty = TCD_is_empty(TCD_1);
	status.TCD_1.is_error = TCD_is_error(TCD_1);
	status.TCD_1.is_lower = TCD_is_lower(TCD_1);
	status.TCD_2.is_empty = TCD_is_empty(TCD_2);
	status.TCD_2.is_error = TCD_is_error(TCD_2);
	status.TCD_2.is_lower = TCD_is_lower(TCD_2);
}

static bool TCDMNG_is_available(TCD_id_t tcd){
	if(tcd == TCD_1){
		return (!status.TCD_1.is_empty &&
				!status.TCD_1.is_lower &&
				!status.TCD_1.is_error);
	}else {
		return (!status.TCD_2.is_empty &&
				!status.TCD_2.is_lower &&
				!status.TCD_2.is_error);
	}
}

static void TCDMNG_timeout(){
	timeout_flag = true;
}
