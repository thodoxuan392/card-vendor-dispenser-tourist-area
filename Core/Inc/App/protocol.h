/*
 * protocol.h
 *
 *  Created on: Dec 17, 2023
 *      Author: xuanthodo
 */

#ifndef INC_APP_PROTOCOL_H_
#define INC_APP_PROTOCOL_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

//#define PROTOCOL_USE_USB
#define PROTOCOL_USE_UART

#define START_BYTE 0x78
#define STOP_BYTE 0x79
#define DATA_MAX_LENGTH 255
#define TX_BUFFER_MAX_LENGTH 512
#define RX_BUFFER_MAX_LENGTH 1024
#define RX_TIMEOUT_MS 500 // 1000ms

typedef struct {
  uint8_t protocol_id;
  uint8_t data_len;
  uint8_t data[DATA_MAX_LENGTH];
} PROTOCOL_t;

typedef enum {
  // Generic Command
  PROTOCOL_ID_CMD_RESET = 0x01,
  PROTOCOL_ID_CMD_REQUEST_VERSION = 0x02,
  PROTOCOL_ID_CMD_CONFIG = 0x03,

  // Specific Function Command
  PROTOCOL_ID_CMD_DISPENSE_CARD = 0x11,
  PROTOCOL_ID_CMD_UPDATE_RFID = 0x12,
  PROTOCOL_ID_CMD_PLAY_AUDIO = 0x13,
  PROTOCOL_ID_CMD_CONTROL_IO = 0x14,

  // Status
  PROTOCOL_ID_STS_DEVICE_ERR = 0x30,
  PROTOCOL_ID_STS_BILL_ACCEPTOR = 0x31,
  PROTOCOL_ID_STS_TCD = 0x32,

  // Event
  PROTOCOL_ID_EVT_RFID_DETECTED = 0x50,
  PROTOCOL_ID_EVT_BILL_ACCEPTED = 0x51,
  PROTOCOL_ID_EVT_CARD_EVENT = 0x52,

  PROTOCOL_ID_MAX
} PROTOCOL_Id_t;

typedef enum {
  PROTOCOL_RESULT_SUCCESS = 0x00,
  PROTOCOL_RESULT_ERROR = 0x01,
  PROTOCOL_RESULT_COMM_PROTOCOL_ID_INVALID = 0x10,
  PROTOCOL_RESULT_COMM_PROTOCOL_CRC_INVALID = 0x11,
  PROTOCOL_RESULT_COMM_PROTOCOL_START_STOP_BYTE_INVALID = 0x12,
  PROTOCOL_RESULT_COMM_PROTOCOL_DATA_LEN_INVALID = 0x13,
  PROTOCOL_RESULT_COMM_PROTOCOL_TIMEOUT = 0x14,
  PROTOCOL_RESULT_CMD_RESET_TIMEOUT = 0x20,
  PROTOCOL_RESULT_CMD_TRANSFER_OTA_DATA_IN_PROGRESS = 0x21,
  PROTOCOL_RESULT_CMD_RFID_NOT_AVAILABLE = 0x30,
  PROTOCOL_RESULT_CMD_RFID_INVALID_FORMAT = 0x31,
  PROTOCOL_RESULT_CMD_RFID_NOT_MATCHED = 0x32,
  PROTOCOL_RESULT_CMD_RFID_AUTHEN_FAILED = 0x33,
  PROTOCOL_RESULT_CMD_RFID_CANNOT_WRITE = 0x34
} PROTOCOL_ResultCode_t;

void PROTOCOL_init(void);
void PROTOCOL_run(void);
bool PROTOCOL_send(PROTOCOL_t *proto);
bool PROTOCOL_receive(PROTOCOL_t *proto);

#endif /* INC_APP_PROTOCOL_H_ */
