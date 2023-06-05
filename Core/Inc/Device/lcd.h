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
void LCD_clear_screen();
void LCD_display_str(char * fmt, ...);
bool LCD_test();

#endif /* INC_DEVICE_LCD_H_ */
