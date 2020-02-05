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

#define CAN_MSG_QUEUE_SIZE	4096

typedef enum {
	CANReceiver_Status_OK = 0,
	CANReceiver_Status_Empty,
	CANReceiver_Status_InitError,
	CANReceiver_Status_CANTransceiverDriverError,
	CANReceiver_Status_FullFIFOError,
	CANReceiver_Status_MSTimerError,
	CANReceiver_Status_FIFOError,
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

#endif /* CAN_RECEIVER_DRIVER_H_ */
