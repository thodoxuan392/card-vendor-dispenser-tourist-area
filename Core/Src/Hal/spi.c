#include <Hal/spi.h>

#include <main.h>
#include <string.h>
#include <Hal/gpio.h>

static GPIO_info_t SPI_csTable[] = {
    [SPI_CS_1] = {GPIOD, { GPIO_PIN_8, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP, GPIO_SPEED_FREQ_HIGH}},
    [SPI_CS_2] = {GPIOD, { GPIO_PIN_11, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP, GPIO_SPEED_FREQ_HIGH}},
    [SPI_CS_3] = {GPIOD, { GPIO_PIN_14, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP, GPIO_SPEED_FREQ_HIGH}}
};

static SPI_HandleTypeDef hspi1 = {
    .Instance = SPI1,
    .Init = {
        .Mode = SPI_MODE_MASTER,
        .Direction = SPI_DIRECTION_2LINES,
        .DataSize = SPI_DATASIZE_8BIT,
        .CLKPolarity = SPI_POLARITY_LOW,
        .CLKPhase = SPI_PHASE_1EDGE,
        .NSS = SPI_NSS_SOFT,
        .BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256,
        .FirstBit = SPI_FIRSTBIT_LSB,
        .TIMode = SPI_TIMODE_DISABLE,
        .CRCCalculation = SPI_CRCCALCULATION_DISABLE,
        .CRCPolynomial = 10
    }
};

static uint8_t SPI_txBuffer[SPI_TX_RX_BUFFER_MAX];
static uint8_t SPI_rxBuffer[SPI_TX_RX_BUFFER_MAX];

void SPI_init(void){
    if (HAL_SPI_Init(&hspi1) != HAL_OK)
    {
        Error_Handler();
    }
    for (size_t i = 0; i < SPI_CS_MAX; i++)
    {
        HAL_GPIO_Init(SPI_csTable[i].port , &SPI_csTable[i].init_info);
        HAL_GPIO_WritePin(SPI_csTable[i].port, SPI_csTable[i].init_info.Pin, GPIO_PIN_SET);
    }
}
void SPI_transmit(SPI_CS cs, uint8_t *txBuffer,  uint32_t size, uint32_t timeout){
	SPI_select(cs);
	HAL_Delay(1);
	HAL_SPI_Transmit(&hspi1, txBuffer, size, timeout);
	SPI_deselect(cs);
}

void SPI_transmitAndReceive(SPI_CS cs, uint8_t *txBuffer, uint32_t txSize,  uint8_t *rxBuffer, uint32_t rxSize, uint32_t timeout){
	SPI_select(cs);
	HAL_Delay(1);
	memcpy(SPI_txBuffer, txBuffer, txSize);
	HAL_SPI_TransmitReceive(&hspi1, SPI_txBuffer, SPI_rxBuffer, txSize + rxSize, timeout);
	memcpy(rxBuffer, &SPI_rxBuffer[txSize], rxSize);
	SPI_deselect(cs);
}

void SPI_select(SPI_CS cs){
    HAL_GPIO_WritePin(SPI_csTable[cs].port, SPI_csTable[cs].init_info.Pin, GPIO_PIN_RESET);
}

void SPI_deselect(SPI_CS cs){
    HAL_GPIO_WritePin(SPI_csTable[cs].port, SPI_csTable[cs].init_info.Pin, GPIO_PIN_SET);
}
void SPI_test(void){
	uint8_t data[3] = {1,2,3};
	while(1){
	    HAL_SPI_Transmit(&hspi1, data, 3, 1000);
	    HAL_Delay(1000);
	}
}
