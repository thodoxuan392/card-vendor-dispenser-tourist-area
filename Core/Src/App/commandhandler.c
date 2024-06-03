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

#include "utils/utils_logger.h"

typedef void (*COMMANDHANDLER_HandleFunc)(PROTOCOL_t *);

static void COMMANDHANDLER_handleReset(PROTOCOL_t *);
static void COMMANDHANDLER_handleSyncTime(PROTOCOL_t *);
static void COMMANDHANDLER_handleRequestVersion(PROTOCOL_t *);
static void COMMANDHANDLER_handleRequestOTA(PROTOCOL_t *);
static void COMMANDHANDLER_handleRequestTransferOTADataSize(PROTOCOL_t *);
static void COMMANDHANDLER_handleRequestTransferOTAData(PROTOCOL_t *);
static void COMMANDHANDLER_handleSetting(PROTOCOL_t *);
static void COMMANDHANDLER_handleDispenseCard(PROTOCOL_t *);
static void COMMANDHANDLER_handleUpdateRFID(PROTOCOL_t *);
static void COMMANDHANDLER_handlePlayAudio(PROTOCOL_t *);
static void COMMANDHANDLER_handleControlIO(PROTOCOL_t *);

static bool COMMANDHANDLER_isCommandValid(PROTOCOL_t *);

static COMMANDHANDLER_HandleFunc COMMANDHANDLER_handleFuncTable[] = {
    [PROTOCOL_ID_CMD_RESET] = COMMANDHANDLER_handleReset,
    [PROTOCOL_ID_CMD_REQUEST_VERSION] = COMMANDHANDLER_handleRequestVersion,
    [PROTOCOL_ID_CMD_SETTING] = COMMANDHANDLER_handleSetting,
    [PROTOCOL_ID_CMD_DISPENSE_CARD] = COMMANDHANDLER_handleDispenseCard,
    [PROTOCOL_ID_CMD_UPDATE_RFID] = COMMANDHANDLER_handleUpdateRFID,
    [PROTOCOL_ID_CMD_PLAY_AUDIO] = COMMANDHANDLER_handlePlayAudio,
    [PROTOCOL_ID_CMD_CONTROL_IO] = COMMANDHANDLER_handleControlIO,
};
static PROTOCOL_t protocolMessage;

bool COMMANDHANDLER_init() {}

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
}

static void COMMANDHANDLER_handleReset(PROTOCOL_t *protocol) {}

static void COMMANDHANDLER_handleSyncTime(PROTOCOL_t *protocol) {}

static void COMMANDHANDLER_handleRequestVersion(PROTOCOL_t *protocol) {}

static void COMMANDHANDLER_handleRequestOTA(PROTOCOL_t *protocol) {}

static void
COMMANDHANDLER_handleRequestTransferOTADataSize(PROTOCOL_t *protocol) {}

static void COMMANDHANDLER_handleRequestTransferOTAData(PROTOCOL_t *protocol) {}

static void COMMANDHANDLER_handleSetting(PROTOCOL_t *protocol) {}

static void COMMANDHANDLER_handleDispenseCard(PROTOCOL_t *protocol) {}

static void COMMANDHANDLER_handleUpdateRFID(PROTOCOL_t *protocol) {}

static void COMMANDHANDLER_handlePlayAudio(PROTOCOL_t *protocol) {}

static void COMMANDHANDLER_handleControlIO(PROTOCOL_t *protocol) {}

static bool COMMANDHANDLER_isCommandValid(PROTOCOL_t *protocol) {}
