/*
 * FIFOQueue.h
 *
 *  Created on: 03.06.2017
 *      Author: Kowalik
 */

#ifndef FIFOQUEUE_H_
#define FIFOQUEUE_H_

#include "stdint.h"

typedef enum {
	FIFOStatus_OK = 0,
	FIFOStatus_Full,
	FIFOStatus_Empty,
	FIFOStatus_Error
} FIFOStatus_TypeDef;

typedef struct {
	void* pTabPtr;
	uint8_t elementSize;
	uint16_t queueLength;
	volatile uint16_t elementsNumber;
	volatile uint16_t headIndex;
	volatile uint16_t tailIndex;
} FIFOQueue_TypeDef;

FIFOStatus_TypeDef FIFOQueue_init(FIFOQueue_TypeDef* self, void* pTabPtrArg, uint8_t elementSize, uint16_t size);
FIFOStatus_TypeDef FIFOQueue_enqueue(FIFOQueue_TypeDef* self, void* pElement);
FIFOStatus_TypeDef FIFOQueue_dequeue(FIFOQueue_TypeDef* self, void* pRetElement);
uint16_t FIFOQueue_elementsNumber(FIFOQueue_TypeDef* self);
uint8_t FIFOQueue_full(FIFOQueue_TypeDef* self);



#endif /* FIFOQUEUE_H_ */
