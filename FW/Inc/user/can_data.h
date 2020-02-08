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
	uint32_t	msTimestamp;
	uint8_t		Data[8];
} CANData_TypeDef;


typedef enum {
	CANErrorCode_None				= 0,
	CANErrorCode_ProtocolErrWarn	= (1<<0),
	CANErrorCode_ErrPassive			= (1<<1),
	CANErrorCode_BusOff				= (1<<2),
	CANErrorCode_BitStuffingError	= (1<<3),
	CANErrorCode_FormError			= (1<<4),
	CANErrorCode_ACKError			= (1<<5),
	CANErrorCode_BitRecessiveError	= (1<<6),
	CANErrorCode_BitDominantError	= (1<<7),
	CANErrorCode_CRCError			= (1<<8),
	CANErrorCode_TransceiverError	= (1<<9),
	CANErrorCode_OtherError			= (1<<10)
} CANErrorCode_TypeDef;

typedef struct {
	CANErrorCode_TypeDef	errorCode;
	uint32_t				msTimestamp;
} CANErrorData_TypeDef;

typedef enum {
	CANData_Status_OK = 0,
	CANData_Status_SignalNotInGivenFrameError,
	CANData_Status_NullPointerError,
	CANData_Status_DataTooShortError,
	CANData_Status_Error
} CANData_Status_TypeDef;

CANData_Status_TypeDef CANData_GetValueRawOfSignal(const CANData_TypeDef* pData, const ConfigSignal_TypeDef* pSignal, uint32_t* pRetSignalRawVal);

#ifdef USE_FULL_ASSERT
void CANData_UnitTests();
#endif //USE_FULL_ASSERT

#endif /* CAN_DATA_H_ */
