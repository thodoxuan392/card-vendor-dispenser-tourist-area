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
void LCD_clear();
void LCD_home();
void LCD_display();
void LCD_no_display();
void LCD_blink();
void LCD_no_blink();
void LCD_set_cursor(uint8_t x, uint8_t y);
void LCD_display_str(char * character);
// For test IO
bool LCD_test();

#endif /* INC_DEVICE_LCD_H_ */
