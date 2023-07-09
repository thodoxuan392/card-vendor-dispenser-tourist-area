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
	SM_WAIT_FOR_PAYOUTING_CARD,
	SM_CALLBACKING_CARD,
	SM_WAIT_FOR_CALLBACKING_CARD
};

static void SM_init();
static void SM_wait_for_init();
static void SM_idle();
static void SM_bill_accepted();
static void SM_payouting_card();
static void SM_wait_for_payouting_card();
static void SM_callbacking_card();
static void SM_wait_for_callbacking_card();
static void SM_update_total_card_by_time(RTC_t *rtc , CONFIG_t *config);
static void SM_timeout();
static void SM_timeout_for_update();
static void SM_printf();

static uint8_t prev_state = SM_INIT;
static uint8_t state = SM_INIT;
static uint8_t timeout_task_id;
static const char * state_name[] = {
		[SM_INIT] = "SM_INIT\r\n",
		[SM_WAITING_FOR_INIT] = "SM_WAITING_FOR_INIT\r\n",
		[SM_IDLE] = "SM_IDLE\r\n",
		[SM_BILL_ACCEPTED] = "SM_BILL_ACCEPTED\r\n",
		[SM_PAYOUTING_CARD] = "SM_PAYOUTING_CARD\r\n",
		[SM_WAIT_FOR_PAYOUTING_CARD] = "SM_WAIT_FOR_PAYOUTING_CARD\r\n",
		[SM_CALLBACKING_CARD] = "SM_CALLBACKING_CARD\r\n",
		[SM_WAIT_FOR_CALLBACKING_CARD] = "SM_WAIT_FOR_CALLBACKING_CARD\r\n",
};
static bool timeout_for_update = true;
static bool timeout = false;

bool STATEMACHINE_init(){

}

bool STATEMACHINE_run(){
	MQTT_run();
	STATUSREPORTER_run();
	COMMANDHANDLER_run();
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
		case SM_WAIT_FOR_PAYOUTING_CARD:
			SM_wait_for_payouting_card();
			break;
		case SM_CALLBACKING_CARD:
			SM_callbacking_card();
			break;
		case SM_WAIT_FOR_CALLBACKING_CARD:
			SM_wait_for_callbacking_card();
			break;
		default:
			break;
	}
	SM_printf();
	prev_state = state;
}


static void SM_init(){
	state = SM_WAITING_FOR_INIT;
	timeout_task_id = SCH_Add_Task(SM_timeout, SM_INIT_DURATION, 0);

}
static void SM_wait_for_init(){
	if(timeout){
		state = SM_IDLE;
	}
}
static void SM_idle(){

	// LCD Manager set IDLE screen
	CONFIG_t *config;
	RTC_t rtc;
	if(timeout_for_update){
		timeout_for_update = false;
		rtc = RTC_get_time();
		config = CONFIG_get();
		LCDMNG_set_working_screen_without_draw(&rtc, config->amount);
		SM_update_total_card_by_time(&rtc, config);
		SCH_Add_Task(SM_timeout_for_update, SM_UPDATE_DURATION, 0);
	}
	// Check if Card is error
	if(TCDMNG_is_error()){
		LCDMNG_set_card_error_screen();
	}else{
		LCDMNG_clear_card_error_screen();
	}

	// Check if Card is empty
	if(TCDMNG_is_empty()){
		LCDMNG_set_card_empty_screen();
	}else{
		LCDMNG_clear_card_empty_screen();
	}

	if(TCDMNG_is_empty() || TCDMNG_is_error()){
		if(BILLACCEPTORMNG_is_enabled()){
			utils_log_warn("Disable BillAcceptor because TCD is empty\r\n");
			BILLACCEPTORMNG_disable();
		}
	}else{
		if(!BILLACCEPTORMNG_is_enabled()){
			utils_log_warn("Renable BillAcceptor because TCD is not empty more\r\n");
			BILLACCEPTORMNG_enable();
		}
	}


	// Check if Card is lower
	if(TCDMNG_is_lower()){
		LCDMNG_set_card_lower_screen();
	}else{
		LCDMNG_clear_card_lower_screen();
	}

	// Check if BILL is accepted
	if(BILLACCEPTORMNG_is_accepted()){
		LCDMNG_clear_idle_screen();
		BILLACCEPTORMNG_clear_accepted();
		config = CONFIG_get();
		rtc = RTC_get_time();
		LCDMNG_set_working_screen(&rtc, config->amount);
		// Timeout to wait user can view money change
		timeout = false;
		timeout_task_id = SCH_Add_Task(SM_timeout, SM_BILLACCEPTOR_DURATION, 0);
		state = SM_BILL_ACCEPTED;
		return;
	}
	// Check amount and payout card
	uint32_t amount = BILLACCEPTORMNG_get_amount();
	config = CONFIG_get();
	if(amount >= config->card_price
			&& amount > 0
			&& config->card_price > 0){
		LCDMNG_clear_idle_screen();
		if(TCDMNG_is_idle() && TCDMNG_is_available_for_use()){
			state = SM_PAYOUTING_CARD;
		}
		return;
	}
	LCDMNG_set_idle_screen();
}
static void SM_bill_accepted(){
	if(timeout){
		state = SM_IDLE;
	}
}

static void SM_payouting_card(){
	// LCD Manager set IDLE screen
	CONFIG_t *config = CONFIG_get();
	RTC_t rtc = RTC_get_time();
	uint32_t amount = BILLACCEPTORMNG_get_amount();
	// Update amount
	amount -= config->card_price;
	config->total_card_by_day++;
	config->total_card_by_month++;
	CONFIG_set(config);
	BILLACCEPTORMNG_set_amount(amount);
	LCDMNG_set_working_screen(&rtc, config->amount);

	TCDMNG_payout();
	state = SM_WAIT_FOR_PAYOUTING_CARD;
}

static void SM_wait_for_payouting_card(){
	CONFIG_t *config;
	RTC_t rtc;
	if(TCDMNG_is_idle()){
		config = CONFIG_get();
		rtc = RTC_get_time();
		LCDMNG_set_working_screen(&rtc, config->amount);
		state = SM_IDLE;
	}

	// Check bill accepted
	if(BILLACCEPTORMNG_is_accepted()){
		BILLACCEPTORMNG_clear_accepted();
		config = CONFIG_get();
		rtc = RTC_get_time();
		LCDMNG_set_working_screen(&rtc, config->amount);
	}
}

static void SM_callbacking_card(){
}

static void SM_wait_for_callbacking_card(){
}

static void SM_update_total_card_by_time(RTC_t *rtc , CONFIG_t *config){
	if(rtc->hour == 0 && rtc->minute == 0){
		// Update total card by day
		config->total_card_by_day = 0;
		if(rtc->date == 0){
			// Update total card by month
			config->total_card_by_month = 0;
		}
		CONFIG_set(config);
	}
}

static void SM_timeout(){
	timeout = true;
}

static void SM_timeout_for_update(){
	timeout_for_update = true;
}

static void SM_printf(){
	if(prev_state != state){
		utils_log_info(state_name[state]);
	}
}
