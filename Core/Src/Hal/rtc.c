/*
 * rtc.c
 *
 *  Created on: Jun 10, 2023
 *      Author: xuanthodo
 */

#ifndef SRC_HAL_RTC_C_
#define SRC_HAL_RTC_C_

#include "main.h"
#include "Hal/rtc.h"

RTC_HandleTypeDef hrtc;

void RTC_init(){
	/** Initialize RTC Only
	*/
	hrtc.Instance = RTC;
	hrtc.Init.AsynchPrediv = RTC_AUTO_1_SECOND;
	hrtc.Init.OutPut = RTC_OUTPUTSOURCE_ALARM;

	if (HAL_RTC_Init(&hrtc) != HAL_OK)
	{
		Error_Handler();
	}
}

RTC_t RTC_get_time(){
	RTC_t rtc_time;
	RTC_TimeTypeDef time;
	RTC_DateTypeDef date;

	if(HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN)){
		Error_Handler();
	}
	if(HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN)){
		Error_Handler();
	}
	rtc_time.hour = time.Hours%24;
	rtc_time.minute = time.Minutes%60;
	rtc_time.second = time.Seconds%60;
	rtc_time.date = (date.Date)%31 + 1;
	rtc_time.month = (date.Month)%12 + 1;
	rtc_time.year = date.Year + 2000;
	return rtc_time;
}

void RTC_set_time(RTC_t *rtc){
	RTC_TimeTypeDef time = {
		.Hours = rtc->hour,
		.Minutes = rtc->minute,
		.Seconds = rtc->second
	};
	if(HAL_RTC_SetTime(&hrtc, &time, RTC_FORMAT_BIN)){
		Error_Handler();
	}
	RTC_DateTypeDef date = {
		.WeekDay = 0x01,
		.Date = rtc->date,
		.Month = rtc->month,
		.Year = rtc->year - 2000
	};
	if(HAL_RTC_SetDate(&hrtc, &date, RTC_FORMAT_BIN)){
		Error_Handler();
	}

}

#endif /* SRC_HAL_RTC_C_ */
