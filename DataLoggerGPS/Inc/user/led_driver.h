/*
 * led_driver.h
 *
 *  Created on: 26.07.2018
 *      Author: Kowalik
 */

#ifndef USER_LED_DRIVER_H_
#define USER_LED_DRIVER_H_

#include "stdint.h"
#include "gpio.h"

typedef enum {
	LedDriver_State_UnInitialized = 0,
	LedDriver_State_Off,
	LedDriver_State_OnBlinking,
	LedDriver_State_OnStady
} LedDriver_State_TypeDef;

typedef enum {
	LedDriver_Status_OK = 0,
	LedDriver_Status_UnInitializedErrror,
	LedDriver_Status_Errror
} LedDriver_Status_TypeDef;

typedef	GPIO_TypeDef	LedDriver_Port_TypeDef;
typedef	uint16_t		LedDriver_Pin_TypeDef;

typedef struct {
	LedDriver_State_TypeDef	state;
	LedDriver_Port_TypeDef*	port;
	LedDriver_Pin_TypeDef*	pin;
	uint32_t				onTimeMs;
	uint32_t				offTimeMs;
	uint32_t				onOffTimeCounter;
} LedDriver_TypeDef;

LedDriver_Status_TypeDef LedDriver_init(LedDriver_TypeDef* pSelf, LedDriver_Port_TypeDef* port, LedDriver_Pin_TypeDef* pin);
LedDriver_Status_TypeDef LedDriver_OnLed(LedDriver_TypeDef* pSelf);
LedDriver_Status_TypeDef LedDriver_OffLed(LedDriver_TypeDef* pSelf);
LedDriver_Status_TypeDef LedDriver_BlinkingLed(LedDriver_TypeDef* pSelf, uint32_t onTimeMs, uint32_t offTimeMs);
LedDriver_Status_TypeDef LedDriver_1msElapsedCallbackHandler(LedDriver_TypeDef* pSelf);



#endif /* USER_LED_DRIVER_H_ */
