/*
 * transaction.h
 *
 *  Created on: Dec 11, 2023
 *      Author: xuanthodo
 */

#ifndef INC_APP_TRANSACTION_H_
#define INC_APP_TRANSACTION_H_

#include <stdint.h>

void TRANSACTION_add_quantity(uint32_t quantity);
void TRANSACTION_commit();


#endif /* INC_APP_TRANSACTION_H_ */
