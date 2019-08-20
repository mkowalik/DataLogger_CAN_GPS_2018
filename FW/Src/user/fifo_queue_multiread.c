/*
 * fifo_queue_multireadears.c
 *
 *  Created on: 28.06.2019
 *      Author: Michal Kowalik
 */

#include <user/fifo_queue_multiread.h>
#include "stdint.h"
#include "string.h"

FIFOMultiread_Status_TypeDef FIFOMultiread_init(volatile FIFOMultiread_TypeDef* pSelf, volatile void* pTabPtrArg, uint8_t elementSize, uint16_t queueSize){

	if (pSelf==NULL || pTabPtrArg == NULL){
		return FIFOMultiread_Status_Error;
	}

	if (pSelf->state != FIFOMultiread_State_UnInitialized){
		return FIFOMultiread_Status_Error;
	}

	pSelf->pTabPtr		= pTabPtrArg;
	pSelf->elementSize	= elementSize;
	pSelf->queueLength	= queueSize;
	pSelf->tailIndex	= 0;

	for (uint8_t i = 0; i < FIFO_MULTIREAD_MAX_READERS; i++){
		pSelf->elementsNumber[i]	= 0;
		pSelf->headIndex[i]			= 0;
		pSelf->readerActive[i]		= false;
	}

	memset((void*)pSelf->pTabPtr, 0, queueSize * elementSize);

	pSelf->state 				= FIFOMultiread_State_Ready;

	return FIFOMultiread_Status_OK;
}

FIFOMultiread_Status_TypeDef FIFOMultiread_registerReaderIdentifier(volatile FIFOMultiread_TypeDef* pSelfFifo, volatile FIFOMultireadReaderIdentifier_TypeDef* pReaderHandler){

	if (pSelfFifo == NULL || pReaderHandler == NULL){
		return FIFOMultiread_Status_Error;
	}

	if (pSelfFifo->state == FIFOMultiread_State_UnInitialized){
		return FIFOMultiread_Status_UnInitializedError;
	}

	pReaderHandler->readerId	= FIFO_MULTIREAD_MAX_READERS;
	for (uint8_t i=0; i<FIFO_MULTIREAD_MAX_READERS; i++){
		if (pSelfFifo->readerActive[i] == false){
			pSelfFifo->readerActive[i]	= true;
			pReaderHandler->readerId	= i;
			break;
		}
	}

	if (pReaderHandler->readerId != FIFO_MULTIREAD_MAX_READERS){
		pReaderHandler->pFifoHandler	= pSelfFifo;
	} else {
		return FIFOMultiread_Status_TooManyRegisteredReadedsError;
	}

	return FIFOMultiread_Status_OK;

}


FIFOMultiread_Status_TypeDef FIFOMultiread_unregisterReaderIdentifier(volatile FIFOMultireadReaderIdentifier_TypeDef* pReaderHandler){

	if (pReaderHandler == NULL){
		return FIFOMultiread_Status_Error;
	}

	if (pReaderHandler->pFifoHandler->state == FIFOMultiread_State_UnInitialized){
		return FIFOMultiread_Status_UnInitializedError;
	}

	if (pReaderHandler->pFifoHandler->readerActive[pReaderHandler->readerId] == false){
		return FIFOMultiread_Status_NotRegisteredReaderError;
	}

	pReaderHandler->pFifoHandler->readerActive[pReaderHandler->readerId] = false;

	return FIFOMultiread_Status_OK;
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
	memcpy((void*)pSelf->pTabPtr + (pSelf->tailIndex * pSelf->elementSize), (void*)pElement, pSelf->elementSize);
	pSelf->tailIndex = (pSelf->tailIndex + 1) % (pSelf->queueLength); // Notice incrementing tail value

	for (uint8_t i = 0; i < FIFO_MULTIREAD_MAX_READERS; i++){
		if (pSelf->readerActive[i]){
			pSelf->elementsNumber[i]++;
		}
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

	for (uint8_t i = 0; i < FIFO_MULTIREAD_MAX_READERS; i++){
		if (pSelf->readerActive[i] && pSelf->elementsNumber[i] >= pSelf->queueLength){
			return 1;
		}
	}

	return 0;

}

bool FIFOMultiread_isEmpty(volatile FIFOMultireadReaderIdentifier_TypeDef* volatile pSelf){

	if (pSelf == NULL){
		return true;
	}

	if (pSelf->pFifoHandler->state == FIFOMultiread_State_UnInitialized){
		return true;
	}

	if (pSelf->pFifoHandler->readerActive[pSelf->readerId] == false){
		return true;
	}

	if (pSelf->pFifoHandler->elementsNumber[pSelf->readerId] == 0){
		return true;
	} else {
		return false;
	}
}

FIFOMultiread_Status_TypeDef FIFOMultiread_dequeue(volatile FIFOMultireadReaderIdentifier_TypeDef* volatile pSelf, volatile void* volatile pRetElement){

	if (pSelf == NULL || pRetElement == NULL){
		return FIFOMultiread_Status_Error;
	}

	if (pSelf->pFifoHandler->state == FIFOMultiread_State_UnInitialized){
		return FIFOMultiread_Status_UnInitializedError;
	}

	if (pSelf->pFifoHandler->readerActive[pSelf->readerId] == false){
		return FIFOMultiread_Status_NotRegisteredReaderError;
	}

	if (FIFOMultiread_isEmpty(pSelf)){
		return FIFOMultiread_Status_Empty;
	}

	memcpy((void*)pRetElement, (void*)pSelf->pFifoHandler->pTabPtr + (pSelf->pFifoHandler->headIndex[pSelf->readerId] * pSelf->pFifoHandler->elementSize), pSelf->pFifoHandler->elementSize);

	pSelf->pFifoHandler->headIndex[pSelf->readerId] = (pSelf->pFifoHandler->headIndex[pSelf->readerId] + 1) % (pSelf->pFifoHandler->queueLength); // Notice incrementing head value
	pSelf->pFifoHandler->elementsNumber[pSelf->readerId]--;

	return FIFOMultiread_Status_OK;
}

FIFOMultiread_Status_TypeDef FIFOMultiread_lastElement(volatile FIFOMultireadReaderIdentifier_TypeDef* volatile pSelf, volatile void* volatile pRetElement){

	if (pSelf == NULL || pRetElement == NULL){
		return FIFOMultiread_Status_Error;
	}

	if (pSelf->pFifoHandler->state == FIFOMultiread_State_UnInitialized){
		return FIFOMultiread_Status_UnInitializedError;
	}

	if (pSelf->pFifoHandler->readerActive[pSelf->readerId] == false){
		return FIFOMultiread_Status_NotRegisteredReaderError;
	}

	if (FIFOMultiread_isEmpty(pSelf)){
		return FIFOMultiread_Status_Empty;
	}

	memcpy((void*)pRetElement, (void*)pSelf->pFifoHandler->pTabPtr + (pSelf->pFifoHandler->headIndex[pSelf->readerId] * pSelf->pFifoHandler->elementSize), pSelf->pFifoHandler->elementSize);

	return FIFOMultiread_Status_OK;
}

FIFOMultiread_Status_TypeDef FIFOMultiread_elementsNumber(volatile FIFOMultireadReaderIdentifier_TypeDef* volatile pSelf, volatile uint16_t* volatile retElementsNumber){

	if (pSelf == NULL || retElementsNumber == NULL){
		return FIFOMultiread_Status_Error;
	}

	if (pSelf->pFifoHandler->state == FIFOMultiread_State_UnInitialized){
		return FIFOMultiread_Status_UnInitializedError;
	}

	if (pSelf->pFifoHandler->readerActive[pSelf->readerId] == false){
		return FIFOMultiread_Status_NotRegisteredReaderError;
	}

	*retElementsNumber = pSelf->pFifoHandler->elementsNumber[pSelf->readerId];

	return FIFOMultiread_Status_OK;

}
