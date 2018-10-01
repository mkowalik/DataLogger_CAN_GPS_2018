/*
 * can_receiver.c
 *
 *  Created on: 03.06.2017
 *      Author: Kowalik
 */

#include "user/can_receiver.h"
#include "main.h"

CANReceiver_Status_TypeDef CANReceiver_init(CANReceiver_TypeDef* pSelf, Config_TypeDef* pConfig, CANTransceiverDriver_TypeDef* pCanTransceiverHandler, MSTimerDriver_TypeDef* pMsTimerDriverHandler){

	pSelf->pConfig = pConfig;
	pSelf->pCanTransceiverHandler = pCanTransceiverHandler;
	pSelf->pMsTimerDriverHandler = pMsTimerDriverHandler;

	if (FIFOQueue_init(&(pSelf->framesFIFO), pSelf->aReceiverQueueBuffer, sizeof(CANData_TypeDef), CAN_MSG_QUEUE_SIZE) != FIFOStatus_OK){ //TODO czy alignment nie popusje sizeof
		return CANReceiver_Status_InitError;
	}

	for (uint16_t i=0; i<CAN_MSG_QUEUE_SIZE; i++){
		pSelf->aReceiverQueueBuffer[i] = (CANData_TypeDef){0};
	}

	uint16_t aFilterIDsTab[pConfig->num_of_frames];

	for (uint16_t i=0; i<pConfig->num_of_frames; i++){
		aFilterIDsTab[i] = pConfig->frames[i].ID;
	}

	if (CANTransceiverDriver_configFiltering(pSelf->pCanTransceiverHandler, aFilterIDsTab, pConfig->num_of_frames) != CANTransceiverDriver_Status_OK){
		return CANReceiver_Status_InitError;
	}

	if (CANTransceiverDriver_registerReceiveCallbackToCall(pSelf->pCanTransceiverHandler, CANReceiver_RxCallbackWrapper, (void*) pSelf) != CANTransceiverDriver_Status_OK){
		return CANReceiver_Status_InitError;
	}

	if (CANTransceiverDriver_registerErrorCallbackToCall(pSelf->pCanTransceiverHandler, CANReceiver_ErrorCallbackWrapper, (void*) pSelf) != CANTransceiverDriver_Status_OK){
		return CANReceiver_Status_InitError;
	}

	return CANReceiver_Status_OK;

}

CANReceiver_Status_TypeDef CANReceiver_start(CANReceiver_TypeDef* pSelf){

	if (MSTimerDriver_startCounting(pSelf->pMsTimerDriverHandler) != MSTimerDriver_Status_OK){
		return CANReceiver_Status_Error;
	}

	if (CANTransceiverDriver_start(pSelf->pCanTransceiverHandler) != CANTransceiverDriver_Status_OK){
		return CANReceiver_Status_Error;
	}

	return CANReceiver_Status_OK;
}


CANReceiver_Status_TypeDef CANReceiver_pullLastFrame(CANReceiver_TypeDef* pSelf, CANData_TypeDef* pRetMsg){

	FIFOStatus_TypeDef fifoStatus = FIFOStatus_OK;

	fifoStatus = FIFOQueue_dequeue(&(pSelf->framesFIFO), pRetMsg);

	switch(fifoStatus){
		case FIFOStatus_OK:
			return CANReceiver_Status_OK;
		case FIFOStatus_Empty:
			return CANReceiver_Status_Empty;
		case FIFOStatus_Error:
		default:
			return CANReceiver_Status_Error;
	}

	return CANReceiver_Status_OK;

}

CANReceiver_Status_TypeDef CANReceiver_RxCallback(CANReceiver_TypeDef* pSelf, CANData_TypeDef* pData){

	if (MSTimerDriver_getMSTime(pSelf->pMsTimerDriverHandler, &pData->msTime) != MSTimerDriver_Status_OK){ //TODO trzeba tu wykorzystac ten czas z CANa
		return CANReceiver_Status_RunTimeError;
	}

	FIFOStatus_TypeDef fifoStatus;
	if ((fifoStatus = FIFOQueue_enqueue(&(pSelf->framesFIFO), pData)) != FIFOStatus_OK){
		return CANReceiver_Status_RunTimeError;	//TODO moze jak sie nie zmiesci do kolejki, to nie Error tylko jakas sytuacja wyjatkowa???
	}

	return CANReceiver_Status_OK;

}

void CANReceiver_RxCallbackWrapper(CANData_TypeDef* pData, void* pVoidSelf){

	if(CANReceiver_RxCallback((CANReceiver_TypeDef*) pVoidSelf, pData) != CANReceiver_Status_OK){
		Error_Handler();
	}

}

CANReceiver_Status_TypeDef CANReceiver_ErrorCallback(CANReceiver_TypeDef* pSelf, CANTransceiverDriver_ErrorCode_TypeDef errorcode){

	uint32_t msTime;
	if (MSTimerDriver_getMSTime(pSelf->pMsTimerDriverHandler, &msTime) != MSTimerDriver_Status_OK){ //TODO trzeba tu wykorzystac ten czas z CANa
		return CANReceiver_Status_RunTimeError;
	}

	if (errorcode != CANTransceiverDriver_ErrorCode_None){

		//TODO

	}

	return CANReceiver_Status_OK;

}

void CANReceiver_ErrorCallbackWrapper(CANTransceiverDriver_ErrorCode_TypeDef errorcode, void* pVoidSelf){

	if (CANReceiver_ErrorCallback((CANReceiver_TypeDef*) pVoidSelf, errorcode) != CANReceiver_Status_OK){
		Error_Handler();
	}

}
