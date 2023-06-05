/*
 * keypadmanager.c
 *
 *  Created on: Jun 3, 2023
 *      Author: xuanthodo
 */


#include "DeviceManager/keypadmanager.h"
#include "Device/keypad.h"

// Keypad manager
static uint8_t keypad_buf[KEYPAD_BUF_SIZE];
static uint8_t keypad_buf_len = 0;

// Keypad status
static uint16_t keypad_prev_status = 0x0000;
static uint16_t keypad_status = 0x0000;
static bool is_entered = false;
static bool is_cancelled = false;


// Private function
static bool KEYPADMNG_is_pressed(uint16_t keypad_status, uint16_t keypad_prev_status, uint8_t key);
static bool KEYPADMNG_is_release(uint16_t keypad_status, uint16_t keypad_prev_status, uint8_t key);

void KEYPADMNG_init(){

}

void KEYPADMNG_run(){
	// Get keypad status
	keypad_status = KEYPAD_get_status();
	// Check number is pressed
	for (int key = KEY_0; key <= KEY_9; ++key) {
		if(KEYPADMNG_is_pressed(keypad_status, keypad_prev_status, key)){
			keypad_buf[keypad_buf_len] = key;
			keypad_buf_len = (keypad_buf_len + 1) % KEYPAD_BUF_SIZE;
			break;
		}
	}
	// Check entered
	is_entered = KEYPADMNG_is_pressed(keypad_status, keypad_prev_status, KEY_ENTER_OR_STAR);
	is_cancelled = KEYPADMNG_is_pressed(keypad_status, keypad_prev_status, KEY_ENTER_OR_STAR);
	keypad_prev_status = keypad_status;
}

bool KEYPADMNG_is_entered(){
	return is_entered;
}

bool KEYPADMNG_is_cancelled(){
	return is_cancelled;
}

void KEYPADMNG_get_data(uint8_t * data, size_t * data_len){
	memcpy(data, keypad_buf, keypad_buf_len);
	* data_len = keypad_buf_len;
}


static bool KEYPADMNG_is_pressed(uint16_t keypad_status, uint16_t keypad_prev_status, uint8_t key){
	return (keypad_status & (1 << key)) && !(keypad_prev_status & (1 << key));
}

static bool KEYPADMNG_is_release(uint16_t keypad_status, uint16_t keypad_prev_status, uint8_t key){
	return !(keypad_status & (1 << key)) && (keypad_prev_status & (1 << key));
}
