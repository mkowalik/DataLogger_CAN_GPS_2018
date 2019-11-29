/*
 * uart_receiver.c
 *
 *  Created on: 20.08.2019
 *      Author: Michal Kowalik
 */
#include "main.h"
#include <string.h>
#include <user/uart_receiver_start_term.h>

//< ----- Private functions definitions ----- >//

static void UartReceiverStartTerm_receivedByteCallback(uint8_t dataByte, uint32_t timestamp, void* pArgs);

//< ----- Public functions implementations ----- >//

UartReceiverStartTerm_Status_TypeDef UartReceiverStartTerm_init(UartReceiverStartTerm_TypeDef* pSelf, UartDriver_TypeDef* pUartDriver){

	if (pSelf == NULL || pUartDriver == NULL){
		return UartReceiverStartTerm_Status_NullPointerError;
	}

	pSelf->state		= UartReceiverStartTerm_State_DuringInit;
	pSelf->pUartDriver	= pUartDriver;

	if (FIFOMultiread_init((FIFOMultiread_TypeDef*)&pSelf->rxFifo, (void*)pSelf->receiveBuffer, sizeof(UartReceiverStartTerm_FIFOElem_TypeDef), UART_RECEIVER_START_TERM_BUFFER_SIZE) != FIFOMultiread_Status_OK){
		return UartReceiverStartTerm_Status_FIFOError;
	}

	for (uint16_t i=0; i<UART_RECEIVER_START_TERM_MAX_READERS_NUMBER; i++){
		pSelf->readerRegistered[i]						= false;
		memset((void*)&(pSelf->startTerminationSignFIFOReaders[i]), 0, sizeof(FIFOMultireadReader_TypeDef));
		pSelf->startSignVal[i]							= 0;
		pSelf->terminationSignVal[i]					= 0;
		pSelf->receivedStartSignsNumber[i]				= 0;
		pSelf->receivedTerminationSignsNumber[i]		= 0;
	}

	if (UartDriver_setReceivedByteCallback(pSelf->pUartDriver, UartReceiverStartTerm_receivedByteCallback, (void*)pSelf, &pSelf->uartDriverCallbackIterator) != UartDriver_Status_OK){
		return UartReceiverStartTerm_Status_UartDriverError;
	}

	pSelf->state = UartReceiverStartTerm_State_Initialized;

	return UartReceiverStartTerm_Status_OK;
}

UartReceiverStartTerm_Status_TypeDef UartReceiverStartTerm_registerReader(
		volatile UartReceiverStartTerm_TypeDef* pSelf,
		volatile UartReceiverStartTerm_ReaderIterator_TypeDef* pRetReaderIterator,
		uint8_t startSign,
		uint8_t terminationSign){

	if (pSelf == NULL || pRetReaderIterator == NULL){
		return UartReceiverStartTerm_Status_NullPointerError;
	}

	if (pSelf->state == UartReceiverStartTerm_State_UnInitialized || pSelf->state == UartReceiverStartTerm_State_DuringInit){
		return UartReceiverStartTerm_Status_UnInitializedErrror;
	}

	if (pSelf->state != UartReceiverStartTerm_State_Initialized){
		return UartReceiverStartTerm_Status_UnInitializedErrror;
	}

	uint16_t i;
	for (i=0; i<UART_RECEIVER_START_TERM_MAX_READERS_NUMBER; i++){
		if (pSelf->readerRegistered[i] == false){
			pSelf->startSignVal[i]						= startSign;
			pSelf->terminationSignVal[i]				= terminationSign;
			pSelf->receivedStartSignsNumber[i]			= 0;
			pSelf->receivedTerminationSignsNumber[i]	= 0;
			if (FIFOMultiread_registerReader(&pSelf->rxFifo, &pSelf->startTerminationSignFIFOReaders[i]) != FIFOMultiread_Status_OK){
				return UartReceiverStartTerm_Status_FIFOError;
			}
			pSelf->readerRegistered[i]					= true;
			*pRetReaderIterator = i;
			break;
		}
	}

	if (i == UART_RECEIVER_START_TERM_MAX_READERS_NUMBER){
		return UartReceiverStartTerm_Status_TooManyReadersError;
	}

	return UartReceiverStartTerm_Status_OK;
}

UartReceiverStartTerm_Status_TypeDef UartReceiverStartTerm_unregisterReader(volatile UartReceiverStartTerm_TypeDef* pSelf, UartReceiverStartTerm_ReaderIterator_TypeDef readerIt){

	if (pSelf == NULL){
		return UartReceiverStartTerm_Status_NullPointerError;
	}

	if (pSelf->state == UartReceiverStartTerm_State_UnInitialized || pSelf->state == UartReceiverStartTerm_State_DuringInit){
		return UartReceiverStartTerm_Status_UnInitializedErrror;
	}

	if (pSelf->state != UartReceiverStartTerm_State_Initialized){
		return UartReceiverStartTerm_Status_Error;
	}

	if (readerIt >= UART_RECEIVER_START_TERM_MAX_READERS_NUMBER){
		return UartReceiverStartTerm_Status_InvalidArgumentsError;
	}

	if (pSelf->readerRegistered[readerIt] == false){
		return UartReceiverStartTerm_Status_NotRegisteredReaderError;
	}

	pSelf->startSignVal[readerIt]					= 0;
	pSelf->terminationSignVal[readerIt]				= 0;
	pSelf->receivedStartSignsNumber[readerIt]		= 0;
	pSelf->receivedTerminationSignsNumber[readerIt]	= 0;
	if (FIFOMultiread_unregisterReader(&pSelf->startTerminationSignFIFOReaders[readerIt]) != FIFOMultiread_Status_OK){
		return UartReceiverStartTerm_Status_FIFOError;
	}
	pSelf->readerRegistered[readerIt]				= false;

	return UartReceiverStartTerm_Status_OK;
}

UartReceiverStartTerm_Status_TypeDef UartReceiverStartTerm_start(volatile UartReceiverStartTerm_TypeDef* pSelf){

	if (pSelf == NULL){
		return UartReceiverStartTerm_Status_NullPointerError;
	}

	if (pSelf->state == UartReceiverStartTerm_State_UnInitialized || pSelf->state == UartReceiverStartTerm_State_DuringInit){
		return UartReceiverStartTerm_Status_UnInitializedErrror;
	}

	if (pSelf->state != UartReceiverStartTerm_State_Initialized){
		return UartReceiverStartTerm_Status_ReceivedAlreadyStartedError;
	}

	pSelf->state	= UartReceiverStartTerm_State_Receiving;

	return UartReceiverStartTerm_Status_OK;
}

UartReceiverStartTerm_Status_TypeDef UartReceiverStartTerm_stop(volatile UartReceiverStartTerm_TypeDef* pSelf){

	if (pSelf == NULL){
		return UartReceiverStartTerm_Status_NullPointerError;
	}

	if (pSelf->state == UartReceiverStartTerm_State_UnInitialized || pSelf->state == UartReceiverStartTerm_State_DuringInit){
		return UartReceiverStartTerm_Status_UnInitializedErrror;
	}

	if (pSelf->state != UartReceiverStartTerm_State_Receiving){
		return UartReceiverStartTerm_Status_ReceiverNotReceivingError;
	}

	pSelf->state	= UartReceiverStartTerm_State_Initialized;

	return UartReceiverStartTerm_Status_OK;
}

UartReceiverStartTerm_Status_TypeDef UartReceiverStartTerm_pullLastSentence(volatile UartReceiverStartTerm_TypeDef* pSelf, UartReceiverStartTerm_ReaderIterator_TypeDef readerIt, uint8_t* pRetSentence, uint16_t* pRetLength, uint32_t* pRetTimestamp){

	volatile UartReceiverStartTerm_FIFOElem_TypeDef	elemBuffer;
	volatile FIFOMultiread_Status_TypeDef			fifoStatus;

	if (pSelf == NULL || pRetSentence == NULL || pRetLength == NULL){
		return UartReceiverStartTerm_Status_NullPointerError;
	}

	if (pSelf->state == UartReceiverStartTerm_State_UnInitialized || pSelf->state == UartReceiverStartTerm_State_DuringInit){
		return UartReceiverStartTerm_Status_UnInitializedErrror;
	}

	if (readerIt >= UART_RECEIVER_START_TERM_MAX_READERS_NUMBER){
		return UartReceiverStartTerm_Status_InvalidArgumentsError;
	}

	if (pSelf->readerRegistered[readerIt] == false){
		return UartReceiverStartTerm_Status_NotRegisteredReaderError;
	}

	//< ----- removing chars from the beginning until it is not a start signs ----- >//

	while (true){

		fifoStatus = FIFOMultiread_lastElement(&pSelf->startTerminationSignFIFOReaders[readerIt], &elemBuffer);

		if (fifoStatus == FIFOMultiread_Status_Empty){
			break;
		} else if (fifoStatus != FIFOMultiread_Status_OK ){
			return UartReceiverStartTerm_Status_FIFOError;
		}

		if (elemBuffer.dataByte == pSelf->startSignVal[readerIt]){ //< it's start start sign. Leave it.
			break;
		} else {
			if (elemBuffer.dataByte == pSelf->terminationSignVal[readerIt]){ //< Found termination without start sign at first. Remove it.
				pSelf->receivedTerminationSignsNumber[readerIt]--;
			}
			fifoStatus = FIFOMultiread_dequeue(&pSelf->startTerminationSignFIFOReaders[readerIt], &elemBuffer);

			if (fifoStatus != FIFOMultiread_Status_OK){
				return UartReceiverStartTerm_Status_FIFOError;
			}
		}
	}

	//< ----- handling sentences with start and termination sign ----- >//

	if (pSelf->receivedStartSignsNumber[readerIt] > 0 && pSelf->receivedTerminationSignsNumber[readerIt] > 0){

		*pRetLength = 0;

		// searching for start sign
		while (true){

			fifoStatus = FIFOMultiread_dequeue(&pSelf->startTerminationSignFIFOReaders[readerIt], &elemBuffer);

			if (fifoStatus != FIFOMultiread_Status_OK){ //< queue should not be empty. Minimum one start sign and minimum one termination sign are threre
				return UartReceiverStartTerm_Status_FIFOError;
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
			fifoStatus = FIFOMultiread_dequeue(&pSelf->startTerminationSignFIFOReaders[readerIt], &elemBuffer);

			if (fifoStatus != FIFOMultiread_Status_OK){ //< queue should not be empty. Minimum one start sign and minimum one termination sign are threre
				return UartReceiverStartTerm_Status_FIFOError;
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
		return UartReceiverStartTerm_Status_Empty;
	}

	return UartReceiverStartTerm_Status_OK;
}


//< ----- IRQ Callback ----- >//

#define	UART_RECEIVER_START_TERM_FULL_ASSER_IN_CALLBACK 0

static void UartReceiverStartTerm_receivedByteCallback(uint8_t dataByte, uint32_t timestamp, void* pArgs){

	UartReceiverStartTerm_FIFOElem_TypeDef byteWithTimestamp;
	UartReceiverStartTerm_TypeDef* pSelf = (UartReceiverStartTerm_TypeDef*) pArgs;

#if UART_RECEIVER_START_TERM_FULL_ASSER_IN_CALLBACK

	if (pSelf == NULL){
		Warning_Handler("UartReceiverStartTerm_Status_NullPointerError");
		return;
	}

	if (pSelf->state == UartReceiverStartTerm_State_UnInitialized || pSelf->state == UartReceiverStartTerm_State_DuringInit){
		Warning_Handler("UartReceiverStartTerm_Status_UnInitializedErrror");
		return;
	}

#endif

	if (pSelf->state != UartReceiverStartTerm_State_Receiving){
		return;
	}

	byteWithTimestamp.dataByte	= dataByte;
	byteWithTimestamp.msTime	= timestamp;

	if (FIFOMultiread_enqueue(&pSelf->rxFifo, (void*) &byteWithTimestamp) != FIFOMultiread_Status_OK){
		Warning_Handler("UartReceiverStartTerm_receivedByteCallback function. FIFOMultiread_enqueue returned error.");
	}

	//< ----- Handling start and termination sign counters ----- >//
	for (uint8_t fooIt = 0; fooIt<UART_RECEIVER_START_TERM_MAX_READERS_NUMBER; fooIt++){
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
