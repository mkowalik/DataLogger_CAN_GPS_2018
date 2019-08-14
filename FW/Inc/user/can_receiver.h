/*
 * can_receiver.h
 *
 *  Created on: 03.06.2017
 *      Author: Michal Kowalik
 */

#include <user/ms_timer_driver.h>
#include "user/fifo_queue.h"
#include "user/can_data.h"
#include "user/config.h"
#include "user/can_transceiver_driver.h"

#ifndef CAN_RECEIVER_DRIVER_H_
#define CAN_RECEIVER_DRIVER_H_

#define 	CAN_MSG_QUEUE_SIZE	512

#define	CAN_MAX_CHANNELS_PER_FRAME	8

typedef enum {
	CANReceiver_Status_OK = 0,
	CANReceiver_Status_Empty,
	CANReceiver_Status_InitError,
	CANReceiver_Status_RunTimeError,
	CANReceiver_Status_Error
} CANReceiver_Status_TypeDef;

typedef struct {
	//TODO dodac state zeby trzeba bylo zainicjalizowac
	CANData_TypeDef 				aReceiverQueueBuffer [CAN_MSG_QUEUE_SIZE];
	FIFOQueue_TypeDef				framesFIFO;
	CANTransceiverDriver_TypeDef*	pCanTransceiverHandler;
	MSTimerDriver_TypeDef*			pMsTimerDriverHandler;
	Config_TypeDef*					pConfig;
} CANReceiver_TypeDef;

CANReceiver_Status_TypeDef CANReceiver_init(CANReceiver_TypeDef* pSelf, Config_TypeDef* pConfig, CANTransceiverDriver_TypeDef* pCanTransceiverHandler, MSTimerDriver_TypeDef* pMsTimerDriverHandler);
CANReceiver_Status_TypeDef CANReceiver_start(CANReceiver_TypeDef* pSelf);
CANReceiver_Status_TypeDef CANReceiver_pullLastFrame(CANReceiver_TypeDef* pSelf, CANData_TypeDef* pRetMsg);

CANReceiver_Status_TypeDef CANReceiver_RxCallback(CANReceiver_TypeDef* pSelf, CANData_TypeDef* pData);
void CANReceiver_RxCallbackWrapper(CANData_TypeDef* pData, void* pVoidSelf);
CANReceiver_Status_TypeDef CANReceiver_ErrorCallback(CANReceiver_TypeDef* pSelf, CANTransceiverDriver_ErrorCode_TypeDef errorcode);
void CANReceiver_ErrorCallbackWrapper(CANTransceiverDriver_ErrorCode_TypeDef errorcode, void* pVoidSelf);

#endif /* CAN_RECEIVER_DRIVER_H_ */
