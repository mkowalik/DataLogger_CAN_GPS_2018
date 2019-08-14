/*
 * FIFOQueue.h
 *
 *  Created on: 03.06.2017
 *      Author: Michal Kowalik
 */

#ifndef FIFOQUEUE_H_
#define FIFOQUEUE_H_

#include <stdbool.h>
#include "stdint.h"

typedef enum {
	FIFO_Status_OK = 0,
	FIFO_Status_Full,
	FIFO_Status_Empty,
	FIFO_Status_UnInitializedError,
	FIFO_Status_Error
} FIFO_Status_TypeDef;

typedef enum {
	FIFO_State_UnInitialized = 0,
	FIFO_State_Ready,
} FIFO_State_TypeDef;

typedef struct {
	volatile void* volatile		pTabPtr;
	volatile uint8_t			elementSize;
	volatile uint16_t			queueLength;
	volatile FIFO_State_TypeDef	state;
	volatile uint16_t			elementsNumber;volatile
	volatile uint16_t			headIndex;
	volatile uint16_t			tailIndex;
} FIFOQueue_TypeDef;

FIFO_Status_TypeDef	FIFOQueue_init(volatile FIFOQueue_TypeDef* pSelf, volatile void* pTabPtrArg, uint8_t elementSize, uint16_t size);
FIFO_Status_TypeDef	FIFOQueue_enqueue(volatile FIFOQueue_TypeDef* pSelf, volatile void* pElement);
FIFO_Status_TypeDef	FIFOQueue_dequeue(volatile FIFOQueue_TypeDef* pSelf, volatile void* pRetElement);
FIFO_Status_TypeDef	FIFOQueue_lastElement(volatile FIFOQueue_TypeDef* pSelf, volatile void* pLastElement);
FIFO_Status_TypeDef	FIFOQueue_elementsNumber(volatile FIFOQueue_TypeDef* pSelf, volatile uint16_t* pRetElementsNumber);
uint8_t				FIFOQueue_isFull(volatile FIFOQueue_TypeDef* pSelf);
uint8_t				FIFOQueue_isEmpty(volatile FIFOQueue_TypeDef* pSelf);


#endif /* FIFOQUEUE_H_ */
