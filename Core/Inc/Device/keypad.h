/*
 * keypad.h
 *
 *  Created on: May 13, 2023
 *      Author: xuanthodo
 */

#ifndef INC_DEVICE_KEYPAD_H_
#define INC_DEVICE_KEYPAD_H_

#include "stdio.h"
#include "stdbool.h"

typedef enum {
	KEY_0 = 4,
	KEY_1 = 3,
	KEY_2 = 7,
	KEY_3 = 11,
	KEY_4 = 2,
	KEY_5 = 6,
	KEY_6 = 10,
	KEY_7 = 1,
	KEY_8 = 5,
	KEY_9 = 9,
	KEY_STAR = 0,
	KEY_SHARP = 8,
}KEY_t;

bool KEYPAD_init();
uint8_t KEYPAD_is_pressed();

#endif /* INC_DEVICE_KEYPAD_H_ */
