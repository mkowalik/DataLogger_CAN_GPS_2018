/*
 * fifo_queue_multireadears.c
 *
 *  Created on: 28.06.2019
 *      Author: Michal Kowalik
 */

#include <user/fifo_queue_multiread.h>
#include "stdint.h"
#include "string.h"
#include "stm32f7xx_hal.h"

FIFOMultiread_Status_TypeDef FIFOMultiread_init(volatile FIFOMultiread_TypeDef* pSelf, volatile void* pTabPtrArg, uint8_t elementSize, uint16_t queueSize){

	if ((pSelf==NULL) || (pTabPtrArg == NULL)){
		return FIFOMultiread_Status_Error;
	}

	pSelf->pTabPtr							= pTabPtrArg;
	pSelf->elementSize						= elementSize;
	pSelf->queueLength						= queueSize;
	pSelf->tailIndex						= 0;
	pSelf->notEnqueueOperationInProgress	= false;

	for (uint8_t i = 0; i < FIFO_MULTIREAD_MAX_READERS; i++){
		pSelf->headIndex[i]			= 0;
		pSelf->readerActive[i]		= false;
	}

	memset((void*)pSelf->pTabPtr, 0, queueSize * elementSize);

	pSelf->state 				= FIFOMultiread_State_Ready;

	return FIFOMultiread_Status_OK;
}

FIFOMultiread_Status_TypeDef FIFOMultiread_clear(volatile FIFOMultiread_TypeDef* pSelf){

	if (pSelf == NULL){
		return FIFOMultiread_Status_Error;
	}

	if (pSelf->state == FIFOMultiread_State_UnInitialized){
		return FIFOMultiread_Status_UnInitializedError;
	}

	if (pSelf->notEnqueueOperationInProgress != false){
		return FIFOMultiread_Status_OperationInProgressError;
	}

	pSelf->tailIndex					= 0;
	for (uint8_t i = 0; i < FIFO_MULTIREAD_MAX_READERS; i++){
		pSelf->headIndex[i]			= 0;
	}

	return FIFOMultiread_Status_OK;
}

FIFOMultiread_Status_TypeDef FIFOMultiread_registerReader(volatile FIFOMultiread_TypeDef* pSelf, volatile FIFOMultireadReader_TypeDef* pReaderHandler){

	if ((pSelf==NULL) || (pReaderHandler == NULL)){
		return FIFOMultiread_Status_Error;
	}

	if (pSelf->state == FIFOMultiread_State_UnInitialized){
		return FIFOMultiread_Status_UnInitializedError;
	}

	if (pSelf->notEnqueueOperationInProgress){
		return FIFOMultiread_Status_OperationInProgressError;
	}
	pSelf->notEnqueueOperationInProgress = true; //TODO make atomic

	FIFOMultiread_Status_TypeDef ret = FIFOMultiread_Status_OK;

	pReaderHandler->readerId	= FIFO_MULTIREAD_MAX_READERS;
	for (uint8_t i=0; i<FIFO_MULTIREAD_MAX_READERS; i++){
		if (pSelf->readerActive[i] == false){
			pSelf->headIndex[i]		= pSelf->tailIndex;
			pSelf->readerActive[i]	= true;
			pReaderHandler->readerId	= i;
			break;
		}
	}

	if (pReaderHandler->readerId != FIFO_MULTIREAD_MAX_READERS){
		pReaderHandler->pFifoHandler	= pSelf;
	} else {
		ret = FIFOMultiread_Status_TooManyRegisteredReadedsError;
	}

	pSelf->notEnqueueOperationInProgress = false; //TODO make atomic
	return ret;
}

FIFOMultiread_Status_TypeDef FIFOMultiread_unregisterReader(volatile FIFOMultireadReader_TypeDef* pSelfReader){

	if (pSelfReader == NULL){
		return FIFOMultiread_Status_Error;
	}

	if (pSelfReader->pFifoHandler->state == FIFOMultiread_State_UnInitialized){
		return FIFOMultiread_Status_UnInitializedError;
	}

	FIFOMultiread_Status_TypeDef ret = FIFOMultiread_Status_OK;

	if (pSelfReader->pFifoHandler->notEnqueueOperationInProgress){
		return FIFOMultiread_Status_OperationInProgressError;
	}
	pSelfReader->pFifoHandler->notEnqueueOperationInProgress = true; //TODO make atomic

	if (pSelfReader->pFifoHandler->readerActive[pSelfReader->readerId] == false){
		ret = FIFOMultiread_Status_NotRegisteredReaderError;
	}

	pSelfReader->pFifoHandler->readerActive[pSelfReader->readerId] = false;

	pSelfReader->pFifoHandler->notEnqueueOperationInProgress = false; //TODO make atomic
	return ret;
}

FIFOMultiread_Status_TypeDef FIFOMultiread_enqueue(volatile FIFOMultiread_TypeDef* volatile pSelf, volatile void* volatile pElement){

	if ((pSelf == NULL) || (pElement == NULL)){
		return FIFOMultiread_Status_Error;
	}

	if (pSelf->state == FIFOMultiread_State_UnInitialized){
		return FIFOMultiread_Status_UnInitializedError;
	}

	if (FIFOMultiread_isFull(pSelf)){
		return FIFOMultiread_Status_Full;
	}

	volatile void* pTo = ((uint8_t*)pSelf->pTabPtr) + ((pSelf->tailIndex % pSelf->queueLength) * pSelf->elementSize);
	for (uint16_t i=0; i<pSelf->elementSize; i++){
		*(((uint8_t*)pTo) + i) = *(((uint8_t*)pElement) + i);
	}
	pSelf->tailIndex++;

	if ((pSelf->notEnqueueOperationInProgress == false) && (pSelf->tailIndex > (pSelf->queueLength * FIFO_MULTIREAD_RESIZE_FACTOR))){
		for (uint8_t i = 0; i < FIFO_MULTIREAD_MAX_READERS; i++){
			if (pSelf->readerActive[i]){
				pSelf->headIndex[i] -= pSelf->queueLength;
			}
		}
		pSelf->tailIndex -= pSelf->queueLength;
	}

	return FIFOMultiread_Status_OK;
}

bool FIFOMultiread_isFull(volatile FIFOMultiread_TypeDef* volatile pSelf){

	if (pSelf == NULL){
		return FIFOMultiread_Status_Error;
	}

	if (pSelf->state == FIFOMultiread_State_UnInitialized){
		return FIFOMultiread_Status_UnInitializedError;
	}

	bool ret = false;

	for (uint8_t i = 0; i < FIFO_MULTIREAD_MAX_READERS; i++){
		if (pSelf->readerActive[i] && (pSelf->tailIndex - pSelf->headIndex[i]) >= pSelf->queueLength){
			ret = true;
			break;
		}
	}

	return ret;
}

bool FIFOMultiread_isEmpty(volatile FIFOMultireadReader_TypeDef* volatile pSelf){

	if (pSelf == NULL){
		return true;
	}

	if (pSelf->pFifoHandler->state == FIFOMultiread_State_UnInitialized){
		return true;
	}

	if (pSelf->pFifoHandler->readerActive[pSelf->readerId] == false){
		return true;
	}

	bool ret = false;
	if (pSelf->pFifoHandler->headIndex[pSelf->readerId] == pSelf->pFifoHandler->tailIndex){
		ret = true;
	} else {
		ret = false;
	}

	return ret;
}

FIFOMultiread_Status_TypeDef FIFOMultiread_dequeue(volatile FIFOMultireadReader_TypeDef* volatile pSelfReader, volatile void* volatile pRetElement){

	if ((pSelfReader == NULL) || (pRetElement == NULL)) {
		return FIFOMultiread_Status_Error;
	}

	if (pSelfReader->pFifoHandler->state == FIFOMultiread_State_UnInitialized){
		return FIFOMultiread_Status_UnInitializedError;
	}

	FIFOMultiread_Status_TypeDef ret = FIFOMultiread_Status_OK;

	if (pSelfReader->pFifoHandler->notEnqueueOperationInProgress){
		return FIFOMultiread_Status_OperationInProgressError;
	}
	pSelfReader->pFifoHandler->notEnqueueOperationInProgress = true; //TODO make atomic

	if (pSelfReader->pFifoHandler->readerActive[pSelfReader->readerId] == false){
		ret = FIFOMultiread_Status_NotRegisteredReaderError;
	} else {

		if (FIFOMultiread_isEmpty(pSelfReader)){
			ret = FIFOMultiread_Status_Empty;
		} else {

			volatile void* pFrom = ((uint8_t*)pSelfReader->pFifoHandler->pTabPtr) + ((pSelfReader->pFifoHandler->headIndex[pSelfReader->readerId] % pSelfReader->pFifoHandler->queueLength) * pSelfReader->pFifoHandler->elementSize);
			for (uint16_t i=0; i<pSelfReader->pFifoHandler->elementSize; i++){
				*(((uint8_t*)pRetElement) + i) = *(((uint8_t*)pFrom) + i);
			}

			pSelfReader->pFifoHandler->headIndex[pSelfReader->readerId]++;
		}
	}

	pSelfReader->pFifoHandler->notEnqueueOperationInProgress = false; //TODO make atomic
	return ret;
}

FIFOMultiread_Status_TypeDef FIFOMultiread_lastElement(volatile FIFOMultireadReader_TypeDef* volatile pSelfReader, volatile void* volatile pRetElement){

	if ((pSelfReader == NULL) || (pRetElement == NULL)) {
		return FIFOMultiread_Status_Error;
	}

	if (pSelfReader->pFifoHandler->state == FIFOMultiread_State_UnInitialized){
		return FIFOMultiread_Status_UnInitializedError;
	}

	if (pSelfReader->pFifoHandler->readerActive[pSelfReader->readerId] == false){
		return FIFOMultiread_Status_NotRegisteredReaderError;
	}

	FIFOMultiread_Status_TypeDef ret = FIFOMultiread_Status_OK;

	if (pSelfReader->pFifoHandler->notEnqueueOperationInProgress){
		return FIFOMultiread_Status_OperationInProgressError;
	}
	pSelfReader->pFifoHandler->notEnqueueOperationInProgress = true; //TODO make atomic

	if (FIFOMultiread_isEmpty(pSelfReader)){
		ret = FIFOMultiread_Status_Empty;
	} else {
		volatile void* pFrom = ((uint8_t*)pSelfReader->pFifoHandler->pTabPtr) + ((pSelfReader->pFifoHandler->headIndex[pSelfReader->readerId] % pSelfReader->pFifoHandler->queueLength) * pSelfReader->pFifoHandler->elementSize);
		for (uint16_t i=0; i<pSelfReader->pFifoHandler->elementSize; i++){
			*(((uint8_t*)pRetElement) + i) = *(((uint8_t*)pFrom) + i);
		}
	}

	pSelfReader->pFifoHandler->notEnqueueOperationInProgress = false; //TODO make atomic
	return ret;
}

FIFOMultiread_Status_TypeDef FIFOMultiread_elementValOnPosition(volatile FIFOMultireadReader_TypeDef* volatile pSelfReader, uint16_t positionIndex, volatile void* volatile pRetElement){

	if ((pSelfReader == NULL) || (pRetElement == NULL)) {
		return FIFOMultiread_Status_Error;
	}

	if (pSelfReader->pFifoHandler->state == FIFOMultiread_State_UnInitialized){
		return FIFOMultiread_Status_UnInitializedError;
	}

	FIFOMultiread_Status_TypeDef ret = FIFOMultiread_Status_OK;

	uint16_t size = 0;
	if ((ret = FIFOMultiread_elementsNumber(pSelfReader, &size)) != FIFOMultiread_Status_OK){
		return ret;
	}
	if (positionIndex >= size){
		return FIFOMultiread_Status_InvalidPositionArgumentError;
	}

	if (pSelfReader->pFifoHandler->notEnqueueOperationInProgress){
		return FIFOMultiread_Status_OperationInProgressError;
	}
	pSelfReader->pFifoHandler->notEnqueueOperationInProgress = true; //TODO make atomic

	volatile void* pFrom = ((uint8_t*)pSelfReader->pFifoHandler->pTabPtr) + (((pSelfReader->pFifoHandler->headIndex[pSelfReader->readerId] + positionIndex) % pSelfReader->pFifoHandler->queueLength) * pSelfReader->pFifoHandler->elementSize);
	for (uint16_t i=0; i<pSelfReader->pFifoHandler->elementSize; i++){
		*(((uint8_t*)pRetElement) + i) = *(((uint8_t*)pFrom) + i);
	}

	pSelfReader->pFifoHandler->notEnqueueOperationInProgress = false; //TODO make atomic
	return ret;
}

FIFOMultiread_Status_TypeDef FIFOMultiread_elementsNumber(volatile FIFOMultireadReader_TypeDef* volatile pSelfReader, volatile uint16_t* volatile pRetElementsNumber){

	if ((pSelfReader == NULL) || (pRetElementsNumber == NULL)) {
		return FIFOMultiread_Status_Error;
	}

	if (pSelfReader->pFifoHandler->state == FIFOMultiread_State_UnInitialized){
		return FIFOMultiread_Status_UnInitializedError;
	}

	FIFOMultiread_Status_TypeDef ret = FIFOMultiread_Status_OK;

	if (pSelfReader->pFifoHandler->readerActive[pSelfReader->readerId] == false){
		ret = FIFOMultiread_Status_NotRegisteredReaderError;
	} else {
		*pRetElementsNumber = pSelfReader->pFifoHandler->tailIndex - pSelfReader->pFifoHandler->headIndex[pSelfReader->readerId];
	}
	return ret;
}
