/*
 * transaction.c
 *
 *  Created on: Dec 11, 2023
 *      Author: xuanthodo
 */

#include <App/transaction.h>

#include <App/statusreporter.h>


static uint32_t transaction_bill = 0;
static uint32_t transaction_quantity = 0;

void TRANSACTION_add_bill(uint32_t bill_value){
	transaction_bill += bill_value;
}

void TRANSACTION_add_quantity(uint32_t quantity){
	transaction_quantity += quantity;
}

void TRANSACTION_commit(){
	if(transaction_bill > 0 && transaction_quantity > 0){
		STATUSREPORTER_report_transaction(transaction_bill, transaction_quantity);
		transaction_bill = 0;
		transaction_quantity = 0;
	}
}

