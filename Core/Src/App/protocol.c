/**
 * @file protocol.c
 * @author Xuan Tho Do (tho.dok17@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-03-01
 *
 * @copyright Copyright (c) 2024 Kinis AI
 * @ref
 * - CRC16/ARC https://github.com/naver/nbase-arc/blob/master/api/arcci/crc16.c
 */

#include <App/protocol.h>

#include <App/crc.h>

#include <string.h>

#include <Hal/usb.h>
#include <Hal/timer.h>
#include <utils/utils_logger.h>

#define PROTOCOL_RX_QUEUE_MAX_SIZE 5
#define PROTOCOL_BUFFER_MAX 128

static void PROTOCOL_timerInterrupt1ms(void);
static bool PROTOCOL_parse(uint8_t* data, size_t data_len, PROTOCOL_t* proto);
static uint16_t PROTOCOL_calCheckSum(uint8_t* data, uint8_t data_len);
static void PROTOCOL_serialize(PROTOCOL_t* proto, uint8_t* data, size_t* data_len);

static PROTOCOL_t PROTOCOL_message;
static PROTOCOL_t PROTOCOL_rxQueue[PROTOCOL_RX_QUEUE_MAX_SIZE];
static uint8_t PROTOCOL_rxQueueTail = 0;
static uint8_t PROTOCOL_rxQueueHead = 0;

static uint8_t PROTOCOL_rxBuffer[RX_BUFFER_MAX_LENGTH];
static uint32_t PROTOCOL_rxBufferLen = 0;
static uint8_t PROTOCOL_txBuffer[TX_BUFFER_MAX_LENGTH];
static uint32_t PROTOCOL_rxTimeCnt = 0;

static void PROTOCOL_onUARTCallback(uint8_t* data, uint32_t dataSize);

void PROTOCOL_init(void)
{
	TIMER_attach_intr_1ms(PROTOCOL_timerInterrupt1ms);
	USB_setReceiveCallback(PROTOCOL_onUARTCallback);
}

bool PROTOCOL_send(PROTOCOL_t* proto)
{
	size_t tx_len;
	PROTOCOL_serialize(proto, PROTOCOL_txBuffer, &tx_len);
	return USB_send(PROTOCOL_txBuffer, tx_len);
}

bool PROTOCOL_receive(PROTOCOL_t* proto)
{
	if(PROTOCOL_rxQueueHead != PROTOCOL_rxQueueTail){
		memcpy(proto, &PROTOCOL_rxQueue[PROTOCOL_rxQueueTail], sizeof(PROTOCOL_t));
		PROTOCOL_rxQueueTail = (PROTOCOL_rxQueueTail + 1) % PROTOCOL_RX_QUEUE_MAX_SIZE;
		return true;
	}
	return false;
}

static void PROTOCOL_timerInterrupt1ms(void)
{
	if(PROTOCOL_rxTimeCnt > 0)
	{
		PROTOCOL_rxTimeCnt--;
		if(PROTOCOL_rxTimeCnt == 0)
		{
			PROTOCOL_rxBufferLen = 0;
		}
	}
}

static bool PROTOCOL_parse(uint8_t* data, size_t data_len, PROTOCOL_t* proto)
{
	if(data[0] != START_BYTE)
	{
		return false;
	}
	uint8_t dataL = data[2];
	uint16_t expectedChecksum = PROTOCOL_calCheckSum(&data[3], dataL);
	uint16_t checksum = ((uint16_t)data[3 + dataL] << 8) | data[4 + dataL];
	if(expectedChecksum != checksum)
	{
		return false;
	}
	if(data[5 + dataL] != STOP_BYTE)
	{
		return false;
	}
	if(data_len < 6 + dataL)
	{
		return false;
	}
	proto->protocol_id = data[1];
	memcpy(proto->data, &data[3], dataL);
	proto->data_len = dataL;
	return true;
}

static uint16_t PROTOCOL_calCheckSum(uint8_t* data, uint8_t data_len)
{
	return CRC_calculate(data, data_len);
}

static void PROTOCOL_serialize(PROTOCOL_t* proto, uint8_t* data, size_t* data_len)
{
	uint8_t data_len_temp = 0;
	data[data_len_temp++] = START_BYTE;
	data[data_len_temp++] = proto->protocol_id;
	data[data_len_temp++] = proto->data_len;
	for(int var = 0; var < proto->data_len; ++var)
	{
		data[data_len_temp++] = proto->data[var];
	}
	uint16_t checksum = PROTOCOL_calCheckSum(proto->data, proto->data_len);
	data[data_len_temp++] = (uint8_t)(checksum >> 8);
	data[data_len_temp++] = (uint8_t)(checksum & 0xFF);
	data[data_len_temp++] = STOP_BYTE;
	*data_len = data_len_temp;
}

static void PROTOCOL_onUARTCallback(uint8_t* data, uint32_t dataSize)
{
	PROTOCOL_rxTimeCnt = RX_TIMEOUT_MS;
	if(PROTOCOL_rxBufferLen + dataSize > RX_BUFFER_MAX_LENGTH)
	{
		utils_log_error("[PROTOCOL] Rx buffer overflow, cleaning up\r\n");
		PROTOCOL_rxBufferLen = 0;
	}

	memcpy(&PROTOCOL_rxBuffer[PROTOCOL_rxBufferLen], data, dataSize);
	PROTOCOL_rxBufferLen += dataSize;
	if(PROTOCOL_parse(PROTOCOL_rxBuffer, PROTOCOL_rxBufferLen, &PROTOCOL_message))
	{
		PROTOCOL_rxBufferLen = 0;
		PROTOCOL_rxQueue[PROTOCOL_rxQueueHead] = PROTOCOL_message;
		PROTOCOL_rxQueueHead = (PROTOCOL_rxQueueHead + 1) % PROTOCOL_RX_QUEUE_MAX_SIZE;
	}
	else if(PROTOCOL_rxBufferLen > PROTOCOL_BUFFER_MAX)
	{
		// Buffer is too big, but cannot parse -> Cleaning up
		PROTOCOL_rxBufferLen = 0;
	}
}
