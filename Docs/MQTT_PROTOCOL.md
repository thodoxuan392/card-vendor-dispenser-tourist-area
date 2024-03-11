# Card Vendor Interface over MQTT

This document will describe the interface of Card Vendor Machine communicated over the MQTT protocol.

## Protocol Specification

-   MQTT 3.1 over TCP
-   Using Credential (username, password)
-   Topic Pattern: \${model}/\${deviceId}/\${action}/\${data}.  
     e.g: For Report Machine Status: cardvendor/123/rp/status
-   Payload: JSON format

## Machine Status

-   From Device To Server
-   Topic: **cardvendor/\${deviceId}/rp/status**
-   Payload:

| Field    | Type   | Value                                      | Description                           |
| -------- | ------ | ------------------------------------------ | ------------------------------------- |
| v        | string |                                            | Version of Machine. e.g: 1.0.0, 1.1.1 |
| con_type | int    | 0: DISCONNECT, 1: 4G, 2: WIFI, 3: ETHERNET | Type of machine connection to server  |
| cp       | int    |                                            | Card price                            |
| amt      | int    |                                            | Current Amount                        |
| to_amt   | int    |                                            | Total amount from factory time to now |
| to_ca    | int    |                                            | Total card from factory time to now   |
| to_ca_d  | int    |                                            | Total card on this day                |
| to_ca_m  | int    |                                            | Total card on this month              |
| tcd_1    | int[]  | [isEmpty, isError, isLower]                | The status of Card Dispenser 1        |
| tcd_2    | int[]  | [isEmpty, isError, isLower]                | The status of Card Dispenser 2        |
| bill     | int    |                                            | The status of Bill Acceptor           |

## Transaction

-   From Device To Server
-   Topic: **cardvendor/\${deviceId}/rp/transaction**
-   Payload:

| Field    | Type | Value | Description                          |
| -------- | ---- | ----- | ------------------------------------ |
| bill     | int  |       | Total bill value in this transaction |
| quantity | int  |       | Card Quantity in this transaction    |

## Operation Event

### Bill Accepted Event

-   From Device To Server
-   Topic: **cardvendor/\${deviceId}/rp/bill_accepted**
-   Payload:

| Field | Type | Value | Description                                    |
| ----- | ---- | ----- | ---------------------------------------------- |
| value | int  |       | The value of bill is accepted in Bill Acceptor |

### Dispenser Event

-   From Device To Server
-   Topic: **cardvendor/\${deviceId}/rp/dispense**
-   Payload:

| Field | Type | Value                             | Description                                                                                                            |
| ----- | ---- | --------------------------------- | ---------------------------------------------------------------------------------------------------------------------- |
| dir   | int  | 0: DISPENSE_OUT, 1: RETURN_TO_BOX | When dir is 0, it mean that dispenser pushed card out of the box. Otherwise, dispenser pulled card into the box again. |

## Config

-   Send Configuration to Device
-   From Server To Device
-   Topic: **cardvendor/\${deviceId}/config**
-   Payload:

| Field | Type   | Value | Description         |
| ----- | ------ | ----- | ------------------- |
| pwd   | string |       | Password of Machine |
| cp    | int    |       | Card Prices         |

## Command

-   Send Command to control device
-   From Server To Device
-   Topic: **cardvendor/\${deviceId}/command**
-   Payload:

| Field | Type | Value                                                                                   | Description |
| ----- | ---- | --------------------------------------------------------------------------------------- | ----------- |
| cmd   | int  | 0: RESET, 1: OTA, 2: RESET_DEFAULT_CONFIG, 3: DELETE_TOTAL_CARD, 4: DELETE_TOTAL_AMOUNT | Command Id  |
