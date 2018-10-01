/*
 * FIFOQueue.c
 *
 *  Created on: 03.06.2017
 *      Author: Kowalik
 */

#include "user/fifo_queue.h"
#include "stdint.h"
#include "string.h"

FIFOStatus_TypeDef FIFOQueue_init(FIFOQueue_TypeDef* self, void* pTabPtrArg, uint8_t elementSize, uint16_t size){

	if (pTabPtrArg == NULL){
		return FIFOStatus_Error;
	}

	self->pTabPtr = pTabPtrArg;
	self->elementSize = elementSize;
	self->queueLength = size;
	self->elementsNumber = 0;
	self->headIndex = 0;
	self->tailIndex = 0;

	return FIFOStatus_OK;
}

FIFOStatus_TypeDef FIFOQueue_enqueue(FIFOQueue_TypeDef* pSelf,  void* pElement){

	if (FIFOQueue_full(pSelf)) return FIFOStatus_Full;

	pSelf->tailIndex = (pSelf->tailIndex + 1) % (pSelf->queueLength); // Notice incrementing tail value
	pSelf->elementsNumber++;

	memcpy(pSelf->pTabPtr + (pSelf->tailIndex * pSelf->elementSize), pElement, pSelf->elementSize);

	return FIFOStatus_OK;

}

FIFOStatus_TypeDef FIFOQueue_dequeue(FIFOQueue_TypeDef* pSelf, void* pRetElement){

	if (FIFOQueue_elementsNumber(pSelf) == 0) return FIFOStatus_Empty;

	memcpy(pRetElement, pSelf->pTabPtr + (pSelf->headIndex * pSelf->elementSize), pSelf->elementSize);

	pSelf->headIndex = (pSelf->headIndex + 1) % (pSelf->queueLength); // Notice incrementing head value
	pSelf->elementsNumber--;

	return FIFOStatus_OK;
}

uint16_t FIFOQueue_elementsNumber(FIFOQueue_TypeDef* pSelf){

	return pSelf->elementsNumber;

}

uint8_t FIFOQueue_full(FIFOQueue_TypeDef* pSelf){

	return pSelf->elementsNumber >= pSelf->queueLength;

}
