/*
 * uart.c
 *
 *  Created on: May 10, 2023
 *      Author: xuanthodo
 */


#include "main.h"
#include "Hal/uart.h"
#include "utils/utils_buffer.h"

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

bool UART_init(){
	bool success = true;
	// Init hal
	success = (HAL_UART_Init(&huart2) != HAL_OK) && success;
	success = (HAL_UART_Init(&huart3) != HAL_OK) && success;
	success = (HAL_UART_Init(&huart4) != HAL_OK) && success;
	// Init buffer
	success = utils_buffer_init(&uart_buffer[UART_2], sizeof(uint8_t)) && success;
	success = utils_buffer_init(&uart_buffer[UART_3], sizeof(uint8_t)) && success;
	success = utils_buffer_init(&uart_buffer[UART_4], sizeof(uint8_t)) && success;
	return success;
}
bool UART_send(UART_id_t id, uint8_t *data , size_t len);
bool UART_receive_available(UART_id_t id);
uint8_t UART_receive_byte(UART_id_t id);
