/*
 * tcd_manager.h
 *
 *  Created on: Jun 2, 2023
 *      Author: xuanthodo
 */

#ifndef INC_DEVICEMANAGER_TCDMANAGER_H_
#define INC_DEVICEMANAGER_TCDMANAGER_H_

#include "stdio.h"
#include "stdbool.h"
#include "string.h"

typedef struct {
	struct {
		bool is_error;
		bool is_lower;
		bool is_empty;
	}TCD_1;

	struct {
		bool is_error;
		bool is_lower;
		bool is_empty;
	}TCD_2;
}TCDMNG_Status_t;

void TCDMNG_init();
void TCDMNG_run();
TCDMNG_Status_t TCDMNG_get_status();
uint8_t TCDMNG_get_state();
bool TCDMNG_is_idle();
bool TCDMNG_is_in_processing();
void TCDMNG_reset();
void TCDMNG_payout();
void TCDMNG_callback();
bool TCDMNG_is_error();
bool TCDMNG_is_lower();
bool TCDMNG_is_empty();
bool TCDMNG_is_available_for_use();



#endif /* INC_DEVICEMANAGER_TCDMANAGER_H_ */
