/*
 * sim7070_io.c
 *
 *  Created on: Oct 15, 2023
 *      Author: xuanthodo
 */

#include "Hal/gpio.h"

enum {
	SIM7070_PWR,
	SIM7070_NET,
	SIM7070_MAX
};

static GPIO_info_t sim7060_io_table[] = {
		[SIM7070_PWR] = {GPIOC, { GPIO_PIN_7, GPIO_MODE_INPUT, GPIO_PULLDOWN, GPIO_SPEED_FREQ_HIGH}},
		[SIM7070_NET] = {GPIOC, { GPIO_PIN_9, GPIO_MODE_INPUT, GPIO_PULLDOWN, GPIO_SPEED_FREQ_HIGH}},
};

void SIM7070_IO_init(){
	for (int io = 0; io < SIM7070_MAX; ++io) {
		HAL_GPIO_Init(sim7060_io_table[io].port, sim7060_io_table[io].init_info.Pin);
	}
}
bool SIM7070_power(bool enable){
	HAL_GPIO_WritePin(sim7060_io_table[SIM7070_PWR].port, sim7060_io_table[SIM7070_PWR].init_info.Pin, enable);
}
bool SIM7070_reset(bool enable){
	(void)enable;
}
bool SIM7070_is_net(){
	return HAL_GPIO_ReadPin(sim7060_io_table[SIM7070_NET].port, sim7060_io_table[SIM7070_NET].init_info.Pin);
}
