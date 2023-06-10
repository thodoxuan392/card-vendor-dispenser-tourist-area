/*
 * billacceptormanager.c
 *
 *  Created on: Jun 2, 2023
 *      Author: xuanthodo
 */
#include "main.h"
#include "config.h"
#include "DeviceManager/billacceptormanager.h"
#include "Device/billacceptor.h"
#include "Device/eeprom.h"
#include "Device/lcd.h"

#include "Lib/utils/utils_logger.h"
#define EEPROM_AMOUNT_ADDRESS		0x01
#define POLL_INTERVAL				200	//200ms

/**
 * BULLROUTE
 */
enum {
	BILLROUTE_BILL_STACKED = 0x0,
	BILLROUTE_ESCROW_POSITION = 0x1,
	BILLROUTE_BILL_RETURNED = 0x2,
	BILLROUTE_BILL_TO_RECYCLER = 0x3,
	BILLROUTE_DISABLE_BILL_REJECTED = 0x4,
	BILLROUTE_MANUAL_FILL = 0x5,
	BILLROUTE_MANUAL_DISPENSE = 0x6,
	BILLROUTE_TRANSFERED_FROM_RECYCLER_TO_CASHBOX = 0x7
};

/**
 * Bill Type
 */
enum{
	BILLTYPE_1K = 1,
	BILLTYPE_2K,
	BILLTYPE_5K,
	BILLTYPE_10K,
	BILLTYPE_20K,
	BILLTYPE_50K,
	BILLTYPE_100K,
	BILLTYPE_200K
};

enum {
	BILLACCEPTORMNG_IDLE,
	BILLACCEPTORMNG_SETTING_UP,
	BILLACCEPTORMNG_HAVE_BILL,
	BILLACCEPTORMNG_ERROR
};

// Bill Info
static uint8_t billacceptormng_state = BILLACCEPTORMNG_SETTING_UP;
static uint8_t billacceptor_status = STATUS_NONE;
static bool is_accepted = false;
static uint8_t bill_routing;
static uint8_t bill_type_accepted;
static uint32_t amount = 0;
static BILLACCEPTOR_BillType_t billtype = {
	.bill_enable = 0b0000000111111110,
	.bill_escrow_enable = 0b0000000111111110
};
static BILLACCEPTOR_Escrow_t escrow = {
	.escrow_status = 0x01
};

static BILLACCEPTOR_Security_t security = {
	.bill_type = 0x0000
};

static BILLACCEPTOR_Poll_t poll;

// BillType Mapping
static uint32_t bill_mapping[] = {
	[BILLTYPE_1K] = 1000,
	[BILLTYPE_2K] = 2000,
	[BILLTYPE_5K] = 5000,
	[BILLTYPE_10K] = 10000,
	[BILLTYPE_20K] = 20000,
	[BILLTYPE_50K] = 50000,
	[BILLTYPE_100K] = 100000,
	[BILLTYPE_200K] = 200000,
};

static bool timeout = true;

// Private function
static void BILLACCEPTORMNG_idle();
static void BILLACCEPTORMNG_setting_up();
static void BILLACCEPTORMNG_have_bill();
static void BILLACCEPTORMNG_error();
static void BILLACCEPTOR_handle_accepted();
static void BILLACCEPTORMNG_timeout();

bool BILLACCEPTORMNG_init(){
	CONFIG_t * config = CONFIG_get();
	amount = config->amount;
	BILLACCEPTOR_reset();
	BILLACCEPTOR_Poll_t poll;
	BILLACCEPTOR_poll(&poll);
	BILLACCEPTOR_Setup_t setup;
	BILLACCEPTOR_setup(&setup);
	BILLACCEPTOR_security(&security);
	BILLACCEPTOR_billtype(&billtype);

}

bool BILLACCEPTORMNG_run(){
	switch (billacceptormng_state) {
		case BILLACCEPTORMNG_IDLE:
			BILLACCEPTORMNG_idle();
			break;
		case BILLACCEPTORMNG_SETTING_UP:
			BILLACCEPTORMNG_setting_up();
			break;
		case BILLACCEPTORMNG_HAVE_BILL:
			BILLACCEPTORMNG_have_bill();
			break;
		case BILLACCEPTORMNG_ERROR:
			BILLACCEPTORMNG_error();
			break;
		default:
			break;
	}
}

uint8_t BILLACCEPTORMNG_get_state(){
	return billacceptormng_state;
}

uint8_t BILLACCEPTORMNG_get_status(){
	return billacceptor_status;
}

bool BILLACCEPTORMNG_is_error(){
	return (billacceptor_status != STATUS_NONE);
}

bool BILLACCEPTORMNG_is_accepted(){
	return is_accepted;
}

bool BILLACCEPTORMNG_clear_accepted(){
	is_accepted = false;
}

uint32_t BILLACCEPTORMNG_get_amount(){
	return amount;
}

void BILLACCEPTORMNG_set_amount(uint32_t _amount){
	amount = _amount;
}

void BILLACCEPTORMNG_test(){

}



// Private function
static void BILLACCEPTORMNG_idle(){
	if(timeout){
		timeout = false;
		// Call Polling BillAcceptor
		memset(&poll, 0, sizeof(BILLACCEPTOR_PollType_t));
		BILLACCEPTOR_poll(&poll);
		switch (poll.type) {
			case IS_BILLACCEPTED:
				is_accepted = true;
				utils_log_info("Accepted: Type %x, Routing %x\r\n", poll.BillAccepted.bill_type, poll.BillAccepted.bill_routing);
				bill_type_accepted = poll.BillAccepted.bill_type;
				bill_routing = poll.BillAccepted.bill_routing;
				BILLACCEPTOR_handle_accepted();
				break;
			case IS_STATUS:
				utils_log_info("Status %x\r\n", poll.Status.status);
				billacceptor_status = poll.Status.status;
//				billacceptormng_state = BILLACCEPTORMNG_ERROR;
				break;
			default:
				break;
		}
		SCH_Add_Task(BILLACCEPTORMNG_timeout, POLL_INTERVAL, 0);
	}

}

static void BILLACCEPTORMNG_setting_up(){
	// Load amount from EEPROM
	EEPROM_read(EEPROM_AMOUNT_ADDRESS, (uint8_t*)&amount, sizeof(amount));
	// Call set up BillAcceptor
	// Switch to IDLE
	billacceptormng_state = BILLACCEPTORMNG_IDLE;
}

static void BILLACCEPTORMNG_have_bill(){

}


static void BILLACCEPTORMNG_error(){
	// LCD display waning to indicate that BillAcceptor is not good
	LCD_clear_screen();
	LCD_display_str("Bill Acceptor is error with code: %d", billacceptor_status);
	// Polling status utils it's not stacked more
	// If timeout utils stacking resolve -> Call RESET command and switch to Setting Up
	// Switch to IDLE state
	billacceptormng_state = BILLACCEPTORMNG_IDLE;
}


static void BILLACCEPTOR_handle_accepted(){
	switch (bill_routing) {
		case BILL_STACKED:
//			utils_log_info("Bill is stacked\r\n");
//			BILLACCEPTOR_escrow(&escrow);
//			is_accepted = false;
//			// Calculate Bill Value and add it to amount
//			amount += bill_mapping[bill_type_accepted];
//			// Save it to EEPROM
//			EEPROM_write(EEPROM_AMOUNT_ADDRESS, (uint8_t*)&amount, sizeof(amount));
//			// LCD display Bill detected and Bill value
//			LCD_clear_screen();
//			LCD_display_str("Bill Value %d VND is accepted", bill_mapping[bill_type_accepted]);
			break;
		case BILL_ESCROW_POSITION:
			utils_log_info("Bill escrow position\r\n");
			// Call escrow
			HAL_Delay(1000);
			BILLACCEPTOR_escrow(&escrow);
//			HAL_Delay(1000);
//			memset(&poll, 0, sizeof(BILLACCEPTOR_PollType_t));
//			BILLACCEPTOR_poll(&poll);
			break;
		case BILL_RETURNED:
			utils_log_info("Bill return\r\n");
			break;
		case BILL_TO_RECYCLER:
			utils_log_info("Bill to recycler\r\n");
			break;
		default:
			break;
	}
}

static void BILLACCEPTORMNG_timeout(){
	timeout = true;
}

