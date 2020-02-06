/*
 * FIFOQueue.c
 *
 *  Created on: 03.06.2017
 *      Author: Michal Kowalik
 */

#include "user/fifo_queue.h"
#include "string.h"

FIFO_Status_TypeDef FIFOQueue_init(volatile FIFOQueue_TypeDef* pSelf, volatile void* pTabPtrArg, uint8_t elementSize, uint16_t queueSize){

	if (pSelf == NULL || pTabPtrArg == NULL){
		return FIFO_Status_Error;
	}

	if (pSelf->state != FIFO_State_UnInitialized){
		return FIFO_Status_UnInitializedError;
	}

	pSelf->pTabPtr			= pTabPtrArg;
	pSelf->elementSize		= elementSize;
	pSelf->queueLength		= queueSize;
	pSelf->headIndex		= 0;
	pSelf->tailIndex		= 0;

	pSelf->state			= FIFO_State_Ready;

	return FIFO_Status_OK;
}

FIFO_Status_TypeDef FIFOQueue_enqueue(volatile FIFOQueue_TypeDef* pSelf, volatile void* pElement){

	if (pSelf == NULL || pElement == NULL){
		return FIFO_Status_Error;
	}

	if (pSelf->state == FIFO_State_UnInitialized){
		return FIFO_Status_UnInitializedError;
	}

	if (FIFOQueue_isFull(pSelf)){
		return FIFO_Status_Full;
	}

	(pSelf->tailIndex)++;

	memcpy((void*)(((uint8_t*)pSelf->pTabPtr) + ((pSelf->tailIndex % pSelf->queueLength) * pSelf->elementSize)), (void*)pElement, pSelf->elementSize);

	if (pSelf->dequeueInProgress == false){
		if ((pSelf->tailIndex > pSelf->queueLength) && (pSelf->headIndex > pSelf->queueLength)){
			pSelf->tailIndex -= pSelf->queueLength;
			pSelf->headIndex -= pSelf->queueLength;
		}
	}

	return FIFO_Status_OK;

}

FIFO_Status_TypeDef FIFOQueue_dequeue(volatile FIFOQueue_TypeDef* pSelf, volatile void* pRetElement){

	if (pSelf == NULL || pRetElement == NULL){
		return FIFO_Status_Error;
	}

	if (pSelf->state == FIFO_State_UnInitialized){
		return FIFO_Status_UnInitializedError;
	}

	pSelf->dequeueInProgress = true;

	if (FIFOQueue_isEmpty(pSelf)){
		return FIFO_Status_Empty;
	}

	memcpy((void*)pRetElement, ((uint8_t*)pSelf->pTabPtr) + ((pSelf->headIndex % pSelf->queueLength) * pSelf->elementSize), pSelf->elementSize);

	(pSelf->headIndex)++;

	pSelf->dequeueInProgress = false;

	return FIFO_Status_OK;
}

FIFO_Status_TypeDef FIFOQueue_lastElement(volatile FIFOQueue_TypeDef* pSelf, volatile void* pRetElement){

	if (pSelf == NULL || pRetElement == NULL){
		return FIFO_Status_Error;
	}

	if (pSelf->state == FIFO_State_UnInitialized){
		return FIFO_Status_UnInitializedError;
	}

	if (FIFOQueue_isEmpty(pSelf)){
		return FIFO_Status_Empty;
	}

	memcpy((void*)pRetElement, (void*)(((uint8_t*)pSelf->pTabPtr) + ((pSelf->headIndex % pSelf->queueLength) * pSelf->elementSize)), pSelf->elementSize);

	return FIFO_Status_OK;
}

FIFO_Status_TypeDef FIFOQueue_elementsNumber(volatile FIFOQueue_TypeDef* pSelf, volatile uint16_t* pRetElementsNumber){

	if (pSelf == NULL || pRetElementsNumber == NULL){
		return FIFO_Status_Error;
	}

	if (pSelf->state == FIFO_State_UnInitialized){
		return FIFO_Status_UnInitializedError;
	}

	*pRetElementsNumber = (pSelf->tailIndex - pSelf->headIndex);

	return FIFO_Status_OK;
}

FIFO_Status_TypeDef	FIFOQueue_clear(volatile FIFOQueue_TypeDef* pSelf){

	if (pSelf == NULL){
		return FIFO_Status_Error;
	}

	if (pSelf->state == FIFO_State_UnInitialized){
		return FIFO_Status_UnInitializedError;
	}

	if (pSelf->dequeueInProgress){
		return FIFO_Status_DequeueInProgressError;
	}

	pSelf->tailIndex			= 0;
	pSelf->headIndex			= 0;

	return FIFO_Status_OK;
}

bool FIFOQueue_isFull(volatile FIFOQueue_TypeDef* pSelf){

	if (pSelf == NULL){
		return FIFO_Status_Error;
	}

	if (pSelf->state == FIFO_State_UnInitialized){
		return true;
	}

	return ((pSelf->tailIndex - pSelf->headIndex) >= pSelf->queueLength);
}

bool FIFOQueue_isEmpty(volatile FIFOQueue_TypeDef* pSelf){

	if (pSelf == NULL){
		return FIFO_Status_Error;
	}

	if (pSelf->state == FIFO_State_UnInitialized){
		return true;
	}

	return (pSelf->tailIndex == pSelf->headIndex);
}
