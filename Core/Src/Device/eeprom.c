/*
 * eeprom.c
 *
 *  Created on: May 14, 2023
 *      Author: xuanthodo
 */


#include "main.h"
#include "string.h"
#include "Device/eeprom.h"
#include "Hal/i2c.h"

#define I2C_ADDRESS	0xA0
#define PAGE_SIZE	32	// 32 bytes

enum {
	EEPROM_READ_OP,
	EEPROM_WRITE_OP,
	EEPROM_ERASE_OP
};

static uint8_t i2c_buffer_wr[PAGE_SIZE + 2];	// 2 byte for Address

bool EEPROM_init(){
	// Do nothing
	return true;
}

bool EEPROM_read(uint16_t address , uint8_t * data, size_t data_len){
	// Dummy address to read
	i2c_buffer_wr[0] = (uint8_t)(address >> 8);
	i2c_buffer_wr[1] = (uint8_t)address ;
	I2C_write(I2C_ADDRESS, i2c_buffer_wr, 2);
	// Read
	I2C_read(I2C_ADDRESS, data, data_len);
	return true;
}

bool EEPROM_write(uint16_t address , uint8_t * data, size_t data_len){
	size_t remain_size = data_len;
	size_t write_size;
	for(;remain_size > 0;){
		if(remain_size > PAGE_SIZE){
			write_size = PAGE_SIZE;
		}else{
			write_size = remain_size;
		}
		i2c_buffer_wr[0] = (uint8_t)(address >> 8);
		i2c_buffer_wr[1] = (uint8_t)address ;
		memcpy(&i2c_buffer_wr[2], &data[data_len - remain_size], write_size);
		I2C_write(I2C_ADDRESS, i2c_buffer_wr, write_size + 2);
		remain_size -= write_size;
	}
	return true;
}

bool EEPROM_test(){
	// Write
	const char * test_str = "Hello123123123";
	EEPROM_write(0x00, test_str, strlen(test_str));
	// Read
	static uint8_t read_buf[100] = {0};
	EEPROM_read(0x00, read_buf, 100);
}
