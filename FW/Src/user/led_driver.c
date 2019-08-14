/*
 * led_driver.c
 *
 *  Created on: 26.07.2018
 *      Author: Michal Kowalik
 */

#include "user/led_driver.h"

LedDriver_Status_TypeDef LedDriver_init(LedDriver_TypeDef* pSelf, LedDriver_Port_TypeDef* port, LedDriver_Pin_TypeDef* pin){
	if (pSelf->state != LedDriver_State_UnInitialized){
		return LedDriver_Status_Errror;
	}

	pSelf->port				= port;
	pSelf->pin				= pin;
	pSelf->onTimeMs			= 0;
	pSelf->offTimeMs		= 0;
	pSelf->onOffTimeCounter	= 0;

	HAL_GPIO_WritePin(pSelf->port, *pSelf->pin, GPIO_PIN_SET);

	pSelf->state = LedDriver_State_Off;

	return LedDriver_Status_OK;

}

LedDriver_Status_TypeDef LedDriver_OnLed(LedDriver_TypeDef* pSelf){
	if (pSelf->state == LedDriver_State_UnInitialized){
		return LedDriver_Status_UnInitializedErrror;
	}

	HAL_GPIO_WritePin(pSelf->port, *pSelf->pin, GPIO_PIN_RESET);

	pSelf->state = LedDriver_State_OnStady;

	return LedDriver_Status_OK;

}

LedDriver_Status_TypeDef LedDriver_OffLed(LedDriver_TypeDef* pSelf){
	if (pSelf->state == LedDriver_State_UnInitialized){
		return LedDriver_Status_UnInitializedErrror;
	}

	HAL_GPIO_WritePin(pSelf->port, *pSelf->pin, GPIO_PIN_SET);

	pSelf->state = LedDriver_State_Off;

	return LedDriver_Status_OK;
}

LedDriver_Status_TypeDef LedDriver_BlinkingLed(LedDriver_TypeDef* pSelf, uint32_t onTimeMs, uint32_t offTimeMs){
	if (pSelf->state == LedDriver_State_UnInitialized){
		return LedDriver_Status_UnInitializedErrror;
	}

	pSelf->onTimeMs			= onTimeMs;
	pSelf->offTimeMs		= offTimeMs;
	pSelf->onOffTimeCounter	= 0;


	HAL_GPIO_WritePin(pSelf->port, *pSelf->pin, GPIO_PIN_RESET);

	pSelf->state			= LedDriver_State_OnBlinking;

	return LedDriver_Status_OK;

}

LedDriver_Status_TypeDef LedDriver_1msElapsedCallbackHandler(LedDriver_TypeDef* pSelf){

	if (pSelf->state != LedDriver_State_OnBlinking){
		return LedDriver_Status_OK;
	}

	pSelf->onOffTimeCounter++;

	if (pSelf->onOffTimeCounter == pSelf->onTimeMs){

		HAL_GPIO_WritePin(pSelf->port, *pSelf->pin, GPIO_PIN_SET);

	} else if (pSelf->onOffTimeCounter == pSelf->onTimeMs + pSelf->offTimeMs){

		HAL_GPIO_WritePin(pSelf->port, *pSelf->pin, GPIO_PIN_RESET);
		pSelf->onOffTimeCounter = 0;

	}

	return LedDriver_Status_OK;

}
