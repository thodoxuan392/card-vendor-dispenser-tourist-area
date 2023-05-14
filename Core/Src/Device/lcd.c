/*
 * lcd.c
 *
 *  Created on: May 14, 2023
 *      Author: xuanthodo
 */


#include "main.h"
#include "Device/lcd.h"
#include "Hal/gpio.h"

enum {
	LCD_RS,
	LCD_RW,
	LCD_ENABLE,
	LCD_D0,
	LCD_D1,
	LCD_D2,
	LCD_D3,
	LCD_D4,
	LCD_D5,
	LCD_D6,
	LCD_D7
};

static GPIO_info_t gpio_table[] = {
		[LCD_RS] = 		{GPIOB, { GPIO_PIN_13, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP, GPIO_SPEED_FREQ_HIGH}},
		[LCD_RW] = 		{GPIOB,{ GPIO_PIN_14, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP, GPIO_SPEED_FREQ_HIGH }},
		[LCD_ENABLE] = 	{GPIOB,{ GPIO_PIN_15, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP, GPIO_SPEED_FREQ_HIGH }},
		[LCD_D0] = 		{GPIOD,{ GPIO_PIN_8, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP, GPIO_SPEED_FREQ_HIGH }},
		[LCD_D1] = 		{GPIOD,{ GPIO_PIN_9, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP, GPIO_SPEED_FREQ_HIGH }},
		[LCD_D2] = 		{GPIOD,{ GPIO_PIN_10, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP, GPIO_SPEED_FREQ_HIGH }},
		[LCD_D3] = 		{GPIOD,{ GPIO_PIN_11, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP, GPIO_SPEED_FREQ_HIGH }},
		[LCD_D4] = 		{GPIOD,{ GPIO_PIN_12, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP, GPIO_SPEED_FREQ_HIGH }},
		[LCD_D5] = 		{GPIOD,{ GPIO_PIN_13, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP, GPIO_SPEED_FREQ_HIGH }},
		[LCD_D6] = 		{GPIOD,{ GPIO_PIN_14, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP, GPIO_SPEED_FREQ_HIGH }},
		[LCD_D7] = 		{GPIOD,{ GPIO_PIN_15, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP, GPIO_SPEED_FREQ_HIGH }},
};


bool LCD_init(){
	// Init GPIO
	int nb_io = sizeof(gpio_table)/sizeof(GPIO_info_t);
	for (uint8_t var = 0; var < nb_io; ++var) {
		HAL_GPIO_Init(gpio_table[var].port, &gpio_table[var].init_info);
	}
	return true;
}
bool LCD_clear(){
	// TODO:
}
bool LCD_display_char(uint8_t row, uint8_t col, char character){
	// TODO:
}
bool LCD_display_str(uint8_t row, uint8_t col, char * character){
	// TODO:
}
