/*
 * can_data.h
 *
 *  Created on: 12.11.2017
 *      Author: Michal Kowalik
 */

#ifndef CAN_DATA_H_
#define CAN_DATA_H_

#include "stdint.h"
#include "config.h"

typedef struct {
	uint16_t	ID;
	uint8_t		DLC;
	uint32_t	msTime;
	uint8_t		Data[8];
} CANData_TypeDef;

typedef enum {
	CANData_Status_OK = 0,
	CANData_Status_SignalNotInGivenFrameError,
	CANData_Status_NullPointerError,
	CANData_Status_DataTooShortError,
	CANData_Status_Error
} CANData_Status_TypeDef;

CANData_Status_TypeDef CANData_GetValueRawOfSignal(CANData_TypeDef* pData, ConfigSignal_TypeDef* pSignal, uint32_t* pRetSignalRawVal);

#endif /* CAN_DATA_H_ */
