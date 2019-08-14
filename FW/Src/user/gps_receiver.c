/*
 * gps_receiver.c
 *
 *  Created on: 04.08.2019
 *      Author: Michal Kowalik
 */

#include "stddef.h"
#include <string.h>

#include "main.h"
#include "user/gps_receiver.h"

//< ----- Uart Callback function ----- >//

void _GPSReceiver_dataReceivedCallback(GPSData_TypeDef gpsData, void* pArgs){

	GPSReceiver_TypeDef* pSelf = (GPSReceiver_TypeDef*)pArgs;

	if (FIFOQueue_enqueue(&pSelf->receivedDataFIFO, (void*)&gpsData) != FIFO_Status_OK){
		Warning_Handler("FIFOQueue_enqueue error in _GPSReceiver_dataReceivedCallback");
	}

}

//< ----- Public functions ----- >//

GPSReceiver_Status_TypeDef GPSReceiver_init(GPSReceiver_TypeDef* pSelf, GPSDriver_TypeDef* pGPSDriver){

	if (pSelf == NULL || pGPSDriver == NULL){
		return GPSReceiver_Status_NullPointerError;
	}

	pSelf->state		= GPSDriver_State_DuringInit;
	memset((void*)pSelf->receivedDataFIFOBuffer, 0, sizeof(GPSData_TypeDef) * GPS_RECEIVER_MSG_QUEUE_SIZE);
	if (FIFOQueue_init(&pSelf->receivedDataFIFO, pSelf->receivedDataFIFOBuffer, sizeof(GPSData_TypeDef), GPS_RECEIVER_MSG_QUEUE_SIZE) != FIFO_Status_OK){
		return GPSReceiver_Status_Error;
	}
	pSelf->pGPSDriver	= pGPSDriver;

	if (GPSDriver_setReceivedDataCallback(pSelf->pGPSDriver, _GPSReceiver_dataReceivedCallback, (void*)pSelf, &pSelf->gpsDriverCallbackIterator) != GPSDriver_Status_OK){
		return GPSReceiver_Status_Error;
	}

	return GPSReceiver_Status_OK;
}

GPSReceiver_Status_TypeDef GPSReceiver_start(GPSReceiver_TypeDef* pSelf){

	if (pSelf == NULL){
		return GPSReceiver_Status_NullPointerError;
	}

	if (GPSDriver_startReceiver(pSelf->pGPSDriver) != GPSDriver_Status_OK){
		return GPSReceiver_Status_GPSDriverError;
	}

	return GPSReceiver_Status_OK;
}

GPSReceiver_Status_TypeDef GPSReceiver_stop(GPSReceiver_TypeDef* pSelf){

	if (pSelf == NULL){
		return GPSReceiver_Status_NullPointerError;
	}

	if (GPSDriver_stopReceiver(pSelf->pGPSDriver) != GPSDriver_Status_OK){
		return GPSReceiver_Status_GPSDriverError;
	}

	return GPSReceiver_Status_OK;
}

GPSReceiver_Status_TypeDef GPSReceiver_pullLastFrame(GPSReceiver_TypeDef* pSelf, GPSData_TypeDef* pRetGPSData){

	if (pSelf == NULL || pRetGPSData == NULL){
		return GPSReceiver_Status_NullPointerError;
	}

	if (FIFOQueue_dequeue(&pSelf->receivedDataFIFO, pRetGPSData) != FIFO_Status_OK){
		return GPSReceiver_Status_Error;
	}

	return GPSReceiver_Status_OK;
}
