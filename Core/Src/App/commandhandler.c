/*
 * commandhandler.c
 *
 *  Created on: Jun 3, 2023
 *      Author: xuanthodo
 */
#include "App/commandhandler.h"

#include "main.h"
#include "config.h"
#include "string.h"

#include "App/protocol.h"
#include "App/statemachine.h"

#include <DeviceManager/tcdmanager.h>

#include <Device/genericio.h>
#include <Device/rfid.h>

#include "utils/utils_logger.h"

typedef void (*COMMANDHANDLER_HandleFunc)(PROTOCOL_t *);

/**
 * @defgroup Command Handler Function Group
 *
 */
static void COMMANDHANDLER_handleReset(PROTOCOL_t *);
static void COMMANDHANDLER_handleRequestVersion(PROTOCOL_t *);
static void COMMANDHANDLER_handleConfig(PROTOCOL_t *);
static void COMMANDHANDLER_handleDispenseCard(PROTOCOL_t *);
static void COMMANDHANDLER_handlePlayAudio(PROTOCOL_t *);
static void COMMANDHANDLER_handleControlIO(PROTOCOL_t *);

/**
 * @defgroup Command Response Function Group
 *
 */
static void COMMANDHANDLER_sendResetResponse(PROTOCOL_ResultCode_t resultCode);
static void COMMANDHANDLER_sendRequestVersionResponse(PROTOCOL_ResultCode_t resultCode,
															uint8_t *deviceId,
															uint8_t *firmwareVersion,
															uint8_t *boardVersion);
static void COMMANDHANDLER_sendConfigResponse(PROTOCOL_ResultCode_t resultCode);
static void COMMANDHANDLER_sendDispenseCardResponse(PROTOCOL_ResultCode_t resultCode, uint32_t remainCard);
static void COMMANDHANDLER_sendControlIOResponse(PROTOCOL_ResultCode_t resultCode);

/**
 * @defgroup Callback Group Function
 *
 */
static void COMMANDHANDLER_dispenseCardResultCallback(uint8_t result, uint32_t remainCard);

/**
 * @defgroup Miscellaneous Function Group
 */
static bool COMMANDHANDLER_isCommandValid(PROTOCOL_t *);
static PROTOCOL_ResultCode_t COMMANDHANDLER_rfidErrorToResultCode(RFID_Error_t err);

static COMMANDHANDLER_HandleFunc COMMANDHANDLER_handleFuncTable[] = {
    [PROTOCOL_ID_CMD_RESET] = COMMANDHANDLER_handleReset,
    [PROTOCOL_ID_CMD_REQUEST_VERSION] = COMMANDHANDLER_handleRequestVersion,
    [PROTOCOL_ID_CMD_CONFIG] = COMMANDHANDLER_handleConfig,
    [PROTOCOL_ID_CMD_DISPENSE_CARD] = COMMANDHANDLER_handleDispenseCard,
    [PROTOCOL_ID_CMD_CONTROL_IO] = COMMANDHANDLER_handleControlIO,
    [PROTOCOL_ID_CMD_CONTROL_IO] = COMMANDHANDLER_handleControlIO,
};
static PROTOCOL_t protocolMessage;

bool COMMANDHANDLER_init() {
	STATEMACHINE_setDispenseResultCallback(COMMANDHANDLER_dispenseCardResultCallback);
}

bool COMMANDHANDLER_run() {
  if (PROTOCOL_receive(&protocolMessage)) {
    if (!COMMANDHANDLER_isCommandValid(&protocolMessage)) {
      utils_log_error("[COMMANDHANDLER] Command is not valid\r\n");
      return false;
    }
    // Execute Command Handler
    COMMANDHANDLER_handleFuncTable[protocolMessage.protocol_id](
        &protocolMessage);
  }
  return true;
}

static void COMMANDHANDLER_handleReset(PROTOCOL_t *proto) {
  if (proto->data_len != 0) {
    utils_log_error("HandleReset failed: Invalid data_len %d, expected 0\r\n",
                    proto->data_len);
    COMMANDHANDLER_sendResetResponse(
        PROTOCOL_RESULT_COMM_PROTOCOL_DATA_LEN_INVALID);
    return;
  }
  COMMANDHANDLER_sendResetResponse(PROTOCOL_RESULT_SUCCESS);

  // Reset
  HAL_NVIC_SystemReset();
}

static void COMMANDHANDLER_handleRequestVersion(PROTOCOL_t *proto) {
	uint8_t deviceId[6], firmwareVersion[3], boardVersion[3];

	if (proto->data_len != 0) {
		utils_log_error(
			"HandleRequestVersion failed: Invalid data_len %d, expected 0\r\n",
			proto->data_len);
		COMMANDHANDLER_sendRequestVersionResponse(
			PROTOCOL_RESULT_COMM_PROTOCOL_DATA_LEN_INVALID, deviceId, firmwareVersion,
			boardVersion);
		return;
	}
	char * deviceIdStr = CONFIG_get()->device_id;
	memcpy(deviceId, deviceIdStr, sizeof(deviceId));

	firmwareVersion[0] = FIRMWARE_VERSION_MAJOR;
	firmwareVersion[1] = FIRMWARE_VERSION_MINOR;
	firmwareVersion[2] = FIRMWARE_VERSION_PATCH;
	boardVersion[0] = BOARD_VERSION_MAJOR;
	boardVersion[1] = BOARD_VERSION_MINOR;
	boardVersion[2] = BOARD_VERSION_PATCH;

	COMMANDHANDLER_sendRequestVersionResponse(PROTOCOL_RESULT_SUCCESS, deviceId, firmwareVersion, boardVersion);
}

static void COMMANDHANDLER_handleConfig(PROTOCOL_t *proto) {
  if (proto->data_len != 1) {
    utils_log_error(
        "HandleRequestVersion failed: Invalid data_len %d, expected 1\r\n",
        proto->data_len);
    COMMANDHANDLER_sendConfigResponse(
        PROTOCOL_RESULT_COMM_PROTOCOL_DATA_LEN_INVALID);
    return;
  }

  CONFIG_t *config = CONFIG_get();

  CONFIG_set(config);

  COMMANDHANDLER_sendConfigResponse(PROTOCOL_RESULT_SUCCESS);
}

static void COMMANDHANDLER_handleDispenseCard(PROTOCOL_t *proto) {
  if (proto->data_len != 2) {
    utils_log_error(
        "HandleDispenseCard failed: Invalid data_len %d, expected 2\r\n",
        proto->data_len);
    COMMANDHANDLER_sendDispenseCardResponse(
        PROTOCOL_RESULT_COMM_PROTOCOL_DATA_LEN_INVALID, 0);
    return;
  }
  uint8_t nbCard = proto->data[0];
  uint8_t type = proto->data[1];

  if (!STATEMACHINE_requestDispense(nbCard, type)) {
    utils_log_error("HandleDispenseCard failed: Cannot dispense card\r\n",
                    proto->data_len);
    COMMANDHANDLER_sendDispenseCardResponse(PROTOCOL_RESULT_ERROR, 0);
    return;
  }
}

static void COMMANDHANDLER_handleControlIO(PROTOCOL_t *proto) {
  if (proto->data_len != 2) {
    utils_log_error(
        "HandleGenericIo failed: Invalid data_len %d, expected 2\r\n",
        proto->data_len);
    COMMANDHANDLER_sendControlIOResponse(
        PROTOCOL_RESULT_COMM_PROTOCOL_DATA_LEN_INVALID);
    return;
  }

  uint8_t selectedMask = proto->data[0];
  uint8_t levelMask = proto->data[1];

  for (size_t id = 0; id < 8; id++) {
    if ((selectedMask >> id) & 0x01) {
      GENERICIO_set(id, (levelMask >> id) & 0x01);
    }
  }
  COMMANDHANDLER_sendControlIOResponse(PROTOCOL_RESULT_SUCCESS);
}

static void COMMANDHANDLER_sendResetResponse(PROTOCOL_ResultCode_t resultCode) {
  PROTOCOL_t protocol;
  protocol.protocol_id = PROTOCOL_ID_CMD_RESET;
  protocol.data_len = 1;
  protocol.data[0] = resultCode;

  PROTOCOL_send(&protocol);
}
static void COMMANDHANDLER_sendRequestVersionResponse(PROTOCOL_ResultCode_t resultCode,
											  uint8_t *deviceId,
											  uint8_t *firmwareVersion,
											  uint8_t *boardVersion) {
	PROTOCOL_t protocol;
	protocol.protocol_id = PROTOCOL_ID_CMD_RESET;
	protocol.data_len = 0;
	protocol.data[protocol.data_len++] = resultCode;
	protocol.data[protocol.data_len++] = deviceId[0];
	protocol.data[protocol.data_len++] = deviceId[1];
	protocol.data[protocol.data_len++] = deviceId[2];
	protocol.data[protocol.data_len++] = deviceId[3];
	protocol.data[protocol.data_len++] = deviceId[4];
	protocol.data[protocol.data_len++] = deviceId[5];
	protocol.data[protocol.data_len++] = firmwareVersion[0];
	protocol.data[protocol.data_len++] = firmwareVersion[1];
	protocol.data[protocol.data_len++] = firmwareVersion[2];
	protocol.data[protocol.data_len++] = boardVersion[0];
	protocol.data[protocol.data_len++] = boardVersion[1];
	protocol.data[protocol.data_len++] = boardVersion[2];

	PROTOCOL_send(&protocol);
}
static void COMMANDHANDLER_sendConfigResponse(PROTOCOL_ResultCode_t resultCode) {
  PROTOCOL_t protocol;
  protocol.protocol_id = PROTOCOL_ID_CMD_CONFIG;
  protocol.data_len = 1;
  protocol.data[0] = resultCode;

  PROTOCOL_send(&protocol);
}
static void COMMANDHANDLER_sendDispenseCardResponse(PROTOCOL_ResultCode_t resultCode, uint32_t remainCard) {
  PROTOCOL_t protocol;
  protocol.protocol_id = PROTOCOL_ID_CMD_DISPENSE_CARD;
  protocol.data_len = 2;
  protocol.data[0] = resultCode;
  protocol.data[1] = remainCard;

  PROTOCOL_send(&protocol);
}

static void COMMANDHANDLER_sendControlIOResponse(PROTOCOL_ResultCode_t resultCode) {
  PROTOCOL_t protocol;
  protocol.protocol_id = PROTOCOL_ID_CMD_CONTROL_IO;
  protocol.data_len = 1;
  protocol.data[0] = resultCode;

  PROTOCOL_send(&protocol);
}

static void COMMANDHANDLER_dispenseCardResultCallback(uint8_t result, uint32_t remainCard){
	COMMANDHANDLER_sendDispenseCardResponse(result, remainCard);
}


static bool COMMANDHANDLER_isCommandValid(PROTOCOL_t *proto) {
	if(proto->protocol_id >= PROTOCOL_ID_MAX){
		return false;
	}
	if(COMMANDHANDLER_handleFuncTable[proto->protocol_id] == NULL){
		return false;
	}
	return true;
}

