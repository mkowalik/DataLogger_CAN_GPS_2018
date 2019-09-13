/*
 * uart_receiver.c
 *
 *  Created on: 20.08.2019
 *      Author: Michal Kowalik
 */
#include "user/uart_receiver.h"
#include "main.h"
#include <string.h>

//< ----- Private functions definitions ----- >//

static void UartReceiver_receivedByteCallback(uint8_t dataByte, uint32_t timestamp, void* pArgs);

//< ----- Public functions implementations ----- >//

UartReceiver_Status_TypeDef UartReceiver_init(UartReceiver_TypeDef* pSelf, UartDriver_TypeDef* pUartDriver){

	if (pSelf == NULL || pUartDriver == NULL){
		return UartReceiver_Status_NullPointerError;
	}

	if (pSelf->state != UartReceiver_State_UnInitialized){
		return UartReceiver_Status_Error;
	}

	pSelf->state		= UartReceiver_State_DuringInit;
	pSelf->pUartDriver	= pUartDriver;

	if (FIFOMultiread_init((FIFOMultiread_TypeDef*)&pSelf->rxFifo, (void*)pSelf->receiveBuffer, sizeof(UartReceiver_FIFOElem_TypeDef), UART_RECEIVER_BUFFER_SIZE) != FIFOMultiread_Status_OK){
		return UartReceiver_Status_FIFOError;
	}

	for (uint16_t i=0; i<UART_RECEIVER_MAX_READERS_NUMBER; i++){
		pSelf->readerRegistered[i]						= false;
		memset((void*)&pSelf->startAndTerminationSignFIFOReaders[i], 0, sizeof(FIFOMultireadReaderIdentifier_TypeDef));
		pSelf->startSignVal[i]							= 0;
		pSelf->terminationSignVal[i]					= 0;
		pSelf->receivedStartSignsNumber[i]				= 0;
		pSelf->receivedTerminationSignsNumber[i]		= 0;
	}

	if (UartDriver_setReceivedByteCallback(pSelf->pUartDriver, UartReceiver_receivedByteCallback, (void*)pSelf, &pSelf->uartDriverCallbackIterator) != UartDriver_Status_OK){
		return UartReceiver_Status_UartDriverError;
	}

	pSelf->state = UartReceiver_State_Initialized;

	return UartDriver_Status_OK;
}

UartReceiver_Status_TypeDef UartReceiver_registerStartAndTerminationSignReader(volatile UartReceiver_TypeDef* pSelf, volatile UartReceiver_ReaderIterator_TypeDef* pRetReaderIterator,
		uint8_t startSign, uint8_t terminationSign){

	if (pSelf == NULL || pRetReaderIterator == NULL){
		return UartReceiver_Status_NullPointerError;
	}

	if (pSelf->state == UartReceiver_State_UnInitialized || pSelf->state == UartReceiver_State_DuringInit){
		return UartReceiver_Status_UnInitializedErrror;
	}

	if (pSelf->state != UartReceiver_State_Initialized){
		return UartReceiver_Status_UnInitializedErrror;
	}

	uint16_t i;
	for (i=0; i<UART_RECEIVER_MAX_READERS_NUMBER; i++){
		if (pSelf->readerRegistered[i] == false){
			pSelf->startSignVal[i]						= startSign;
			pSelf->terminationSignVal[i]				= terminationSign;
			pSelf->receivedStartSignsNumber[i]			= 0;
			pSelf->receivedTerminationSignsNumber[i]	= 0;
			if (FIFOMultiread_registerReader(&pSelf->rxFifo, &pSelf->startAndTerminationSignFIFOReaders[i]) != FIFOMultiread_Status_OK){
				return UartReceiver_Status_FIFOError;
			}
			pSelf->readerRegistered[i]					= true;
			*pRetReaderIterator = i;
			break;
		}
	}

	if (i == UART_RECEIVER_MAX_READERS_NUMBER){
		return UartReceiver_Status_TooManyReadersError;
	}

	return UartReceiver_Status_OK;
}

UartReceiver_Status_TypeDef UartReceiver_removeStartAndTerminationSignReader(volatile UartReceiver_TypeDef* pSelf, UartReceiver_ReaderIterator_TypeDef readerIterator){

	if (pSelf == NULL){
		return UartReceiver_Status_NullPointerError;
	}

	if (pSelf->state == UartReceiver_State_UnInitialized || pSelf->state == UartReceiver_State_DuringInit){
		return UartReceiver_Status_UnInitializedErrror;
	}

	if (pSelf->state != UartReceiver_State_Initialized){
		return UartReceiver_Status_Error;
	}

	if (pSelf->readerRegistered[readerIterator] == false){
		return UartReceiver_Status_NotRegisteredReaderError;
	}

	pSelf->startSignVal[readerIterator]					= 0;
	pSelf->terminationSignVal[readerIterator]				= 0;
	pSelf->receivedStartSignsNumber[readerIterator]		= 0;
	pSelf->receivedTerminationSignsNumber[readerIterator]	= 0;
	if (FIFOMultiread_unregisterReader(&pSelf->startAndTerminationSignFIFOReaders[readerIterator]) != FIFOMultiread_Status_OK){
		return UartReceiver_Status_Error;
	}
	pSelf->readerRegistered[readerIterator]				= false;

	return UartReceiver_Status_OK;
}

UartReceiver_Status_TypeDef UartReceiver_start(volatile UartReceiver_TypeDef* pSelf){

	if (pSelf == NULL){
		return UartReceiver_Status_NullPointerError;
	}

	if (pSelf->state == UartReceiver_State_UnInitialized || pSelf->state == UartReceiver_State_DuringInit){
		return UartReceiver_Status_UnInitializedErrror;
	}

	if (pSelf->state != UartReceiver_State_Initialized){
		return UartReceiver_Status_Error;
	}

	if (UartDriver_startReceiver(pSelf->pUartDriver) != UartDriver_Status_OK){
		return UartReceiver_Status_UartDriverError;
	}

	return UartReceiver_Status_OK;
}

UartReceiver_Status_TypeDef UartReceiver_stop(volatile UartReceiver_TypeDef* pSelf){

	if (pSelf == NULL){
		return UartReceiver_Status_NullPointerError;
	}

	if (pSelf->state == UartReceiver_State_UnInitialized || pSelf->state == UartReceiver_State_DuringInit){
		return UartReceiver_Status_UnInitializedErrror;
	}

	if (pSelf->state != UartReceiver_State_Initialized){
		return UartReceiver_Status_Error;
	}

	if (UartDriver_stopReceiver(pSelf->pUartDriver) != UartDriver_Status_OK){
		return UartReceiver_Status_UartDriverError;
	}

	return UartReceiver_Status_OK;
}

UartReceiver_Status_TypeDef UartReceiver_pullLastSentence(volatile UartReceiver_TypeDef* pSelf, UartReceiver_ReaderIterator_TypeDef readerIt, uint8_t* pRetSentence, uint16_t* pRetLength, uint32_t* pRetTimestamp){

	volatile UartReceiver_FIFOElem_TypeDef	elemBuffer;
	volatile FIFOMultiread_Status_TypeDef	fifoStatus;

	if (pSelf == NULL || pRetSentence == NULL || pRetLength == NULL){
		return UartReceiver_Status_NullPointerError;
	}

	if (pSelf->readerRegistered[readerIt] == false){
		return UartReceiver_Status_NotRegisteredReaderError;
	}

	while (true){

		fifoStatus = FIFOMultiread_lastElement(&pSelf->startAndTerminationSignFIFOReaders[readerIt], &elemBuffer);

		if (fifoStatus == FIFOMultiread_Status_Empty){
			break;
		} else if (fifoStatus != FIFOMultiread_Status_OK ){
			return UartReceiver_Status_FIFOError;
		}

		if (elemBuffer.dataByte == pSelf->startSignVal[readerIt]){ //< it's start start sign. Leave it.
			break;
		} else {
			if (elemBuffer.dataByte == pSelf->terminationSignVal[readerIt]){ //< Found termination without start sign at first. Remove it.
				pSelf->receivedTerminationSignsNumber[readerIt]--;
			}
			fifoStatus = FIFOMultiread_dequeue(&pSelf->startAndTerminationSignFIFOReaders[readerIt], &elemBuffer);

			if (fifoStatus != FIFOMultiread_Status_OK){
				return UartReceiver_Status_FIFOError;
			}
		}
	}

	//< ----- handling sentences with start and termination sign ----- >//

	if (pSelf->receivedStartSignsNumber[readerIt] > 0 && pSelf->receivedTerminationSignsNumber[readerIt] > 0){

		*pRetLength = 0;

		// searching for start sign
		while (true){

			fifoStatus = FIFOMultiread_dequeue(&pSelf->startAndTerminationSignFIFOReaders[readerIt], &elemBuffer);

			if (fifoStatus != FIFOMultiread_Status_OK){ //< queue should not be empty. Minimum one start sign and minimum one termination sign are threre
				return UartReceiver_Status_FIFOError;
			}

			if (elemBuffer.dataByte == pSelf->startSignVal[readerIt]){ //< found start sign
				pRetSentence[(*pRetLength)++]	= elemBuffer.dataByte;
				if (pRetTimestamp != NULL){
					*pRetTimestamp				= elemBuffer.msTime;
				}
				pSelf->receivedStartSignsNumber[readerIt]--;
				break;
			}
		}

		// searching for termination sign
		while (true){
			fifoStatus = FIFOMultiread_dequeue(&pSelf->startAndTerminationSignFIFOReaders[readerIt], &elemBuffer);

			if (fifoStatus != FIFOMultiread_Status_OK){ //< queue should not be empty. Minimum one start sign and minimum one termination sign are threre
				return UartReceiver_Status_FIFOError;
			}

			if (elemBuffer.dataByte == pSelf->startSignVal[readerIt]){ //< found another start sign. Remove everything what was before
				*pRetLength		= 0;
				if (pRetTimestamp != NULL){
					*pRetTimestamp	= elemBuffer.msTime;
				}
				pSelf->receivedStartSignsNumber[readerIt]--;
			}

			pRetSentence[(*pRetLength)++] = elemBuffer.dataByte;

			if (elemBuffer.dataByte == pSelf->terminationSignVal[readerIt]){ //< found termination sign
				pSelf->receivedTerminationSignsNumber[readerIt]--;
				break;
			}
		}
	} else {
		return UartReceiver_Status_Empty;
	}

	return UartReceiver_Status_OK;
}


//< ----- IRQ Callback ----- >//

#define	UART_RECEIVER_FULL_ASSER_IN_CALLBACK 0

static void UartReceiver_receivedByteCallback(uint8_t dataByte, uint32_t timestamp, void* pArgs){

	UartReceiver_FIFOElem_TypeDef byteWithTimestamp;
	UartReceiver_TypeDef* pSelf = (UartReceiver_TypeDef*) pArgs;

#if UART_RECEIVER_FULL_ASSER_IN_CALLBACK

	if (pSelf == NULL){
		return UartDriver_Status_NullPointerError;
	}

	if (pSelf->state == UartDriver_State_UnInitialized || pSelf->state == UartDriver_State_DuringInit){
		return UartDriver_Status_UnInitializedErrror;
	}

	if (pSelf->state != UartDriver_State_Receiving){
		return UartDriver_Status_NotReceivingErrror;
	}

#endif

	byteWithTimestamp.dataByte	= dataByte;
	byteWithTimestamp.msTime	= timestamp;

	if (FIFOMultiread_enqueue(&pSelf->rxFifo, (void*) &byteWithTimestamp) != FIFOMultiread_Status_OK){
		Warning_Handler("UartDriver_receivedByteCallback function. FIFOMultiread_enqueue returned error.");
	}

	//< ----- Handling start and termination sign counters ----- >//
	for (uint8_t fooIt = 0; fooIt<UART_RECEIVER_MAX_READERS_NUMBER; fooIt++){
		if (pSelf->readerRegistered[fooIt] == true){
			if (dataByte == pSelf->startSignVal[fooIt]){
				pSelf->receivedStartSignsNumber[fooIt]++;
			}
			if (dataByte == pSelf->terminationSignVal[fooIt]){
				pSelf->receivedTerminationSignsNumber[fooIt]++;
			}
		}
	}
}
