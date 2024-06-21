/**
 * @file rs485.c
 * @author Xuan Tho Do (tho.dok17@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-03-01
 *
 * @ref
 * - CRC16/ARC https://github.com/naver/nbase-arc/blob/master/api/arcci/crc16.c
 */

#include <Tp/rs485.h>

#include <App/crc.h>

#include <string.h>

#include <Hal/uart.h>
#include <Hal/timer.h>
#include <utils/utils_logger.h>

#define RS485_UART_ID UART_1
#define RS485_RX_MESSAGE_QUEUE_MAX_SIZE 5
#define RS485_TX_BUFFER_MAX 1024
#define RS485_RX_BUFFER_MAX 1024
#define RS485_RX_TIMEOUT 500

static void RS485_MessageimerInterrupt1ms(void);
static bool RS485_parse(uint8_t* data, size_t data_len, RS485_Message* message);
static uint16_t RS485_calCheckSum(uint8_t* data, uint8_t data_len);
static void RS485_serialize(RS485_Message* message, uint8_t* data, size_t* data_len);

static RS485_Message RS485_message;
static RS485_Message RS485_rxQueue[RS485_RX_MESSAGE_QUEUE_MAX_SIZE];
static uint8_t RS485_rxQueueTail = 0;
static uint8_t RS485_rxQueueHead = 0;

static uint8_t RS485_rxBuffer[RS485_TX_BUFFER_MAX];
static uint32_t RS485_rxBufferLen = 0;
static uint8_t RS485_MessagexBuffer[RS485_RX_BUFFER_MAX];
static uint32_t RS485_rxTimeCnt = 0;

static void RS485_onUARTCallback(uint8_t* data, uint32_t dataSize);

void RS485_init(void)
{
	TIMER_attach_intr_1ms(RS485_MessageimerInterrupt1ms);
	UART_set_callback(RS485_UART_ID, RS485_onUARTCallback);
}

bool RS485_send(RS485_Message* message)
{
	size_t tx_len;
	RS485_serialize(message, RS485_MessagexBuffer, &tx_len);
	return UART_send(RS485_UART_ID, RS485_MessagexBuffer, tx_len);
}

bool RS485_receive(RS485_Message* message)
{
	if(RS485_rxQueueHead != RS485_rxQueueTail)
	{
		memcpy(message, &RS485_rxQueue[RS485_rxQueueTail], sizeof(RS485_Message));
		RS485_rxQueueTail = (RS485_rxQueueTail + 1) % RS485_RX_MESSAGE_QUEUE_MAX_SIZE;
		return true;
	}
	return false;
}

static void RS485_MessageimerInterrupt1ms(void)
{
	if(RS485_rxTimeCnt > 0)
	{
		RS485_rxTimeCnt--;
		if(RS485_rxTimeCnt == 0)
		{
			RS485_rxBufferLen = 0;
		}
	}
}

static bool RS485_parse(uint8_t* data, size_t data_len, RS485_Message* message)
{
	if(data[0] != RS485_MESSAGE_START_BYTE)
	{
		return false;
	}
	RS485_NetworkId networkId = data[1];
	RS485_NodeId srcNode = data[2];
	RS485_NodeId desNode = data[3];
	RS485_MessageId messageId = data[4];
	RS485_ResultCode resultCode = data[5];

	uint8_t dataL = data[6];
	uint16_t expectedChecksum = RS485_calCheckSum(&data[7], dataL);
	uint16_t checksum = ((uint16_t)data[7 + dataL] << 8) | data[8 + dataL];
	if(expectedChecksum != checksum)
	{
		return false;
	}
	if(data[9 + dataL] != RS485_MESSAGE_STOP_BYTE)
	{
		return false;
	}
	if(data_len < 10 + dataL)
	{
		return false;
	}
	message->networkId = networkId;
	message->srcNode = srcNode;
	message->desNode = desNode;
	message->messageId = messageId;
	message->resultCode = resultCode;
	memcpy(message->data, &data[7], dataL);
	message->dataLen = dataL;
	return true;
}

static uint16_t RS485_calCheckSum(uint8_t* data, uint8_t data_len)
{
	return CRC_calculate(data, data_len);
}

static void RS485_serialize(RS485_Message* message, uint8_t* data, size_t* data_len)
{
	uint8_t data_len_temp = 0;
	data[data_len_temp++] = RS485_MESSAGE_START_BYTE;
	data[data_len_temp++] = message->networkId;
	data[data_len_temp++] = message->srcNode;
	data[data_len_temp++] = message->desNode;
	data[data_len_temp++] = message->messageId;
	data[data_len_temp++] = message->dataLen;
	for(int var = 0; var < message->dataLen; ++var)
	{
		data[data_len_temp++] = message->data[var];
	}
	uint16_t checksum = RS485_calCheckSum(message->data, message->dataLen);
	data[data_len_temp++] = (uint8_t)(checksum >> 8);
	data[data_len_temp++] = (uint8_t)(checksum & 0xFF);
	data[data_len_temp++] = RS485_MESSAGE_STOP_BYTE;
	*data_len = data_len_temp;
}

static void RS485_onUARTCallback(uint8_t* data, uint32_t dataSize)
{
	RS485_rxTimeCnt = RS485_RX_TIMEOUT;
	if(RS485_rxBufferLen + dataSize > RS485_RX_BUFFER_MAX)
	{
		utils_log_error("[PROTOCOL] Rx buffer overflow, cleaning up\r\n");
		RS485_rxBufferLen = 0;
	}

	memcpy(&RS485_rxBuffer[RS485_rxBufferLen], data, dataSize);
	RS485_rxBufferLen += dataSize;
	if(RS485_parse(RS485_rxBuffer, RS485_rxBufferLen, &RS485_message))
	{
		RS485_rxBufferLen = 0;
		RS485_rxQueue[RS485_rxQueueHead] = RS485_message;
		RS485_rxQueueHead = (RS485_rxQueueHead + 1) % RS485_RX_MESSAGE_QUEUE_MAX_SIZE;
	}
	else if(RS485_rxBufferLen > RS485_RX_BUFFER_MAX)
	{
		// Buffer is too big, but cannot parse -> Cleaning up
		RS485_rxBufferLen = 0;
	}
}
