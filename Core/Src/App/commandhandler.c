/*
 * commandhandler.c
 *
 *  Created on: Jun 3, 2023
 *      Author: xuanthodo
 */
#include "App/commandhandler.h"

#include "config.h"
#include "main.h"

#include "App/protocol.h"

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
static void COMMANDHANDLER_handleUpdateRFID(PROTOCOL_t *);
static void COMMANDHANDLER_handlePlayAudio(PROTOCOL_t *);
static void COMMANDHANDLER_handleControlIO(PROTOCOL_t *);

/**
 * @defgroup Command Response Function Group
 *
 */
static void COMMANDHANDLER_sendResetResponse(PROTOCOL_ResultCode_t resultCode);
static void COMMANDHANDLER_sendRequestVersionResponse(PROTOCOL_ResultCode_t resultCode,
                                          uint8_t *firmwareVersion,
                                          uint8_t *boardVersion);
static void COMMANDHANDLER_sendConfigResponse(PROTOCOL_ResultCode_t resultCode);
static void COMMANDHANDLER_sendDispenseCardResponse(PROTOCOL_ResultCode_t resultCode,
                                        	uint8_t direction);
static void COMMANDHANDLER_sendUpdateRFIDResponse(PROTOCOL_ResultCode_t resultCode, RFID_Id_t id);
static void COMMANDHANDLER_sendPlayAudioResponse(PROTOCOL_ResultCode_t resultCode);
static void COMMANDHANDLER_sendControlIOResponse(PROTOCOL_ResultCode_t resultCode);

/**
 * @defgroup Callback Group Function
 *
 */
static void COMMANDHANDLER_takeCardCb(TCD_id_t tcdIndex);
static void COMMANDHANDLER_callbackCardCb(TCD_id_t tcdIndex);
static void COMMANDHANDLER_updateRFIDResultCb(RFID_Id_t id, RFID_Error_t error);

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
    [PROTOCOL_ID_CMD_UPDATE_RFID] = COMMANDHANDLER_handleUpdateRFID,
    [PROTOCOL_ID_CMD_PLAY_AUDIO] = COMMANDHANDLER_handlePlayAudio,
    [PROTOCOL_ID_CMD_CONTROL_IO] = COMMANDHANDLER_handleControlIO,
};
static PROTOCOL_t protocolMessage;

bool COMMANDHANDLER_init() {
	TCDMNG_set_take_card_cb(COMMANDHANDLER_takeCardCb);
	TCDMNG_set_callback_card_cb(COMMANDHANDLER_callbackCardCb);
	RFID_setUpdateResultCallback(COMMANDHANDLER_updateRFIDResultCb);
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
  uint8_t firmwareVersion[3], boardVersion[3];

  if (proto->data_len != 0) {
    utils_log_error(
        "HandleRequestVersion failed: Invalid data_len %d, expected 0\r\n",
        proto->data_len);
    COMMANDHANDLER_sendRequestVersionResponse(
        PROTOCOL_RESULT_COMM_PROTOCOL_DATA_LEN_INVALID, firmwareVersion,
        boardVersion);
    return;
  }

  firmwareVersion[0] = FIRMWARE_VERSION_MAJOR;
  firmwareVersion[1] = FIRMWARE_VERSION_MINOR;
  firmwareVersion[2] = FIRMWARE_VERSION_PATCH;
  boardVersion[0] = BOARD_VERSION_MAJOR;
  boardVersion[1] = BOARD_VERSION_MINOR;
  boardVersion[2] = BOARD_VERSION_PATCH;

  COMMANDHANDLER_sendRequestVersionResponse(PROTOCOL_RESULT_SUCCESS,
                                            firmwareVersion, boardVersion);
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
  config->enableStsPolling = proto->data[0];

  CONFIG_set(config);

  COMMANDHANDLER_sendConfigResponse(PROTOCOL_RESULT_SUCCESS);
}

static void COMMANDHANDLER_handleDispenseCard(PROTOCOL_t *proto) {
  if (proto->data_len != 0) {
    utils_log_error(
        "HandleDispenseCard failed: Invalid data_len %d, expected 0\r\n",
        proto->data_len);
    COMMANDHANDLER_sendDispenseCardResponse(
        PROTOCOL_RESULT_COMM_PROTOCOL_DATA_LEN_INVALID, 0x00);
    return;
  }
  uint8_t tcdIndex = proto->data[0];
  uint8_t nbCard = proto->data[1];

  if (!TCDMNG_payoutNbCard(tcdIndex, nbCard)) {
    utils_log_error("HandleDispenseCard failed: Cannot dispense card\r\n",
                    proto->data_len);
    COMMANDHANDLER_sendDispenseCardResponse(PROTOCOL_RESULT_ERROR, 0x00);
    return;
  }
}

static void COMMANDHANDLER_handleUpdateRFID(PROTOCOL_t *proto) {
  uint8_t rfidIndex = proto->data[0];
  uint8_t rfidLen = proto->data[1];

  if (proto->data_len != rfidLen + 9) {
    utils_log_error(
        "HandleDispenseCard failed: Invalid data_len %d, expected %d\r\n",
        proto->data_len, rfidLen + 9);
    COMMANDHANDLER_sendUpdateRFIDResponse(PROTOCOL_RESULT_COMM_PROTOCOL_DATA_LEN_INVALID, rfidLen);
    return;
  }

  RFID_t rfid;
  rfid.id_len = rfidLen;
  for (size_t i = 0; i < rfidLen; i++) {
    rfid.id[i] = proto->data[2 + i];
  }
  rfid.money = ((uint32_t)proto->data[rfidLen + 3] << 24) |
               ((uint32_t)proto->data[rfidLen + 4] << 16) |
               ((uint32_t)proto->data[rfidLen + 5] << 8) |
               ((uint32_t)proto->data[rfidLen + 6]);

  rfid.issueDate[2] = proto->data[rfidLen + 7];
  rfid.issueDate[1] = proto->data[rfidLen + 6];
  rfid.issueDate[0] = proto->data[rfidLen + 5];

  RFID_Error_t err = RFID_set(rfidIndex, &rfid);
  if (err != RFID_SUCCESS) {
    utils_log_error(
        "HandleUpdateRFID failed: Cannot update RFID with err %d\r\n", err);
    PROTOCOL_ResultCode_t resultCode = COMMANDHANDLER_rfidErrorToResultCode(err);
    COMMANDHANDLER_sendUpdateRFIDResponse(resultCode, rfidIndex);
    return;
  }
  COMMANDHANDLER_sendUpdateRFIDResponse(PROTOCOL_RESULT_SUCCESS, rfidIndex);
}

static void COMMANDHANDLER_handlePlayAudio(PROTOCOL_t *proto) {
  if (proto->data_len != 1) {
    utils_log_error(
        "HandlePlayAudio failed: Invalid data_len %d, expected 0\r\n",
        proto->data_len);
    COMMANDHANDLER_sendPlayAudioResponse(
        PROTOCOL_RESULT_COMM_PROTOCOL_DATA_LEN_INVALID);
    return;
  }
  // Nothing to do
  COMMANDHANDLER_sendPlayAudioResponse(PROTOCOL_RESULT_SUCCESS);
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
                                          uint8_t *firmwareVersion,
                                          uint8_t *boardVersion) {
  PROTOCOL_t protocol;
  protocol.protocol_id = PROTOCOL_ID_CMD_RESET;
  protocol.data_len = 7;
  protocol.data[0] = resultCode;
  protocol.data[1] = firmwareVersion[0];
  protocol.data[2] = firmwareVersion[1];
  protocol.data[3] = firmwareVersion[2];
  protocol.data[4] = boardVersion[0];
  protocol.data[5] = boardVersion[1];
  protocol.data[6] = boardVersion[2];

  PROTOCOL_send(&protocol);
}
static void COMMANDHANDLER_sendConfigResponse(PROTOCOL_ResultCode_t resultCode) {
  PROTOCOL_t protocol;
  protocol.protocol_id = PROTOCOL_ID_CMD_CONFIG;
  protocol.data_len = 1;
  protocol.data[0] = resultCode;

  PROTOCOL_send(&protocol);
}
static void COMMANDHANDLER_sendDispenseCardResponse(PROTOCOL_ResultCode_t resultCode,
                                        uint8_t direction) {
  PROTOCOL_t protocol;
  protocol.protocol_id = PROTOCOL_ID_CMD_DISPENSE_CARD;
  protocol.data_len = 1;
  protocol.data[0] = resultCode;

  PROTOCOL_send(&protocol);
}
static void COMMANDHANDLER_sendUpdateRFIDResponse(PROTOCOL_ResultCode_t resultCode, RFID_Id_t id) {
  PROTOCOL_t protocol;
  protocol.protocol_id = PROTOCOL_ID_CMD_UPDATE_RFID;
  protocol.data_len = 2;
  protocol.data[0] = resultCode;
  protocol.data[1] = id;

  PROTOCOL_send(&protocol);
}
static void COMMANDHANDLER_sendPlayAudioResponse(PROTOCOL_ResultCode_t resultCode) {
  PROTOCOL_t protocol;
  protocol.protocol_id = PROTOCOL_ID_CMD_PLAY_AUDIO;
  protocol.data_len = 1;
  protocol.data[0] = resultCode;

  PROTOCOL_send(&protocol);
}

static void COMMANDHANDLER_sendControlIOResponse(PROTOCOL_ResultCode_t resultCode) {
  PROTOCOL_t protocol;
  protocol.protocol_id = PROTOCOL_ID_CMD_CONTROL_IO;
  protocol.data_len = 1;
  protocol.data[0] = resultCode;

  PROTOCOL_send(&protocol);
}

static void COMMANDHANDLER_takeCardCb(TCD_id_t tcdIndex) {
  COMMANDHANDLER_sendDispenseCardResponse(PROTOCOL_RESULT_SUCCESS, 0x00);
}

static void COMMANDHANDLER_callbackCardCb(TCD_id_t tcdIndex) {
  COMMANDHANDLER_sendDispenseCardResponse(PROTOCOL_RESULT_SUCCESS, 0x01);
}

static void COMMANDHANDLER_updateRFIDResultCb(RFID_Id_t id, RFID_Error_t error){
	PROTOCOL_ResultCode_t resultCode = COMMANDHANDLER_rfidErrorToResultCode(error);
	COMMANDHANDLER_sendUpdateRFIDResponse(resultCode , id);
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

static PROTOCOL_ResultCode_t COMMANDHANDLER_rfidErrorToResultCode(RFID_Error_t err) {
  PROTOCOL_ResultCode_t result;
  switch (err) {
  case RFID_ERROR_AUTHEN_FAILED:
    result = PROTOCOL_RESULT_CMD_RFID_AUTHEN_FAILED;
    break;
  case RFID_ERROR_CANNOT_WRITE:
    result = PROTOCOL_RESULT_CMD_RFID_CANNOT_WRITE;
    break;
  case RFID_ERROR_ID_NOT_MATCHED:
    result = PROTOCOL_RESULT_CMD_RFID_NOT_MATCHED;
    break;
  case RFID_ERROR_INVALID_FORMAT:
    result = PROTOCOL_RESULT_CMD_RFID_INVALID_FORMAT;
    break;
  case RFID_ERROR_NOT_AVAILABLE:
    result = PROTOCOL_RESULT_CMD_RFID_NOT_AVAILABLE;
    break;
  default:
    break;
  }
  return result;
}
