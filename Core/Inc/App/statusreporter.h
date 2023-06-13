/*
 * status_reporter.h
 *
 *  Created on: Jun 2, 2023
 *      Author: xuanthodo
 */

#ifndef INC_APP_STATUSREPORTER_H_
#define INC_APP_STATUSREPORTER_H_

#include "stdio.h"
#include "stdbool.h"


bool STATUSREPORTER_init();
bool STATUSREPORTER_run();
void STATUSREPORTER_amount_remain(uint32_t amount);

#endif /* INC_APP_STATUSREPORTER_H_ */
