/*
 * lcd.h
 *
 *  Created on: May 13, 2023
 *      Author: xuanthodo
 */

#ifndef INC_DEVICE_LCD_H_
#define INC_DEVICE_LCD_H_

#include "stdio.h"
#include "stdbool.h"

bool LCD_init();
bool LCD_clear();
bool LCD_display_char(uint8_t row, uint8_t col, char character);
bool LCD_display_str(uint8_t row, uint8_t col, char * character);


#endif /* INC_DEVICE_LCD_H_ */
