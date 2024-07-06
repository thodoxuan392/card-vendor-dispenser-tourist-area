/*
 * usb.h
 *
 *  Created on: Jun 21, 2024
 *      Author: xuanthodo
 */

#ifndef INC_HAL_USB_H_
#define INC_HAL_USB_H_

#include <stdint.h>
#include <stdbool.h>


typedef void (*USB_ReceiveCallback)(uint8_t *data, uint32_t dataLen);

void USB_init(void);
void USB_deinit(void);
void USB_setReceiveCallback(USB_ReceiveCallback callback);

bool USB_send(uint8_t *data, uint32_t dataLen);
bool USB_isConnected(void);

#endif /* INC_HAL_USB_H_ */
