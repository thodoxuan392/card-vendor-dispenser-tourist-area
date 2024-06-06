# Communication Protocol over UART

[Encrypt](#encrypt-future-work)  
[Protocol Format](#protocol-format)

-   [Command](#command-android-to-controllerbox)
-   [Status](#status-from-controllerbox-to-phone)
-   [Event](#event-from-controllerbox-to-phone)
-   [ResultCode](#result-code)

## Encrypt (Future Work)

This feature will be implemented when this product go to mass production.

## Protocol Format

| START_BYTE (1 byte) | PROTOCOL_ID (1 byte) | DATA_LEN (1 byte) | DATA[0:DATA_LEN] (Maximum 255 bytes) | CHECK_SUM (2 bytes - Algorithms CRC-16/ARC) | STOP_BYTE (1 byte) |
| ------------------- | -------------------- | ----------------- | ------------------------------------ | ------------------------------------------- | ------------------ |
| 0x78                | XX                   | XX                | XX[0:DATA_LEN]                       | XX                                          | 0x79               |

1. Request
2. Response (Indicate Android/ControllerBox processed message)

## Command (Android to ControllerBox)

1. **Reset (Protocol 0x01)**

    - Request:
      | START_BYTE | PROTOCOL_ID | DATA_LEN | CHECK_SUM | STOP_BYTE |
      | ---------- | ----------- | -------- | ---------------- | --------- |
      | 0x78 | 0x01 | 0 | ${computationByCrc16Arc} | 0x79 |
    - Response:
      | START_BYTE | PROTOCOL_ID | DATA_LEN |RESULT_CODE| CHECK_SUM | STOP_BYTE |
      | ---------- | ----------- | -------- | -------- | ---------------- | --------- |
      | 0x78 | 0x01 | 1 | ${ResultCode} |${computationByCrc16Arc} | 0x79 |

2. **Request Version (Protocol 0x02)**

    - Request:
      | START_BYTE | PROTOCOL_ID | DATA_LEN | CHECK_SUM | STOP_BYTE |
      | ---------- | ----------- | -------- | ---------------- | --------- |
      | 0x78 | 0x02 | 0 | ${computationByCrc16Arc} | 0x79 |
    - Response:
      | START_BYTE | PROTOCOL_ID | DATA_LEN |RESULT_CODE| FIRMWARE_VERSION[0:2] | BOARD_VERSION[0:2]|CHECK_SUM | STOP_BYTE |
      | ---------- | -----------| -------- | -------- | -------- | -------- | ---------------- | --------- |
      | 0x78 | 0x02 | 7 | ${ResultCode} |${firmwareVersion} | ${boardVersion} |${computationByCrc16Arc} | 0x79 |

    - Firmware Version: Major (Index 0), Minor (Index 1), Patch (Index 2)
    - Board Version: Major (Index 0), Minor (Index 1), Patch (Index 2)

3. **Setting (Protocol 0x03)**

    Android will send the setting to the ControllerBox including:

    - Request:
      | START_BYTE | PROTOCOL_ID | DATA_LEN | ENABLE_STS_POLLING | CHECK_SUM | STOP_BYTE |
      | ---------- | ----------- | -------- |-------- | -------- | ---------------- |
      | 0x78 | 0x03 | 1| ${enableStsPolling} | | ${computationByCrc16Arc} | 0x79 |
    - Response:
      | START_BYTE | PROTOCOL_ID | DATA_LEN | RESULT_CODE | CHECK_SUM | STOP_BYTE |
      | ---------- | ----------- | -------- | ------------- | ------------------------ | --------- |
      | 0x78 | 0x03 | 1 | ${ResultCode} | ${computationByCrc16Arc} | 0x79 |

    - EnableStsPolling: 0 - disable polling report status, 1 - enable polling report status

4. **Dispense Card (Protocol 0x11)**

    Android will send the request to dispense Card

    - Request:
      | START_BYTE | PROTOCOL_ID | DATA_LEN | CHECK_SUM | STOP_BYTE |
      | ---------- | ----------- | -------- | -------- | ---------------- |
      | 0x78 | 0x11 | 1 | | ${computationByCrc16Arc} | 0x79 |
    - Response:
      | START_BYTE | PROTOCOL_ID | DATA_LEN | RESULT_CODE | CHECK_SUM | STOP_BYTE |
      | ---------- | ----------- | -------- | ------------- | ------------------------ | --------- |
      | 0x78 | 0x11 | 1 | ${ResultCode} | ${computationByCrc16Arc} | 0x79 |

    - tcdId: Index of TCD where Card will be dispensed

5. **Update RFID (Protocol 0x12)**

    Android will send the request to update data to RFID Card.

    - Request:
      | START_BYTE | PROTOCOL_ID | DATA_LEN | RFID_INDEX | RFID_LEN | RFID[RFID_LEN-1:0] | MONEY[3:0] | ISSUE_DATE[2:0] | CHECK_SUM | STOP_BYTE |
      | ---------- | ----------- | -------- |-------- | -------- | ---------------- |-------- | -------- | ---------------- |---------------- |
      | 0x78 | 0x12 | rfidLen + 9 | ${rfidIndex}| ${rfidLen} |${rfid} |${money} |${issueDate} | | ${computationByCrc16Arc} | 0x79 |
    - Response:
      | START_BYTE | PROTOCOL_ID | DATA_LEN | RESULT_CODE | CHECK_SUM | STOP_BYTE |
      | ---------- | ----------- | -------- | ------------- | ------------------------ | --------- |
      | 0x78 | 0x12 | 1 | ${ResultCode} | ${computationByCrc16Arc} | 0x79 |

    - rfidIndex: Index of RFID module
    - rfidLen: Number of bytes of Rfid ID
    - rfid: Id of RFID
    - money: Amount of RFID
    - issueDate: the Date when RFID was issued

6. **Play Audio (Protocol 0x13)**

    Android request Controller to play audio.

    - Request:
      | START_BYTE | PROTOCOL_ID | DATA_LEN | AUDIO_INDEX | CHECK_SUM | STOP_BYTE |
      | ---------- | ----------- | -------- |-------- | -------- | ---------------- |
      | 0x78 | 0x13 | 1 | ${audioIndex} | ${computationByCrc16Arc} | 0x79 |
    - Response:
      | START_BYTE | PROTOCOL_ID | DATA_LEN | RESULT_CODE | CHECK_SUM | STOP_BYTE |
      | ---------- | ----------- | -------- | ------------- | ------------------------ | --------- |
      | 0x78 | 0x13 | 1 | ${ResultCode} | ${computationByCrc16Arc} | 0x79 |

    - audioIndex: Index of Audio in ControllerBox

7. **Control IO (Protocol 0x14)**

    Android request Controller to play audio.

    - Request:
      | START_BYTE | PROTOCOL_ID | DATA_LEN | SELECT_MASK | LEVEL_MASK | CHECK_SUM | STOP_BYTE |
      | ---------- | ----------- | -------- | -------- |-------- | -------- | ---------------- |
      | 0x78 | 0x14 | 2 | ${selectMask} | ${levelMask} | ${computationByCrc16Arc} | 0x79 |
    - Response:
      | START_BYTE | PROTOCOL_ID | DATA_LEN | RESULT_CODE | CHECK_SUM | STOP_BYTE |
      | ---------- | ----------- | -------- | ------------- | ------------------------ | --------- |
      | 0x78 | 0x14 | 1 | ${ResultCode} | ${computationByCrc16Arc} | 0x79 |

    - selectMask: Mask of IOs are selected to control
    - levelMask: Mask of IOs level

## Status (From ControllerBox to Phone)

1. **Device Error (Protocol 0x30)**

    - Request:
      | START_BYTE | PROTOCOL_ID | DATA_LEN | ERROR_FLAG[1:0]| CHECK_SUM | STOP_BYTE |
      | ---------- | ----------- | -------- | ----------- | -------- | ----------- |
      | 0x78 | 0x37 | 1 | ${errorFlag}| ${computationByCrc16Arc} | 0x79 |

    - Error Flag (16bits): Bit Mask
    - Bit 0: BILL_ACCEPTOR_ERROR
    - Bit 1: EEPROM_ERROR
    - Bit 2: KEYPAD_ERROR
    - Bit 3: LED_ERROR
    - Bit 4: RFID_ERROR
    - Bit 5: SIM_ERROR
    - Bit 6: SOUND_ERROR
    - Bit 7: TCD_ERROR
    - Bit 8: WIFI_ERROR

2. **Bill Acceptor Status (Protocol 0x31)**

    - Request:
      | START_BYTE | PROTOCOL_ID | DATA_LEN | BILL_ACCEPTOR_STS| CHECK_SUM | STOP_BYTE |
      | ---------- | ----------- | -------- | ----------- | -------- | ----------- |
      | 0x78 | 0x37 | 1 | ${billAcceptorSts}| ${computationByCrc16Arc} | 0x79 |

    - BILL_ACCEPTOR_STS_SUCCESS = 0x00
    - BILL_ACCEPTOR_STS_DEFECTIVE_MOTOR = 0x01
    - BILL_ACCEPTOR_STS_SENSOR_PROBLEM = 0x02
    - BILL_ACCEPTOR_STS_VALIDATOR_BUSY = 0x03
    - BILL_ACCEPTOR_STS_ROM_CHECKSUM_ERROR = 0x04
    - BILL_ACCEPTOR_STS_VALIDATOR_JAMMED = 0x05
    - BILL_ACCEPTOR_STS_VALIDATOR_WAS_RESET = 0x06
    - BILL_ACCEPTOR_STS_BILL_REMOVED = 0x07
    - BILL_ACCEPTOR_STS_CASHBOX_OUTOF_POSITION = 0x08
    - BILL_ACCEPTOR_STS_VALIDATOR_DISABLE = 0x09
    - BILL_ACCEPTOR_STS_INVALID_ESCROW_REQ = 0x0A
    - BILL_ACCEPTOR_STS_BILL_REJECTED = 0x0B
    - BILL_ACCEPTOR_STS_POSSIBLE_CREDITED_BILL_REMOVAL = 0x0C
    - BILL_ACCEPTOR_STS_NB_ATTEMP_INPUT_BILL = 0x0D

3. **TCD Status (Protocol 0x32)**

    - Request:
      | START_BYTE | PROTOCOL_ID | DATA_LEN | TCD_INDEX | IS_ERROR | IS_LOWER | IS_EMPTY| CHECK_SUM | STOP_BYTE |
      | ---------- | ----------- | -------- | ----------- | -------- | ----------- | ----------- | -------- | ----------- |
      | 0x78 | 0x37 | 4 | ${tcdIndex}| ${isError}|${isLower}|${isEmpty}| ${computationByCrc16Arc} | 0x79 |

    - isError: Indicate that TCD at tcdIndex is being error.
    - isLower: Indicate that Card remaining on TCD at tcdIndex is being low.
    - isEmpty: Indicate that Card remaining on TCD at tcdIndex is being empty

## Event (From ControllerBox to Phone)

1. **RFID Detected (Protocol 0x51)**

    - Request:
    - Request:
      | START_BYTE | PROTOCOL_ID | DATA_LEN | RFID_LEN | RFID[RFID_LEN-1:0] | MONEY[3:0] | ISSUE_DATE[2:0] | ERROR | CHECK_SUM | STOP_BYTE |
      | ---------- | ----------- | -------- | -------- |-------- | -------- | ---------------- |-------- | -------- | ---------------- |
      | 0x78 | 0x50 | rfidLen + 8 | ${rfidLen} |${rfid} |${money} |${issueDate} | {issueDate} | | ${computationByCrc16Arc} | 0x79 |

    - rfidLen: Number of bytes of Rfid ID
    - rfid: Id of RFID
    - money: Amount of RFID
    - issueDate: the Date when RFID was issued
    - error: The Error when RFID detected
        - RFID_SUCCESS = 0x00
        - RFID_INVALID_FORMAT = 0x71
        - RFID_AUTHEN_FAILED = 0x73

2. **Bill Accepted (Protocol 0x51)**

    - Request:
    - Request:
      | START_BYTE | PROTOCOL_ID | DATA_LEN | BILL_VALUE[3:0] | BILl_STATUS | CHECK_SUM | STOP_BYTE |
      | ---------- | ----------- | -------- | -------- |-------- | -------- | ---------------- |
      | 0x78 | 0x51 | 4 | ${billValue} | | ${computationByCrc16Arc} | 0x79 |

    - rfidLen: Number of bytes of Rfid ID
    - rfid: Id of RFID
    - money: Amount of RFID
    - issueDate: the Date when RFID was issued
    - status: Reference to the status of Bill Acceptor Status at **Status/2.BillAcceptorStatus**

3. **Card Event (Protocol 0x52)**

    - Request:
      | START_BYTE | PROTOCOL_ID | DATA_LEN | TCD_INDEX | DIRECTION | CHECK_SUM | STOP_BYTE |
      | ---------- | ----------- | -------- | ----------- |----------- | -------- | ----------- |
      | 0x78 | 0x52 | 1 | ${tcdIndex}| ${direction}| ${computationByCrc16Arc} | 0x79 |

    - tcdIndex: Index of TCD
    - direction:
        - DISPENSE_OUT = 0
        - CALLBACK = 1

## Result Code

| RESULT_CODE | Description                                  |
| ----------- | -------------------------------------------- |
| 0x00        | RESULT_SUCCESS                               |
| 0x01        | RESULT_ERROR                                 |
| 0x02        | RESULT_COMM_PROTOCOL_ID_INVALID              |
| 0x03        | RESULT_COMM_PROTOCOL_CRC_INVALID             |
| 0x04        | RESULT_COMM_PROTOCOL_START_STOP_BYTE_INVALID |
| 0x05        | RESULT_COMM_PROTOCOL_TIMEOUT                 |
| 0x11        | RESULT_CMD_RESET_TIMEOUT                     |
| 0x12        | RESULT_CMD_TRANSFER_OTA_DATA_IN_PROGRESS     |
| 0x13        | RESULT_CMD_TRANSFER_OTA_DATA                 |
