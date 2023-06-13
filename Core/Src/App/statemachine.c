/*
 * statemachine.c
 *
 *  Created on: May 10, 2023
 *      Author: xuanthodo
 */


#include "config.h"
#include "App/mqtt.h"
#include "App/statemachine.h"
#include "App/statusreporter.h"
#include "App/commandhandler.h"
#include "DeviceManager/billacceptormanager.h"
#include "DeviceManager/keypadmanager.h"
#include "DeviceManager/tcdmanager.h"
#include "DeviceManager/lcdmanager.h"
#include "Lib/scheduler/scheduler.h"
#include "Lib/utils/utils_logger.h"

enum {
	SM_INIT,
	SM_WAITING_FOR_INIT,
	SM_IDLE,
	SM_BILL_ACCEPTED,
	SM_PAYOUTING_CARD,
	SM_CALLBACKING_CARD,
};

static void SM_init();
static void SM_wait_for_init();
static void SM_idle();
static void SM_bill_accepted();
static void SM_payouting_card();
static void SM_callbacking_card();
static void SM_handle_keypad();
static void SM_timeout();
static void SM_printf();

static uint8_t prev_state = SM_IDLE;
static uint8_t state = SM_IDLE;
static const char * state_name[] = {
		[SM_INIT] = "SM_INIT\r\n",
		[SM_WAITING_FOR_INIT] = "SM_WAITING_FOR_INIT\r\n",
		[SM_IDLE] = "SM_IDLE\r\n",
		[SM_BILL_ACCEPTED] = "SM_BILL_ACCEPTED\r\n",
		[SM_PAYOUTING_CARD] = "SM_PAYOUTING_CARD\r\n",
		[SM_CALLBACKING_CARD] = "SM_CALLBACKING_CARD\r\n",
};
static bool timeout = false;

bool STATEMACHINE_init(){

}

bool STATEMACHINE_run(){
//	MQTT_run();
//	STATUSREPORTER_run();
//	COMMANDHANDLER_run();
	BILLACCEPTORMNG_run();
	LCDMNG_run();
	KEYPADMNG_run();
	KEYPADHANDLER_run();
	TCDMNG_run();
	SCH_Dispatch_Tasks();
	switch (state) {
		case SM_INIT:
			SM_init();
			break;
		case SM_WAITING_FOR_INIT:
			SM_wait_for_init();
			break;
		case SM_IDLE:
			SM_idle();
			break;
		case SM_BILL_ACCEPTED:
			SM_bill_accepted();
			break;
		case SM_PAYOUTING_CARD:
			SM_payouting_card();
			break;
		case SM_CALLBACKING_CARD:
			SM_callbacking_card();
			break;
		default:
			break;
	}
	SM_printf();
	prev_state = state;
}


static void SM_init(){
	// LCD Manager Set Init screen
	LCDMNG_set_init_screen();
	state = SM_WAITING_FOR_INIT;
	SCH_Add_Task(SM_timeout, SM_INIT_DURATION, 0);

}
static void SM_wait_for_init(){
	if(timeout){
		state = SM_IDLE;
	}
}
static void SM_idle(){

	// LCD Manager set IDLE screen
	CONFIG_t *config = CONFIG_get();
	RTC_t rtc = RTC_get_time();
	// Update screen
	LCDMNG_set_working_screen_without_draw(&rtc, config->amount);
	// Check if BILL is accepted
	if(BILLACCEPTORMNG_is_accepted()){
		BILLACCEPTORMNG_clear_accepted();
		state = SM_BILL_ACCEPTED;
	}
	// Check amount and payout card
	uint32_t amount = BILLACCEPTORMNG_get_amount();
	if(amount >= config->card_price){
		state = SM_PAYOUTING_CARD;
	}
}
static void SM_bill_accepted(){
	CONFIG_t *config = CONFIG_get();
	RTC_t rtc = RTC_get_time();
	LCDMNG_set_working_screen(&rtc, config->amount);
	state = SM_IDLE;
}
static void SM_payouting_card(){
	// LCD Manager set IDLE screen
	CONFIG_t *config = CONFIG_get();
	RTC_t rtc = RTC_get_time();
	// Payout card and switch to waiting
	uint32_t amount = BILLACCEPTORMNG_get_amount();
	if(TCDMNG_is_idle() && !TCDMNG_is_error()){
		LCDMNG_set_working_screen(&rtc, config->amount);
		amount -= config->card_price;
		BILLACCEPTORMNG_set_amount(amount);
		TCDMNG_payout();
		state = SM_IDLE;
	}

	if(TCDMNG_is_in_processing()){
		LCDMNG_set_processing_screen();
	}
}

static void SM_callbacking_card(){
	// Callback card when TCDMNG idle
	if(TCDMNG_is_idle() && !TCDMNG_is_error()){
		TCDMNG_callback();
		state = SM_IDLE;
	}
}

static void SM_timeout(){
	timeout = true;
}

static void SM_printf(){
	if(prev_state != state){
		utils_log_info(state_name[state]);
	}
}
