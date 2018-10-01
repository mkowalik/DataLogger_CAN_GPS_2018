/*
 * ms_timer_middleware.h
 *
 *  Created on: 19.03.2018
 *      Author: Kowalik
 */

#ifndef MS_TIMER_DRIVER_H_
#define MS_TIMER_DRIVER_H_

#include "stdint.h"

typedef enum {
	MSTimerDriver_State_NotInitialised = 0,
	MSTimerDriver_State_Idle,
	MSTimerDriver_State_Suspended,
	MSTimerDriver_State_Running
} MSTimerDriver_State_TypeDef;

typedef enum {
	MSTimerDriver_Status_OK = 0,
	MSTimerDriver_Status_Error_NotInitialised,
	MSTimerDriver_Status_Error
} MSTimerDriver_Status_TypeDef;

typedef struct  {
	MSTimerDriver_State_TypeDef	state;
	uint32_t					uiStartTickValue;
	uint32_t					uiStopTickValue;
} MSTimerDriver_TypeDef;

MSTimerDriver_Status_TypeDef MSTimerDriver_init(MSTimerDriver_TypeDef* pSelf);
MSTimerDriver_Status_TypeDef MSTimerDriver_startCounting(MSTimerDriver_TypeDef* pSelf);
MSTimerDriver_Status_TypeDef MSTimerDriver_stopCounting(MSTimerDriver_TypeDef* pSelf);
MSTimerDriver_Status_TypeDef MSTimerDriver_resetCounter(MSTimerDriver_TypeDef* pSelf);
MSTimerDriver_Status_TypeDef MSTimerDriver_getMSTime(MSTimerDriver_TypeDef* pSelf, uint32_t* pRetTime);

#endif /* MS_TIMER_DRIVER_H_ */
