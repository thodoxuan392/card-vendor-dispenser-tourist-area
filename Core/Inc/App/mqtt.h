/*
 * mqtt.h
 *
 *  Created on: May 10, 2023
 *      Author: xuanthodo
 */

#ifndef INC_APP_MQTT_H_
#define INC_APP_MQTT_H_

#include "interface.h"

typedef struct {
    Interface_Type_t type; 
    union {
        MONEY_DETECTED_Interface_t money_detected;
        CARD_DETECTED_Interface_t card_detected;
        STATUS_REPORT_Interface_t status_report;
        SYSTEM_RESET_Interface_t system_reset;
        FOTA_Interface_t fota;
    }data;
}MQTT_message_t;

void MQTT_init();
void MQTT_loop();
bool MQTT_send_message(MQTT_message_t * message);
bool MQTT_receive_message(MQTT_message_t * message);

#endif /* INC_APP_MQTT_H_ */
