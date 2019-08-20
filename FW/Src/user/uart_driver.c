/*
 * UARTDriver.c
 *
 *  Created on: 02.11.2018
 *      Author: Michal Kowalik
 */

#include "user/uart_driver.h"
#include "usart.h"
#include <string.h>

//< ----- Private functions prototypes ----- >//

static UartDriver_Status_TypeDef UartDriver_transmitBytes(volatile UartDriver_TypeDef* pSelf, uint8_t* pBuffer, uint16_t bytes);

//< ----- Public functions implementation ----->//

UartDriver_Status_TypeDef UartDriver_init(volatile UartDriver_TypeDef* pSelf, UART_HandleTypeDef* pUartHandler, USART_TypeDef* pUartInstance, MSTimerDriver_TypeDef* pMsTimerHandler, uint32_t baudRate){

	if (pSelf == NULL || pUartHandler == NULL || pMsTimerHandler == NULL){
		return UartDriver_Status_NullPointerError;
	}

	if (pSelf->state != UartDriver_State_UnInitialized){
		return UartDriver_Status_Error;
	}

	pSelf->state					= UartDriver_State_DuringInit;
	pSelf->pMsTimerHandler			= pMsTimerHandler;
	pSelf->pUartHandler				= pUartHandler;
	pSelf->transmitStartTimestamp	= 0;
	pSelf->transmitInProgress		= false;

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

	{	//< HAL init
		pSelf->pUartHandler->Instance						= pUartInstance;
		pSelf->pUartHandler->Init.BaudRate					= baudRate;
		pSelf->pUartHandler->Init.WordLength				= UART_WORDLENGTH_8B;
		pSelf->pUartHandler->Init.StopBits					= UART_STOPBITS_1;
		pSelf->pUartHandler->Init.Parity					= UART_PARITY_NONE;
		pSelf->pUartHandler->Init.Mode						= UART_MODE_TX_RX;
		pSelf->pUartHandler->Init.HwFlowCtl					= UART_HWCONTROL_NONE;
		pSelf->pUartHandler->Init.OverSampling				= UART_OVERSAMPLING_16;
		pSelf->pUartHandler->Init.OneBitSampling			= UART_ONE_BIT_SAMPLE_DISABLE;
		pSelf->pUartHandler->AdvancedInit.AdvFeatureInit	= UART_ADVFEATURE_NO_INIT;

//		__disable_irq();
		if (HAL_UART_Init(pSelf->pUartHandler) != HAL_OK)
		{
			return UartDriver_Status_HALError;
		}
//		if (HAL_UART_AbortReceive(pSelf->pUartHandler) != HAL_OK){
//			return UartDriver_Status_HALError;
//		}
//		__enable_irq();
	}

	if (pSelf->pUartHandler->gState != HAL_UART_STATE_READY){
		return UartDriver_Status_Error;
	}

	pSelf->state = UartDriver_State_Ready;

	return UartDriver_Status_OK;
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

	if (pSelf->state == UartDriver_State_Receiving){
		if ((ret = UartDriver_stopReceiver(pSelf)) != UartDriver_Status_OK){
			return ret;
		}
	}

	if (pSelf->state != UartDriver_State_DuringInit && pSelf->state != UartDriver_State_Ready){
		return UartDriver_Status_Error;
	}

	while (pSelf->transmitInProgress != false){
		uint32_t actualTimesamp;
		if (MSTimerDriver_getMSTime(pSelf->pMsTimerHandler, &actualTimesamp) != MSTimerDriver_Status_OK){
			return UartDriver_Status_Error;
		}
		if (actualTimesamp - pSelf->transmitStartTimestamp > UART_DRIVER_TX_TIMEOUT){
			return UartDriver_Status_TimeoutError;
		}
	}

	if (HAL_UART_Abort((UART_HandleTypeDef*)pSelf->pUartHandler) != HAL_OK){
		return UartDriver_Status_Error;
	}

	if (HAL_UART_DeInit((UART_HandleTypeDef*)pSelf->pUartHandler) != HAL_OK){
		return UartDriver_Status_Error;
	}

	pSelf->pUartHandler->Init.BaudRate = baudRate;

	{
//		__disable_irq();
		if (HAL_UART_Init(pSelf->pUartHandler) != HAL_OK)
		{
			return UartDriver_Status_HALError;
		}
//		if (HAL_UART_AbortReceive(pSelf->pUartHandler) != HAL_OK){
//			return UartDriver_Status_HALError;
//		}
//		__enable_irq();
	}

	if (pSelf->pUartHandler->gState != HAL_UART_STATE_READY){
		return UartDriver_Status_Error;
	}

	if (pSelf->state == UartDriver_State_Receiving){
		return UartDriver_startReceiver(pSelf);
	}

	return UartDriver_Status_OK;
}

UartDriver_Status_TypeDef UartDriver_sendBytes(volatile UartDriver_TypeDef* pSelf, uint8_t* pBuffer, uint16_t length){

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

	if ((ret = UartDriver_transmitBytes(pSelf, pBuffer, length)) != UartDriver_Status_OK){
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

	if (pSelf == NULL || foo == NULL){
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

	if (pRetCallbackIterator != NULL){
		*pRetCallbackIterator = (UartDriver_CallbackIterator_TypeDef)i;
	}

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

	if (pSelf == NULL || foo == NULL){
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

	if (pRetCallbackIterator != NULL){
		*pRetCallbackIterator = (UartDriver_CallbackIterator_TypeDef)i;
	}

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

UartDriver_Status_TypeDef UartDriver_startReceiver(volatile UartDriver_TypeDef* pSelf){

	if (pSelf == NULL){
		return UartDriver_Status_NullPointerError;
	}

	if (pSelf->state != UartDriver_State_Ready){
		return UartDriver_Status_Error;
	}

	if (HAL_UART_Receive_IT(pSelf->pUartHandler, (uint8_t*)&pSelf->actuallyReceivingByte, 1) != HAL_OK){
		return UartDriver_Status_Error;
	}

	pSelf->state = UartDriver_State_Receiving;

	return UartDriver_Status_OK;
}

UartDriver_Status_TypeDef UartDriver_stopReceiver(volatile UartDriver_TypeDef* pSelf){

	if (pSelf->state != UartDriver_State_Receiving){
		return UartDriver_Status_NotReceivingErrror;
	}

	HAL_UART_IRQHandler(pSelf->pUartHandler);	//TODO do sprawdzenia

	pSelf->state = UartDriver_State_Ready;

	return UartDriver_Status_OK;
}

//< ----- Private functions implementation ----->//

static UartDriver_Status_TypeDef UartDriver_transmitBytes(volatile UartDriver_TypeDef* pSelf, uint8_t* pBuffer, uint16_t bytes){

	while (pSelf->transmitInProgress != false){
		uint32_t actualTimesamp;
		if (MSTimerDriver_getMSTime(pSelf->pMsTimerHandler, &actualTimesamp) != MSTimerDriver_Status_OK){
			return UartDriver_Status_Error;
		}
		if (actualTimesamp - pSelf->transmitStartTimestamp > UART_DRIVER_TX_TIMEOUT){
			return UartDriver_Status_TimeoutError;
		}
	}

	if (HAL_UART_Transmit_DMA(pSelf->pUartHandler, pBuffer, bytes) != HAL_OK){
		return UartDriver_Status_Error;
	}

	pSelf->transmitInProgress = true;
	if (MSTimerDriver_getMSTime(pSelf->pMsTimerHandler, &pSelf->transmitStartTimestamp) != MSTimerDriver_Status_OK){
		return UartDriver_Status_Error;
	}

	return UartDriver_Status_OK;
}

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

	volatile UartDriver_FIFOElem_TypeDef	data;
	volatile FIFOMultiread_Status_TypeDef	fifoStatus;

	data.dataByte = pSelf->actuallyReceivingByte;
	if (MSTimerDriver_getMSTime(pSelf->pMsTimerHandler, &data.msTime) != MSTimerDriver_Status_OK){
		return UartDriver_Status_MSTimerError;
	}

	if (FIFOMultiread_enqueue(&pSelf->rxFifo, (void*) &data) != FIFOMultiread_Status_OK){
		return UartDriver_Status_FIFOError;
	}

	//< ----- Handling one byte callback ----- >//
	for (uint16_t fooIt=0; fooIt<UART_DRIVER_MAX_CALLBACK_NUMBER; fooIt++){

		if (pSelf->callbacksByte[fooIt] != NULL){

			while (true){

				if (fifoStatus == FIFOMultiread_Status_Empty){
					break;
				} else if (fifoStatus != FIFOMultiread_Status_OK ){
					return UartDriver_Status_FIFOError;
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

	volatile UartDriver_FIFOElem_TypeDef	charBuffer;
	volatile uint8_t						arrayBuffer[UART_DRIVER_BUFFER_SIZE];
	volatile uint16_t						bufferIt;

	for (uint16_t fooIt=0; fooIt<UART_DRIVER_MAX_CALLBACK_NUMBER; fooIt++){

		if (pSelf->callbacksStartAndTerminationSign[fooIt] == NULL){
			continue;
		}

		while (true){

			fifoStatus = FIFOMultiread_lastElement(&pSelf->callbacksStartAndTerminationSignReaders[fooIt], &charBuffer);

			if (fifoStatus == FIFOMultiread_Status_Empty){
				break;
			} else if (fifoStatus != FIFOMultiread_Status_OK ){
				return UartDriver_Status_FIFOError;
			}

			if (charBuffer.dataByte != pSelf->startSignVal[fooIt] || //< it's not start start sign. Remove it.
					pSelf->receivedStartSignsNumber[fooIt] > pSelf->receivedTerminationSignsNumber[fooIt] + 1){ //< if there is more than one start sign matched to termination (willing to receive maybe) sign, remove everything what is in the front

				fifoStatus = FIFOMultiread_dequeue(&pSelf->callbacksStartAndTerminationSignReaders[fooIt], &charBuffer);

				if (fifoStatus != FIFOMultiread_Status_OK){
					return UartDriver_Status_FIFOError;
				}

				if (charBuffer.dataByte == pSelf->terminationSignVal[fooIt]){ //< Found termination without start sign at first. Remove it.
					pSelf->receivedTerminationSignsNumber[fooIt]--;
				}
				if (charBuffer.dataByte == pSelf->startSignVal[fooIt]){ //< Found tart sign at first.
					pSelf->receivedStartSignsNumber[fooIt]--;
				}
			} else {
				break;
			}
		}
	}

	//< ----- handling sentences with start and termination sign ----- >//

	static uint32_t						timestampBuffer;

	for (uint16_t fooIt=0; fooIt<UART_DRIVER_MAX_CALLBACK_NUMBER; fooIt++){

		if (pSelf->callbacksStartAndTerminationSign[fooIt] == NULL){
			continue;
		}

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
					pSelf->receivedStartSignsNumber[fooIt]--;
					break;
				}
			}

			// searching for termination sign
			while (true){

				fifoStatus = FIFOMultiread_dequeue(&pSelf->callbacksStartAndTerminationSignReaders[fooIt], &charBuffer);

				if (fifoStatus != FIFOMultiread_Status_OK){ //< queue should not be empty. Minimum one start sign and minimum one termination sign are threre
					return UartDriver_Status_FIFOError;
				}

				arrayBuffer[bufferIt++] = charBuffer.dataByte;

				if (charBuffer.dataByte == pSelf->terminationSignVal[fooIt]){ //< found termination sign
					pSelf->receivedTerminationSignsNumber[fooIt]--;
					break;
				}
			}

			pSelf->callbacksStartAndTerminationSign[fooIt](arrayBuffer, bufferIt, timestampBuffer, pSelf->callbackStartAndTerminationSignArgs[fooIt]);
		}
	}

	if (HAL_UART_Receive_IT(pSelf->pUartHandler, (uint8_t*)&pSelf->actuallyReceivingByte, 1) != HAL_OK){
		return UartDriver_Status_HALError;
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

	if (UartDriver_transmitCompleteCallback(&uart1Driver) != UartDriver_Status_OK){
		Error_Handler();
	}

}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){

	UartDriver_Status_TypeDef ret;
	if ((ret = UartDriver_receivedBytesCallback(&uart1Driver)) != UartDriver_Status_OK){
		Error_Handler();
	}

}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart){
	huart->ErrorCode = HAL_UART_ERROR_NONE;
}
