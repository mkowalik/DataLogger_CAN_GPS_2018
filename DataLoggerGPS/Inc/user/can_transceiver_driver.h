/*
 * message_medium_driver.h
 *
 *  Created on: 28.06.2018
 *      Author: Kowalik
 */

#ifndef CAN_TRANSCEIVER_DRIVER_H_
#define CAN_TRANSCEIVER_DRIVER_H_

#include "user/can_data.h"
#include "can.h"

#define	CAN_MAX_CALLBACK_NUMBER	3

typedef enum {
	CANTransceiverDriver_Status_OK = 0,
	CANTransceiverDriver_Status_StartError,
	CANTransceiverDriver_Status_TooManyFramesIDsError,
	CANTransceiverDriver_Status_Error
} CANTransceiverDriver_Status_TypeDef;

typedef enum {
	CANTransceiverDriver_ErrorCode_None					= 0,
	CANTransceiverDriver_ErrorCode_ProtocolErrWarn		= (1<<0),
	CANTransceiverDriver_ErrorCode_ErrPassive			= (1<<1),
	CANTransceiverDriver_ErrorCode_BusOff				= (1<<2),
	CANTransceiverDriver_ErrorCode_BitStuffingError		= (1<<3),
	CANTransceiverDriver_ErrorCode_FormError			= (1<<4),
	CANTransceiverDriver_ErrorCode_ACKError				= (1<<5),
	CANTransceiverDriver_ErrorCode_BitRecessiveError	= (1<<6),
	CANTransceiverDriver_ErrorCode_BitDominantError		= (1<<7),
	CANTransceiverDriver_ErrorCode_CRCError				= (1<<8),
	CANTransceiverDriver_ErrorCode_RxFIFOError			= (1<<9),
	CANTransceiverDriver_ErrorCode_TxMailboxError		= (1<<10),
	CANTransceiverDriver_ErrorCode_TimeoutError			= (1<<11),
	CANTransceiverDriver_ErrorCode_OtherError			= (1<<12),
} CANTransceiverDriver_ErrorCode_TypeDef;

typedef struct {
	CAN_HandleTypeDef* pHcan;
	void (*pRxCallbackFunctions[CAN_MAX_CALLBACK_NUMBER]) (CANData_TypeDef* pData, void* arg); //pointer to void funtion geting as argument pData and void* argument
	void* pRxCallbackArguemnts[CAN_MAX_CALLBACK_NUMBER];
	void (*pErrorCallbackFunctions[CAN_MAX_CALLBACK_NUMBER]) (uint16_t errorcode, void* arg);
	void* pErrorCallbackArguemnts[CAN_MAX_CALLBACK_NUMBER];
} CANTransceiverDriver_TypeDef;

CANTransceiverDriver_Status_TypeDef CANTransceiverDriver_init(CANTransceiverDriver_TypeDef* pSelf, CAN_HandleTypeDef* pHcan);
CANTransceiverDriver_Status_TypeDef CANTransceiverDriver_configFiltering(CANTransceiverDriver_TypeDef* pSelf, uint16_t* pFilters, uint16_t filtersNumber);

CANTransceiverDriver_Status_TypeDef CANTransceiverDriver_receivedMsgCallbackHandler(CANTransceiverDriver_TypeDef* pSelf, uint16_t ID, uint8_t DLC, uint8_t aData[8], uint16_t timestamp);
CANTransceiverDriver_Status_TypeDef CANTransceiverDriver_registerReceiveCallbackToCall(CANTransceiverDriver_TypeDef* pSelf, void (*pCallbackFunction) (CANData_TypeDef* pData, void* arg), void* pArgument);

CANTransceiverDriver_Status_TypeDef CANTransceiverDriver_errorCallbackHandler(CANTransceiverDriver_TypeDef* pSelf);
CANTransceiverDriver_Status_TypeDef CANTransceiverDriver_registerErrorCallbackToCall(CANTransceiverDriver_TypeDef* pSelf, void (*pCallbackFunction) (CANTransceiverDriver_ErrorCode_TypeDef errorcode, void* arg), void* pArgument);

CANTransceiverDriver_Status_TypeDef CANTransceiverDriver_start(CANTransceiverDriver_TypeDef* pSelf);

CANTransceiverDriver_Status_TypeDef CANTransceiverDriver_transmitMsg(CANTransceiverDriver_TypeDef* pSelf, CANData_TypeDef* pData);

//TODO zapisuje tu: time triggered communication mode

#endif /* CAN_TRANSCEIVER_DRIVER_H_ */
