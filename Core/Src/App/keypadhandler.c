/*
 * keypadhandler.c
 *
 *  Created on: Jun 9, 2023
 *      Author: xuanthodo
 */


#include "main.h"
#include "config.h"
#include "App/keypadhandler.h"
#include "DeviceManager/lcdmanager.h"
#include "Lib/utils/utils_string.h"
#include "Lib/utils/utils_logger.h"
#include "Hal/rtc.h"

enum {
	FN_TIME = 1,
	FN_CARD_PRICE,
	FN_PASSWORD,
	FN_TOTAL_CARD,
	FN_DELETE_TOTAL_CARD,
	FN_TOTAL_AMOUNT,
	FN_DELETE_TOTAL_AMOUNT,
	FN_TOTAL_CARD_BY_DAY,
	FN_TOTAL_CARD_BY_MONTH
};

enum {
	KEYPADHANDLER_STATE_NOT_IN_SETTING,
	KEYPADHANDLER_STATE_IN_SETTING,
	KEYPADHANDLER_STATE_IN_SETTING_DATA
};


static uint8_t prev_state = KEYPADHANDLER_STATE_NOT_IN_SETTING;
static uint8_t state = KEYPADHANDLER_STATE_NOT_IN_SETTING;
static uint8_t fn = FN_CARD_PRICE;
static char * state_name[] = {
		[KEYPADHANDLER_STATE_NOT_IN_SETTING] = "KEYPADHANDLER_STATE_NOT_IN_SETTING\r\n",
		[KEYPADHANDLER_STATE_IN_SETTING] = "KEYPADHANDLER_STATE_IN_SETTING\r\n",
		[KEYPADHANDLER_STATE_IN_SETTING_DATA] = "KEYPADHANDLER_STATE_IN_SETTING_DATA\r\n"
};
static uint8_t keypad_buf[KEYPADHANDLER_BUFFER_LEN];
static size_t keypad_buf_len = 0;
static size_t prev_keypad_buf_len = 0;

static bool KEYPADHANDLER_execute(uint8_t fn, uint8_t *data, size_t data_len);
static void KEYPADHANDLER_card_prices(uint8_t *data, size_t data_len);
static void KEYPADHANDLER_time(uint8_t *data, size_t data_len);
static void KEYPADHANDLER_password(uint8_t *data, size_t data_len);
static void KEYPADHANDLER_total_card( uint8_t *data, size_t data_len);
static void KEYPADHANDLER_total_card_by_day( uint8_t *data, size_t data_len);
static void KEYPADHANDLER_total_card_by_month( uint8_t *data, size_t data_len);
static void KEYPADHANDLER_delete_total_card( uint8_t *data, size_t data_len);
static void KEYPADHANDLER_total_amount( uint8_t *data, size_t data_len);
static void KEYPADHANDLER_delete_total_amount( uint8_t *data, size_t data_len);
static uint32_t KEYPADHANDLER_cal_int(uint8_t *data, size_t data_len);
static void KEYPADHANDLER_int_to_str(uint8_t * str, uint8_t *data, size_t data_len);
static bool KEYPADHANDLER_check_password_valid(uint8_t *data, size_t data_len, char* password);
static bool KEYPADHANDLER_clear_data();
static void KEYPADHANDLER_printf();

bool KEYPADHANDLER_init(){

}

bool KEYPADHANDLER_run(){
	CONFIG_t *config;
	RTC_t rtc;
	switch (state) {
		case KEYPADHANDLER_STATE_NOT_IN_SETTING:
			if(KEYPADMNG_is_entered()){
				config = CONFIG_get();
				KEYPADMNG_clear_entered();
				// Handle keypad
				KEYPADMNG_get_data(keypad_buf, &keypad_buf_len);
				if(KEYPADHANDLER_check_password_valid(keypad_buf, keypad_buf_len, config->password)){
					utils_log_info("Entered setting mode\r\n");
					LCDMNG_set_setting_screen();
					state = KEYPADHANDLER_STATE_IN_SETTING;
				}
				// Clear buffered data
				KEYPADHANDLER_clear_data();
			}
			if(KEYPADMNG_is_cancelled()){
				KEYPADMNG_clear_cancelled();
				KEYPADHANDLER_clear_data();
				// Exit setting mode
			}
			break;
		case KEYPADHANDLER_STATE_IN_SETTING:
			if (KEYPADMNG_is_cancelled()){
				KEYPADHANDLER_clear_data();
				rtc = RTC_get_time();
				config = CONFIG_get();
				LCDMNG_set_working_screen(&rtc, config->amount);
				state = KEYPADHANDLER_STATE_NOT_IN_SETTING;
			}
			KEYPADMNG_get_data(keypad_buf, &keypad_buf_len);
			if(keypad_buf_len > 0){
				if(keypad_buf[0] >= 0 && keypad_buf[0] <= 9){
					fn = keypad_buf[0];
					KEYPADHANDLER_execute(fn, NULL , 0);
					KEYPADHANDLER_clear_data();
					state = KEYPADHANDLER_STATE_IN_SETTING_DATA;
				}
			}
			break;
		case KEYPADHANDLER_STATE_IN_SETTING_DATA:
			KEYPADMNG_get_data(keypad_buf, &keypad_buf_len);
			if(keypad_buf_len > prev_keypad_buf_len){
				prev_keypad_buf_len = keypad_buf_len;
				KEYPADHANDLER_execute(fn, keypad_buf, keypad_buf_len);
			}
			else if (KEYPADMNG_is_cancelled()){
				KEYPADMNG_clear_cancelled();
				KEYPADHANDLER_clear_data();
				prev_keypad_buf_len = 0;
				LCDMNG_set_setting_screen();
				state = KEYPADHANDLER_STATE_IN_SETTING;
			}
			break;
		default:
			break;
	}
	KEYPADHANDLER_printf();
	prev_state = state;
}

static bool KEYPADHANDLER_execute(uint8_t fn, uint8_t *data, size_t data_len){
	switch (fn) {
		case FN_TIME:
			KEYPADHANDLER_time(data, data_len);
			break;
		case FN_CARD_PRICE:
			KEYPADHANDLER_card_prices(data, data_len);
			break;
		case FN_PASSWORD:
			KEYPADHANDLER_password(data, data_len);
			break;
		case FN_TOTAL_CARD:
			KEYPADHANDLER_total_card(data, data_len);
			break;
		case FN_TOTAL_CARD_BY_DAY:
			KEYPADHANDLER_total_card_by_day(data, data_len);
			break;
		case FN_TOTAL_CARD_BY_MONTH:
			KEYPADHANDLER_total_card_by_month(data, data_len);
			break;
		case FN_DELETE_TOTAL_CARD:
			KEYPADHANDLER_delete_total_card(data, data_len);
			break;
		case FN_TOTAL_AMOUNT:
			KEYPADHANDLER_total_amount(data, data_len);
			break;
		case FN_DELETE_TOTAL_AMOUNT:
			KEYPADHANDLER_delete_total_amount(data, data_len);
			break;
		default:
			break;
	}
	return true;
}

static void KEYPADHANDLER_time(uint8_t *data, size_t data_len){
	RTC_t rtc;
	if(data){
		// Data should be hour:minute:second date:month:year is 14 digits
		rtc.hour = data[0] * 10 + data[1];
		rtc.minute = data[2] * 10 + data[3];
		rtc.date = data[4] * 10 + data[5];
		rtc.month = data[6] * 10 + data[7];
		rtc.year = data[8] * 1000 + data[9] * 100 + data[10] * 10 + data[11];
		utils_log_info("Hours :%d, Minutes: %d, Seconds: %d, Date :%d, Month: %d, Year: %d\r\n",
							rtc.hour, rtc.minute, rtc.second, rtc.date, rtc.month, rtc.year);
		RTC_set_time(&rtc);
	}else{
		rtc = RTC_get_time();
	}

	LCDMNG_set_setting_data_screen(fn, &rtc , sizeof(rtc));
}

static void KEYPADHANDLER_card_prices(uint8_t *data, size_t data_len){
	CONFIG_t * config = CONFIG_get();
	uint32_t card_price;
	if(data){
		card_price = KEYPADHANDLER_cal_int(data, data_len);
	}else{
		card_price = config->card_price;
	}
	config->card_price = card_price;
	CONFIG_set(config);
	LCDMNG_set_setting_data_screen(FN_CARD_PRICE, &config->card_price, sizeof(config->card_price));
}

static void KEYPADHANDLER_password( uint8_t *data, size_t data_len){
	CONFIG_t * config = CONFIG_get();
	if(data){
		memset(config->password, 0 ,sizeof(config->password));
		KEYPADHANDLER_int_to_str(config->password, data, data_len);
	}
	CONFIG_set(config);
	utils_log_info("New password: %s\r\n",config->password);
	LCDMNG_set_setting_data_screen(FN_PASSWORD, config->password, sizeof(config->password));
}

static void KEYPADHANDLER_total_card( uint8_t *data, size_t data_len){
	CONFIG_t * config = CONFIG_get();
	utils_log_info("Total card: %d\r\n",config->total_card);
	LCDMNG_set_setting_data_screen(FN_TOTAL_CARD, &config->total_card, sizeof(config->total_card));
}

static void KEYPADHANDLER_delete_total_card( uint8_t *data, size_t data_len){
	CONFIG_t * config = CONFIG_get();
	utils_log_info("Delete total card\r\n");
	config->total_card = 0;
	CONFIG_set(config);
	LCDMNG_set_setting_data_screen(FN_DELETE_TOTAL_CARD, NULL, 0);
}

static void KEYPADHANDLER_total_amount( uint8_t *data, size_t data_len){
	CONFIG_t * config = CONFIG_get();
	utils_log_info("Total amount %d\r\n",config->total_amount);
	LCDMNG_set_setting_data_screen(FN_TOTAL_AMOUNT, &config->total_amount, sizeof(config->total_amount));
}

static void KEYPADHANDLER_delete_total_amount( uint8_t *data, size_t data_len){
	CONFIG_t * config = CONFIG_get();
	utils_log_info("Delete total amount\r\n");
	config->total_amount = 0;
	CONFIG_set(config);
	LCDMNG_set_setting_data_screen(FN_DELETE_TOTAL_AMOUNT, NULL, 0);
}


static void KEYPADHANDLER_total_card_by_day( uint8_t *data, size_t data_len){
	CONFIG_t * config = CONFIG_get();
	utils_log_info("Total card by day: %d\r\n",config->total_card_by_day);
	LCDMNG_set_setting_data_screen(FN_TOTAL_CARD_BY_DAY, &config->total_card_by_day, sizeof(config->total_card_by_day));
}

static void KEYPADHANDLER_total_card_by_month( uint8_t *data, size_t data_len){
	CONFIG_t * config = CONFIG_get();
	utils_log_info("Total card by month: %d\r\n",config->total_card_by_month);
	LCDMNG_set_setting_data_screen(FN_TOTAL_CARD_BY_MONTH, &config->total_card_by_month, sizeof(config->total_card_by_month));
}

static uint32_t KEYPADHANDLER_cal_int(uint8_t *data, size_t data_len){
	uint32_t value = 0;
	for (size_t var = 0; var < data_len; ++var) {
		value = value * 10 + data[var];
	}
	return value;
}

static void KEYPADHANDLER_int_to_str(uint8_t * str, uint8_t *data, size_t data_len){
	for (size_t var = 0; var < data_len; ++var) {
		str[var] = data[var] + '0';
	}
}

static bool KEYPADHANDLER_check_password_valid(uint8_t *data, size_t data_len, char* password){
	if(data_len != strlen(password)){
		utils_log_error("Password is not valid\r\n");
		return false;
	}
	for (int var = 0; var < data_len; ++var) {
		if(password[var] != data[var] + '0'){
			return false;
		}
	}
	return true;
}

static bool KEYPADHANDLER_clear_data(){
	KEYPADMNG_clear_data();
	memset(keypad_buf, 0 , sizeof(keypad_buf));
	keypad_buf_len = 0;
	prev_keypad_buf_len = 0;
}

static void KEYPADHANDLER_printf(){
	if(prev_state != state){
		utils_log_info(state_name[state]);
	}
}

