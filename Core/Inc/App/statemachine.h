/*
 * statemachine.h
 *
 *  Created on: May 10, 2023
 *      Author: xuanthodo
 */

#ifndef INC_APP_STATEMACHINE_H_
#define INC_APP_STATEMACHINE_H_

typedef void (*sm_fn)(void);

bool STATEMACHINE_init();
bool STATEMACHINE_loop();

#endif /* INC_APP_STATEMACHINE_H_ */
