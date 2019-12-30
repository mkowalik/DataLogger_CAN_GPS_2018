/*
 * uart_receiver_start_length.c
 *
 *  Created on: 25.11.2019
 *      Author: kowal
 */

#include "string.h"
#include "main.h"

#include "user/uart_receiver_start_length.h"

//< ----- Private functions definitions ----- >//

static void 									_UartReceiverStartLength_receivedByteCallback(uint8_t dataByte, uint32_t timestamp, void* pArgs);
static UartReceiverStartLength_Status_TypeDef	_UartReceiverStartLength_removeNotStartCharsFromQueueFront(volatile UartReceiverStartLength_TypeDef* pSelf, UartReceiverStartLength_ReaderIterator_TypeDef readerIt);

//< ----- Public functions implementations ----- >//

UartReceiverStartLength_Status_TypeDef UartReceiverStartLength_init(UartReceiverStartLength_TypeDef* pSelf, UartDriver_TypeDef* pUartDriver){

	if (pSelf == NULL || pUartDriver == NULL){
		return UartReceiverStartLength_Status_NullPointerError;
	}

	memset((void*)pSelf, 0, sizeof(UartReceiverStartLength_TypeDef));

	pSelf->state		= UartReceiverStartLength_State_DuringInit;
	pSelf->pUartDriver	= pUartDriver;

	if (FIFOMultiread_init((FIFOMultiread_TypeDef*)&pSelf->rxFifo, (void*)pSelf->receiveBuffer, sizeof(UartReceiverStartLength_FIFOElem_TypeDef), UART_RECEIVER_START_LENGTH_BUFFER_SIZE) != FIFOMultiread_Status_OK){
		return UartReceiverStartLength_Status_FIFOError;
	}

	for (uint16_t i=0; i<UART_RECEIVER_START_LENGTH_MAX_READERS_NUMBER; i++){
		pSelf->readerRegistered[i]		= false;
		pSelf->startPatternLength[i]	= 0;
		pSelf->sentenceLength[i]		= 0;
		memset((void*)&(pSelf->startPattern[i]), 0, sizeof(pSelf->startPattern[0][0]) * UART_RECEIVER_START_LENGTH_MAX_START_PATERN_LENGTH);
		memset((void*)&(pSelf->startLengthFIFOReaders[i]), 0, sizeof(FIFOMultireadReader_TypeDef));
	}

	if (UartDriver_setReceivedByteCallback(pSelf->pUartDriver, _UartReceiverStartLength_receivedByteCallback, (void*)pSelf, &pSelf->uartDriverCallbackIterator) != UartDriver_Status_OK){
		return UartReceiverStartLength_Status_UartDriverError;
	}

	pSelf->state	= UartReceiverStartLength_State_Initialized;

	return UartReceiverStartLength_Status_OK;
}

UartReceiverStartLength_Status_TypeDef UartReceiverStartLength_registerReader(
		volatile UartReceiverStartLength_TypeDef* pSelf,
		volatile UartReceiverStartLength_ReaderIterator_TypeDef* pRetReaderIterator,
		uint8_t startPatternLength,
		const uint8_t* startPattern,
		uint16_t sentenceLength){

	if (pSelf == NULL || pRetReaderIterator == NULL || startPattern == NULL){
		return UartReceiverStartLength_Status_NullPointerError;
	}

	if (pSelf->state == UartReceiverStartLength_State_UnInitialized || pSelf->state == UartReceiverStartLength_State_DuringInit){
		return UartReceiverStartLength_Status_UnInitializedError;
	}

	if (startPatternLength == 0 || sentenceLength == 0){
		return UartReceiverStartLength_Status_InvalidArgumentsError;
	}

	if (sentenceLength < startPatternLength){
		return UartReceiverStartLength_Status_InvalidArgumentsError;
	}

	uint16_t i;
	for (i=0; i<UART_RECEIVER_START_LENGTH_MAX_READERS_NUMBER; i++){
		if (pSelf->readerRegistered[i] == false){
			pSelf->startPatternLength[i]	= startPatternLength;
			memcpy((void*)(pSelf->startPattern[i]), (void*)startPattern, startPatternLength);
			if (FIFOMultiread_registerReader(&(pSelf->rxFifo), &(pSelf->startLengthFIFOReaders[i])) != FIFOMultiread_Status_OK){
				return UartReceiverStartLength_Status_FIFOError;
			}
			pSelf->sentenceLength[i]	= sentenceLength;
			pSelf->readerRegistered[i]	= true;
			*pRetReaderIterator			= i;
			break;
		}
	}

	if (i == UART_RECEIVER_START_LENGTH_MAX_READERS_NUMBER){
		return UartReceiverStartLength_Status_TooManyReadersError;
	}

	return UartReceiverStartLength_Status_OK;
}

UartReceiverStartLength_Status_TypeDef UartReceiverStartLength_unregisterReader(volatile UartReceiverStartLength_TypeDef* pSelf, UartReceiverStartLength_ReaderIterator_TypeDef readerIt){

	if (pSelf == NULL){
		return UartReceiverStartLength_Status_NullPointerError;
	}

	if (pSelf->state == UartReceiverStartLength_State_UnInitialized || pSelf->state == UartReceiverStartLength_State_DuringInit){
		return UartReceiverStartLength_Status_UnInitializedError;
	}

	if (readerIt >= UART_RECEIVER_START_LENGTH_MAX_READERS_NUMBER){
		return UartReceiverStartLength_Status_InvalidArgumentsError;
	}

	if (pSelf->readerRegistered[readerIt] == false){
		return UartReceiverStartLength_Status_NotRegisteredReaderError;
	}

	pSelf->startPatternLength[readerIt]	= 0;
	pSelf->sentenceLength[readerIt]		= 0;
	memset((void*)&(pSelf->startPattern[readerIt]), 0, sizeof(pSelf->startPattern[readerIt][0]) * UART_RECEIVER_START_LENGTH_MAX_START_PATERN_LENGTH);
	if (FIFOMultiread_unregisterReader(&(pSelf->startLengthFIFOReaders[readerIt])) != FIFOMultiread_Status_OK){
		return UartReceiverStartLength_Status_FIFOError;
	}
	pSelf->readerRegistered[readerIt]	= 0;

	return UartReceiverStartLength_Status_OK;
}

UartReceiverStartLength_Status_TypeDef UartReceiverStartLength_start(volatile UartReceiverStartLength_TypeDef* pSelf){

	if (pSelf == NULL){
		return UartReceiverStartLength_Status_NullPointerError;
	}

	if (pSelf->state == UartReceiverStartLength_State_UnInitialized || pSelf->state == UartReceiverStartLength_State_DuringInit){
		return UartReceiverStartLength_Status_UnInitializedError;
	}

	if (pSelf->state != UartReceiverStartLength_State_Initialized){
		return UartReceiverStartLength_Status_ReceiverAlreadyStartedError;
	}

	pSelf->state	= UartReceiverStartLength_State_Receiving;

	return UartReceiverStartLength_Status_OK;

}

UartReceiverStartLength_Status_TypeDef UartReceiverStartLength_stop(volatile UartReceiverStartLength_TypeDef* pSelf){

	if (pSelf == NULL){
		return UartReceiverStartLength_Status_NullPointerError;
	}

	if (pSelf->state == UartReceiverStartLength_State_UnInitialized || pSelf->state == UartReceiverStartLength_State_DuringInit){
		return UartReceiverStartLength_Status_UnInitializedError;
	}

	if (pSelf->state != UartReceiverStartLength_State_Receiving){
		return UartReceiverStartLength_Status_ReceiverNotReceivingError;
	}

	pSelf->state	= UartReceiverStartLength_State_Initialized;

	return UartReceiverStartLength_Status_OK;

}

UartReceiverStartLength_Status_TypeDef UartReceiverStartLength_pullLastSentence(volatile UartReceiverStartLength_TypeDef* pSelf, UartReceiverStartLength_ReaderIterator_TypeDef readerIt, uint8_t* pRetSentence, uint32_t* pRetTimestamp){

	volatile UartReceiverStartLength_FIFOElem_TypeDef	elemBuffer;
	volatile FIFOMultiread_Status_TypeDef				fifoStatus;
	UartReceiverStartLength_Status_TypeDef				ret;

	if (pSelf == NULL || pRetSentence == NULL || pRetTimestamp == NULL){
		return UartReceiverStartLength_Status_NullPointerError;
	}

	if (pSelf->state == UartReceiverStartLength_State_UnInitialized || pSelf->state == UartReceiverStartLength_State_DuringInit){
		return UartReceiverStartLength_Status_UnInitializedError;
	}

	if (readerIt >= UART_RECEIVER_START_LENGTH_MAX_READERS_NUMBER){
		return UartReceiverStartLength_Status_InvalidArgumentsError;
	}

	if (pSelf->readerRegistered[readerIt] == false){
		return UartReceiverStartLength_Status_NotRegisteredReaderError;
	}

	//< ----- removing chars from the beginning until it is not a start signs ----- >//

	if ((ret = _UartReceiverStartLength_removeNotStartCharsFromQueueFront(pSelf, readerIt)) != UartReceiverStartLength_Status_OK){
		return ret;
	}

	//< ----- handling sentences with start and proper length ----- >//

	bool foundSentence = false;
	uint16_t elementsNumber;

	if (FIFOMultiread_elementsNumber(&pSelf->startLengthFIFOReaders[readerIt], &elementsNumber) != FIFOMultiread_Status_OK) {
		return UartReceiverStartLength_Status_FIFOError;
	}

	while (pSelf->receivedStartSignsNumber[readerIt] > 0 && (elementsNumber >= pSelf->sentenceLength[readerIt]) ){

		//< ----- start sign must be on the front of the queue thanks to the part above. Checking if whole prefix matches pattern ----- >//
		uint16_t i;
		for (i = 0; i<pSelf->startPatternLength[readerIt]; i++){
			if (FIFOMultiread_elementValOnPosition(&pSelf->startLengthFIFOReaders[readerIt], i, &elemBuffer) != FIFOMultiread_Status_OK){
				return UartReceiverStartLength_Status_FIFOError;
			}

			if (pSelf->startPattern[readerIt][i] != elemBuffer.dataByte){
				break;
			}
		}

		if (i == pSelf->startPatternLength[readerIt]){
			foundSentence = true;
			break;
		} else {
			//< ----- case where start pattern doesn't match. Remove front until next start char and start over ----- >//
			fifoStatus = FIFOMultiread_dequeue(&pSelf->startLengthFIFOReaders[readerIt], &elemBuffer);
			if (fifoStatus != FIFOMultiread_Status_OK){
				return UartReceiverStartLength_Status_FIFOError;
			}
			if ((ret = _UartReceiverStartLength_removeNotStartCharsFromQueueFront(pSelf, readerIt)) != UartReceiverStartLength_Status_OK){
				return ret;
			}
		}
		if (FIFOMultiread_elementsNumber(&pSelf->startLengthFIFOReaders[readerIt], &elementsNumber) != FIFOMultiread_Status_OK) {
			return UartReceiverStartLength_Status_FIFOError;
		}
	}

	if (foundSentence){

		if (FIFOMultiread_lastElement(&pSelf->startLengthFIFOReaders[readerIt], &elemBuffer) != FIFOMultiread_Status_OK){
			return UartReceiverStartLength_Status_FIFOError;
		}

		*pRetTimestamp = elemBuffer.msTime;

		for (uint16_t i=0; i<pSelf->sentenceLength[readerIt]; i++){
			if (FIFOMultiread_dequeue(&pSelf->startLengthFIFOReaders[readerIt], &elemBuffer) != FIFOMultiread_Status_OK){
				return UartReceiverStartLength_Status_FIFOError;
			}
			pRetSentence[i] = elemBuffer.dataByte;
		}
		ret = UartReceiverStartLength_Status_OK;
	} else {
		ret = UartReceiverStartLength_Status_Empty;
	}
	return ret;
}

//< ----- Private functions implementations ----- >//

static UartReceiverStartLength_Status_TypeDef _UartReceiverStartLength_removeNotStartCharsFromQueueFront(volatile UartReceiverStartLength_TypeDef* pSelf, UartReceiverStartLength_ReaderIterator_TypeDef readerIt){

	volatile UartReceiverStartLength_FIFOElem_TypeDef	elemBuffer;
	volatile FIFOMultiread_Status_TypeDef				fifoStatus;
	while (true){

		fifoStatus = FIFOMultiread_lastElement(&pSelf->startLengthFIFOReaders[readerIt], &elemBuffer);

		if (fifoStatus == FIFOMultiread_Status_Empty){
			break;
		} else if (fifoStatus != FIFOMultiread_Status_OK){
			return UartReceiverStartLength_Status_FIFOError;
		}

		if (elemBuffer.dataByte == pSelf->startPattern[readerIt][0]){
			break;
		} else {
			fifoStatus = FIFOMultiread_dequeue(&pSelf->startLengthFIFOReaders[readerIt], &elemBuffer);
			if (fifoStatus != FIFOMultiread_Status_OK){
				return UartReceiverStartLength_Status_FIFOError;
			}
		}
	}

	return UartReceiverStartLength_Status_OK;
}

//< ----- IRQ Callback ----- >//

#define	UART_RECEIVER_START_LENGTH_FULL_ASSER_IN_CALLBACK 0

static void _UartReceiverStartLength_receivedByteCallback(uint8_t dataByte, uint32_t timestamp, void* pArgs){

	UartReceiverStartLength_FIFOElem_TypeDef byteWithTimestamp;
	UartReceiverStartLength_TypeDef* pSelf = (UartReceiverStartLength_TypeDef*) pArgs;

#if UART_RECEIVER_START_LENGTH_FULL_ASSER_IN_CALLBACK

	if (pSelf == NULL){
		Warning_Handler("UartReceiverStartLength_tatus_NullPointerError");
		return;
	}

	if (pSelf->state == UartReceiverStartLength_State_UnInitialized || pSelf->state == UartReceiverStartLength_State_DuringInit){
		Warning_Handler("UartReceiverStartLength_Status_UnInitializedError");
		return;
	}

#endif

	if (pSelf->state != UartReceiverStartLength_State_Receiving){
		return;
	}

	byteWithTimestamp.dataByte	= dataByte;
	byteWithTimestamp.msTime	= timestamp;

	if (FIFOMultiread_enqueue(&pSelf->rxFifo, (void*) &byteWithTimestamp) != FIFOMultiread_Status_OK){
		Warning_Handler("UartDriver_receivedByteCallback function. FIFOMultiread_enqueue returned error.");
	}

	//< ----- Handling start sign counters ----- >//
	for (uint8_t fooIt = 0; fooIt<UART_RECEIVER_START_LENGTH_MAX_READERS_NUMBER; fooIt++){
		if (pSelf->readerRegistered[fooIt] == true){
			if (dataByte == pSelf->startPattern[fooIt][0]){
				pSelf->receivedStartSignsNumber[fooIt]++;
			}
		}
	}
}
