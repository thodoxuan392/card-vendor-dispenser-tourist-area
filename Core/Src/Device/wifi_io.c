/*
 * wifi_io.c
 *
 *  Created on: Nov 13, 2023
 *      Author: xuanthodo
 */

#include "Device/wifi_io.h"

#include "Hal/gpio.h"

enum {
	WIFI_ENA,
	WIFI_MAX
};

static GPIO_info_t wifi_io_table[] = {
		[WIFI_ENA] = {GPIOE, { GPIO_PIN_15, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP, GPIO_SPEED_FREQ_LOW}},
};


void WIFIIO_init(){
	for (int io = 0; io < WIFI_MAX; ++io) {
		HAL_GPIO_Init(wifi_io_table[io].port, &wifi_io_table[io].init_info);
	}
}

bool WIFIIO_reset(bool enable){
	HAL_GPIO_WritePin(wifi_io_table[WIFI_ENA].port, wifi_io_table[WIFI_ENA].init_info.Pin, !enable);
	return true;
}
