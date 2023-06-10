/*
 * config.h
 *
 *  Created on: Jun 2, 2023
 *      Author: xuanthodo
 */

#ifndef INC_APP_CONFIG_H_
#define INC_APP_CONFIG_H_

#include "stdio.h"
#include "stdbool.h"

#define	CFG_BILLACCEPTOR_RESPONSE_TIMEOUT_DFG	100
#define CFG_STATUS

typedef struct {
	char device_id[20];
	uint32_t card_price;
	uint32_t amount;
}CONFIG_t;

bool CONFIG_init();
CONFIG_t * CONFIG_get();
void CONFIG_set(CONFIG_t *);
void CONFIG_clear();
#endif /* INC_APP_CONFIG_H_ */
