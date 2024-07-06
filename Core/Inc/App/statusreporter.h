/*
 * status_reporter.h
 *
 *  Created on: Jun 2, 2023
 *      Author: xuanthodo
 */

#ifndef INC_APP_STATUSREPORTER_H_
#define INC_APP_STATUSREPORTER_H_

#include "stdio.h"
#include "stdbool.h"

#include "Device/rfid.h"
#include "Device/tcd.h"
#include "DeviceManager/tcdmanager.h"

#define DISPENSE_DIR_OUT    0
#define DISPENSE_DIR_IN     1

bool STATUSREPORTER_init();
bool STATUSREPORTER_run();

void STATUSREPORTER_sendAll(void);
void STATUSREPORTER_sendStsDeviceErr(uint16_t errorFlags);
void STATUSREPORTER_sendStsBillAcceptor(uint8_t billAcceptorStatus);
void STATUSREPORTER_sendStsTcd(TCD_id_t id, TCD_status_t tcdStatus);
void STATUSREPORTER_sendEvtRfidDetected(RFID_Id_t id, RFID_t *rfid);
void STATUSREPORTER_sendEvtBillAccepted(uint32_t billValue);
void STATUSREPORTER_sendEvtCard(TCD_id_t id, uint8_t cardDirection);


#endif /* INC_APP_STATUSREPORTER_H_ */
