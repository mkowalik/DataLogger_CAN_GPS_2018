/*
 * fifo_queue_multireaders.h
 *
 *  Created on: 28.06.2019
 *      Author: Michal Kowalik
 */

#ifndef USER_FIFO_QUEUE_MULTIREAD_H_
#define USER_FIFO_QUEUE_MULTIREAD_H_

#include <stdbool.h>
#include "stdint.h"

#define FIFO_MULTIREAD_MAX_READERS		5
#define FIFO_MULTIREAD_RESIZE_FACTOR	2

typedef enum {
	FIFOMultiread_Status_OK = 0,
	FIFOMultiread_Status_Full,
	FIFOMultiread_Status_Empty,
	FIFOMultiread_Status_TooManyRegisteredReadedsError,
	FIFOMultiread_Status_UnInitializedError,
	FIFOMultiread_Status_NotRegisteredReaderError,
	FIFOMultiread_Status_InvalidPositionArgumentError,
	FIFOMultiread_Status_OperationInProgressError,
	FIFOMultiread_Status_Error
} FIFOMultiread_Status_TypeDef;

typedef enum {
	FIFOMultiread_State_UnInitialized = 0,
	FIFOMultiread_State_Ready,
} FIFOMultiread_State_TypeDef;

typedef struct {
	volatile void* volatile					pTabPtr;
	volatile uint8_t						elementSize;
	volatile uint32_t						queueLength;
	volatile FIFOMultiread_State_TypeDef	state;
	volatile uint32_t						headIndex[FIFO_MULTIREAD_MAX_READERS];
	volatile uint32_t						tailIndex;
	volatile bool							readerActive[FIFO_MULTIREAD_MAX_READERS];
	volatile bool							notEnqueueOperationInProgress;
} FIFOMultiread_TypeDef;

typedef struct {
	volatile FIFOMultiread_TypeDef* volatile	pFifoHandler;
	volatile uint8_t							readerId;
} FIFOMultireadReader_TypeDef;

FIFOMultiread_Status_TypeDef	FIFOMultiread_init(volatile FIFOMultiread_TypeDef* pSelf, volatile void* pTabPtrArg, uint8_t elementSize, uint16_t queueSize);
FIFOMultiread_Status_TypeDef	FIFOMultiread_clear(volatile FIFOMultiread_TypeDef* pSelf);
FIFOMultiread_Status_TypeDef	FIFOMultiread_registerReader(volatile FIFOMultiread_TypeDef* pSelfFifo, volatile FIFOMultireadReader_TypeDef* pReaderHandler);
FIFOMultiread_Status_TypeDef	FIFOMultiread_unregisterReader(volatile FIFOMultireadReader_TypeDef* pReaderHandler);

FIFOMultiread_Status_TypeDef	FIFOMultiread_enqueue(volatile FIFOMultiread_TypeDef* volatile pSelf, volatile void* volatile pElement);
bool							FIFOMultiread_isFull(volatile FIFOMultiread_TypeDef* volatile pSelf);
bool							FIFOMultiread_isEmpty(volatile FIFOMultireadReader_TypeDef* volatile pSelf);

FIFOMultiread_Status_TypeDef	FIFOMultiread_dequeue(volatile FIFOMultireadReader_TypeDef* volatile pSelf, volatile void* volatile pRetElement);
FIFOMultiread_Status_TypeDef	FIFOMultiread_lastElement(volatile FIFOMultireadReader_TypeDef* volatile pSelf, volatile void* volatile pRetElement);
FIFOMultiread_Status_TypeDef	FIFOMultiread_elementValOnPosition(volatile FIFOMultireadReader_TypeDef* volatile pSelf, uint16_t positionIndex, volatile void* volatile pRetElement);
FIFOMultiread_Status_TypeDef	FIFOMultiread_elementsNumber(volatile FIFOMultireadReader_TypeDef* volatile pSelf, volatile uint16_t* volatile retElementsNumber);


#endif /* USER_FIFO_QUEUE_MULTIREAD_H_ */
