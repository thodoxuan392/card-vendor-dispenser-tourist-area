/*
 * keypad.c
 *
 *  Created on: May 14, 2023
 *      Author: xuanthodo
 */


#include "main.h"
#include "Device/keypad.h"
#include "Hal/gpio.h"

enum {
	KEYPAD_R1,
	KEYPAD_R2,
	KEYPAD_R3,
	KEYPAD_R4,
	KEYPAD_C1,
	KEYPAD_C2,
	KEYPAD_C3,
	KEYPAD_LEDP,	// LED+
};

static GPIO_info_t gpio_table[] = {
		[KEYPAD_R1] = 		{GPIOD, { GPIO_PIN_0, GPIO_MODE_INPUT, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH}},
		[KEYPAD_R2] = 		{GPIOD,{ GPIO_PIN_1, GPIO_MODE_INPUT, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH }},
		[KEYPAD_R3] = 		{GPIOD,{ GPIO_PIN_2, GPIO_MODE_INPUT, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH }},
		[KEYPAD_R4] = 		{GPIOD,{ GPIO_PIN_3, GPIO_MODE_INPUT, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH }},
		[KEYPAD_C1] = 		{GPIOD,{ GPIO_PIN_4, GPIO_MODE_INPUT, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH }},
		[KEYPAD_C2] = 		{GPIOD,{ GPIO_PIN_5, GPIO_MODE_INPUT, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH }},
		[KEYPAD_C3] = 		{GPIOD,{ GPIO_PIN_6, GPIO_MODE_INPUT, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH }},
		[KEYPAD_LEDP] = 	{GPIOD,{ GPIO_PIN_7, GPIO_MODE_INPUT, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH }}
};

// Internal function
static uint8_t KEYPAD_calculate_key(uint8_t row, uint8_t col);

bool KEYPAD_init(){
	// Init GPIO
	int nb_io = sizeof(gpio_table)/sizeof(GPIO_info_t);
	for (uint8_t var = 0; var < nb_io; ++var) {
		HAL_GPIO_Init(gpio_table[var].port, &gpio_table[var].init_info);
	}
	return true;
}

uint8_t KEYPAD_is_pressed(){
	// Check row
	uint8_t row_pressed = 0xFF;
	for (uint8_t var = KEYPAD_R1; var <= KEYPAD_R4; ++var) {
		if(HAL_GPIO_ReadPin(gpio_table[var].port, gpio_table[var].init_info.Pin)){
			row_pressed = var;
			break;
		}
	}
	// Check column
	uint8_t column_pressed = 0xFF;
	for (uint8_t var = KEYPAD_C1; var <= KEYPAD_C3; ++var) {
		if(HAL_GPIO_ReadPin(gpio_table[var].port, gpio_table[var].init_info.Pin)){
			column_pressed = var;
			break;
		}
	}
	if(row_pressed != 0xFF && column_pressed != 0xFF){
		// Calculate Key
		return KEYPAD_calculate_key(row_pressed, column_pressed);
	}
	return 0xFF;
}

static uint8_t KEYPAD_calculate_key(uint8_t row, uint8_t col){
	return (col - KEYPAD_C1) * 4 + (row - KEYPAD_R1);
}

