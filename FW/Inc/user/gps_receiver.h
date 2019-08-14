/*
 * gps_receiver.h
 *
 *  Created on: 04.08.2019
 *      Author: Michal Kowalik
 */

#ifndef USER_GPS_RECEIVER_H_
#define USER_GPS_RECEIVER_H_

#include "user/gps_data.h"
#include "user/fifo_queue.h"
#include "user/gps_driver.h"

#define	GPS_RECEIVER_MSG_QUEUE_SIZE	256

typedef enum {
	GPSReceiver_State_NotInitalized = 0,
	GPSReceiver_State_DuringInit,
	GPSReceiver_State_Initialized
} GPSReceiver_State_TypeDef;

typedef enum {
	GPSReceiver_Status_OK = 0,
	GPSReceiver_Status_Empty,
	GPSReceiver_Status_NullPointerError,
	GPSReceiver_Status_GPSDriverError,
	GPSReceiver_Status_Error
} GPSReceiver_Status_TypeDef;

typedef struct {
	volatile GPSReceiver_State_TypeDef			state;
	volatile GPSData_TypeDef					receivedDataFIFOBuffer[GPS_RECEIVER_MSG_QUEUE_SIZE];
	volatile FIFOQueue_TypeDef					receivedDataFIFO;

	volatile GPSDriver_TypeDef*					pGPSDriver;
	volatile GPSDriver_CallbackIterator_TypeDef	gpsDriverCallbackIterator;
} GPSReceiver_TypeDef;

GPSReceiver_Status_TypeDef GPSReceiver_init(GPSReceiver_TypeDef* pSelf, GPSDriver_TypeDef* pGPSDriver);
GPSReceiver_Status_TypeDef GPSReceiver_start(GPSReceiver_TypeDef* pSelf);
GPSReceiver_Status_TypeDef GPSReceiver_stop(GPSReceiver_TypeDef* pSelf);
GPSReceiver_Status_TypeDef GPSReceiver_pullLastFrame(GPSReceiver_TypeDef* pSelf, GPSData_TypeDef* pRetGPSData);



#endif /* USER_GPS_RECEIVER_H_ */
