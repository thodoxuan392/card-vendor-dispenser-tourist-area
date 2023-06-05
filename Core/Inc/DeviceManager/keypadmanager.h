/*
 * keypadmanager.h
 *
 *  Created on: Jun 3, 2023
 *      Author: xuanthodo
 */

#ifndef INC_DEVICEMANAGER_KEYPADMANAGER_H_
#define INC_DEVICEMANAGER_KEYPADMANAGER_H_

#include "stdio.h"
#include "stdbool.h"

#define KEYPAD_BUF_SIZE		64

void KEYPADMNG_init();
void KEYPADMNG_run();
bool KEYPADMNG_is_entered();
bool KEYPADMNG_is_cancelled();
void KEYPADMNG_get_data(uint8_t * data, size_t * data_len);
void KEYPADMNG_clear_data();



#endif /* INC_DEVICEMANAGER_KEYPADMANAGER_H_ */
