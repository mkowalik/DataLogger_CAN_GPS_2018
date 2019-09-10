/*
 * precise_time_middleware.c
 *
 *  Created on: 12.11.2017
 *      Author: Michal Kowalik
 */

#include "user/rtc_driver.h"
#include "rtc.h"



static RTCDriver_Status_TypeDef RTCDriver_HALInit(RTCDriver_TypeDef* pSelf){

	if (pSelf->pRTCHandler->State != HAL_RTC_STATE_RESET){
		return RTCDriver_Status_Error;
	}

	pSelf->pRTCHandler->Instance = RTC;
	pSelf->pRTCHandler->Init.HourFormat = RTC_HOURFORMAT_24;
	pSelf->pRTCHandler->Init.AsynchPrediv = 127;
	pSelf->pRTCHandler->Init.SynchPrediv = 255;
	pSelf->pRTCHandler->Init.OutPut = RTC_OUTPUT_DISABLE;
	pSelf->pRTCHandler->Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
	pSelf->pRTCHandler->Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
	if (HAL_RTC_Init(pSelf->pRTCHandler) != HAL_OK)
	{
		return RTCDriver_Status_Error;
	}
	return RTCDriver_Status_OK;
}

static RTCDriver_Status_TypeDef RTCDriver_HALRestoreTimeAndDate(RTCDriver_TypeDef* pSelf ){

	if (pSelf->pRTCHandler->State != HAL_RTC_STATE_READY){
		return RTCDriver_Status_NotInitialisedError;
	}

	for (uint8_t i=0; i<backupRegistersCount; i++){
		if (HAL_RTCEx_BKUPRead(pSelf->pRTCHandler, backupRegistersIndexes[i]) != backupRegistersValues[i]){
			return RTCDriver_Status_TimeAndDateNotRestoredError;
		}
	}

	RTC_TimeTypeDef sTime = {0};
	RTC_DateTypeDef sDate = {0};
	if (HAL_RTC_GetTime(pSelf->pRTCHandler, &sTime, RTC_FORMAT_BIN) != HAL_OK){
		return RTCDriver_Status_TimeAndDateNotRestoredError;
	}

	if (HAL_RTC_GetDate(pSelf->pRTCHandler, &sDate, RTC_FORMAT_BIN) != HAL_OK){
		return RTCDriver_Status_TimeAndDateNotRestoredError;
	}

	return RTCDriver_Status_OK;
}

static RTCDriver_Status_TypeDef RTCDriver_HALSetTimeAndDate(RTCDriver_TypeDef* pSelf, RTC_TimeTypeDef* pTime, RTC_DateTypeDef* pDate){


	if (HAL_RTC_SetTime(pSelf->pRTCHandler, pTime, RTC_FORMAT_BIN) != HAL_OK) {
		return RTCDriver_Status_Error;
	}

	if (HAL_RTC_SetDate(pSelf->pRTCHandler, pDate, RTC_FORMAT_BIN) != HAL_OK) {
		return RTCDriver_Status_Error;
	}

	HAL_PWR_EnableBkUpAccess();
	for (uint8_t i=0; i<backupRegistersCount; i++){
		HAL_RTCEx_BKUPWrite(pSelf->pRTCHandler, backupRegistersIndexes[i], backupRegistersValues[i]);
	}
	HAL_PWR_DisableBkUpAccess();

	return RTCDriver_Status_OK;
}


RTCDriver_Status_TypeDef RTCDriver_init(RTCDriver_TypeDef* pSelf, RTC_HandleTypeDef* pRTCHandler){

	if (pSelf->state != RTCDriver_State_UnInitialized){
		return RTCDriver_Status_Error;
	}

	if (pRTCHandler == NULL){
		return RTCDriver_Status_Error;
	}

	RTCDriver_Status_TypeDef ret = RTCDriver_Status_OK;
	pSelf->pRTCHandler = pRTCHandler;

	if ((ret = RTCDriver_HALInit(pSelf)) != RTCDriver_Status_OK){
		return ret;
	}

	ret = RTCDriver_HALRestoreTimeAndDate(pSelf);
	if (ret != RTCDriver_Status_OK && ret != RTCDriver_Status_TimeAndDateNotRestoredError){
		return ret;
	}
	if (ret == RTCDriver_Status_TimeAndDateNotRestoredError){

		RTC_TimeTypeDef sTime;
		RTC_DateTypeDef sDate;

		sTime.Hours = 23;
		sTime.Minutes = 56;
		sTime.Seconds = 0;
		sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
		sTime.StoreOperation = RTC_STOREOPERATION_RESET;
		sDate.WeekDay = RTC_WEEKDAY_MONDAY;
		sDate.Month = RTC_MONTH_APRIL;
		sDate.Date = 17;
		sDate.Year = (2019-RTC_DRIVER_YEAR_MIN_VALUE);

		if ((ret = RTCDriver_HALSetTimeAndDate(pSelf, &sTime, &sDate)) != RTCDriver_Status_OK){
			return ret;
		}
	}

	pSelf->state = RTCDriver_State_Ready;

	return RTCDriver_Status_OK;
}

RTCDriver_Status_TypeDef RTCDriver_getDateAndTime(RTCDriver_TypeDef* pSelf, DateTime_TypeDef* pRetDateTime){

	if (pSelf->state == RTCDriver_State_UnInitialized){
		return RTCDriver_Status_NotInitialisedError;
	}

	RTC_DateTypeDef date;
	RTC_TimeTypeDef time;

	if (HAL_RTC_WaitForSynchro(pSelf->pRTCHandler) != HAL_OK){
		return RTCDriver_Status_Error;
	}
	if (HAL_RTC_GetTime(pSelf->pRTCHandler, &time, RTC_FORMAT_BIN) != HAL_OK){
		return RTCDriver_Status_Error;
	}
	if (HAL_RTC_GetDate(pSelf->pRTCHandler, &date, RTC_FORMAT_BIN) != HAL_OK){
		return RTCDriver_Status_Error;
	}

	pRetDateTime->year = 	(uint16_t)(date.Year + RTC_DRIVER_YEAR_MIN_VALUE);
	pRetDateTime->month = 	date.Month;
	pRetDateTime->day = 	date.Date;
	pRetDateTime->hour = 	time.Hours;
	pRetDateTime->minute = 	time.Minutes;
	pRetDateTime->second = 	time.Seconds;

	return RTCDriver_Status_OK;

}

RTCDriver_Status_TypeDef RTCDriver_setDateAndTime(RTCDriver_TypeDef* pSelf, DateTime_TypeDef dateTime){

	if (pSelf->state == RTCDriver_State_UnInitialized){
		return RTCDriver_Status_NotInitialisedError;
	}

	if ((dateTime.year > RTC_DRIVER_YEAR_MAX_VALUE) || (dateTime.year < RTC_DRIVER_YEAR_MIN_VALUE) ||
			(dateTime.month > RTC_DRIVER_MONTH_MAX_VALUE) || (dateTime.month < RTC_DRIVER_MONTH_MIN_VALUE) ||
			(dateTime.day > RTC_DRIVER_DAY_MAX_VALUE) || (dateTime.day < RTC_DRIVER_DAY_MIN_VALUE)) {
		return RTCDriver_Status_WrongDateFormatError;
	} else if ((dateTime.hour > RTC_DRIVER_HOUR_MAX_VALUE) || (dateTime.minute > RTC_DRIVER_MINUTE_MAX_VALUE) || (dateTime.second > RTC_DRIVER_SECOND_MAX_VALUE) ) {
		return RTCDriver_Status_WrongTimeFormatError;
	}

	RTC_DateTypeDef date;
	RTC_TimeTypeDef time;

	date.Year =		(dateTime.year - RTC_DRIVER_YEAR_MIN_VALUE);
	date.Month =	dateTime.month;
	date.Date =		dateTime.day;
	time.Hours = 	dateTime.hour;
	time.Minutes =	dateTime.minute;
	time.Seconds =	dateTime.second;

	return RTCDriver_HALSetTimeAndDate(pSelf, &time, &date);
}

