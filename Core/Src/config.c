/*
 * config.c
 *
 *  Created on: Jun 2, 2023
 *      Author: xuanthodo
 */

#include "Device/eeprom.h"
#include "Lib/utils/utils_logger.h"
#include "config.h"

#define EEPROM_CONFIG_ADDRESS 0x0000
#define CONFIG_DEFAULT                                                         \
  {                                                                            \
      .version = VERSION,                                                      \
      .device_id = DEVICE_ID_DEFAULT,                                          \
  };

static CONFIG_t config = CONFIG_DEFAULT;

static bool CONFIG_set_default(CONFIG_t *config, CONFIG_t *config_temp);
static bool CONFIG_field_is_empty(uint8_t *data, size_t data_len);

bool CONFIG_init() {
  CONFIG_t temp;
  EEPROM_read(EEPROM_CONFIG_ADDRESS, (uint8_t *)&temp, sizeof(CONFIG_t));
  CONFIG_set_default(&config, &temp);
  utils_log_info("CONFIG init done\r\n");
  CONFIG_printf();
}

CONFIG_t *CONFIG_get() { return &config; }

void CONFIG_set(CONFIG_t *_config) {
  memcpy(&config, _config, sizeof(CONFIG_t));
  EEPROM_write(EEPROM_CONFIG_ADDRESS, (uint8_t *)&config, sizeof(CONFIG_t));
  CONFIG_printf();
}

void CONFIG_reset_default() {
  CONFIG_t default_config = CONFIG_DEFAULT;
  CONFIG_set(&default_config);
}

void CONFIG_printf() {
  utils_log_info("Version: %s\r\n", config.version);
  utils_log_info("DeviceId: %s\r\n", config.device_id);
  utils_log_info("Password: %s\r\n", config.password);
}

void CONFIG_clear() {
  memset(&config, 0xFF, sizeof(CONFIG_t));
  EEPROM_write(EEPROM_CONFIG_ADDRESS, &config, sizeof(CONFIG_t));
}

void CONFIG_test() {
  CONFIG_t *newConfig = CONFIG_get();
  CONFIG_set(newConfig);
}

static bool CONFIG_set_default(CONFIG_t *_config, CONFIG_t *config_temp) {
  // Set version
  if (!CONFIG_field_is_empty((uint8_t *)config_temp->version,
                             sizeof(config_temp->version))) {
    memcpy(_config->version, config_temp->version, sizeof(_config->version));
  }
  // Set device_id
  if (!CONFIG_field_is_empty((uint8_t *)config_temp->device_id,
                             sizeof(config_temp->device_id))) {
    memcpy(_config->device_id, config_temp->device_id,
           sizeof(_config->device_id));
  }
}

static bool CONFIG_field_is_empty(uint8_t *data, size_t data_len) {
  for (int var = 0; var < data_len; ++var) {
    if (data[var] != 0xFF) {
      return false;
    }
  }
  return true;
}
