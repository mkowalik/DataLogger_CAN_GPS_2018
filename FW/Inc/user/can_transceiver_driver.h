/*
 * message_medium_driver.h
 *
 *  Created on: 28.06.2018
 *      Author: Michal Kowalik
 */

#ifndef CAN_TRANSCEIVER_DRIVER_H_
#define CAN_TRANSCEIVER_DRIVER_H_

#include "user/can_data.h"
#include "can.h"
#include "config.h"

#define	CAN_MAX_CALLBACK_NUMBER	3

typedef enum {
	CANTransceiverDriver_Status_OK = 0,
	CANTransceiverDriver_Status_StartError,
	CANTransceiverDriver_Status_TooManyFramesIDsError,
	CANTransceiverDriver_Status_IncorrectCANBitrateValueError,
	CANTransceiverDriver_Status_NullPointerError,
	CANTransceiverDriver_Status_Error
} CANTransceiverDriver_Status_TypeDef;

typedef struct {
	CAN_HandleTypeDef*	pHcan;
	void 			  (*pRxCallbackFunctions[CAN_MAX_CALLBACK_NUMBER]) (CANData_TypeDef* pData, void* arg); //pointer to void funtion geting as argument pData and void* argument
	void* 				pRxCallbackArguemnts[CAN_MAX_CALLBACK_NUMBER];
	void			  (*pErrorCallbackFunctions[CAN_MAX_CALLBACK_NUMBER]) (uint16_t errorcode, void* arg);
	void*				pErrorCallbackArguemnts[CAN_MAX_CALLBACK_NUMBER];
} CANTransceiverDriver_TypeDef;

CANTransceiverDriver_Status_TypeDef CANTransceiverDriver_init(CANTransceiverDriver_TypeDef* pSelf, Config_TypeDef* pConfig, CAN_HandleTypeDef* pHcan, CAN_TypeDef* pCANInstance);
CANTransceiverDriver_Status_TypeDef CANTransceiverDriver_configFiltering(CANTransceiverDriver_TypeDef* pSelf, const uint16_t* pFilters, uint16_t filtersNumber);

CANTransceiverDriver_Status_TypeDef CANTransceiverDriver_receivedMsgCallbackHandler(CANTransceiverDriver_TypeDef* pSelf, uint16_t ID, uint8_t DLC, uint8_t aData[8], uint16_t timestamp);
CANTransceiverDriver_Status_TypeDef CANTransceiverDriver_registerReceiveCallbackToCall(CANTransceiverDriver_TypeDef* pSelf, void (*pCallbackFunction) (CANData_TypeDef* pData, void* arg), void* pArgument);

CANTransceiverDriver_Status_TypeDef CANTransceiverDriver_errorCallbackHandler(CANTransceiverDriver_TypeDef* pSelf);
CANTransceiverDriver_Status_TypeDef CANTransceiverDriver_registerErrorCallbackToCall(CANTransceiverDriver_TypeDef* pSelf, void (*pCallbackFunction) (CANErrorCode_TypeDef errorcode, void* arg), void* pArgument);

CANTransceiverDriver_Status_TypeDef CANTransceiverDriver_start(CANTransceiverDriver_TypeDef* pSelf);

CANTransceiverDriver_Status_TypeDef CANTransceiverDriver_transmitMsg(CANTransceiverDriver_TypeDef* pSelf, const CANData_TypeDef* pData);

#endif /* CAN_TRANSCEIVER_DRIVER_H_ */
