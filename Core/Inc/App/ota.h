/*
 * ota.h
 *
 *  Created on: Jun 26, 2023
 *      Author: xuanthodo
 */

#ifndef INC_APP_OTA_H_
#define INC_APP_OTA_H_

#include "stdio.h"
#include "stdbool.h"

#define NO_OTA			0
#define FACTORY			1
#define CURRENT			0

/*
 * Firmware address
 */
#define NO_OTA_FIRMWARE_ADDR		0x08000000
#define FACTORY_FIRMWARE_ADDR		0x08008000			// Address of Factory Firmware 96KBytes
#define CURRENT_FIRMWARE_ADDR		0x08020000			// Address of Current Firmware 96KBytes

#if defined(NO_OTA) && NO_OTA == 1
#define FIRMWARE_ADDR		NO_OTA_FIRMWARE_ADDR
#elif defined(FACTORY) && FACTORY == 1
#define FIRMWARE_ADDR		FACTORY_FIRMWARE_ADDR
#elif defined(CURRENT) && CURRENT == 1
#define FIRMWARE_ADDR		CURRENT_FIRMWARE_ADDR
#endif
/*
 * Ota Requested
 */
#define OTA_IS_NOT_REQUESTED		0
#define OTA_IS_REQUESTED			1
#define OTA_REQUESTED_ADDRESS		0x0803F800			// 	0x0803F800 - 0x0803F802: 4B

enum {
	BOOTLOADER_CHOOSEN,
	APPLICATION_1_CHOOSEN,
	APPLICATION_2_CHOOSEN
};

void OTA_set_ota_requested();

#endif /* INC_APP_OTA_H_ */
