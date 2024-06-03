/**
 * @file spi.h
 * @author Xuan Tho Do (tho.dok17@gmail.com)
 * @brief SPI Hal
 * @version 0.1
 * @date 2023-12-25
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef HAL_SPI_H
#define HAL_SPI_H

#include <stdint.h>

#define SPI_TX_RX_BUFFER_MAX	128

typedef enum {
    SPI_CS_1 = 0,
    SPI_CS_2,
    SPI_CS_3,
    SPI_CS_MAX
}SPI_CS;

void SPI_init(void);
void SPI_transmit(SPI_CS cs, uint8_t *txBuffer,  uint32_t size, uint32_t timeout);
void SPI_transmitAndReceive(SPI_CS cs, uint8_t *txBuffer, uint32_t txSize,  uint8_t *rxBuffer, uint32_t rxSize, uint32_t timeout);
void SPI_test(void);

#endif // HAL_SPI_H
