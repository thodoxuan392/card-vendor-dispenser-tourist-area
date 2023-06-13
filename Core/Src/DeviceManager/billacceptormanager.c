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
 * Bill Type
 */
enum{
	BILLTYPE_2K = 1,
	BILLTYPE_5K,
	BILLTYPE_10K,
	BILLTYPE_20K,
	BILLTYPE_50K,
	BILLTYPE_100K,
	BILLTYPE_200K
};

enum {
	BILLACCEPTORMNG_IDLE,
	BILLACCEPTORMNG_HAVE_BILL,
	BILLACCEPTORMNG_STATUS
};

// Bill Info
static uint8_t billacceptormng_state = BILLACCEPTORMNG_IDLE;
static uint8_t prev_billacceptor_status = STATUS_SUCCESS;
static uint8_t billacceptor_status = STATUS_SUCCESS;
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
	[BILLTYPE_2K] = 2000,
	[BILLTYPE_5K] = 5000,
	[BILLTYPE_10K] = 10000,
	[BILLTYPE_20K] = 20000,
	[BILLTYPE_50K] = 50000,
	[BILLTYPE_100K] = 100000,
	[BILLTYPE_200K] = 200000,
};

// Bill Status name
static const char * bill_status_name[] = {
		[STATUS_SUCCESS] = "STATUS_SUCCESS\r\n",
		[STATUS_DEFECTIVE_MOTOR] = "STATUS_DEFECTIVE_MOTOR\r\n",
		[STATUS_SENSOR_PROBLEM] = "STATUS_SENSOR_PROBLEM\r\n",
		[STATUS_VALIDATOR_BUSY] = "STATUS_VALIDATOR_BUSY\r\n",
		[STATUS_ROM_CHECKSUM_ERROR] = "STATUS_ROM_CHECKSUM_ERROR\r\n",
		[STATUS_VALIDATOR_JAMMED] = "STATUS_VALIDATOR_JAMMED\r\n",
		[STATUS_VALIDATOR_WAS_RESET] = "STATUS_VALIDATOR_WAS_RESET\r\n",
		[STATUS_BILL_REMOVED] = "STATUS_BILL_REMOVED\r\n",
		[STATUS_CASHBOX_OUTOF_POSITION] = "STATUS_CASHBOX_OUTOF_POSITION\r\n",
		[STATUS_VALIDATOR_DISABLE] = "STATUS_VALIDATOR_DISABLE\r\n",
		[STATUS_INVALID_ESCROW_REQ] = "STATUS_INVALID_ESCROW_REQ\r\n",
		[STATUS_BILL_REJECTED] = "STATUS_BILL_REJECTED\r\n",
		[STATUS_POSSIBLE_CREDITED_BILL_REMOVAL] = "STATUS_POSSIBLE_CREDITED_BILL_REMOVAL\r\n",
		[STATUS_NB_ATTEMP_INPUT_BILL] = "STATUS_NB_ATTEMP_INPUT_BILL\r\n",
};

static void BILLACCEPTORMNG_status_printf(uint8_t bill_status);

static bool timeout = true;

// Private function
static void BILLACCEPTORMNG_idle();
static void BILLACCEPTORMNG_have_bill();
static void BILLACCEPTORMNG_status();
static void BILLACCEPTORMNG_timeout();
static void BILLACCEPTOR_save_amount_to_eeprom(uint32_t amount);

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
		case BILLACCEPTORMNG_HAVE_BILL:
			BILLACCEPTORMNG_have_bill();
			break;
		case BILLACCEPTORMNG_STATUS:
			BILLACCEPTORMNG_status();
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
	return (billacceptor_status != STATUS_SUCCESS);
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
	BILLACCEPTOR_save_amount_to_eeprom(amount);
}

void BILLACCEPTORMNG_test(){

}



// Private function
static void BILLACCEPTORMNG_idle(){
	if(timeout){
		timeout = false;
		// Call Polling BillAcceptor
		memset(&poll, 0xFF, sizeof(BILLACCEPTOR_Poll_t));
		BILLACCEPTOR_poll(&poll);
		switch (poll.type) {
			case IS_BILLACCEPTED:
				bill_type_accepted = poll.BillAccepted.bill_type;
				bill_routing = poll.BillAccepted.bill_routing;
				billacceptormng_state = BILLACCEPTORMNG_HAVE_BILL;
				break;
			case IS_STATUS:
				billacceptor_status = poll.Status.status;
				billacceptormng_state = BILLACCEPTORMNG_STATUS;
				break;
			default:
				break;
		}
		SCH_Add_Task(BILLACCEPTORMNG_timeout, POLL_INTERVAL, 0);
	}

}

static void BILLACCEPTORMNG_have_bill(){
	switch (bill_routing) {
		case BILL_STACKED:
			is_accepted = true;
			// Calculate Bill Value and add it to amount
			amount += bill_mapping[bill_type_accepted];
			// Save it to EEPROM
			BILLACCEPTOR_save_amount_to_eeprom(amount);
			// LCD display Bill detected and Bill value
			utils_log_info("Bill %d accepted\r\n", bill_mapping[bill_type_accepted]);
			utils_log_info("Amount %d\r\n", amount);
			break;
		case BILL_ESCROW_POSITION:
			utils_log_info("Billacceptor escrow\r\n");
			BILLACCEPTOR_escrow(&escrow);
			break;
		case BILL_RETURNED:
			utils_log_info("Bill returned\r\n");
			break;
		case BILL_TO_RECYCLER:
			utils_log_info("Bill to recycler\r\n");
			break;
		default:
			break;
	}
	billacceptormng_state = BILLACCEPTORMNG_IDLE;
}


static void BILLACCEPTORMNG_status(){
	if(prev_billacceptor_status != billacceptor_status){
		BILLACCEPTORMNG_status_printf(billacceptor_status);
	}
	prev_billacceptor_status = billacceptor_status;
	billacceptormng_state = BILLACCEPTORMNG_IDLE;
}


static void BILLACCEPTORMNG_timeout(){
	timeout = true;
}

static void BILLACCEPTOR_save_amount_to_eeprom(uint32_t _amount){
	CONFIG_t * config = CONFIG_get();
	config->amount = _amount;
	CONFIG_set(config);
}


static void BILLACCEPTORMNG_status_printf(uint8_t bill_status){
	if(bill_status == STATUS_SUCCESS){
		utils_log_info(bill_status_name[bill_status]);
	}else{
		utils_log_error(bill_status_name[bill_status]);
	}
}

