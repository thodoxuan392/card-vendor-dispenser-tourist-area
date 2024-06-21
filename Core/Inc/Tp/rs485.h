/*
 * rs485pkt.h
 *
 *  Created on: Jun 17, 2024
 *      Author: xuanthodo
 */

#ifndef INC_APP_RS485_H_
#define INC_APP_RS485_H_

#include <stdint.h>
#include <stdbool.h>

#define RS485_NODE_ID_BROADCAST 0xFF
typedef uint8_t RS485_NodeId;

typedef uint8_t RS485_NetworkId;

#define RS485_MESSAGE_START_BYTE 0x88
#define RS485_MESSAGE_STOP_BYTE 0x89

typedef uint8_t RS485_MessageId;

#define RS485_MESSAGE_DATA_MAX_LEN 255

#define RS485_RESULT_CODE_SUCCESS 0x00
#define RS485_RESULT_CODE_FAILED 0x01
typedef uint8_t RS485_ResultCode;

typedef struct
{
	RS485_NetworkId networkId;
	RS485_NodeId srcNode;
	RS485_NodeId desNode;
	RS485_MessageId messageId;
	RS485_ResultCode resultCode;
	uint8_t dataLen;
	uint8_t data[RS485_MESSAGE_DATA_MAX_LEN];
} RS485_Message;

void RS485_init(void);
void RS485_deinit(void);
void RS485_test(void);

bool RS485_send(RS485_Message* message);
bool RS485_receive(RS485_Message* message);

#endif /* INC_APP_RS485_H_ */
