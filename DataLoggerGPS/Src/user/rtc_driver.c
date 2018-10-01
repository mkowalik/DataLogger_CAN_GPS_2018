/*
 * precise_time_middleware.c
 *
 *  Created on: 12.11.2017
 *      Author: Kowalik
 */

#include <user/rtc_driver.h>


RTCDriver_Status_TypeDef RTCDriver_init(RTCDriver_TypeDef* pSelf, RTC_HandleTypeDef* pRTCHandler){

	//TODO trzeba sie tutaj upewnic, ze RTC nie ma nic w rejestrach backup (dzialala sobie na baterii), a jesli ma to trzeba to przepisac. Fajny przyklad wraz z Cube'owymi przykladami
	if (pSelf->state != RTCDriver_State_UnInitialized){
		return RTCDriver_Status_Error;
	}

	if (pRTCHandler == NULL){
		return RTCDriver_Status_Error;
	}

	pSelf->pRTCHandler = pRTCHandler;

	pSelf->state = RTCDriver_State_Ready;

	return RTCDriver_Status_OK;
}

RTCDriver_Status_TypeDef RTCDriver_getDateAndTime(RTCDriver_TypeDef* pSelf, DateTime_TypeDef* pRetDateTime){

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

	pRetDateTime->year = 	date.Year;
	pRetDateTime->month = 	date.Month;
	pRetDateTime->day = 	date.Date;
	pRetDateTime->hour = 	time.Hours;
	pRetDateTime->minute = 	time.Minutes;
	pRetDateTime->second = 	time.Seconds;

	return RTCDriver_Status_OK;

}


