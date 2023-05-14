/*
 * money_scanner.h
 *
 *  Created on: May 10, 2023
 *      Author: xuanthodo
 */

#ifndef INC_DEVICE_BILL_ACCEPTOR_H_
#define INC_DEVICE_BILL_ACCEPTOR_H_

#include "stdio.h"
#include "stdbool.h"

typedef enum {
	BILL_ERR
}BILL_error_t;

typedef enum {
	BILL_1K,
	BILL_2K,
    BILL_5K,
	BILL_10K,
	BILL_20K,
	BILL_50K,
	BILL_100K,
	BILL_200K,
	BILL_500K
}BILL_t;

typedef struct {
    // Serial number
	BILL_t bill;
}BILL_info_t;

bool BILLACCEPTOR_init();
bool BILLACCEPTOR_loop();
bool BILLACCEPTOR_is_detected();
bool BILLACCEPTOR_get();


#endif /* INC_DEVICE_BILL_ACCEPTOR_H_ */
