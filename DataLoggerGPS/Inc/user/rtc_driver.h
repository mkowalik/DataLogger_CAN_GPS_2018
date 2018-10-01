/*
 * precise_time_middleware.h
 *
 *  Created on: 12.11.2017
 *      Author: Kowalik
 */

#ifndef RTC_MIDDLEWARE_H_
#define RTC_MIDDLEWARE_H_

#include "stdint.h"
#include "rtc.h"

typedef struct {
	uint16_t	year;
	uint8_t		month;
	uint8_t		day;
	uint8_t		hour;
	uint8_t		minute;
	uint8_t		second;
} DateTime_TypeDef;

typedef enum {
	RTCDriver_Status_OK = 0,
	RTCDriver_Status_NotInitialisedError,
	RTCDriver_Status_Error
} RTCDriver_Status_TypeDef;

typedef enum {
	RTCDriver_State_UnInitialized = 0,
	RTCDriver_State_Ready
} RTCDriver_State_TypeDef;

typedef struct {
	RTC_HandleTypeDef*		pRTCHandler;
	RTCDriver_State_TypeDef	state;
} RTCDriver_TypeDef;

RTCDriver_Status_TypeDef RTCDriver_init(RTCDriver_TypeDef* pSelf, RTC_HandleTypeDef* pRTCHandler);
RTCDriver_Status_TypeDef RTCDriver_getDateAndTime(RTCDriver_TypeDef* pSelf, DateTime_TypeDef* pRetDateTime);


#endif /* RTC_MIDDLEWARE_H_ */
