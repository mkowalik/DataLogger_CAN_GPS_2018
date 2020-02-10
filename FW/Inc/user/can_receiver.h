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

#define CAN_MSG_QUEUE_SIZE		4096
#define CAN_ERROR_QUEUE_SIZE	512

typedef enum {
	CANReceiver_Status_OK = 0,
	CANReceiver_Status_Empty,
	CANReceiver_Status_NotInitialisedError,
	CANReceiver_Status_NullPointerError,
	CANReceiver_Status_AlreadyRunningError,
	CANReceiver_Status_NotRunningError,
	CANReceiver_Status_InitError,
	CANReceiver_Status_CANTransceiverDriverError,
	CANReceiver_Status_FullFIFOError,
	CANReceiver_Status_MSTimerError,
	CANReceiver_Status_FIFOError,
	CANReceiver_Status_Error
} CANReceiver_Status_TypeDef;

typedef enum {
	CANReceiver_State_NotInitialised = 0,
	CANReceiver_State_Initialised,
	CANReceiver_State_Running
} CANReceiver_State_TypeDef;

typedef struct {
	volatile CANData_TypeDef 					aReceiverQueueBuffer [CAN_MSG_QUEUE_SIZE];
	volatile FIFOQueue_TypeDef					framesFIFO;
	volatile CANErrorData_TypeDef				aReceiverCANErrorsQueueBuffer [CAN_ERROR_QUEUE_SIZE];
	volatile FIFOQueue_TypeDef					canErrorsFIFO;
	volatile CANTransceiverDriver_TypeDef*		pCanTransceiverHandler;
	volatile MSTimerDriver_TypeDef* volatile	pMsTimerDriverHandler;
	Config_TypeDef*								pConfig;
	CANReceiver_State_TypeDef					state;
} CANReceiver_TypeDef;

CANReceiver_Status_TypeDef CANReceiver_init(volatile CANReceiver_TypeDef* pSelf, Config_TypeDef* pConfig, volatile CANTransceiverDriver_TypeDef* pCanTransceiverHandler, volatile MSTimerDriver_TypeDef* pMsTimerDriverHandler);
CANReceiver_Status_TypeDef CANReceiver_start(volatile CANReceiver_TypeDef* pSelf);
CANReceiver_Status_TypeDef CANReceiver_pullLastFrame(volatile CANReceiver_TypeDef* pSelf, CANData_TypeDef* pRetMsg);
CANReceiver_Status_TypeDef CANReceiver_pullLastCANBusError(volatile CANReceiver_TypeDef* pSelf, CANErrorData_TypeDef* pRetErrorData);
CANReceiver_Status_TypeDef CANReceiver_clear(volatile CANReceiver_TypeDef* pSelf);

#endif /* CAN_RECEIVER_DRIVER_H_ */
