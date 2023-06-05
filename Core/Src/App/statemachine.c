/*
 * statemachine.c
 *
 *  Created on: May 10, 2023
 *      Author: xuanthodo
 */


#include "App/statemachine.h"
#include "App/statusreporter.h"
#include "App/commandhandler.h"
#include "DeviceManager/billacceptormanager.h"
#include "DeviceManager/keypadmanager.h"
#include "DeviceManager/tcdmanager.h"

enum {
	SM_IDLE,
	SM_RECEIVING_BILL,
	SM_RECYCLING_BILL,
	SM_PAYOUT_CARD,
	SM_CALLBACK_CARD
};

static void SM_idle();
static void SM_idle();
static void SM_idle();
static void SM_idle();
static void SM_idle();
static void SM_idle();

bool STATEMACHINE_init(){

}

bool STATEMACHINE_run(){
	STATUSREPORTER_run();
	COMMANDHANDLER_run();
	BILLACCEPTORMNG_run();
	KEYPADMNG_run();
	TCDMNG_run();

}
