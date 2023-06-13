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
#include "Lib/utils/utils_logger.h"

#define INIT_DURATION					3000	// 3s
#define PAYOUT_DURATION					500	//500ms
#define CALLBACK_DURATION				300	//300ms
#define CARD_TO_PLACE_CARD_TIMEOUT		10000	// 3 seconds
#define TAKING_CARD_TIMEOUT				60000	// 3 minutes
#define ERROR_CHECK_INTERVAL			10000	//500ms
#define UPDATING_STATUS_TIME_WHEN_LOWER			5000	//7000s
#define UPDATING_STATUS_TIME_WHEN_NORMAL		100	//100ms


enum {
	TCDMNG_WAIT_FOR_INIT,
	TCDMNG_IDLE,
	TCDMNG_RESETING,
	TCDMNG_WAIT_FOR_RESETING,
	TCDMNG_PAYOUTING,
	TCDMNG_WAIT_FOR_PAYOUTING,
	TCDMNG_WAIT_FOR_CARD_IN_PLACE,
	TCDMNG_WAIT_FOR_TAKING_CARD,
	TCDMNG_CALLBACKING,
	TCDMNG_WAIT_FOR_CALLBACKING,
	TCDMNG_WAIT_FOR_UPDATING_STATUS,
	TCDMNG_ERROR
};

static const char * tcdmng_state_name[] = {
		[TCDMNG_WAIT_FOR_INIT] = "TCDMNG_WAIT_FOR_INIT\r\n",
		[TCDMNG_IDLE] = "TCDMNG_IDLE\r\n",
		[TCDMNG_RESETING] = "TCDMNG_RESETING\r\n",
		[TCDMNG_WAIT_FOR_RESETING] = "TCDMNG_WAIT_FOR_RESETING\r\n",
		[TCDMNG_PAYOUTING] = "TCDMNG_PAYOUTING\r\n",
		[TCDMNG_WAIT_FOR_PAYOUTING] = "TCDMNG_WAIT_FOR_PAYOUTING\r\n",
		[TCDMNG_WAIT_FOR_CARD_IN_PLACE] = "TCDMNG_WAIT_FOR_CARD_IN_PLACE\r\n",
		[TCDMNG_WAIT_FOR_TAKING_CARD] = "TCDMNG_WAIT_FOR_TAKING_CARD\r\n",
		[TCDMNG_CALLBACKING] = "TCDMNG_CALLBACKING\r\n",
		[TCDMNG_WAIT_FOR_CALLBACKING] = "TCDMNG_WAIT_FOR_CALLBACKING\r\n",
		[TCDMNG_WAIT_FOR_UPDATING_STATUS] = "TCDMNG_WAIT_FOR_UPDATING_STATUS\r\n",
		[TCDMNG_ERROR] = "TCDMNG_ERROR\r\n",
};

static uint8_t tcdmng_prev_state = TCDMNG_WAIT_FOR_INIT;
static uint8_t tcdmng_state = TCDMNG_WAIT_FOR_INIT;
static TCDMNG_Status_t status = {
	.TCD_1 = {
		.is_error = true,
		.is_lower = true,
		.is_empty = true
	},
	.TCD_2 = {
		.is_error = true,
		.is_lower = true,
		.is_empty = true
	}
};
static TCD_id_t tcd_using;

// Action
static bool payout_enable = false;
static bool callback_enable = false;
static bool reset_enable = false;

// Timeout
static uint32_t task_id;
static bool timeout_flag = false;
static bool timeout_for_init_flag = false;

static uint32_t updating_status_time = UPDATING_STATUS_TIME_WHEN_NORMAL;

// Private function
static void TCDMNG_wait_for_init();
static void TCDMNG_idle();
static void TCDMNG_reseting();
static void TCDMNG_wait_for_reseting();
static void TCDMNG_payouting();
static void TCDMNG_wait_for_payouting();
static void TCDMNG_wait_for_card_in_place();
static void TCDMNG_wait_for_taking_card();
static void TCDMNG_callbacking();
static void TCDMNG_wait_for_callbacking();
static void TCDMNG_wait_for_updating_status();
static void TCDMNG_error();
static void TCDMNG_update_status();
static bool TCDMNG_is_available(TCD_id_t tcd);
static bool TCDMNG_is_lower(TCD_id_t tcd);
static void TCDMNG_timeout();
static void TCDMNG_timeout_for_init();
static void TCDMNG_printf();

void TCDMNG_init(){
	// Do something
	SCH_Add_Task(TCDMNG_timeout_for_init, INIT_DURATION, 0);
}


void TCDMNG_run(){
	TCDMNG_update_status();
	switch (tcdmng_state) {
		case TCDMNG_WAIT_FOR_INIT:
			TCDMNG_wait_for_init();
			break;
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
		case TCDMNG_WAIT_FOR_CARD_IN_PLACE:
			TCDMNG_wait_for_card_in_place();
			break;
		case TCDMNG_WAIT_FOR_TAKING_CARD:
			TCDMNG_wait_for_taking_card();
			break;
		case TCDMNG_CALLBACKING:
			TCDMNG_callbacking();
			break;
		case TCDMNG_WAIT_FOR_CALLBACKING:
			TCDMNG_wait_for_callbacking();
			break;
		case TCDMNG_WAIT_FOR_UPDATING_STATUS:
			TCDMNG_wait_for_updating_status();
			break;
		case TCDMNG_ERROR:
			TCDMNG_error();
			break;
		default:
			break;
	}
	TCDMNG_printf();
	tcdmng_prev_state = tcdmng_state;
}

TCDMNG_Status_t TCDMNG_get_status(){
	return status;
}
uint8_t TCDMNG_get_state(){
	return tcdmng_state;
}

bool TCDMNG_is_idle(){
	return (tcdmng_state == TCDMNG_IDLE);
}

bool TCDMNG_is_in_processing(){
	return (tcdmng_state != TCDMNG_IDLE && tcdmng_state != TCDMNG_ERROR);
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


bool TCDMNG_is_error(){
	TCDMNG_update_status();
	return (status.TCD_1.is_error || status.TCD_1.is_empty) &&
			(status.TCD_2.is_error || status.TCD_2.is_empty);
}

static void TCDMNG_wait_for_init(){
	if(timeout_for_init_flag){
		tcdmng_state = TCDMNG_IDLE;
	}
}




static void TCDMNG_idle(){
	if (TCDMNG_is_error()){
		SCH_Delete_Task(task_id);
		timeout_flag = false;
		task_id = SCH_Add_Task(TCDMNG_timeout, ERROR_CHECK_INTERVAL, 0);
		tcdmng_state = TCDMNG_ERROR;
	}
	else if(reset_enable){
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
}

static void TCDMNG_reseting(){
	TCD_reset(TCD_1, true);
	TCD_reset(TCD_2, true);
	// How long to enable payout signal
	SCH_Delete_Task(task_id);
	timeout_flag = false;
	task_id = SCH_Add_Task(TCDMNG_timeout, PAYOUT_DURATION, 0);
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
	if((TCDMNG_is_available(TCD_1) && !TCDMNG_is_lower(TCD_1))
		|| (TCDMNG_is_available(TCD_1) && TCDMNG_is_lower(TCD_1) && TCDMNG_is_lower(TCD_2))){
		tcd_using = TCD_1;
		TCD_payout_card(TCD_1, true);
		// How long to enable payout signal
		SCH_Delete_Task(task_id);
		timeout_flag = false;
		task_id = SCH_Add_Task(TCDMNG_timeout, PAYOUT_DURATION, 0);
		tcdmng_state = TCDMNG_WAIT_FOR_PAYOUTING;
	}
	else if(TCDMNG_is_available(TCD_2)){
		tcd_using = TCD_2;
		TCD_payout_card(TCD_2, true);
		// How long to enable payout signal
		SCH_Delete_Task(task_id);
		timeout_flag = false;
		task_id = SCH_Add_Task(TCDMNG_timeout, PAYOUT_DURATION, 0);
		tcdmng_state = TCDMNG_WAIT_FOR_PAYOUTING;
	}
	else{
		SCH_Delete_Task(task_id);
		timeout_flag = false;
		task_id = SCH_Add_Task(TCDMNG_timeout, ERROR_CHECK_INTERVAL, 0);
		tcdmng_state = TCDMNG_ERROR;
	}
}

static void TCDMNG_wait_for_payouting(){
	if(timeout_flag){
		TCD_payout_card(tcd_using, false);
		SCH_Delete_Task(task_id);
		timeout_flag = false;
		task_id = SCH_Add_Task(TCDMNG_timeout, CARD_TO_PLACE_CARD_TIMEOUT, 0);
		tcdmng_state = TCDMNG_WAIT_FOR_CARD_IN_PLACE;
	}
}

static void TCDMNG_wait_for_card_in_place(){
	if(timeout_flag){
		utils_log_error("Timeout to payout card, check card in tcd\r\n");
		SCH_Delete_Task(task_id);
		timeout_flag = false;
		task_id = SCH_Add_Task(TCDMNG_timeout, ERROR_CHECK_INTERVAL, 0);
		tcdmng_state = TCDMNG_ERROR;
	}
	if(TCD_is_out_ok(tcd_using)){
		SCH_Delete_Task(task_id);
		timeout_flag = false;
		task_id = SCH_Add_Task(TCDMNG_timeout, TAKING_CARD_TIMEOUT, 0);
		tcdmng_state = TCDMNG_WAIT_FOR_TAKING_CARD;
	}
}

static void TCDMNG_wait_for_taking_card(){
	if(timeout_flag){
		utils_log_error("Timeout to taking card\r\n");
		SCH_Delete_Task(task_id);
		timeout_flag = false;
		task_id = SCH_Add_Task(TCDMNG_timeout, ERROR_CHECK_INTERVAL, 0);
		tcdmng_state = TCDMNG_ERROR;
	}
	// Card is in place but cannot be take
	if(!TCD_is_out_ok(tcd_using)){
		SCH_Delete_Task(task_id);
		timeout_flag = false;
		if(TCDMNG_is_lower(tcd_using)){
			updating_status_time = UPDATING_STATUS_TIME_WHEN_LOWER;
		}else{
			updating_status_time = UPDATING_STATUS_TIME_WHEN_NORMAL;
		}
		task_id = SCH_Add_Task(TCDMNG_timeout, updating_status_time , 0);
		tcdmng_state = TCDMNG_WAIT_FOR_UPDATING_STATUS;

	}
}

static void TCDMNG_callbacking(){
	// How long to enable payout signal
	// Check what TCD is available for payout
	if(TCDMNG_is_available(TCD_1)){
		tcd_using = TCD_1;
		TCD_callback(TCD_1, true);
		// How long to enable payout signal
		SCH_Delete_Task(task_id);
		timeout_flag = false;
		task_id = SCH_Add_Task(TCDMNG_timeout, PAYOUT_DURATION, 0);
		tcdmng_state = TCDMNG_WAIT_FOR_CALLBACKING;
	}else if(TCDMNG_is_available(TCD_2)){
		tcd_using = TCD_2;
		TCD_callback(TCD_2, true);
		// How long to enable payout signal
		SCH_Delete_Task(task_id);
		timeout_flag = false;
		task_id = SCH_Add_Task(TCDMNG_timeout, PAYOUT_DURATION, 0);
		tcdmng_state = TCDMNG_WAIT_FOR_CALLBACKING;
	}else{
		SCH_Delete_Task(task_id);
		timeout_flag = false;
		task_id = SCH_Add_Task(TCDMNG_timeout, ERROR_CHECK_INTERVAL, 0);
		tcdmng_state = TCDMNG_ERROR;
	}

}

static void TCDMNG_wait_for_callbacking(){
	if(timeout_flag){
		TCD_callback(tcd_using, false);
		tcdmng_state = TCDMNG_IDLE;
	}
}

static void TCDMNG_wait_for_updating_status(){
	if(timeout_flag){
		tcdmng_state = TCDMNG_IDLE;
	}
}


static void TCDMNG_error(){
	if(timeout_flag){
		if(!TCDMNG_is_error()){
			tcdmng_state = TCDMNG_IDLE;
		}else{
			SCH_Delete_Task(task_id);
			timeout_flag = false;
			task_id = SCH_Add_Task(TCDMNG_timeout, ERROR_CHECK_INTERVAL, 0);
		}

	}

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


static bool TCDMNG_is_lower(TCD_id_t id){
	return TCD_is_lower(id);
}


static bool TCDMNG_is_available(TCD_id_t tcd){
	TCDMNG_update_status();
	if(tcd == TCD_1){
		return (!status.TCD_1.is_empty &&
				!status.TCD_1.is_error);
	}else {
		return (!status.TCD_2.is_empty &&
				!status.TCD_2.is_error);
	}
}

static void TCDMNG_timeout(){
	timeout_flag = true;
}

static void TCDMNG_timeout_for_init(){
	timeout_for_init_flag = true;
}


static void TCDMNG_printf(){
	if(tcdmng_prev_state != tcdmng_state){
		utils_log_info(tcdmng_state_name[tcdmng_state]);
	}
}
