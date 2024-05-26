/*
 * transaction.c
 *
 *  Created on: Dec 11, 2023
 *      Author: xuanthodo
 */

#include <App/transaction.h>

#include <config.h>
#include <App/statusreporter.h>


static uint32_t transaction_quantity = 0;


void TRANSACTION_add_quantity(uint32_t quantity){
	transaction_quantity += quantity;
}

void TRANSACTION_commit(){
	if(transaction_quantity > 0){
		uint32_t cardPrice = CONFIG_get()->card_price;
		STATUSREPORTER_report_transaction(cardPrice, transaction_quantity);
		transaction_quantity = 0;
	}
}

