/*
 * config.h
 *
 *  Created on: Jun 2, 2023
 *      Author: xuanthodo
 */

#ifndef INC_APP_CONFIG_H_
#define INC_APP_CONFIG_H_

#include "stdbool.h"
#include "stdio.h"

#ifndef VERSION
#define VERSION "1.0.0"
#define VERSION_MAX_LEN 6
#endif

#ifndef FIRMWARE_VERSION_MAJOR
#define FIRMWARE_VERSION_MAJOR 1
#define FIRMWARE_VERSION_MINOR 0
#define FIRMWARE_VERSION_PATCH 0
#endif

#ifndef BOARD_VERSION_MAJOR
#define BOARD_VERSION_MAJOR 1
#define BOARD_VERSION_MINOR 0
#define BOARD_VERSION_PATCH 0
#endif

#define MODEL "cd_tx"

#define MANUAL_MODE_PASSWORD "987654321"

#define SETTING_MODE_PASSWORD_MAX_LEN 10
#ifndef SETTING_MODE_PASSWORD
#define SETTING_MODE_PASSWORD "123"
#endif

#define DEVICE_ID_MAX_LEN 10
#ifndef DEVICE_ID_DEFAULT
#define DEVICE_ID_DEFAULT "123456"
#endif

typedef struct {
  char version[VERSION_MAX_LEN];
  char device_id[DEVICE_ID_MAX_LEN];
  char password[SETTING_MODE_PASSWORD_MAX_LEN];
} CONFIG_t;

bool CONFIG_init();
CONFIG_t *CONFIG_get();
void CONFIG_set(CONFIG_t *);
void CONFIG_reset_default();
void CONFIG_clear();
void CONFIG_test();

#endif /* INC_APP_CONFIG_H_ */
