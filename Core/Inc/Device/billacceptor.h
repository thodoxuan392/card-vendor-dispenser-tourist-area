/*
 * money_scanner.h
 *
 *  Created on: May 10, 2023
 *      Author: xuanthodo
 */

#ifndef INC_DEVICE_BILLACCEPTOR_H_
#define INC_DEVICE_BILLACCEPTOR_H_

#include "stdio.h"
#include "stdbool.h"

typedef struct {
	uint8_t feature_level;
	uint8_t currency_code[2];
	uint8_t scaling_factor[2];
	uint8_t decimal_place;
	uint8_t stacker_capacity[2];
	uint8_t security_level[2];
	uint8_t escrow;
	uint8_t type_credit[16];
}BILLACCEPTOR_Setup_t;

typedef struct{
	uint16_t bill_type;
}BILLACCEPTOR_Security_t;


typedef enum {
	IS_BILLACCEPTED,
	IS_STATUS
}BILLACCEPTOR_PollType_t;

typedef union {
	struct{
		uint8_t bill_routing;
		uint8_t bill_type;
	}BillAccepted;
	struct{
		uint8_t status;
	}Status;
	BILLACCEPTOR_PollType_t type;
}BILLACCEPTOR_Poll_t;

typedef struct {
	uint16_t bill_enable;
	uint16_t bill_escrow_enable;
}BILLACCEPTOR_BillType_t;

typedef struct {
	uint8_t escrow_status;
	uint8_t poll_status;
}BILLACCEPTOR_Escrow_t;

typedef struct {
	uint8_t is_full;
	uint16_t number_of_bills;
}BILLACCEPTOR_Stacker_t;

bool BILLACCEPTOR_init();
bool BILLACCEPTOR_reset();
bool BILLACCEPTOR_setup(BILLACCEPTOR_Setup_t *setup);
bool BILLACCEPTOR_security(BILLACCEPTOR_Security_t * security);
bool BILLACCEPTOR_poll(BILLACCEPTOR_Poll_t * poll);
bool BILLACCEPTOR_billtype(BILLACCEPTOR_BillType_t * billtype);
bool BILLACCEPTOR_escrow(BILLACCEPTOR_Escrow_t * escrow);
bool BILLACCEPTOR_stacker(BILLACCEPTOR_Stacker_t * stacker);
bool BILLACCEPTOR_expansion_cmd();
// For test IO
bool BILLACCEPTOR_test();

#endif /* INC_DEVICE_BILLACCEPTOR_H_ */
