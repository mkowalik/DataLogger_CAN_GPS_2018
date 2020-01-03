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

	if (pSelf==NULL || pTabPtrArg == NULL){
		return FIFOMultiread_Status_Error;
	}

	if (pSelf->state != FIFOMultiread_State_UnInitialized){
		return FIFOMultiread_Status_Error;
	}

	pSelf->pTabPtr						= pTabPtrArg;
	pSelf->elementSize					= elementSize;
	pSelf->queueLength					= queueSize;
	pSelf->tailIndex					= 0;
	pSelf->operationInProgressCounter	= 0;

	for (uint8_t i = 0; i < FIFO_MULTIREAD_MAX_READERS; i++){
//		pSelf->elementsNumber[i]	= 0;
		pSelf->headIndex[i]			= 0;
		pSelf->readerActive[i]		= false;
	}

	memset((void*)pSelf->pTabPtr, 0, queueSize * elementSize);

	pSelf->state 				= FIFOMultiread_State_Ready;

	return FIFOMultiread_Status_OK;
}

FIFOMultiread_Status_TypeDef FIFOMultiread_registerReader(volatile FIFOMultiread_TypeDef* pSelfFifo, volatile FIFOMultireadReader_TypeDef* pReaderHandler){

	if (pSelfFifo == NULL || pReaderHandler == NULL){
		return FIFOMultiread_Status_Error;
	}

	if (pSelfFifo->state == FIFOMultiread_State_UnInitialized){
		return FIFOMultiread_Status_UnInitializedError;
	}

	FIFOMultiread_Status_TypeDef ret = FIFOMultiread_Status_OK;

	++(pSelfFifo->operationInProgressCounter); //TODO make atomic

	pReaderHandler->readerId	= FIFO_MULTIREAD_MAX_READERS;
	for (uint8_t i=0; i<FIFO_MULTIREAD_MAX_READERS; i++){
		if (pSelfFifo->readerActive[i] == false){
			pSelfFifo->headIndex[i]		= pSelfFifo->tailIndex;
			pSelfFifo->readerActive[i]	= true;
//			pSelfFifo->elementsNumber	= pSelfFifo->globalEnqueuedElementsNumber;
			pReaderHandler->readerId	= i;
			break;
		}
	}

	if (pReaderHandler->readerId != FIFO_MULTIREAD_MAX_READERS){
		pReaderHandler->pFifoHandler	= pSelfFifo;
	} else {
		ret = FIFOMultiread_Status_TooManyRegisteredReadedsError;
	}

	--(pSelfFifo->operationInProgressCounter); //TODO make atomic
	return ret;
}

FIFOMultiread_Status_TypeDef FIFOMultiread_unregisterReader(volatile FIFOMultireadReader_TypeDef* pReaderHandler){

	if (pReaderHandler == NULL){
		return FIFOMultiread_Status_Error;
	}

	if (pReaderHandler->pFifoHandler->state == FIFOMultiread_State_UnInitialized){
		return FIFOMultiread_Status_UnInitializedError;
	}

	FIFOMultiread_Status_TypeDef ret = FIFOMultiread_Status_OK;

	++(pReaderHandler->pFifoHandler->operationInProgressCounter); //TODO make atomic

	if (pReaderHandler->pFifoHandler->readerActive[pReaderHandler->readerId] == false){
		ret = FIFOMultiread_Status_NotRegisteredReaderError;
	}

	pReaderHandler->pFifoHandler->readerActive[pReaderHandler->readerId] = false;

	--(pReaderHandler->pFifoHandler->operationInProgressCounter); //TODO make atomic
	return ret;
}

FIFOMultiread_Status_TypeDef FIFOMultiread_enqueue(volatile FIFOMultiread_TypeDef* volatile pSelf, volatile void* volatile pElement){

	if (pSelf == NULL || pElement == NULL){
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

	if (pSelf->operationInProgressCounter == 0 && pSelf->tailIndex > (pSelf->queueLength * FIFO_MULTIREAD_RESIZE_FACTOR)){
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

	++(pSelf->operationInProgressCounter); //TODO make atomic

	bool ret = false;

	for (uint8_t i = 0; i < FIFO_MULTIREAD_MAX_READERS; i++){
		if (pSelf->readerActive[i] && (pSelf->tailIndex - pSelf->headIndex[i]) >= pSelf->queueLength){
			ret = true;
			break;
		}
	}

	--(pSelf->operationInProgressCounter); //TODO make atomic
	return ret;
}

bool FIFOMultiread_isEmpty(volatile FIFOMultireadReader_TypeDef* volatile pSelf){

	if (pSelf == NULL){
		return true;
	}

	if (pSelf->pFifoHandler->state == FIFOMultiread_State_UnInitialized){
		return true;
	}

	++(pSelf->pFifoHandler->operationInProgressCounter); //TODO make atomic

	if (pSelf->pFifoHandler->readerActive[pSelf->readerId] == false){
		return true;
	}

	bool ret = false;

	if (pSelf->pFifoHandler->headIndex[pSelf->readerId] == pSelf->pFifoHandler->tailIndex){
		ret = true;
	} else {
		ret = false;
	}

	--(pSelf->pFifoHandler->operationInProgressCounter); //TODO make atomic
	return ret;
}

FIFOMultiread_Status_TypeDef FIFOMultiread_dequeue(volatile FIFOMultireadReader_TypeDef* volatile pSelf, volatile void* volatile pRetElement){


	if (pSelf == NULL || pRetElement == NULL){
		return FIFOMultiread_Status_Error;
	}

	if (pSelf->pFifoHandler->state == FIFOMultiread_State_UnInitialized){
		return FIFOMultiread_Status_UnInitializedError;
	}

	FIFOMultiread_Status_TypeDef ret = FIFOMultiread_Status_OK;

	++(pSelf->pFifoHandler->operationInProgressCounter); //TODO make atomic

	if (pSelf->pFifoHandler->readerActive[pSelf->readerId] == false){
		ret = FIFOMultiread_Status_NotRegisteredReaderError;
	} else {

		if (FIFOMultiread_isEmpty(pSelf)){
			ret = FIFOMultiread_Status_Empty;
		} else {

			volatile void* pFrom = ((uint8_t*)pSelf->pFifoHandler->pTabPtr) + ((pSelf->pFifoHandler->headIndex[pSelf->readerId] % pSelf->pFifoHandler->queueLength) * pSelf->pFifoHandler->elementSize);
			for (uint16_t i=0; i<pSelf->pFifoHandler->elementSize; i++){
				*(((uint8_t*)pRetElement) + i) = *(((uint8_t*)pFrom) + i);
			}

			pSelf->pFifoHandler->headIndex[pSelf->readerId]++;
		}
	}

	--(pSelf->pFifoHandler->operationInProgressCounter); //TODO make atomic
	return ret;
}

FIFOMultiread_Status_TypeDef FIFOMultiread_lastElement(volatile FIFOMultireadReader_TypeDef* volatile pSelf, volatile void* volatile pRetElement){

	if (pSelf == NULL || pRetElement == NULL){
		return FIFOMultiread_Status_Error;
	}

	if (pSelf->pFifoHandler->state == FIFOMultiread_State_UnInitialized){
		return FIFOMultiread_Status_UnInitializedError;
	}

	if (pSelf->pFifoHandler->readerActive[pSelf->readerId] == false){
		return FIFOMultiread_Status_NotRegisteredReaderError;
	}

	FIFOMultiread_Status_TypeDef ret = FIFOMultiread_Status_OK;

	++(pSelf->pFifoHandler->operationInProgressCounter); //TODO make atomic

	if (FIFOMultiread_isEmpty(pSelf)){
		ret = FIFOMultiread_Status_Empty;
	} else {
		volatile void* pFrom = ((uint8_t*)pSelf->pFifoHandler->pTabPtr) + ((pSelf->pFifoHandler->headIndex[pSelf->readerId] % pSelf->pFifoHandler->queueLength) * pSelf->pFifoHandler->elementSize);
		for (uint16_t i=0; i<pSelf->pFifoHandler->elementSize; i++){
			*(((uint8_t*)pRetElement) + i) = *(((uint8_t*)pFrom) + i);
		}
	}

	--(pSelf->pFifoHandler->operationInProgressCounter); //TODO make atomic
	return ret;
}

FIFOMultiread_Status_TypeDef FIFOMultiread_elementValOnPosition(volatile FIFOMultireadReader_TypeDef* volatile pSelf, uint16_t positionIndex, volatile void* volatile pRetElement){

	if (pSelf == NULL || pRetElement == NULL){
		return FIFOMultiread_Status_Error;
	}

	if (pSelf->pFifoHandler->state == FIFOMultiread_State_UnInitialized){
		return FIFOMultiread_Status_UnInitializedError;
	}

	FIFOMultiread_Status_TypeDef ret = FIFOMultiread_Status_OK;

	uint16_t size = 0;
	if ((ret = FIFOMultiread_elementsNumber(pSelf, &size)) != FIFOMultiread_Status_OK){
		return ret;
	}
	if (positionIndex >= size){
		return FIFOMultiread_Status_InvalidPositionArgumentError;
	}

	++(pSelf->pFifoHandler->operationInProgressCounter); //TODO make atomic

	volatile void* pFrom = ((uint8_t*)pSelf->pFifoHandler->pTabPtr) + (((pSelf->pFifoHandler->headIndex[pSelf->readerId] + positionIndex) % pSelf->pFifoHandler->queueLength) * pSelf->pFifoHandler->elementSize);
	for (uint16_t i=0; i<pSelf->pFifoHandler->elementSize; i++){
		*(((uint8_t*)pRetElement) + i) = *(((uint8_t*)pFrom) + i);
	}

	--(pSelf->pFifoHandler->operationInProgressCounter); //TODO make atomic
	return ret;
}

FIFOMultiread_Status_TypeDef FIFOMultiread_elementsNumber(volatile FIFOMultireadReader_TypeDef* volatile pSelf, volatile uint16_t* volatile retElementsNumber){

	if (pSelf == NULL || retElementsNumber == NULL){
		return FIFOMultiread_Status_Error;
	}

	if (pSelf->pFifoHandler->state == FIFOMultiread_State_UnInitialized){
		return FIFOMultiread_Status_UnInitializedError;
	}

	FIFOMultiread_Status_TypeDef ret = FIFOMultiread_Status_OK;

	++(pSelf->pFifoHandler->operationInProgressCounter); //TODO make atomic

	if (pSelf->pFifoHandler->readerActive[pSelf->readerId] == false){
		ret = FIFOMultiread_Status_NotRegisteredReaderError;
	} else {
		*retElementsNumber = pSelf->pFifoHandler->tailIndex - pSelf->pFifoHandler->headIndex[pSelf->readerId];
	}
	--(pSelf->pFifoHandler->operationInProgressCounter); //TODO make atomic
	return ret;
}
