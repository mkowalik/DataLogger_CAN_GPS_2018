/*
 * UARTDriver.c
 *
 *  Created on: 02.11.2018
 *      Author: Michal Kowalik
 */

#include "user/uart_driver.h"
#include "usart.h"
#include <string.h>

//< ----- Private functions ----- >//

static UartDriver_Status_TypeDef UartDriver_transmitBytes(volatile UartDriver_TypeDef* pSelf, uint8_t* pBuffer, uint16_t bytes);
static UartDriver_Status_TypeDef UartDriver_startReceiver(volatile UartDriver_TypeDef* pSelf);
static UartDriver_Status_TypeDef UartDriver_stopReceiver(volatile UartDriver_TypeDef* pSelf);

UartDriver_Status_TypeDef UartDriver_init(volatile UartDriver_TypeDef* pSelf, UART_HandleTypeDef* pUartHandler, MSTimerDriver_TypeDef* pMsTimerHandler, uint32_t baudRate){

	UartDriver_Status_TypeDef ret;

	if (pSelf == NULL || pUartHandler == NULL || pMsTimerHandler == NULL){
		return UartDriver_Status_NullPointerError;
	}

	if (pSelf->state != UartDriver_State_UnInitialized){
		return UartDriver_Status_Error;
	}

	pSelf->state				= UartDriver_State_DuringInit;
	pSelf->pMsTimerHandler		= pMsTimerHandler;
	pSelf->pUartHandler			= pUartHandler;
	pSelf->transmitInProgress	= false;

	memset((char*)pSelf->receiveBuffer, 0, UART_DRIVER_BUFFER_SIZE);
	if (FIFOMultiread_init((FIFOMultiread_TypeDef*)&pSelf->rxFifo, (void*)pSelf->receiveBuffer, sizeof(UartDriver_FIFOElem_TypeDef), UART_DRIVER_BUFFER_SIZE) != FIFOMultiread_Status_OK){
		return UartDriver_Status_Error;
	}

	for (uint16_t i=0; i<UART_DRIVER_MAX_CALLBACK_NUMBER; i++){
		pSelf->callbacksByte[i]		= NULL;
		pSelf->callbacksByteArgs[i]	= NULL;
	}

	for (uint16_t i=0; i<UART_DRIVER_MAX_CALLBACK_NUMBER; i++){
		pSelf->callbacksStartAndTerminationSign[i]		= NULL;
		pSelf->callbackStartAndTerminationSignArgs[i]	= NULL;
		pSelf->startSignVal[i]							= 0;
		pSelf->terminationSignVal[i]					= 0;
		pSelf->receivedStartSignsNumber[i]				= 0;
		pSelf->receivedTerminationSignsNumber[i]		= 0;
	}

	if (pSelf->pUartHandler->gState != HAL_UART_STATE_READY){
		return UartDriver_Status_Error;
	}

	if (pSelf->pUartHandler->Init.BaudRate != baudRate){
		if ((ret = UartDriver_setBaudRate(pSelf, baudRate)) != UartDriver_Status_OK){
			return ret;
		}
	}

	return UartDriver_startReceiver(pSelf);
}

UartDriver_Status_TypeDef UartDriver_getBaudRate(volatile UartDriver_TypeDef* pSelf, uint32_t* pRetBaudRate){

	if (pSelf == NULL || pRetBaudRate == NULL){
		return UartDriver_Status_NullPointerError;
	}

	if (pSelf->state == UartDriver_State_UnInitialized){
		return UartDriver_Status_UnInitializedErrror;
	}

	*pRetBaudRate = pSelf->pUartHandler->Init.BaudRate;

	return UartDriver_Status_OK;

}

UartDriver_Status_TypeDef UartDriver_setBaudRate(volatile UartDriver_TypeDef* pSelf, uint32_t baudRate){

	if (pSelf == NULL){
		return UartDriver_Status_NullPointerError;
	}

	if (pSelf->state == UartDriver_State_UnInitialized){
		return UartDriver_Status_UnInitializedErrror;
	}

	UartDriver_Status_TypeDef ret;
	UartDriver_State_TypeDef prevState = pSelf->state;

	if (pSelf->state == UartDriver_State_Receiving){
		if ((ret = UartDriver_stopReceiver(pSelf)) != UartDriver_Status_OK){
			return ret;
		}
	}

	if (pSelf->state != UartDriver_State_Ready){
		return UartDriver_Status_Error;
	}

	pSelf->state = UartDriver_State_ChangingSettings;

	if (HAL_UART_DeInit((UART_HandleTypeDef*)pSelf->pUartHandler) != HAL_OK){
		return UartDriver_Status_Error;
	}

	pSelf->pUartHandler->Init.BaudRate = baudRate;

	if (HAL_UART_Init((UART_HandleTypeDef*)pSelf->pUartHandler) != HAL_OK){
		return UartDriver_Status_Error;
	}

	pSelf->state = prevState;

	if (prevState == UartDriver_State_Receiving){
		return UartDriver_startReceiver(pSelf);
	}

	return UartDriver_Status_OK;
}

UartDriver_Status_TypeDef UartDriver_sendBytes(volatile UartDriver_TypeDef* pSelf, uint8_t* pBuffer, uint16_t bytes){

	if (pSelf == NULL || pBuffer == NULL){
		return UartDriver_Status_NullPointerError;
	}

	if (pSelf->state == UartDriver_State_UnInitialized || pSelf->state == UartDriver_State_DuringInit){
		return UartDriver_Status_UnInitializedErrror;
	}

	if (pSelf->state != UartDriver_State_Ready && pSelf->state != UartDriver_State_Receiving){
		return UartDriver_Status_Error;
	}

	UartDriver_Status_TypeDef ret;

	if ((ret = UartDriver_transmitBytes(pSelf, pBuffer, bytes)) != UartDriver_Status_OK){
		return ret;
	}

	return UartDriver_Status_OK;
}

UartDriver_Status_TypeDef UartDriver_receiveBytesTerminationSign(volatile UartDriver_TypeDef* pSelf, uint8_t* pReceiveBuffer, uint16_t bufferSize, uint8_t terminationSign){

	if (pSelf == NULL || pReceiveBuffer == NULL){
		return UartDriver_Status_NullPointerError;
	}

	if (pSelf->state == UartDriver_State_UnInitialized || pSelf->state == UartDriver_State_DuringInit){
		return UartDriver_Status_UnInitializedErrror;
	}

	if (pSelf->state != UartDriver_State_Receiving){
		return UartDriver_Status_Error;
	}

	FIFOMultireadReaderIdentifier_TypeDef readerId;
	if (FIFOMultiread_registerReaderIdentifier(&pSelf->rxFifo, &readerId) != FIFOMultiread_Status_OK){
		return UartDriver_Status_Error;
	}

	uint16_t charCounter = 0;

	while (charCounter < bufferSize){
		if (FIFOMultiread_isEmpty(&readerId) == false){
			if (FIFOMultiread_dequeue(&readerId, &pReceiveBuffer[charCounter]) != FIFOMultiread_Status_OK){
				return UartDriver_Status_Error;
			}
			if (pReceiveBuffer[charCounter++] == terminationSign){
				break;
			}
		}
	}
	if (FIFOMultiread_unregisterReaderIdentifier(&readerId) != FIFOMultiread_Status_OK){
		return UartDriver_Status_Error;
	}

	if (charCounter == UART_DRIVER_BUFFER_SIZE && pReceiveBuffer[charCounter-1] != terminationSign){
		return UartDriver_Status_BufferOverflowError;
	}

	return UartDriver_Status_OK;
}

UartDriver_Status_TypeDef UartDriver_receiveNBytes(volatile UartDriver_TypeDef* pSelf, uint8_t* pReceiveBuffer, uint16_t bytesToRead){

	if (pSelf == NULL || pReceiveBuffer == NULL){
		return UartDriver_Status_NullPointerError;
	}

	if (pSelf->state == UartDriver_State_UnInitialized || pSelf->state == UartDriver_State_DuringInit){
		return UartDriver_Status_UnInitializedErrror;
	}

	if (pSelf->state != UartDriver_State_Receiving){
		return UartDriver_Status_Error;
	}

	uint16_t charCounter = 0;
	FIFOMultireadReaderIdentifier_TypeDef readerId;
	if (FIFOMultiread_registerReaderIdentifier(&pSelf->rxFifo, &readerId) != FIFOMultiread_Status_OK){
		return UartDriver_Status_Error;
	}

	while (charCounter < bytesToRead){
		if (FIFOMultiread_isEmpty(&readerId) == false){
			if (FIFOMultiread_dequeue(&readerId, &pReceiveBuffer[charCounter++]) != FIFOMultiread_Status_OK){
				return UartDriver_Status_Error;
			}
		}
	}

	if (charCounter == UART_DRIVER_BUFFER_SIZE){
		return UartDriver_Status_BufferOverflowError;
	}

	return UartDriver_Status_OK;
}

UartDriver_Status_TypeDef UartDriver_sendAndReceiveTerminationSign(volatile UartDriver_TypeDef* pSelf, uint8_t* pSendBuffer, uint16_t bytesToSend, \
		uint8_t* pReceiveBuffer, uint16_t bufferSize, uint8_t terminationSign){

	if (pSelf == NULL || pSendBuffer == NULL || pReceiveBuffer == NULL){
		return UartDriver_Status_NullPointerError;
	}

	if (pSelf->state == UartDriver_State_UnInitialized || pSelf->state == UartDriver_State_DuringInit){
		return UartDriver_Status_UnInitializedErrror;
	}

	if (pSelf->state != UartDriver_State_Receiving){
		return UartDriver_Status_Error;
	}

	UartDriver_Status_TypeDef ret = UartDriver_Status_OK;
	if ((ret = UartDriver_transmitBytes(pSelf, pSendBuffer, bytesToSend)) != UartDriver_Status_OK){
		return ret;
	}

	return UartDriver_receiveBytesTerminationSign(pSelf, pReceiveBuffer, bufferSize, terminationSign);
}

UartDriver_Status_TypeDef UartDriver_sendAndReceiveNBytes(volatile UartDriver_TypeDef* pSelf, uint8_t* pSendBuffer, uint16_t bytesToSend, \
		uint8_t* pReceiveBuffer, uint16_t bytesToRead){

	if (pSelf == NULL || pSendBuffer == NULL || pReceiveBuffer == NULL){
		return UartDriver_Status_NullPointerError;
	}

	if (pSelf->state == UartDriver_State_UnInitialized || pSelf->state == UartDriver_State_DuringInit){
		return UartDriver_Status_UnInitializedErrror;
	}

	if (pSelf->state != UartDriver_State_Receiving){
		return UartDriver_Status_Error;
	}

	UartDriver_Status_TypeDef ret = UartDriver_Status_OK;
	if ((ret = UartDriver_transmitBytes(pSelf, pSendBuffer, bytesToSend)) != UartDriver_Status_OK){
		return ret;
	}

	return UartDriver_receiveNBytes(pSelf, pReceiveBuffer, bytesToRead);
}

UartDriver_Status_TypeDef UartDriver_setReceivedByteCallback(volatile UartDriver_TypeDef* pSelf, void (*foo)(UartDriver_FIFOElem_TypeDef byteWithTimestamp, void* pArgs), void* pArgs, UartDriver_CallbackIterator_TypeDef* pRetCallbackIterator){

	if (pSelf == NULL || foo == NULL || pRetCallbackIterator == NULL){
		return UartDriver_Status_NullPointerError;
	}

	if (pSelf->state == UartDriver_State_UnInitialized || pSelf->state == UartDriver_State_DuringInit){
		return UartDriver_Status_UnInitializedErrror;
	}

	if (pSelf->state != UartDriver_State_Ready && pSelf->state != UartDriver_State_Receiving){
		return UartDriver_Status_Error;
	}



	uint16_t i;
	for (i=0; i<UART_DRIVER_MAX_CALLBACK_NUMBER; i++){
		if (pSelf->callbacksByte[i] == NULL){
			pSelf->callbacksByte[i]			= foo;
			pSelf->callbacksByteArgs[i]		= pArgs;
			break;
		}
	}

	if (i == UART_DRIVER_MAX_CALLBACK_NUMBER){
		return UartDriver_Status_TooManyCallbacksError;
	}

	*pRetCallbackIterator = (UartDriver_CallbackIterator_TypeDef)i;

	return UartDriver_Status_OK;
}

UartDriver_Status_TypeDef UartDriver_removeReceivedByteCallback(volatile UartDriver_TypeDef* pSelf, UartDriver_CallbackIterator_TypeDef callbackIterator){

	if (pSelf == NULL){
		return UartDriver_Status_NullPointerError;
	}

	if (pSelf->state == UartDriver_State_UnInitialized || pSelf->state == UartDriver_State_DuringInit){
		return UartDriver_Status_UnInitializedErrror;
	}

	if (pSelf->state != UartDriver_State_Ready && pSelf->state != UartDriver_State_Receiving){
		return UartDriver_Status_Error;
	}

	if (pSelf->callbacksByte[callbackIterator] == NULL){
		return UartDriver_Status_Error;
	}

	pSelf->callbacksByte[callbackIterator]		= NULL;
	pSelf->callbacksByteArgs[callbackIterator]	= NULL;

	return UartDriver_Status_OK;
}

UartDriver_Status_TypeDef UartDriver_setReceivedBytesStartAndTerminationSignCallback(volatile UartDriver_TypeDef* pSelf, void (*foo)(uint8_t* bytes, uint16_t length, uint32_t timestamp, void* pArgs),
		void* pArgs, volatile UartDriver_CallbackIterator_TypeDef* pRetCallbackIterator, uint8_t startSign, uint8_t terminationSign){

	if (pSelf == NULL || foo == NULL || pRetCallbackIterator == NULL){
		return UartDriver_Status_NullPointerError;
	}

	if (pSelf->state == UartDriver_State_UnInitialized || pSelf->state == UartDriver_State_DuringInit){
		return UartDriver_Status_UnInitializedErrror;
	}

	if (pSelf->state != UartDriver_State_Ready && pSelf->state != UartDriver_State_Receiving){
		return UartDriver_Status_Error;
	}

	uint16_t i;
	for (i=0; i<UART_DRIVER_MAX_CALLBACK_NUMBER; i++){
		if (pSelf->callbacksStartAndTerminationSign[i] == NULL){
			pSelf->callbacksStartAndTerminationSign[i]		= foo;
			pSelf->callbackStartAndTerminationSignArgs[i]	= pArgs;
			pSelf->startSignVal[i]							= startSign;
			pSelf->terminationSignVal[i]					= terminationSign;
			pSelf->receivedStartSignsNumber[i]				= 0;
			pSelf->receivedTerminationSignsNumber[i]		= 0;
			if (FIFOMultiread_registerReaderIdentifier(&pSelf->rxFifo, &pSelf->callbacksStartAndTerminationSignReaders[i]) != FIFOMultiread_Status_OK){
				return UartDriver_Status_Error;
			}
			break;
		}
	}

	if (i == UART_DRIVER_MAX_CALLBACK_NUMBER){
		return UartDriver_Status_TooManyCallbacksError;
	}

	*pRetCallbackIterator = (UartDriver_CallbackIterator_TypeDef)i;

	return UartDriver_Status_OK;

}

UartDriver_Status_TypeDef UartDriver_removeReceivedBytesStartAndTerminationSignCallback(volatile UartDriver_TypeDef* pSelf, UartDriver_CallbackIterator_TypeDef callbackIterator){

	if (pSelf == NULL){
		return UartDriver_Status_NullPointerError;
	}

	if (pSelf->state == UartDriver_State_UnInitialized || pSelf->state == UartDriver_State_DuringInit){
		return UartDriver_Status_UnInitializedErrror;
	}

	if (pSelf->state != UartDriver_State_Ready && pSelf->state != UartDriver_State_Receiving){
		return UartDriver_Status_Error;
	}

	if (pSelf->callbacksStartAndTerminationSign[callbackIterator] == NULL){
		return UartDriver_Status_Error;
	}

	pSelf->callbacksStartAndTerminationSign[callbackIterator]		= NULL;
	pSelf->callbackStartAndTerminationSignArgs[callbackIterator]	= NULL;
	pSelf->receivedStartSignsNumber[callbackIterator]				= 0;
	pSelf->receivedTerminationSignsNumber[callbackIterator]			= 0;
	if (FIFOMultiread_unregisterReaderIdentifier(&pSelf->callbacksStartAndTerminationSignReaders[callbackIterator]) != FIFOMultiread_Status_OK){
		return UartDriver_Status_Error;
	}

	return UartDriver_Status_OK;
}

static UartDriver_Status_TypeDef UartDriver_transmitBytes(volatile UartDriver_TypeDef* pSelf, uint8_t* pBuffer, uint16_t bytes){

	while (pSelf->transmitInProgress != false){ }

	if (HAL_UART_Transmit_IT(pSelf->pUartHandler, pBuffer, bytes) != HAL_OK){
		return UartDriver_Status_Error;
	}

	pSelf->transmitInProgress = true;

	return UartDriver_Status_OK;
}

static UartDriver_Status_TypeDef UartDriver_startReceiver(volatile UartDriver_TypeDef* pSelf){

	if (pSelf->state != UartDriver_State_Ready){
		return UartDriver_Status_Error;
	}

	if (HAL_UART_Receive_IT(pSelf->pUartHandler, (uint8_t*)&pSelf->actuallyReceivingByte, 1) != HAL_OK){
		return UartDriver_Status_Error;
	}

	pSelf->state = UartDriver_State_Receiving;

	return UartDriver_Status_OK;
}

static UartDriver_Status_TypeDef UartDriver_stopReceiver(volatile UartDriver_TypeDef* pSelf){

	if (pSelf->state != UartDriver_State_Receiving){
		return UartDriver_Status_NotReceivingErrror;
	}

	HAL_UART_IRQHandler(pSelf->pUartHandler);	//TODO do sprawdzenia

	pSelf->state = UartDriver_State_Ready;

	return UartDriver_Status_OK;
}

/*UartDriver_Status_TypeDef UartDriver_thread(volatile UartDriver_TypeDef* pSelf){

	if (pSelf == NULL){
		return UartDriver_Status_NullPointerError;
	}

	if (pSelf->state == UartDriver_State_UnInitialized || pSelf->state == UartDriver_State_DuringInit){
		return UartDriver_Status_UnInitializedErrror;
	}


	return UartDriver_Status_OK;
}*/

//< ----- Interrupt handling ----- >//

#define	UART_DRIVER_FULL_ASSER_IN_CALLBACK 0

UartDriver_Status_TypeDef UartDriver_receivedBytesCallback(volatile UartDriver_TypeDef* pSelf){

#if UART_DRIVER_FULL_ASSER_IN_CALLBACK

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

	static UartDriver_FIFOElem_TypeDef	data;
	static FIFOMultiread_Status_TypeDef	fifoStatus;

	data.dataByte = pSelf->actuallyReceivingByte;
	if (MSTimerDriver_getMSTime(pSelf->pMsTimerHandler, &data.msTime) != MSTimerDriver_Status_OK){
		return UartDriver_Status_Error;
	}

	if (FIFOMultiread_enqueue(&pSelf->rxFifo, (void*) &data) != FIFOMultiread_Status_OK){
		return UartDriver_Status_Error;
	}

	// --- Handling one byte callback --- //
	for (uint16_t fooIt=0; fooIt<UART_DRIVER_MAX_CALLBACK_NUMBER; fooIt++){

		if (pSelf->callbacksByte[fooIt] != NULL){

			while (true){

				if (fifoStatus == FIFOMultiread_Status_Empty){
					break;
				} else if (fifoStatus != FIFOMultiread_Status_OK ){
					return UartDriver_Status_Error;
				}

				pSelf->callbacksByte[fooIt](data, pSelf->callbacksByteArgs[fooIt]);
			}
		}
	}

	//< ----- Handling start and termination sign callback ----- >//
	for (uint8_t fooIt = 0; fooIt<UART_DRIVER_MAX_CALLBACK_NUMBER; fooIt++){
		if (pSelf->callbacksStartAndTerminationSign[fooIt] != NULL){
			if (pSelf->actuallyReceivingByte == pSelf->startSignVal[fooIt]){
				pSelf->receivedStartSignsNumber[fooIt]++;
			}
			if (pSelf->actuallyReceivingByte == pSelf->terminationSignVal[fooIt]){
				pSelf->receivedTerminationSignsNumber[fooIt]++;
			}
		}
	}

	//< ----- removing char not willing to be used ----->//

	static UartDriver_FIFOElem_TypeDef	charBuffer;
	static uint8_t						arrayBuffer[UART_DRIVER_BUFFER_SIZE];
	static uint16_t						bufferIt;

	for (uint16_t fooIt=0; fooIt<UART_DRIVER_MAX_CALLBACK_NUMBER; fooIt++){
		while (true){

			fifoStatus = FIFOMultiread_lastElement(&pSelf->callbacksStartAndTerminationSignReaders[fooIt], &charBuffer);

			if (fifoStatus == FIFOMultiread_Status_Empty){
				break;
			} else if (fifoStatus != FIFOMultiread_Status_OK ){
				return UartDriver_Status_Error;
			}

			if (charBuffer.dataByte == pSelf->startSignVal[fooIt]){ //< found start sign. Leave it in the queue and break.
				break;
			} else { //< it's not start start sign. Remove it.
				fifoStatus = FIFOMultiread_dequeue(&pSelf->callbacksStartAndTerminationSignReaders[fooIt], &charBuffer);

				if (fifoStatus != FIFOMultiread_Status_OK){
					return UartDriver_Status_Error;
				}

				if (charBuffer.dataByte == pSelf->terminationSignVal[fooIt]){ //< Found termination without start sign at first. Remove it.
					pSelf->receivedTerminationSignsNumber[fooIt]--;
				}
			}
		}
	}

	//< ----- handling sentences with start and termination sign ----- >//

	static uint32_t						timestampBuffer;

	for (uint16_t fooIt=0; fooIt<UART_DRIVER_MAX_CALLBACK_NUMBER; fooIt++){

		if (pSelf->callbacksStartAndTerminationSign[fooIt] != NULL){

			if (pSelf->receivedStartSignsNumber[fooIt] > 0 && pSelf->receivedTerminationSignsNumber[fooIt] > 0){

				bufferIt = 0;

				// searching for start sign
				while (true){

					fifoStatus = FIFOMultiread_dequeue(&pSelf->callbacksStartAndTerminationSignReaders[fooIt], &charBuffer);

					if (fifoStatus != FIFOMultiread_Status_OK){ //< queue should not be empty. Minimum one start sign and minimum one termination sign are threre
						return UartDriver_Status_Error;
					}

					if (charBuffer.dataByte == pSelf->startSignVal[fooIt]){ //< found start sign
						arrayBuffer[bufferIt++]	= charBuffer.dataByte;
						timestampBuffer			= charBuffer.msTime;
						break;
					}
				}

				// searching for termination sign
				while (true){

					fifoStatus = FIFOMultiread_dequeue(&pSelf->callbacksStartAndTerminationSignReaders[fooIt], &charBuffer);

					if (fifoStatus != FIFOMultiread_Status_OK){ //< queue should not be empty. Minimum one start sign and minimum one termination sign are threre
						return UartDriver_Status_Error;
					}

					arrayBuffer[bufferIt++] = charBuffer.dataByte;

					if (charBuffer.dataByte == pSelf->terminationSignVal[fooIt]){ //< found termination sign
						break;
					}
				}

				pSelf->callbacksStartAndTerminationSign[fooIt](arrayBuffer, bufferIt, timestampBuffer, pSelf->callbackStartAndTerminationSignArgs[fooIt]);
			}
		}

	}

	if (HAL_UART_Receive_IT(pSelf->pUartHandler, (uint8_t*)&pSelf->actuallyReceivingByte, 1) != HAL_OK){
		return UartDriver_Status_Error;
	}

	return UartDriver_Status_OK;
}

UartDriver_Status_TypeDef UartDriver_transmitCompleteCallback(volatile UartDriver_TypeDef* pSelf){

#if UART_DRIVER_FULL_ASSER_IN_CALLBACK

	if (pSelf->state == UartDriver_State_UnInitialized || pSelf->state == UartDriver_State_DuringInit){
		return UartDriver_Status_UnInitializedErrror;
	}

	if (pSelf->transmitInProgress == false){
		return UartDriver_Status_NotTransmitingErrror;
	}

#endif

	pSelf->transmitInProgress = false;

	return UartDriver_Status_OK;
}


/****************************** Implementations of stm32f7xx_hal_uart.h __weak functions placeholders ******************************/

extern volatile UartDriver_TypeDef uart1Driver;

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart){

	if (huart != &huart1){
		Error_Handler();
	}

	if (UartDriver_transmitCompleteCallback(&uart1Driver) != UartDriver_Status_OK){
		Error_Handler();
	}

}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){

	if (huart != &huart1){
		Error_Handler();
	}

	if (UartDriver_receivedBytesCallback(&uart1Driver) != UartDriver_Status_OK){
		Error_Handler();
	}

}
