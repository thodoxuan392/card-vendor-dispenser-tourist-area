/**
 * @file crc.h
 * @author Xuan Tho Do (tho.dok17@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-03-01
 *
 * @copyright Copyright (c) 2024 Kinis AI
 *
 */

#ifndef UTILS_CRC_H
#define UTILS_CRC_H

#include <stdint.h>

uint16_t CRC_calculate(uint8_t *data, uint32_t dataLen);

#endif // UTILS_CRC_H