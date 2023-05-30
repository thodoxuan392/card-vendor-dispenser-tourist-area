/*
 * uart.c
 *
 *  Created on: May 10, 2023
 *      Author: xuanthodo
 */


#include "main.h"
#include "Hal/uart.h"
#include "utils/utils_buffer.h"

#define TX_TIMEOUT		0xFFFF

typedef struct {
	UART_HandleTypeDef * huart_p;
	utils_buffer_t * buffer;
}UART_info_t;


UART_HandleTypeDef huart1 = {
	.Instance = USART1,
	.Init.BaudRate = 115200,
	.Init.WordLength = UART_WORDLENGTH_8B,
	.Init.StopBits = UART_STOPBITS_1,
	.Init.Parity = UART_PARITY_NONE,
	.Init.Mode = UART_MODE_TX_RX,
	.Init.HwFlowCtl = UART_HWCONTROL_NONE,
	.Init.OverSampling = UART_OVERSAMPLING_16
};

UART_HandleTypeDef huart2 = {
	.Instance = USART2,
	.Init.BaudRate = 115200,
	.Init.WordLength = UART_WORDLENGTH_8B,
	.Init.StopBits = UART_STOPBITS_1,
	.Init.Parity = UART_PARITY_NONE,
	.Init.Mode = UART_MODE_TX_RX,
	.Init.HwFlowCtl = UART_HWCONTROL_NONE,
	.Init.OverSampling = UART_OVERSAMPLING_16
};

UART_HandleTypeDef huart3 = {
	.Instance = USART3,
	.Init.BaudRate = 115200,
	.Init.WordLength = UART_WORDLENGTH_8B,
	.Init.StopBits = UART_STOPBITS_1,
	.Init.Parity = UART_PARITY_NONE,
	.Init.Mode = UART_MODE_TX_RX,
	.Init.HwFlowCtl = UART_HWCONTROL_NONE,
	.Init.OverSampling = UART_OVERSAMPLING_16
};

UART_HandleTypeDef huart4 = {
	.Instance = UART4,
	.Init.BaudRate = 115200,
	.Init.WordLength = UART_WORDLENGTH_8B,
	.Init.StopBits = UART_STOPBITS_1,
	.Init.Parity = UART_PARITY_NONE,
	.Init.Mode = UART_MODE_TX_RX,
	.Init.HwFlowCtl = UART_HWCONTROL_NONE,
	.Init.OverSampling = UART_OVERSAMPLING_16
};

static utils_buffer_t uart_buffer[UART_MAX];

static UART_info_t uart_table[UART_MAX] = {
		[UART_1] = {
			.huart_p = &huart1,
			.buffer = &uart_buffer[UART_1]
		},
		[UART_2] = {
			.huart_p = &huart2,
			.buffer = &uart_buffer[UART_2]
		},
		[UART_3] = {
			.huart_p = &huart3,
			.buffer = &uart_buffer[UART_3]
		},
		[UART_4] = {
			.huart_p = &huart4,
			.buffer = &uart_buffer[UART_4]
		},
};


bool UART_init(){
	bool success = true;
	// Init hal
	success = (HAL_UART_Init(&huart1) == HAL_OK) && success;
	success = (HAL_UART_Init(&huart2) == HAL_OK) && success;
	success = (HAL_UART_Init(&huart3) == HAL_OK) && success;
	success = (HAL_UART_Init(&huart4) == HAL_OK) && success;
	// Init buffer
	success = utils_buffer_init(&uart_buffer[UART_1], sizeof(uint8_t)) && success;
	success = utils_buffer_init(&uart_buffer[UART_2], sizeof(uint8_t)) && success;
	success = utils_buffer_init(&uart_buffer[UART_3], sizeof(uint8_t)) && success;
	success = utils_buffer_init(&uart_buffer[UART_4], sizeof(uint8_t)) && success;
	return success;
}
bool UART_send(UART_id_t id, uint8_t *data , size_t len){
	HAL_UART_Transmit(uart_table[id].huart_p, data, len, TX_TIMEOUT);
}
bool UART_receive_available(UART_id_t id){
	return utils_buffer_is_available(uart_table[id].buffer);
}
uint8_t UART_receive_byte(UART_id_t id){
	uint8_t data;
	utils_buffer_pop(uart_table[id].buffer, &data);
	return data;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef * huart){
	if(huart->Instance == uart_table[UART_2].huart_p->Instance){
		utils_buffer_push(uart_table[UART_2].buffer, &huart->Instance->DR);
	}else if(huart->Instance == uart_table[UART_3].huart_p->Instance){
		utils_buffer_push(uart_table[UART_3].buffer, &huart->Instance->DR);
	}else if(huart->Instance == uart_table[UART_4].huart_p->Instance){
		utils_buffer_push(uart_table[UART_4].buffer, &huart->Instance->DR);
	}
}
