/*
 * ms_timer_middleware.c
 *
 *  Created on: 19.03.2018
 *      Author: Michal Kowalik
 */

#include "user/ms_timer_driver.h"
#include "stm32f7xx_hal.h"

MSTimerDriver_Status_TypeDef MSTimerDriver_init(volatile MSTimerDriver_TypeDef* pSelf, bool startCounting){

	if (pSelf == NULL){
		return MSTimerDriver_Status_NullPointerError;
	}

	pSelf->state = MSTimerDriver_State_Idle;

	if (startCounting){
		return MSTimerDriver_startCounting(pSelf);
	}
	return MSTimerDriver_Status_OK;
}

MSTimerDriver_Status_TypeDef MSTimerDriver_startCounting(volatile MSTimerDriver_TypeDef* pSelf){

	if (pSelf == NULL){
		return MSTimerDriver_Status_NullPointerError;
	}

	if (pSelf->state == MSTimerDriver_State_NotInitialised){
		return MSTimerDriver_State_NotInitialised;
	}

	if (pSelf->state == MSTimerDriver_State_Running){
		return MSTimerDriver_Status_Error;
	}

	if (pSelf->state == MSTimerDriver_State_Suspended){
		pSelf->uiStartTickValue = pSelf->uiStartTickValue - (HAL_GetTick() - pSelf->uiStopTickValue);
	} else if (pSelf->state == MSTimerDriver_State_Idle){
		pSelf->uiStartTickValue = HAL_GetTick();
	}

	pSelf->state = MSTimerDriver_State_Running;

	return MSTimerDriver_Status_OK;

}

MSTimerDriver_Status_TypeDef MSTimerDriver_stopCounting(volatile MSTimerDriver_TypeDef* pSelf){

	if (pSelf == NULL){
		return MSTimerDriver_Status_NullPointerError;
	}

	if (pSelf->state == MSTimerDriver_State_NotInitialised){
		return MSTimerDriver_State_NotInitialised;
	}

	if ((pSelf->state == MSTimerDriver_State_Idle) || ((pSelf->state == MSTimerDriver_State_Suspended))){
		return MSTimerDriver_Status_Error;
	}

	pSelf->uiStopTickValue = HAL_GetTick();
	pSelf->state = MSTimerDriver_State_Suspended;

	return MSTimerDriver_Status_OK;
}

MSTimerDriver_Status_TypeDef MSTimerDriver_resetCounter(volatile MSTimerDriver_TypeDef* pSelf){

	if (pSelf == NULL){
		return MSTimerDriver_Status_NullPointerError;
	}

	if (pSelf->state == MSTimerDriver_State_NotInitialised){
		return MSTimerDriver_State_NotInitialised;
	}

	if (pSelf->state == MSTimerDriver_State_Running){
		return MSTimerDriver_Status_Error;
	}

	pSelf->state = MSTimerDriver_State_Idle;

	return MSTimerDriver_Status_OK;

}

MSTimerDriver_Status_TypeDef MSTimerDriver_getMSTime(volatile MSTimerDriver_TypeDef* pSelf, uint32_t* pRetTime){

	if (pSelf == NULL || pRetTime == NULL){
		return MSTimerDriver_Status_NullPointerError;
	}

	if (pSelf->state == MSTimerDriver_State_NotInitialised){
		return MSTimerDriver_State_NotInitialised;
	}

	switch (pSelf->state){
	case MSTimerDriver_State_Idle:
	case MSTimerDriver_State_NotInitialised:
		*pRetTime = 0;
		break;
	case MSTimerDriver_State_Suspended:
		*pRetTime = pSelf->uiStopTickValue - pSelf->uiStartTickValue;
		break;
	case MSTimerDriver_State_Running:
	default:
		*pRetTime = HAL_GetTick() - pSelf->uiStartTickValue;
		break;
	}

	return MSTimerDriver_Status_OK;

}
