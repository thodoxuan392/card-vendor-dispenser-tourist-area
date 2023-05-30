/*
 * bill_acceptor.c
 *
 *  Created on: May 14, 2023
 *      Author: xuanthodo
 */

#include "main.h"
#include "Device/bill_acceptor.h"
#include "Hal/timer.h"
#include "Hal/uart.h"

#define BILLACCEPTOR_UART	UART_2
#define BILLACCEPTOR_RES_TIMEOUT	100  	// 100ms

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


static void BILLACCEPTOR_on_1ms_interrupt();
static bool BILLACCEPTOR_receive_response(uint8_t *data, size_t data_len);

bool BILLACCEPTOR_init(){
	TIMER_attach_intr_1ms(BILLACCEPTOR_on_1ms_interrupt);
}

bool BILLACCEPTOR_reset(){
	uint8_t cmd[1] = {BILLACCEPTOR_RESET};
	UART_send(BILLACCEPTOR_UART, cmd, 1);
}

bool BILLACCEPTOR_setup(BILLACCEPTOR_Setup_t *setup){
	// Send command
	uint8_t cmd[1];
	cmd[0] = BILLACCEPTOR_SETUP; // Code
	UART_send(BILLACCEPTOR_UART, cmd, 1);
	// Wait for get response
	uint8_t res[27];
	if(!BILLACCEPTOR_receive_response(res, 27)){
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
	uint8_t cmd[1];
	cmd[0] = BILLACCEPTOR_SECURITY; // Code
	UART_send(BILLACCEPTOR_UART, cmd, 1);
	// Wait for get response
	uint8_t res[2];
	if(!BILLACCEPTOR_receive_response(res, 2)){
		return false;
	}
	security->bill_type = res[0];
	security->bill_type |= (uint16_t)res[1] << 8;
}

bool BILLACCEPTOR_poll(BILLACCEPTOR_Poll_t * poll){

	// Send command
	uint8_t cmd[1];
	cmd[0] = BILLACCEPTOR_SECURITY; // Code
	UART_send(BILLACCEPTOR_UART, cmd, 1);
	// Wait for get response
	uint8_t res;
	if(!BILLACCEPTOR_receive_response(res, 1)){
		return false;
	}
	if((res >> 7)){
		// BillAccptec Type
		poll->BillAccepted.bill_routing = (res >> 4) & 0x07;
		poll->BillAccepted.bill_type = res & 0x0F;
		poll->type = IS_BILLACCEPTED;
	}else{
		// Status Type
		poll->Status.status = res;
		poll->type = IS_STATUS;
	}
	return true;
}

bool BILLACCEPTOR_billtype(BILLACCEPTOR_BillType_t * billtype){

}

bool BILLACCEPTOR_escrow(BILLACCEPTOR_Escrow_t * escrow){

}

bool BILLACCEPTOR_stacker(BILLACCEPTOR_Stacker_t * stacker){

}

bool BILLACCEPTOR_expansion_cmd(){

}



bool BILLACCEPTOR_test(){
	UART_send(BILLACCEPTOR_UART, "Hello", 5);
}

static void BILLACCEPTOR_on_1ms_interrupt(){
	if(billacceptor_timecnt > 0){
		billacceptor_timecnt--;
		if(billacceptor_timecnt == 0){
			billacceptor_timeout_occur = true;
		}
	}
}

static bool BILLACCEPTOR_receive_response(uint8_t *data, size_t data_len){
	size_t res_len = 0;
	billacceptor_timecnt = BILLACCEPTOR_RES_TIMEOUT;
	while(1){
		if(billacceptor_timeout_occur){
			return false;
		}
		if(UART_receive_available(BILLACCEPTOR_UART)){
			data[res_len++] = UART_receive_byte(BILLACCEPTOR_UART);
			if(res_len == data_len){
				billacceptor_timecnt = 0;
				return true;
			}
		}
	}
}
