/*
 * rfid.c
 *
 *  Created on: Dec 17, 2023
 *      Author: xuanthodo
 */


#include <Device/rfid.h>

#include <PN532/PN532Debug.h>
#include <PN532/PN532.h>

#include <Hal/spi.h>


//#define RFID_V1
#define RFID_V2

#if defined(RFID_V1)
#define RFID_PN532_DATA_BLOCK	4
#elif defined(RFID_V2)
#define RFID_PN532_DATA_BLOCK	8
#endif

typedef struct {
	PN532_Handle pn532_handle;
	RFID_Error_t error;
	bool isPlaced;
	bool isDetected;
	RFID_t rfid;
}RFID_Handle;

static void SPI_CS1_write(const uint8_t *data, const uint32_t data_len);
static void SPI_CS1_writeThenRead(uint8_t* tx_buf, uint32_t tx_len, uint8_t *rx_buf, uint32_t rx_len);
static void SPI_CS2_write(const uint8_t *data, const uint32_t data_len);
static void SPI_CS2_writeThenRead(uint8_t* tx_buf, uint32_t tx_len, uint8_t *rx_buf, uint32_t rx_len);
static void SPI_CS3_write(const uint8_t *data, const uint32_t data_len);
static void SPI_CS3_writeThenRead(uint8_t* tx_buf, uint32_t tx_len, uint8_t *rx_buf, uint32_t rx_len);

static void RFID_runById(RFID_Id_t id);
static void RFID_buildToBlockData(RFID_t *rfid, uint8_t *data, uint32_t data_len);
static bool RFID_parseFromBlockData(uint8_t *data, uint32_t data_len, RFID_t *rfid);
static uint8_t RFID_getIdKey(uint8_t *id, uint32_t id_len);
static uint16_t RFID_getBlockDataKey(uint8_t *data, uint32_t data_size);
#if defined(RFID_V2)
static uint32_t RFID_volumeToMoney(uint16_t volume);
static uint16_t RFID_moneyToVolume(uint32_t money);
#endif


#if defined(RFID_V1)
static const uint8_t RFID_key[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
#elif defined(RFID_V2)
//static const uint8_t RFID_key[] = { 0xFF, 0xFF, 0x07, 0x09, 0x06, 0x08 };
static const uint8_t RFID_key[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
//static const uint8_t RFID_key[] = { 0x08, 0x06, 0x09, 0x07, 0xFF, 0xFF };
#endif

static RFID_Handle RFID_handleTable[] = {
	[RFID_ID_1] = {
			.pn532_handle = {
				.interface = {
							.write = SPI_CS1_write,
							.write_then_read = SPI_CS1_writeThenRead
				},
			},
	},
	[RFID_ID_2] = {
			.pn532_handle = {
				.interface = {
							.write = SPI_CS2_write,
							.write_then_read = SPI_CS2_writeThenRead
				},
			},
	},
	[RFID_ID_3] = {
			.pn532_handle = {
				.interface = {
							.write = SPI_CS3_write,
							.write_then_read = SPI_CS3_writeThenRead
				},
			},
	}
};

void RFID_init(void){
	for (uint8_t i = 0; i < RFID_ID_MAX; i++)
	{
		PN532_begin(&RFID_handleTable[i].pn532_handle);
	}
}

void RFID_run(void){
	RFID_runById(RFID_ID_1);
	RFID_runById(RFID_ID_2);
	RFID_runById(RFID_ID_3);
}


void RFID_get(RFID_Id_t id, RFID_t * rfid){
	memcpy(rfid, &RFID_handleTable[id].rfid, sizeof(RFID_t));
}

bool RFID_isPlaced(RFID_Id_t id){
	return RFID_handleTable[id].isPlaced;
}

bool RFID_isDetected(RFID_Id_t id){
	return RFID_handleTable[id].isDetected;
}

void RFID_clearDetected(RFID_Id_t id){
	RFID_handleTable[id].isDetected = false;
}

RFID_Error_t RFID_set(RFID_Id_t id, RFID_t * rfid){
	// Check RFID is available
	if(!RFID_handleTable[id].isPlaced){
		return RFID_ERROR_NOT_AVAILABLE;
	}
	// Check RFID uid is matched
	if(rfid->id_len != RFID_handleTable[id].rfid.id_len){
		return RFID_ERROR_INVALID_FORMAT;
	}
	if(memcmp(rfid->id, RFID_handleTable[id].rfid.id, rfid->id_len) != 0){
		return RFID_ERROR_ID_NOT_MATCHED;
	}
	uint8_t writeData[16] = {0};
	RFID_buildToBlockData(rfid, writeData, sizeof(writeData));
	if(!PN532_readPassiveTargetID(&RFID_handleTable[id], PN532_MIFARE_ISO14443A, rfid->id, &rfid->id_len, 100)){
		return RFID_ERROR_NOT_AVAILABLE;
	}
	if(!PN532_mifareclassic_AuthenticateBlock(&RFID_handleTable[id], rfid->id, rfid->id_len, RFID_PN532_DATA_BLOCK, 1, RFID_key)){
		return RFID_ERROR_AUTHEN_FAILED;
	}
	if(!PN532_mifareclassic_WriteDataBlock(&RFID_handleTable[id], RFID_PN532_DATA_BLOCK , writeData)){
		return RFID_ERROR_CANNOT_WRITE;
	}
	return RFID_SUCCESS;
}

bool RFID_isError(RFID_Id_t id){
	return RFID_handleTable[id].error != RFID_SUCCESS;
}

RFID_Error_t RFID_getError(RFID_Id_t id){
	return RFID_handleTable[id].error;
}

void RFID_test(void){
	RFID_t rfid = {
		.id = {163, 52, 18, 8},
		.id_len = 4,
		.volume = 200000,
		.issueDate = { 24, 1, 7},
	};
	while(1){
		HAL_Delay(1000);
		RFID_set(RFID_ID_1, &rfid);
	}
}

static void RFID_runById(RFID_Id_t id){
	uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
	uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
	uint8_t readBlockData[16] = {0};
	if(!PN532_readPassiveTargetID(&RFID_handleTable[id], PN532_MIFARE_ISO14443A, uid, &uidLength, 100)){
		RFID_handleTable[id].isPlaced = false;
		return;
	}
	if(!RFID_handleTable[id].isPlaced){
		RFID_handleTable[id].isDetected = true;
	}
	RFID_handleTable[id].isPlaced = true;
	if(!PN532_mifareclassic_AuthenticateBlock(&RFID_handleTable[id], uid, uidLength, RFID_PN532_DATA_BLOCK, 1, RFID_key)){
		RFID_handleTable[id].rfid.isValid = false;
		return;
	}
	if(!PN532_mifareclassic_ReadDataBlock(&RFID_handleTable[id], RFID_PN532_DATA_BLOCK, readBlockData)){
		RFID_handleTable[id].rfid.isValid = false;
		return;
	}
	// Assign uuid to RFID
	RFID_t rfid;
	rfid.id_len = uidLength;
	memcpy(rfid.id, uid, uidLength);

	// Parse RFID
	if(!RFID_parseFromBlockData(readBlockData, sizeof(readBlockData), &rfid)){
		RFID_handleTable[id].rfid.isValid = false;
		RFID_handleTable[id].error = RFID_ERROR_INVALID_FORMAT;
		return;
	}
	RFID_handleTable[id].error = RFID_SUCCESS;
	RFID_handleTable[id].rfid = rfid;
}

static void SPI_CS1_write(const uint8_t *data, const uint32_t data_len){
	SPI_transmit(SPI_CS_1, data, data_len, 1000);
}
static void SPI_CS1_writeThenRead(uint8_t* tx_buf, uint32_t tx_len, uint8_t *rx_buf, uint32_t rx_len){
	SPI_transmitAndReceive(SPI_CS_1, tx_buf, tx_len, rx_buf, rx_len, 1000);
}
static void SPI_CS2_write(const uint8_t *data, const uint32_t data_len){
	SPI_transmit(SPI_CS_2, data, data_len, 1000);
}
static void SPI_CS2_writeThenRead(uint8_t* tx_buf, uint32_t tx_len, uint8_t *rx_buf, uint32_t rx_len){
	SPI_transmitAndReceive(SPI_CS_2, tx_buf, tx_len, rx_buf, rx_len, 1000);
}
static void SPI_CS3_write(const uint8_t *data, const uint32_t data_len){
	SPI_transmit(SPI_CS_3, data, data_len, 1000);
}
static void SPI_CS3_writeThenRead(uint8_t* tx_buf, uint32_t tx_len, uint8_t *rx_buf, uint32_t rx_len){
	SPI_transmitAndReceive(SPI_CS_3, tx_buf, tx_len, rx_buf, rx_len, 1000);
}

static void RFID_buildToBlockData(RFID_t *rfid, uint8_t *data, uint32_t data_len){
#if defined(RFID_V1)
	if(data_len != 16){
		return;
	}
	uint8_t data_len_out = 0;
	data[data_len_out++] = (rfid->volume >> 24) & 0xFF;
	data[data_len_out++] = (rfid->volume >> 16) & 0xFF;
	data[data_len_out++] = (rfid->volume >> 8) & 0xFF;
	data[data_len_out++] = rfid->volume & 0xFF;
	data[data_len_out++] = rfid->issueDate[0];
	data[data_len_out++] = rfid->issueDate[1];
	data[data_len_out++] = rfid->issueDate[2];
	data[data_len_out++] = rfid->expireDate[0];
	data[data_len_out++] = rfid->expireDate[1];
	data[data_len_out++] = rfid->expireDate[2];

	uint16_t dataKey = RFID_getBlockDataKey(data, data_len_out);
	data[data_len_out++] = (dataKey >> 8 & 0xFF);
	data[data_len_out++] = dataKey & 0xFF;

	uint8_t idKey = RFID_getIdKey(rfid->id, rfid->id_len);
	data[data_len_out++] = idKey;
#elif defined(RFID_V2)
	uint8_t data_len_out = 0;
	uint16_t volume = (rfid->volume * 10) / 1000;
	data[data_len_out++] = (volume >> 8) & 0xFF;
	data[data_len_out++] = volume & 0xFF;
	data[data_len_out++] = rfid->issueDate[0];
	data[data_len_out++] = rfid->issueDate[1];

	uint16_t issueYear = rfid->issueDate[2] + 2000;
	data[data_len_out++] = (issueYear >> 8) & 0xFF;
	data[data_len_out++] = issueYear & 0xFF;
#endif
}

static bool RFID_parseFromBlockData(uint8_t *data, uint32_t data_len, RFID_t *rfid){
#if defined(RFID_V1)
	if(data_len != 16){
		return false;
	}
	rfid->volume = (uint32_t)data[0] << 24  |
					(uint32_t)data[1] << 16  |
					(uint32_t)data[2] << 8  |
					data[3];
	rfid->issueDate[0] = data[4];
	rfid->issueDate[1] = data[5];
	rfid->issueDate[2] = data[6];
	rfid->expireDate[0] = data[7];
	rfid->expireDate[1] = data[8];
	rfid->expireDate[2] = data[9];


	uint16_t dataKey = RFID_getBlockDataKey(data, 10);
	uint16_t expectedDataKey = ((uint16_t)data[10] << 8) | data[11];
	if(dataKey != expectedDataKey){
		return false;
	}

	uint8_t idKey = data[12];
	uint8_t expectedIdKey = RFID_getIdKey(rfid->id, rfid->id_len);

	if(idKey != expectedIdKey){
		return false;
	}
	return true;
#elif defined(RFID_V2)
	if(data_len != 16){
		return false;
	}
	rfid->isValid = true;
	uint16_t volume = (uint16_t)data[0] << 8  |
						data[1];
	rfid->volume = (volume * 1000 ) / 10;
	rfid->issueDate[0] = data[2];
	rfid->issueDate[1] = data[3];
	rfid->issueDate[2] = ((uint16_t)data[4] << 8  | data[5]) - 2000;
	return true;
#endif
}

static uint16_t RFID_getBlockDataKey(uint8_t *data, uint32_t data_size){
	uint8_t xor = 0xFF;
	uint8_t sum = 0;
	for (int i = 0; i < data_size; ++i) {
		xor ^= data[i];
		sum += data[i];
	}
	uint16_t ret = ((uint16_t)xor << 8) | sum;
	return ret;
}


static uint8_t RFID_getIdKey(uint8_t *id, uint32_t id_len){
	uint8_t id_key = 0xFF;
	for (int i = 0; i < id_len; ++i) {
		id_key ^= id[i];
	}
	return id_key;
}


