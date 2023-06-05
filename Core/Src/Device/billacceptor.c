/*
 * bill_acceptor.c
 *
 *  Created on: May 14, 2023
 *      Author: xuanthodo
 */

#include <Device/billacceptor.h>
#include "main.h"
#include "Hal/timer.h"
#include "Hal/uart.h"

#define BILLACCEPTOR_UART	UART_2
#define BILLACCEPTOR_RES_TIMEOUT		200  	// 100ms
#define BILLACCEPTOR_VALIDATOR_MODE		0x30
#define BILLACCEPTOR_ADDRESS_BIT	0x100
#define BILLACCEPTOR_DATA_BIT		0x000
#define BILLACCEPTOR_ACK_BYTE		0x00
#define BILLACCEPTOR_RET_BYTE		0xAA
#define BILLACCEPTOR_NACK_BYTE 		0xFF

typedef enum {
	BILLACCEPTOR_RESET = 0x30,
	BILLACCEPTOR_SETUP = 0x31,
	BILLACCEPTOR_SECURITY = 0x32,
	BILLACCEPTOR_POLL = 0x33,
	BILLACCEPTOR_BILLTYPE = 0x34,
	BILLACCEPTOR_ESCROW = 0x35,
	BILLACCEPTOR_STACKER = 0x36,
	BILLACCEPTOR_EXPANSION_CMD = 0x37
}BILLACCEPTOR_Cmd_Code_t;

static uint32_t billacceptor_timecnt = 0;
static bool billacceptor_timeout_occur = false;
static uint16_t tx_buf[64];


static void BILLACCEPTOR_on_1ms_interrupt();
static bool BILLACCEPTOR_send_data(uint16_t *data , size_t data_len);
static uint8_t BILLACCEPTOR_calculate_chk(uint16_t * data, size_t data_len);
static bool BILLACCEPTOR_receive_response(uint16_t *data, size_t data_len);
static bool BILLACCEPTOR_receive_response_timeout(uint16_t *data, size_t expected_len, size_t * real_len, uint16_t timeout);
static bool BILLACCEPTOR_is_res_ack(uint16_t code);

bool BILLACCEPTOR_init(){
	TIMER_attach_intr_1ms(BILLACCEPTOR_on_1ms_interrupt);
}

bool BILLACCEPTOR_reset(){
	uint8_t cmd[1] = {BILLACCEPTOR_RESET};
	BILLACCEPTOR_send_data(cmd, 1);
	uint16_t code;
	if(!BILLACCEPTOR_receive_response(&code, 1)){
		return false;
	}
	if(!BILLACCEPTOR_is_res_ack(code)){
		return false;
	}
	return true;
}

bool BILLACCEPTOR_setup(BILLACCEPTOR_Setup_t *setup){
	// Send command
	uint16_t cmd[1] = { BILLACCEPTOR_SETUP };
	BILLACCEPTOR_send_data(cmd, 1);
	// Wait for get response
	uint16_t res[28];
	size_t res_size = sizeof(res)/sizeof(uint16_t);
	if(!BILLACCEPTOR_receive_response(res, res_size)){
		return false;
	}
	// Validate checksum
	if(BILLACCEPTOR_calculate_chk(res, res_size-1) != (uint8_t)res[res_size-1]){
		return false;
	}
	setup->feature_level = res[0];
	setup->currency_code[0] = res[1];
	setup->currency_code[1] = res[2];
	setup->scaling_factor[0] = res[3];
	setup->scaling_factor[1] = res[4];
	setup->decimal_place = res[5];
	setup->stacker_capacity[0] = res[6];
	setup->stacker_capacity[1] = res[7];
	setup->security_level[0] = res[8];
	setup->security_level[1] = res[9];
	setup->escrow = res[10];
	memcpy(setup->type_credit, &res[11], 16);
	return true;
}

bool BILLACCEPTOR_security(BILLACCEPTOR_Security_t * security){
	// Send command
	uint16_t cmd[3];
	cmd[0] = BILLACCEPTOR_SECURITY;
	cmd[1] = security->bill_type >> 8;
	cmd[2] = security->bill_type & 0xFF;
	BILLACCEPTOR_send_data(cmd, 3);
	// Wait for get response
	uint16_t code;
	if(!BILLACCEPTOR_receive_response(&code, 1)){
		return false;
	}
	if(!BILLACCEPTOR_is_res_ack(code)){
		return false;
	}
	return true;
}

bool BILLACCEPTOR_poll(BILLACCEPTOR_Poll_t * poll){
	// Send command
	uint16_t cmd[1] = {BILLACCEPTOR_POLL};
	BILLACCEPTOR_send_data(cmd, 1);
	// Wait for get response
	uint16_t res[2];
	size_t expected_res_size = sizeof(res)/sizeof(uint16_t);
	size_t real_res_size;
	if(!BILLACCEPTOR_receive_response_timeout(res, expected_res_size, &real_res_size, 100)){
		return false;
	}
	// Validate checksum
	if(BILLACCEPTOR_calculate_chk(res, real_res_size-1) != (uint8_t)res[real_res_size-1]){
		return false;
	}
	if((res[0] >> 7)){
		// BillAccptec Type
		poll->BillAccepted.bill_routing = (res[0] >> 4) & 0x07;
		poll->BillAccepted.bill_type = res[0] & 0x0F;
		poll->type = IS_BILLACCEPTED;
	}else{
		// Status Type
		poll->Status.status = res[0];
		poll->type = IS_STATUS;
	}
	return true;
}

bool BILLACCEPTOR_billtype(BILLACCEPTOR_BillType_t * billtype){
	// Send command
	uint16_t cmd[5];
	cmd[0] = BILLACCEPTOR_BILLTYPE;
	cmd[1] = billtype->bill_enable >> 8;
	cmd[2] = billtype->bill_enable & 0xFF;
	cmd[3] = billtype->bill_escrow_enable >> 8;
	cmd[4] = billtype->bill_escrow_enable & 0xFF;
	BILLACCEPTOR_send_data(cmd, 5);
	// Wait for get response
	uint16_t code;
	if(!BILLACCEPTOR_receive_response(&code, 1)){
		return false;
	}
	if(!BILLACCEPTOR_is_res_ack(code)){
		return false;
	}
	return true;
}

bool BILLACCEPTOR_escrow(BILLACCEPTOR_Escrow_t * escrow){
	// Send command
	uint16_t cmd[2];
	cmd[0] = BILLACCEPTOR_ESCROW;
	cmd[1] = escrow->escrow_status;
	BILLACCEPTOR_send_data(cmd, 2);
	// Wait for get response
	uint16_t res[2];
	size_t res_size = sizeof(res)/sizeof(uint16_t);
	if(!BILLACCEPTOR_receive_response(&res, 2)){
		return false;
	}
	// Validate checksum
	if(BILLACCEPTOR_calculate_chk(res, res_size-1) != (uint8_t)res[res_size-1]){
		return false;
	}
	escrow->poll_status = res[0];
	return true;
}

bool BILLACCEPTOR_stacker(BILLACCEPTOR_Stacker_t * stacker){
	// Send command
	uint16_t cmd[1] = {BILLACCEPTOR_STACKER};
	BILLACCEPTOR_send_data(cmd, 1);
	// Wait for get response
	uint16_t res[3];
	size_t res_size = sizeof(res)/sizeof(uint16_t);
	if(!BILLACCEPTOR_receive_response(res, res_size)){
		return false;
	}
	// Validate checksum
	if(BILLACCEPTOR_calculate_chk(res, res_size-1) != (uint8_t)res[res_size-1]){
		return false;
	}
	if((res[0] >> 7)){
		// Stacker is full
		stacker->is_full = 0;
		stacker->number_of_bills = (uint16_t)(res[0] & 0x7F) << 8 | (res[0] & 0xFF);
	}else{
		// Status Type
		stacker->is_full = 0;
	}
	return true;
}

bool BILLACCEPTOR_expansion_cmd(){

}



bool BILLACCEPTOR_test(){
//	BILLACCEPTOR_send_data("Hello", 5);
	BILLACCEPTOR_reset();
	BILLACCEPTOR_BillType_t bill_type = {
				.bill_enable = 0xFFFF,
				.bill_escrow_enable = 0xFFFF
	};
	BILLACCEPTOR_billtype(&bill_type);
	BILLACCEPTOR_Escrow_t escrow = {
			.escrow_status = 0x01
	};
	BILLACCEPTOR_escrow(&escrow);
//	BILLACCEPTOR_Setup_t setup;
//	BILLACCEPTOR_setup(&setup);
//	BILLACCEPTOR_Poll_t poll;
//	BILLACCEPTOR_poll(&poll);
//	BILLACCEPTOR_Security_t security;
//	BILLACCEPTOR_security(&security);
//	BILLACCEPTOR_Stacker_t stacker;
//	BILLACCEPTOR_stacker(&stacker);
}

bool BILLACCEPTOR_test_2(){
	BILLACCEPTOR_Poll_t poll;
	BILLACCEPTOR_poll(&poll);
}

static void BILLACCEPTOR_on_1ms_interrupt(){
	if(billacceptor_timecnt > 0){
		billacceptor_timecnt--;
		if(billacceptor_timecnt == 0){
			billacceptor_timeout_occur = true;
		}
	}
}


static bool BILLACCEPTOR_receive_response_timeout(uint16_t *data, size_t expected_len, size_t * real_len, uint16_t timeout){
	size_t res_len = 0;
	billacceptor_timecnt = timeout;
	billacceptor_timeout_occur = false;
	while(1){
		if(billacceptor_timeout_occur){
			break;
		}
		if(UART_receive_available(BILLACCEPTOR_UART)){
			data[res_len++] = UART_receive_data(BILLACCEPTOR_UART);
			if(res_len == expected_len){
				break;
			}
		}
	}
	*real_len = res_len;
	UART_clear_buffer(BILLACCEPTOR_UART);
	return true;
}

static bool BILLACCEPTOR_receive_response(uint16_t *data, size_t data_len){
	bool success = false;
	size_t res_len = 0;
	billacceptor_timecnt = BILLACCEPTOR_RES_TIMEOUT;
	billacceptor_timeout_occur = false;
	while(1){
		if(billacceptor_timeout_occur){
			break;
		}
		if(UART_receive_available(BILLACCEPTOR_UART)){
			data[res_len++] = UART_receive_data(BILLACCEPTOR_UART);
			if(res_len == data_len){
				success = true;
				break;
			}
		}
	}
	UART_clear_buffer(BILLACCEPTOR_UART);
	return success;
}

static bool BILLACCEPTOR_is_res_ack(uint16_t code){
	if(BILLACCEPTOR_ACK_BYTE == (uint8_t)code){
		return true;
	}
	return false;
}

static bool BILLACCEPTOR_send_data(uint16_t *data , size_t data_len){
	size_t tx_len = 0;
	// Command
	tx_buf[tx_len++] = data[0] | BILLACCEPTOR_ADDRESS_BIT;
	// Data
	for (int var = 1; var < data_len; ++var) {
		tx_buf[tx_len++] = data[var] | BILLACCEPTOR_DATA_BIT;
	}
	// Chk
	tx_buf[tx_len] = BILLACCEPTOR_calculate_chk(tx_buf, tx_len);
	tx_len++;
	UART_send(BILLACCEPTOR_UART, tx_buf, tx_len);
}

static uint8_t BILLACCEPTOR_calculate_chk(uint16_t * data, size_t data_len){
	uint16_t chk = 0;
	for (int var = 0; var < data_len; ++var) {
		chk = chk + data[var];
	}
	return (uint8_t)chk;
}
