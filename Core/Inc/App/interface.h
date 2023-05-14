/*
 * interface.h
 *
 *  Created on: May 10, 2023
 *      Author: xuanthodo
 */

#include <Device/bill_acceptor.h>
#include <Device/tcd.h>


#define FOTA_DATA_SIZE  512

typedef enum {
    MONEY_DETECTED,
    CARD_DETECTED,
    STATUS_REPORT,
    SYSTE_RESET,
    FOTA
}Interface_Type_t;

typedef struct {
    MONEY_info_t info;
}MONEY_DETECTED_Interface_t;

typedef struct {
    CARD_info_t info;
}CARD_DETECTED_Interface_t;

typedef struct {
    bool card_scanner_err;
    bool money_scanner_err;
}STATUS_REPORT_Interface_t;

typedef struct {

}SYSTEM_RESET_Interface_t;

typedef struct {
    uint8_t seq;
    uint8_t checksum;
    uint8_t datalen;
    uint8_t data[FOTA_DATA_SIZE];
}FOTA_Interface_t;