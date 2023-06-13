/*
 * lcdmanager.h
 *
 *  Created on: Jun 9, 2023
 *      Author: xuanthodo
 */

#ifndef INC_DEVICEMANAGER_LCDMANAGER_H_
#define INC_DEVICEMANAGER_LCDMANAGER_H_

#include "Hal/rtc.h"

enum {
	LCDMNG_SETTING_FIELD_DATE_TIME = 1,
	LCDMNG_SETTING_FIELD_CARD_PRICE,
	LCDMNG_SETTING_FIELD_PASSWORD,
	LCDMNG_SETTING_FIELD_TOTAL_CARD,
	LCDMNG_SETTING_FIELD_DELETE_TOTAL_CARD,
	LCDMNG_SETTING_FIELD_SUM_OF_AMOUNT,
	LCDMNG_SETTING_FIELD_DELETE_SUM_OF_AMOUNT,
};

void LCDMNG_init();
void LCDMNG_run();
void LCDMNG_set_init_screen();
void LCDMNG_set_welcome_screen();
void LCDMNG_set_working_screen_without_draw(RTC_t * rtc, uint32_t amount);
void LCDMNG_set_working_screen(RTC_t * rtc, uint32_t amount);
void LCDMNG_set_processing_screen();
void LCDMNG_set_setting_screen();
void LCDMNG_set_setting_data_screen(uint32_t setting_field_data, void * data, size_t data_len);
void LCDMNG_set_idle_screen();
void LCDMNG_test();


#endif /* INC_DEVICEMANAGER_LCDMANAGER_H_ */
