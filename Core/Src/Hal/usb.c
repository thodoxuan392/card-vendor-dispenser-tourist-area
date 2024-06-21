/*
 * usb.c
 *
 *  Created on: Jun 21, 2024
 *      Author: xuanthodo
 */


#include <Hal/usb.h>


static USB_ReceiveCallback USB_receiveCallback;

void USB_init(void){

}

void USB_deinit(void){

}

void USB_setReceiveCallback(USB_ReceiveCallback callback){
	USB_receiveCallback = callback;
}

bool USB_send(uint8_t *data, uint32_t dataLen){

}

bool USB_isConnected(void){

}
