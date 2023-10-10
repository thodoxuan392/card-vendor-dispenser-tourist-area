/*
 * ota.c
 *
 *  Created on: Jun 26, 2023
 *      Author: xuanthodo
 */


#include "main.h"
#include "App/ota.h"
#include "Hal/flash.h"
#include "Lib/utils/utils_logger.h"

void OTA_set_ota_requested(){
	uint8_t ota_is_requested = OTA_IS_REQUESTED;
	FLASH_erase(OTA_REQUESTED_ADDRESS, sizeof(ota_is_requested));
	FLASH_write_int(OTA_REQUESTED_ADDRESS, ota_is_requested);
}
