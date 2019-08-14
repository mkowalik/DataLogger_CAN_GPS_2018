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
	pSelf->elementsNumber	= 0;
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

	pSelf->tailIndex = (pSelf->tailIndex + 1) % (pSelf->queueLength); // Notice incrementing tail value
	pSelf->elementsNumber++;

	memcpy(pSelf->pTabPtr + (pSelf->tailIndex * pSelf->elementSize), pElement, pSelf->elementSize);

	return FIFO_Status_OK;

}

FIFO_Status_TypeDef FIFOQueue_dequeue(volatile FIFOQueue_TypeDef* pSelf, volatile void* pRetElement){

	if (pSelf == NULL || pRetElement == NULL){
		return FIFO_Status_Error;
	}

	if (pSelf->state == FIFO_State_UnInitialized){
		return FIFO_Status_UnInitializedError;
	}

	if (FIFOQueue_isEmpty(pSelf)){
		return FIFO_Status_Empty;
	}

	memcpy(pRetElement, pSelf->pTabPtr + (pSelf->headIndex * pSelf->elementSize), pSelf->elementSize);

	pSelf->headIndex = (pSelf->headIndex + 1) % (pSelf->queueLength); // Notice incrementing head value
	pSelf->elementsNumber--;

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

	memcpy(pRetElement, pSelf->pTabPtr + (pSelf->headIndex * pSelf->elementSize), pSelf->elementSize);

	pSelf->headIndex = (pSelf->headIndex + 1) % (pSelf->queueLength); // Notice incrementing head value
	pSelf->elementsNumber--;

	return FIFO_Status_OK;
}

FIFO_Status_TypeDef FIFOQueue_elementsNumber(volatile FIFOQueue_TypeDef* pSelf, volatile uint16_t* pRetElementsNumber){

	if (pSelf == NULL || pRetElementsNumber == NULL){
		return FIFO_Status_Error;
	}

	if (pSelf->state == FIFO_State_UnInitialized){
		return FIFO_Status_UnInitializedError;
	}

	*pRetElementsNumber = pSelf->elementsNumber;

	return FIFO_Status_OK;
}

uint8_t FIFOQueue_isFull(volatile FIFOQueue_TypeDef* pSelf){

	if (pSelf == NULL){
		return FIFO_Status_Error;
	}

	if (pSelf->state == FIFO_State_UnInitialized){
		return 1;
	}

	return pSelf->elementsNumber >= pSelf->queueLength;
}

uint8_t FIFOQueue_isEmpty(volatile FIFOQueue_TypeDef* pSelf){

	if (pSelf == NULL){
		return FIFO_Status_Error;
	}

	if (pSelf->state == FIFO_State_UnInitialized){
		return 1;
	}

	return (pSelf->elementsNumber == 0) ? 1 : 0;
}
