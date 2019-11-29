/*
 * led_driver.h
 *
 *  Created on: 26.07.2018
 *      Author: Michal Kowalik
 */

#ifndef USER_DO_DRIVER_H_
#define USER_DO_DRIVER_H_

#include "stdbool.h"

#include "stdint.h"
#include "gpio.h"

typedef enum {
	DODriver_State_UnInitialized = 0,
	DODriver_State_High,
	DODriver_State_Low
} DODriver_State_TypeDef;

typedef enum {
	DODriver_Status_OK = 0,
	DODriver_Status_UnInitializedError,
	DODriver_Status_NullPointerError,
	DODriver_Status_Error
} DODriver_Status_TypeDef;

typedef	GPIO_TypeDef	DODriver_Port_TypeDef;
typedef	uint16_t		DODriver_Pin_TypeDef;

typedef struct {
	DODriver_State_TypeDef	state;
	DODriver_Port_TypeDef*	port;
	DODriver_Pin_TypeDef*	pin;
	bool					invertLogic;
} DODriver_TypeDef;

DODriver_Status_TypeDef DODriver_init(volatile DODriver_TypeDef* pSelf, DODriver_Port_TypeDef* port, DODriver_Pin_TypeDef* pin, bool invertLogic);
DODriver_Status_TypeDef DODriver_SetHigh(volatile DODriver_TypeDef* pSelf);
DODriver_Status_TypeDef DODriver_SetLow(volatile DODriver_TypeDef* pSelf);
DODriver_Status_TypeDef DODriver_Toggle(volatile DODriver_TypeDef* pSelf);


#endif /* USER_DO_DRIVER_H_ */
