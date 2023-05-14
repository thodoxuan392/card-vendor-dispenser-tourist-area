/*
 * eeprom.h
 *
 *  Created on: May 13, 2023
 *      Author: xuanthodo
 */

#ifndef INC_DEVICE_EEPROM_H_
#define INC_DEVICE_EEPROM_H_

#include "stdio.h"
#include "stdbool.h"

bool EEPROM_init();
bool EEPROM_read(uint32_t address , uint8_t * data, size_t data_len);
bool EEPROM_write(uint32_t address , uint8_t * data, size_t data_len);

#endif /* INC_DEVICE_EEPROM_H_ */
