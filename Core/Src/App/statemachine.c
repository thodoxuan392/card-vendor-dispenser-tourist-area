/*
 * statemachine.c
 *
 *  Created on: May 10, 2023
 *      Author: xuanthodo
 */


#include "config.h"
#include "App/statemachine.h"
#include "App/statusreporter.h"
#include "App/commandhandler.h"
#include "App/mqtt.h"
#include "DeviceManager/billacceptormanager.h"
#include "DeviceManager/keypadmanager.h"
#include "DeviceManager/tcdmanager.h"
#include "Lib/scheduler/scheduler.h"

enum {
	SM_IDLE,
	SM_RECEIVING_BILL,
	SM_RECYCLING_BILL,
	SM_PAYOUTING_CARD,
	SM_CALLBACKING_CARD
};

static void SM_idle();
static void SM_payouting_card();
static void SM_callbacking_card();

static uint8_t state = SM_IDLE;

bool STATEMACHINE_init(){

}

bool STATEMACHINE_run(){
//	MQTT_run();
//	STATUSREPORTER_run();
//	COMMANDHANDLER_run();
	BILLACCEPTORMNG_run();
//	KEYPADMNG_run();
//	TCDMNG_run();
	SCH_Dispatch_Tasks();
	switch (state) {
		case SM_IDLE:
//			SM_idle();
			break;
		default:
			break;
	}
}


static void SM_idle(){
	CONFIG_t* config = CONFIG_get();
	uint32_t amount = BILLACCEPTORMNG_get_amount();
	// Just check bill accepted or not
	if(BILLACCEPTORMNG_is_accepted()){
		BILLACCEPTORMNG_clear_accepted();
		TCDMNG_payout();
		// Report amount to Server
		STATUSREPORTER_amount_remain(amount);
	}
	if(amount > config->card_price){
		amount -= config->card_price;
		BILLACCEPTORMNG_set_amount(amount);
		// Save Amount to
		// Switch to Payouting Card
		TCDMNG_payout();
		state = SM_PAYOUTING_CARD;
	}
}
