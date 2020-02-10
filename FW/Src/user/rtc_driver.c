/*
 * precise_time_middleware.c
 *
 *  Created on: 12.11.2017
 *      Author: Michal Kowalik
 */

#include "user/rtc_driver.h"
#include "rtc.h"

//< ----- private functions prototypes ----- >//

static RTCDriver_Status_TypeDef _RTCDriver_validateDate(const DateTime_TypeDef* pDateTime);
static RTCDriver_Status_TypeDef _RTCDriver_HALInit(RTCDriver_TypeDef* pSelf);
static RTCDriver_Status_TypeDef _RTCDriver_HALRestoreTimeAndDate(RTCDriver_TypeDef* pSelf);
static RTCDriver_Status_TypeDef _RTCDriver_HALSetTimeAndDate(RTCDriver_TypeDef* pSelf, const DateTime_TypeDef* pDateTime);

static RTCDriver_Status_TypeDef _RTCDriver_DateToSecondsFrom2000(const DateTime_TypeDef* pDateTime, uint32_t* pRetSconds);
static RTCDriver_Status_TypeDef _RTCDriver_SecondsFrom2000ToDate(uint32_t seconds, DateTime_TypeDef* pRetDateTime);

//< ----- private functions implementations ----- >//

static RTCDriver_Status_TypeDef _RTCDriver_validateDate(const DateTime_TypeDef* pDateTime){

	if ((pDateTime->year > RTC_DRIVER_YEAR_MAX_VALUE) || (pDateTime->year < RTC_DRIVER_YEAR_MIN_VALUE) ||
			(pDateTime->month > RTC_DRIVER_MONTH_MAX_VALUE) || (pDateTime->month < RTC_DRIVER_MONTH_MIN_VALUE) ||
			(pDateTime->day > RTC_DRIVER_DAY_MAX_VALUE) || (pDateTime->day < RTC_DRIVER_DAY_MIN_VALUE)) {
		return RTCDriver_Status_WrongDateFormatError;
	} else if ((pDateTime->hour > RTC_DRIVER_HOUR_MAX_VALUE) || (pDateTime->minute > RTC_DRIVER_MINUTE_MAX_VALUE) || (pDateTime->second > RTC_DRIVER_SECOND_MAX_VALUE) ) {
		return RTCDriver_Status_WrongTimeFormatError;
	}

	return RTCDriver_Status_OK;
}

static RTCDriver_Status_TypeDef _RTCDriver_HALInit(RTCDriver_TypeDef* pSelf){

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

static RTCDriver_Status_TypeDef _RTCDriver_HALRestoreTimeAndDate(RTCDriver_TypeDef* pSelf){

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

static RTCDriver_Status_TypeDef _RTCDriver_HALSetTimeAndDate(RTCDriver_TypeDef* pSelf, const DateTime_TypeDef* pDateTime){

	RTC_DateTypeDef date = {0};
	RTC_TimeTypeDef time = {0};

	date.Year =		(pDateTime->year - RTC_DRIVER_YEAR_MIN_VALUE);
	date.Month =	pDateTime->month;
	date.Date =		pDateTime->day;
	time.Hours = 	pDateTime->hour;
	time.Minutes =	pDateTime->minute;
	time.Seconds =	pDateTime->second;

	if (HAL_RTC_SetTime(pSelf->pRTCHandler, &time, RTC_FORMAT_BIN) != HAL_OK) {
		return RTCDriver_Status_Error;
	}

	if (HAL_RTC_SetDate(pSelf->pRTCHandler, &date, RTC_FORMAT_BIN) != HAL_OK) {
		return RTCDriver_Status_Error;
	}

	HAL_PWR_EnableBkUpAccess();
	for (uint8_t i=0; i<backupRegistersCount; i++){
		HAL_RTCEx_BKUPWrite(pSelf->pRTCHandler, backupRegistersIndexes[i], backupRegistersValues[i]);
	}
	HAL_PWR_DisableBkUpAccess();

	return RTCDriver_Status_OK;
}

static RTCDriver_Status_TypeDef _RTCDriver_DateToSecondsFrom2000(const DateTime_TypeDef* pDateTime, uint32_t* pRetSconds){

	RTCDriver_Status_TypeDef ret = RTCDriver_Status_OK;
	if ((ret = _RTCDriver_validateDate(pDateTime)) != RTCDriver_Status_OK){
		return ret;
	}

	(*pRetSconds) = 0;

	uint16_t year = (pDateTime->year - RTC_DRIVER_YEAR_MIN_VALUE);
	while (year > 0){
		if (year % 4 == 0){
			(*pRetSconds) += (366 * 24 * 60 * 60);
		} else {
			(*pRetSconds) += (365 * 24 * 60 * 60);
		}
		year--;
	}

	uint16_t month = pDateTime->month -1;

	while (month > 0){
		switch (month){
			case 1:
			case 3:
			case 5:
			case 7:
			case 8:
			case 10:
			case 12:
				(*pRetSconds) += (31 * 24 * 60 * 60);
				break;
			case 4:
			case 6:
			case 9:
			case 11:
				(*pRetSconds) += (30 * 24 * 60 * 60);
				break;
			case 2:
				if (pDateTime->year % 4 == 0){
					(*pRetSconds) += (29 * 24 * 60 * 60);
				} else {
					(*pRetSconds) += (28 * 24 * 60 * 60);
				}
				break;
			default:
				return RTCDriver_Status_Error;
		}
		month--;
	}

	(*pRetSconds) += (((uint32_t)pDateTime->day - 1) * 24 * 60 * 60); //< minus 1 because counting days from 1, not from 0
	(*pRetSconds) += (((uint32_t)pDateTime->hour) * 60 * 60);
	(*pRetSconds) += (((uint32_t)pDateTime->minute) * 60);
	(*pRetSconds) += ((uint32_t)pDateTime->second);

	return RTCDriver_Status_OK;
}


static RTCDriver_Status_TypeDef _RTCDriver_SecondsFrom2000ToDate(uint32_t seconds, DateTime_TypeDef* pRetDateTime){

	pRetDateTime->year = 0;
	while (true){
		uint32_t secsToSubstr = 0;
		if ((pRetDateTime->year) % 4 == 0){
			secsToSubstr = (366 * 24 * 60 * 60);
		} else {
			secsToSubstr = (365 * 24 * 60 * 60);
		}
		if (secsToSubstr <= seconds){
			seconds -= secsToSubstr;
			(pRetDateTime->year)++;
		} else {
			break;
		}
	}
	pRetDateTime->year += RTC_DRIVER_YEAR_MIN_VALUE;

	pRetDateTime->month = 1;
	while (true){
		uint32_t secsToSubstr = 0;
		switch (pRetDateTime->month){
			case 1:
			case 3:
			case 5:
			case 7:
			case 8:
			case 10:
			case 12:
				secsToSubstr = (31 * 24 * 60 * 60);
				break;
			case 4:
			case 6:
			case 9:
			case 11:
				secsToSubstr = (30 * 24 * 60 * 60);
				break;
			case 2:
				if (((pRetDateTime->year) % 4) == 0){
					secsToSubstr = (29 * 24 * 60 * 60);
				} else {
					secsToSubstr = (28 * 24 * 60 * 60);
				}
				break;
			default:
				return RTCDriver_Status_Error;
		}
		if (secsToSubstr <= seconds){
			seconds -= secsToSubstr;
			(pRetDateTime->month)++;
		} else {
			break;
		}
	}

	pRetDateTime->day		= (seconds / (24 * 60 * 60)) + 1; //< plus 1 because counting days from 1, not from 0
	seconds					%= (24 * 60 * 60);
	pRetDateTime->hour		= (seconds / (60 * 60));
	seconds					%= (60 * 60);
	pRetDateTime->minute	= (seconds / 60);
	seconds					%= 60;
	pRetDateTime->second	= seconds;

	return RTCDriver_Status_OK;
}

//< ----- Public functions ----- >//

RTCDriver_Status_TypeDef RTCDriver_init(RTCDriver_TypeDef* pSelf, RTC_HandleTypeDef* pRTCHandler){

	if ((pSelf == NULL) || (pRTCHandler == NULL)){
		return RTCDriver_Status_NullPointerError;
	}

	RTCDriver_Status_TypeDef ret = RTCDriver_Status_OK;
	pSelf->pRTCHandler = pRTCHandler;

	if ((ret = _RTCDriver_HALInit(pSelf)) != RTCDriver_Status_OK){
		return ret;
	}

	ret = _RTCDriver_HALRestoreTimeAndDate(pSelf);
	if (ret != RTCDriver_Status_OK && ret != RTCDriver_Status_TimeAndDateNotRestoredError){
		return ret;
	}
	if (ret == RTCDriver_Status_TimeAndDateNotRestoredError){

		DateTime_TypeDef dateTime = {0};
		dateTime.year			= RTC_DRIVER_YEAR_MIN_VALUE;
		dateTime.month			= 1;
		dateTime.day			= 1;
		dateTime.hour			= 0;
		dateTime.minute			= 0;
		dateTime.second			= 0;
		dateTime.miliseconds	= 0;

		if ((ret = _RTCDriver_HALSetTimeAndDate(pSelf, &dateTime)) != RTCDriver_Status_OK){
			return ret;
		}
	}

	pSelf->state = RTCDriver_State_Ready;

	return RTCDriver_Status_OK;
}

RTCDriver_Status_TypeDef RTCDriver_getDateAndTime(RTCDriver_TypeDef* pSelf, DateTime_TypeDef* pRetDateTime){

	if ((pSelf == NULL) || (pRetDateTime == NULL)){
		return RTCDriver_Status_NullPointerError;
	}

	if (pSelf->state == RTCDriver_State_UnInitialized){
		return RTCDriver_Status_NotInitialisedError;
	}

	RTC_TimeTypeDef sTime = {0};
	RTC_DateTypeDef sDate = {0};

	if (HAL_RTC_GetTime(pSelf->pRTCHandler, &sTime, RTC_FORMAT_BIN) != HAL_OK){
		return RTCDriver_Status_Error;
	}
	if (HAL_RTC_GetDate(pSelf->pRTCHandler, &sDate, RTC_FORMAT_BIN) != HAL_OK){
		return RTCDriver_Status_Error;
	}

	pRetDateTime->year =	(uint16_t)(sDate.Year + RTC_DRIVER_YEAR_MIN_VALUE);
	pRetDateTime->month =	sDate.Month;
	pRetDateTime->day =		sDate.Date;
	pRetDateTime->hour = 	sTime.Hours;
	pRetDateTime->minute =	sTime.Minutes;
	pRetDateTime->second =	sTime.Seconds;

	return RTCDriver_Status_OK;
}

RTCDriver_Status_TypeDef RTCDriver_setDateAndTime(RTCDriver_TypeDef* pSelf, DateTime_TypeDef dateTime){

	if (pSelf == NULL){
		return RTCDriver_Status_NullPointerError;
	}

	if (pSelf->state == RTCDriver_State_UnInitialized){
		return RTCDriver_Status_NotInitialisedError;
	}

	RTCDriver_Status_TypeDef ret = RTCDriver_Status_OK;
	if ((ret = _RTCDriver_validateDate(&dateTime)) != RTCDriver_Status_OK){
		return ret;
	}

	return _RTCDriver_HALSetTimeAndDate(pSelf, &dateTime);
}

RTCDriver_Status_TypeDef RTCDriver_addSeconds(DateTime_TypeDef* pDateTime, uint32_t seconds){

	if (pDateTime == NULL){
		return RTCDriver_Status_Error;
	}

	RTCDriver_Status_TypeDef ret = RTCDriver_Status_OK;
	uint32_t secondsDate = 0;
	if ((ret = _RTCDriver_DateToSecondsFrom2000(pDateTime, &secondsDate)) != RTCDriver_Status_OK){
		return ret;
	}

	return _RTCDriver_SecondsFrom2000ToDate(secondsDate + seconds, pDateTime);
}

RTCDriver_Status_TypeDef RTCDriver_substractSeconds(DateTime_TypeDef* pDateTime, uint32_t seconds){

	if (pDateTime == NULL){
		return RTCDriver_Status_Error;
	}

	RTCDriver_Status_TypeDef ret = RTCDriver_Status_OK;
	uint32_t secondsDate = 0;
	if ((ret = _RTCDriver_DateToSecondsFrom2000(pDateTime, &secondsDate)) != RTCDriver_Status_OK){
		return ret;
	}

	if (seconds > secondsDate){
		return RTCDriver_Status_SecondsValueTooHighError;
	}

	return _RTCDriver_SecondsFrom2000ToDate(secondsDate - seconds, pDateTime);
}


bool RTCDriver_isAfter(DateTime_TypeDef dateTimeExpectedBefore, DateTime_TypeDef dateTimeExpectedAfter){

	if (dateTimeExpectedAfter.month < dateTimeExpectedBefore.month) {
		return false;
	}
	if (dateTimeExpectedAfter.month > dateTimeExpectedBefore.month) {
		return true;
	}

	if (dateTimeExpectedAfter.day < dateTimeExpectedBefore.day) {
		return false;
	}
	if (dateTimeExpectedAfter.day > dateTimeExpectedBefore.day) {
		return true;
	}

	if (dateTimeExpectedAfter.hour < dateTimeExpectedBefore.hour) {
		return false;
	}
	if (dateTimeExpectedAfter.hour > dateTimeExpectedBefore.hour) {
		return true;
	}

	if (dateTimeExpectedAfter.minute < dateTimeExpectedBefore.minute) {
		return false;
	}
	if (dateTimeExpectedAfter.minute > dateTimeExpectedBefore.minute) {
		return true;
	}


	if (dateTimeExpectedAfter.second < dateTimeExpectedBefore.second) {
		return false;
	}
	if (dateTimeExpectedAfter.second > dateTimeExpectedBefore.second) {
		return true;
	}

	return false;
}

