/*
 * led_driver.c
 *
 *  Created on: 26.07.2018
 *      Author: Michal Kowalik
 */

#include "stdbool.h"

#include "user/do_driver.h"


DODriver_Status_TypeDef DODriver_init(volatile DODriver_TypeDef* pSelf, DODriver_Port_TypeDef* pPort, DODriver_Pin_TypeDef* pPin, bool invertLogic){

	if ((pSelf==NULL) || (pPort == NULL) || (pPin == NULL)){
		return DODriver_Status_NullPointerError;
	}

	pSelf->port			= pPort;
	pSelf->pin			= pPin;
	pSelf->invertLogic	= invertLogic;

	HAL_GPIO_WritePin(pSelf->port, *pSelf->pin, GPIO_PIN_RESET);

	pSelf->state = DODriver_State_Low;

	return DODriver_Status_OK;

}

DODriver_Status_TypeDef DODriver_SetHigh(volatile DODriver_TypeDef* pSelf){

	if (pSelf==NULL){
		return DODriver_Status_NullPointerError;
	}
	if (pSelf->state == DODriver_State_UnInitialized){
		return DODriver_Status_UnInitializedError;
	}

	HAL_GPIO_WritePin(pSelf->port, *pSelf->pin, GPIO_PIN_SET);

	pSelf->state = DODriver_State_High;

	return DODriver_Status_OK;

}

DODriver_Status_TypeDef DODriver_SetLow(volatile DODriver_TypeDef* pSelf){

	if (pSelf==NULL){
		return DODriver_Status_NullPointerError;
	}
	if (pSelf->state == DODriver_State_UnInitialized){
		return DODriver_Status_UnInitializedError;
	}

	HAL_GPIO_WritePin(pSelf->port, *pSelf->pin, GPIO_PIN_RESET);

	pSelf->state = DODriver_State_Low;

	return DODriver_Status_OK;
}

DODriver_Status_TypeDef DODriver_Toggle(volatile DODriver_TypeDef* pSelf){

	if (pSelf==NULL){
		return DODriver_Status_NullPointerError;
	}
	if (pSelf->state == DODriver_State_UnInitialized){
		return DODriver_Status_UnInitializedError;
	}

	HAL_GPIO_TogglePin(pSelf->port, *pSelf->pin);

	if (pSelf->state == DODriver_State_Low){
		pSelf->state = DODriver_State_Low;
	} else {
		pSelf->state = DODriver_State_High;
	}

	return DODriver_Status_OK;

	return DODriver_Status_OK;

}
