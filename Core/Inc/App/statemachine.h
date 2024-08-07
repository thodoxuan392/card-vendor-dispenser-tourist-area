/*
 * statemachine.h
 *
 *  Created on: Jul 10, 2024
 *      Author: xuanthodo
 */

#ifndef INC_APP_STATEMACHINE_H_
#define INC_APP_STATEMACHINE_H_

#include "stdint.h"
#include "stdbool.h"

typedef void (*STATEMACHINE_DispenseResultCallback)(uint8_t result, uint32_t remainCard);

void STATEMACHINE_init(void);
void STATEMACHINE_run(void);

bool STATEMACHINE_requestDispense(uint8_t nbCard, uint8_t cardType);
void STATEMACHINE_setDispenseResultCallback(STATEMACHINE_DispenseResultCallback callback);

#endif /* INC_APP_STATEMACHINE_H_ */
