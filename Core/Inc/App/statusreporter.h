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
void STATUSREPORTER_report_billaccepted(uint32_t bill_value);
void STATUSREPORTER_report_dispense(uint32_t direction);

#endif /* INC_APP_STATUSREPORTER_H_ */
