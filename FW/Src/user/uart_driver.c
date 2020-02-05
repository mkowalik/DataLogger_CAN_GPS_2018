/*
 * UARTDriver.c
 *
 *  Created on: 02.11.2018
 *      Author: Michal Kowalik
 */

#include "user/uart_driver.h"
#include "usart.h"
#include <string.h>


//<----- Private functions prototypes ----->//

void								_UartDriver_synchronousReceiverInnerByteReceivedCallback(uint8_t dataByte, uint32_t timestamp, void* pArgs);
static UartDriver_Status_TypeDef	_UartDriver_initHAL(volatile UartDriver_TypeDef* pSelf, uint32_t baudRate);

//<----- Public functions implementation ----->//

UartDriver_Status_TypeDef UartDriver_init(volatile UartDriver_TypeDef* pSelf, UART_HandleTypeDef* pUartHandler, USART_TypeDef* pUartInstance, MSTimerDriver_TypeDef* pMsTimerHandler, uint32_t baudRate){

	UartDriver_Status_TypeDef ret = UartDriver_Status_OK;
	if (pSelf == NULL || pUartHandler == NULL || pMsTimerHandler == NULL){
		return UartDriver_Status_NullPointerError;
	}

	if (pSelf->state != UartDriver_State_UnInitialized){
		return UartDriver_Status_Error;
	}

	pSelf->state								= UartDriver_State_DuringInit;
	pSelf->pUartHandler							= pUartHandler;
	pSelf->pUartInstance						= pUartInstance;
	pSelf->pMsTimerHandler						= pMsTimerHandler;
	pSelf->transmitTimeoutTimestamp				= 0;
	pSelf->transmitInProgress					= false;
	pSelf->actuallyReceivingByte				= 0;

	pSelf->synchronousReceivedBytesCounter		= 0;
	pSelf->pSynchronousReceiverReceiveBuffer	= NULL;

	for (uint16_t i=0; i<UART_DRIVER_MAX_CALLBACKS_NUMBER; i++){
		pSelf->callbacksByte[i]		= NULL;
		pSelf->callbacksByteArgs[i]	= NULL;
	}

	if ((ret = _UartDriver_initHAL(pSelf, baudRate)) != UartDriver_Status_OK){
		return ret;
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

	if ((ret = UartDriver_waitForTxTimeout(pSelf)) != UartDriver_Status_OK){
		return ret;
	}

	if ((ret = UartDriver_stopTransmitter(pSelf)) != UartDriver_Status_OK){
		return ret;
	}

	UartDriver_State_TypeDef prevState = pSelf->state;

	if (prevState == UartDriver_State_Receiving){
		if ((ret = UartDriver_stopReceiver(pSelf)) != UartDriver_Status_OK){
			return ret;
		}
	}

	if (HAL_UART_Abort((UART_HandleTypeDef*)pSelf->pUartHandler) != HAL_OK){
		return UartDriver_Status_Error;
	}

	/*if (HAL_UART_DeInit((UART_HandleTypeDef*)pSelf->pUartHandler) != HAL_OK){
		return UartDriver_Status_Error;
	}*/

	if ((ret = _UartDriver_initHAL(pSelf, baudRate)) != UartDriver_Status_OK){
		return ret;
	}

	if (prevState == UartDriver_State_Receiving){
		return UartDriver_startReceiver(pSelf);
	}

	return UartDriver_Status_OK;
}

UartDriver_Status_TypeDef UartDriver_sendBytesDMA(volatile UartDriver_TypeDef* pSelf, uint8_t* pBuffer, uint16_t length, uint32_t timeout){

	if (pSelf == NULL || pBuffer == NULL){
		return UartDriver_Status_NullPointerError;
	}

	if (pSelf->state == UartDriver_State_UnInitialized || pSelf->state == UartDriver_State_DuringInit){
		return UartDriver_Status_UnInitializedErrror;
	}

	if (pSelf->state != UartDriver_State_Ready && pSelf->state != UartDriver_State_Receiving){
		return UartDriver_Status_Error;
	}

	while (pSelf->transmitInProgress != false){
		uint32_t actualTimesamp;
		if (MSTimerDriver_getMSTime(pSelf->pMsTimerHandler, &actualTimesamp) != MSTimerDriver_Status_OK){
			return UartDriver_Status_MSTimerError;
		}
		if (actualTimesamp > pSelf->transmitTimeoutTimestamp){
			return UartDriver_Status_TxTimeoutError;
		}
	}

	pSelf->transmitInProgress = true;
	if (HAL_UART_Transmit_DMA(pSelf->pUartHandler, pBuffer, length) != HAL_OK){
		return UartDriver_Status_Error;
	}

	uint32_t actualTimesamp;
	if (MSTimerDriver_getMSTime(pSelf->pMsTimerHandler, &actualTimesamp) != MSTimerDriver_Status_OK){
		return UartDriver_Status_Error;
	}
	pSelf->transmitTimeoutTimestamp	= actualTimesamp + timeout;

	return UartDriver_Status_OK;
}

UartDriver_Status_TypeDef UartDriver_receiveNBytes(volatile UartDriver_TypeDef* pSelf, volatile uint8_t* pReceiveBuffer, uint16_t bytesToRead, uint32_t timeout){

	if (pSelf == NULL || pReceiveBuffer == NULL){
		return UartDriver_Status_NullPointerError;
	}

	if (pSelf->state == UartDriver_State_UnInitialized || pSelf->state == UartDriver_State_DuringInit){
		return UartDriver_Status_UnInitializedErrror;
	}

	bool receiverToStop 			= false;
	UartDriver_Status_TypeDef ret	= UartDriver_Status_OK;
	UartDriver_Status_TypeDef ret2	= UartDriver_Status_OK;

	UartDriver_ByteReceivedCallbackIterator_TypeDef callbackIt;
	uint32_t startTimestamp;

	pSelf->synchronousReceivedBytesCounter		= 0;
	pSelf->synchronousReceiverBufferSize		= bytesToRead;
	pSelf->pSynchronousReceiverReceiveBuffer	= pReceiveBuffer;

	ret = UartDriver_setReceivedByteCallback(pSelf, _UartDriver_synchronousReceiverInnerByteReceivedCallback, (void*)pSelf, &callbackIt);

	if (ret == UartDriver_Status_OK && pSelf->state != UartDriver_State_Receiving){
		if ((ret = UartDriver_startReceiver(pSelf)) == UartDriver_Status_OK){
			receiverToStop = true;
		}
	}

	if (ret == UartDriver_Status_OK && MSTimerDriver_getMSTime(pSelf->pMsTimerHandler, &startTimestamp) != MSTimerDriver_Status_OK){
		ret = UartDriver_Status_MSTimerError;
	}

	while (ret == UartDriver_Status_OK && pSelf->synchronousReceivedBytesCounter < bytesToRead){
		uint32_t actualTimesamp;
		if (MSTimerDriver_getMSTime(pSelf->pMsTimerHandler, &actualTimesamp) != MSTimerDriver_Status_OK){
			ret = UartDriver_Status_MSTimerError;
		}
		if (ret == UartDriver_Status_OK && actualTimesamp - startTimestamp > timeout){
			ret = UartDriver_Status_RxTimeoutError;
		}
	}

	if (receiverToStop == true){
		if ((ret2 = UartDriver_stopReceiver(pSelf)) != UartDriver_Status_OK){
			ret = (ret == UartDriver_Status_OK) ? ret2 : ret;
		}
	}

	if ((ret2 = UartDriver_removeReceivedByteCallback(pSelf, callbackIt)) != UartDriver_Status_OK){
		ret = (ret == UartDriver_Status_OK) ? ret2 : ret;
	}

	pSelf->synchronousReceivedBytesCounter		= 0;
	pSelf->synchronousReceiverBufferSize		= 0;
	pSelf->pSynchronousReceiverReceiveBuffer	= NULL;

	return ret;
}

UartDriver_Status_TypeDef UartDriver_receiveBytesTerminationSign(volatile UartDriver_TypeDef* pSelf, uint8_t* pReceiveBuffer, uint16_t bufferSize, uint8_t terminationSign, uint32_t timeout){

	if (pSelf == NULL || pReceiveBuffer == NULL){
		return UartDriver_Status_NullPointerError;
	}

	if (pSelf->state == UartDriver_State_UnInitialized || pSelf->state == UartDriver_State_DuringInit){
		return UartDriver_Status_UnInitializedErrror;
	}

	bool receiverToStop 			= false;
	UartDriver_Status_TypeDef ret	= UartDriver_Status_OK;
	UartDriver_Status_TypeDef ret2	= UartDriver_Status_OK;
	uint16_t bytesIt				= 0;

	UartDriver_ByteReceivedCallbackIterator_TypeDef callbackIt;
	uint32_t startTimestamp;

	pSelf->synchronousReceivedBytesCounter		= 0;
	pSelf->synchronousReceiverBufferSize		= bufferSize;
	pSelf->pSynchronousReceiverReceiveBuffer	= pReceiveBuffer;

	ret = UartDriver_setReceivedByteCallback(pSelf, _UartDriver_synchronousReceiverInnerByteReceivedCallback, (void*)pSelf, &callbackIt);

	if (ret == UartDriver_Status_OK && pSelf->state != UartDriver_State_Receiving){
		if ((ret = UartDriver_startReceiver(pSelf)) == UartDriver_Status_OK){
			receiverToStop = true;
		}
	}

	if (ret == UartDriver_Status_OK && MSTimerDriver_getMSTime(pSelf->pMsTimerHandler, &startTimestamp) != MSTimerDriver_Status_OK){
		ret = UartDriver_Status_MSTimerError;
	}

	while (ret == UartDriver_Status_OK && pSelf->synchronousReceivedBytesCounter < bufferSize){

		if (bytesIt < pSelf->synchronousReceivedBytesCounter){
			if (pReceiveBuffer[bytesIt++] == terminationSign){
				break;
			}
		}

		uint32_t actualTimesamp;
		if (MSTimerDriver_getMSTime(pSelf->pMsTimerHandler, &actualTimesamp) != MSTimerDriver_Status_OK){
			ret = UartDriver_Status_MSTimerError;
		}
		if (ret == UartDriver_Status_OK && actualTimesamp - startTimestamp > timeout){
			ret = UartDriver_Status_RxTimeoutError;
		}
	}

	if (receiverToStop == true){
		if ((ret2 = UartDriver_stopReceiver(pSelf)) != UartDriver_Status_OK){
			ret = (ret == UartDriver_Status_OK) ? ret2 : ret;
		}
	}

	if ((ret2 = UartDriver_removeReceivedByteCallback(pSelf, callbackIt)) != UartDriver_Status_OK){
		ret = (ret == UartDriver_Status_OK) ? ret2 : ret;
	}

	pSelf->synchronousReceivedBytesCounter		= 0;
	pSelf->synchronousReceiverBufferSize		= 0;
	pSelf->pSynchronousReceiverReceiveBuffer	= NULL;

	return ret;
}

UartDriver_Status_TypeDef UartDriver_sendAndReceiveTerminationSign(volatile UartDriver_TypeDef* pSelf, uint8_t* pSendBuffer, uint16_t bytesToSend, \
		uint8_t* pReceiveBuffer, uint16_t bufferSize, uint8_t terminationSign, uint32_t timeout){

	if (pSelf == NULL || pSendBuffer == NULL || pReceiveBuffer == NULL){
		return UartDriver_Status_NullPointerError;
	}

	if (pSelf->state == UartDriver_State_UnInitialized || pSelf->state == UartDriver_State_DuringInit){
		return UartDriver_Status_UnInitializedErrror;
	}

	if (pSelf->state != UartDriver_State_Receiving){
		return UartDriver_Status_Error;
	}

	uint32_t startTimestamp, txEndTimestamp;
	UartDriver_Status_TypeDef ret = UartDriver_Status_OK;

	if (MSTimerDriver_getMSTime(pSelf->pMsTimerHandler, &startTimestamp) != MSTimerDriver_Status_OK){
		return UartDriver_Status_MSTimerError;
	}

	if ((ret = UartDriver_sendBytesDMA(pSelf, pSendBuffer, bytesToSend, timeout)) != UartDriver_Status_OK){
		return ret;
	}

	if (MSTimerDriver_getMSTime(pSelf->pMsTimerHandler, &txEndTimestamp) != MSTimerDriver_Status_OK){
		return UartDriver_Status_MSTimerError;
	}

	timeout = timeout + txEndTimestamp - startTimestamp;

	return UartDriver_receiveBytesTerminationSign(pSelf, pReceiveBuffer, bufferSize, terminationSign, timeout);
}

UartDriver_Status_TypeDef UartDriver_sendAndReceiveNBytes(volatile UartDriver_TypeDef* pSelf, uint8_t* pSendBuffer, uint16_t bytesToSend, \
		uint8_t* pReceiveBuffer, uint16_t bytesToRead, uint32_t timeout){

	if (pSelf == NULL || pSendBuffer == NULL || pReceiveBuffer == NULL){
		return UartDriver_Status_NullPointerError;
	}

	if (pSelf->state == UartDriver_State_UnInitialized || pSelf->state == UartDriver_State_DuringInit){
		return UartDriver_Status_UnInitializedErrror;
	}

	if (pSelf->state != UartDriver_State_Receiving){
		return UartDriver_Status_Error;
	}

	uint32_t startTimestamp, txEndTimestamp;
	UartDriver_Status_TypeDef ret = UartDriver_Status_OK;

	if (MSTimerDriver_getMSTime(pSelf->pMsTimerHandler, &startTimestamp) != MSTimerDriver_Status_OK){
		return UartDriver_Status_MSTimerError;
	}

	if ((ret = UartDriver_sendBytesDMA(pSelf, pSendBuffer, bytesToSend, timeout)) != UartDriver_Status_OK){
		return ret;
	}

	if (MSTimerDriver_getMSTime(pSelf->pMsTimerHandler, &txEndTimestamp) != MSTimerDriver_Status_OK){
		return UartDriver_Status_MSTimerError;
	}

	timeout = timeout + txEndTimestamp - startTimestamp;

	return UartDriver_receiveNBytes(pSelf, pReceiveBuffer, bytesToRead, timeout);
}

UartDriver_Status_TypeDef UartDriver_setReceivedByteCallback(volatile UartDriver_TypeDef* pSelf,
		void (*foo)(uint8_t dataByte, uint32_t timestamp, void* pArgs),
		void* pArgs, UartDriver_ByteReceivedCallbackIterator_TypeDef* pRetCallbackIterator){

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
	for (i=0; i<UART_DRIVER_MAX_CALLBACKS_NUMBER; i++){
		if (pSelf->callbacksByte[i] == NULL){
			pSelf->callbacksByteArgs[i]	= pArgs;
			pSelf->callbacksByte[i]		= foo;
			break;
		}
	}

	if (i == UART_DRIVER_MAX_CALLBACKS_NUMBER){
		return UartDriver_Status_TooManyCallbacksError;
	}

	if (pRetCallbackIterator != NULL){
		*pRetCallbackIterator = (UartDriver_ByteReceivedCallbackIterator_TypeDef)i;
	}

	return UartDriver_Status_OK;
}

UartDriver_Status_TypeDef UartDriver_removeReceivedByteCallback(volatile UartDriver_TypeDef* pSelf, UartDriver_ByteReceivedCallbackIterator_TypeDef callbackIterator){

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

UartDriver_Status_TypeDef UartDriver_startReceiver(volatile UartDriver_TypeDef* pSelf){

	if (pSelf == NULL){
		return UartDriver_Status_NullPointerError;
	}

	if (pSelf->state != UartDriver_State_Ready){
		return UartDriver_Status_Error;
	}

	uint8_t retryCount = 0;
	HAL_UART_StateTypeDef halState = HAL_UART_GetState(pSelf->pUartHandler);
	while ((halState & HAL_UART_STATE_BUSY_RX) != HAL_UART_STATE_BUSY_RX && retryCount < UART_DRIVER_MAX_RX_START_RETRY_NUMBER){
		if (HAL_UART_Receive_IT(pSelf->pUartHandler, (uint8_t*)&pSelf->actuallyReceivingByte, 1) != HAL_OK){
			return UartDriver_Status_HALError;
		}
		retryCount++;
		halState = HAL_UART_GetState(pSelf->pUartHandler);
	}

	if (retryCount == UART_DRIVER_MAX_RX_START_RETRY_NUMBER){
		return UartDriver_Status_RxStartMaxRetryCountError;
	}

	pSelf->state = UartDriver_State_Receiving;

	return UartDriver_Status_OK;
}

UartDriver_Status_TypeDef UartDriver_stopReceiver(volatile UartDriver_TypeDef* pSelf){

	if (pSelf == NULL){
		return UartDriver_Status_NullPointerError;
	}

	if (pSelf->state != UartDriver_State_Receiving){
		return UartDriver_Status_NotReceivingErrror;
	}

	if (HAL_UART_AbortReceive(pSelf->pUartHandler) != HAL_OK){
		return UartDriver_Status_HALError;	//TODO do sprawdzenia
	}

	pSelf->state = UartDriver_State_Ready;

	return UartDriver_Status_OK;
}

UartDriver_Status_TypeDef UartDriver_stopTransmitter(volatile UartDriver_TypeDef* pSelf){

	if (pSelf == NULL){
		return UartDriver_Status_NullPointerError;
	}

	if (HAL_UART_AbortTransmit(pSelf->pUartHandler) != HAL_OK){
		return UartDriver_Status_HALError;
	}

	pSelf->transmitInProgress		= false;
	pSelf->transmitTimeoutTimestamp	= 0;

	return UartDriver_Status_OK;
}

UartDriver_Status_TypeDef UartDriver_waitForTxTimeout(volatile UartDriver_TypeDef* pSelf){

	if (pSelf == NULL){
		return UartDriver_Status_NullPointerError;
	}

	if (pSelf->state == UartDriver_State_UnInitialized || pSelf->state == UartDriver_State_DuringInit){
		return UartDriver_Status_UnInitializedErrror;
	}

	while (pSelf->transmitInProgress != false){
		uint32_t actualTimesamp;
		if (MSTimerDriver_getMSTime(pSelf->pMsTimerHandler, &actualTimesamp) != MSTimerDriver_Status_OK){
			return UartDriver_Status_MSTimerError;
		}
		if (actualTimesamp > pSelf->transmitTimeoutTimestamp){
			return UartDriver_Status_TxTimeoutError;
		}
	}

	return UartDriver_Status_OK;
}

UartDriver_Status_TypeDef UartDriver_getState(volatile UartDriver_TypeDef* pSelf, UartDriver_State_TypeDef* pRetState){

	if (pSelf == NULL){
		return UartDriver_Status_NullPointerError;
	}

	*pRetState = pSelf->state;

	return UartDriver_Status_OK;
}

//<----- Private functions implementations ----->//

void _UartDriver_synchronousReceiverInnerByteReceivedCallback(uint8_t dataByte, uint32_t timestamp, void* pArgs){
	UNUSED(timestamp);

	UartDriver_TypeDef* pSelf = (UartDriver_TypeDef*) pArgs;

	if (pSelf->synchronousReceivedBytesCounter < pSelf->synchronousReceiverBufferSize){
		pSelf->pSynchronousReceiverReceiveBuffer[pSelf->synchronousReceivedBytesCounter++] = dataByte;
	}
}
//<----- Interrupt handling ----- >//

#define	UART_DRIVER_FULL_ASSER_IN_CALLBACK 0

static UartDriver_Status_TypeDef _UartDriver_receivedBytesCallback(volatile UartDriver_TypeDef* pSelf){

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

	uint32_t timestamp;

	if (MSTimerDriver_getMSTime(pSelf->pMsTimerHandler, &timestamp) != MSTimerDriver_Status_OK){
		return UartDriver_Status_MSTimerError;
	}

	//< ----- Handling one byte callback ----- >//
	for (uint16_t fooIt=0; fooIt<UART_DRIVER_MAX_CALLBACKS_NUMBER; fooIt++){
		if (pSelf->callbacksByte[fooIt] != NULL){
			pSelf->callbacksByte[fooIt](pSelf->actuallyReceivingByte, timestamp, pSelf->callbacksByteArgs[fooIt]);
		}
	}

	if (HAL_UART_Receive_IT(pSelf->pUartHandler, (uint8_t*)&pSelf->actuallyReceivingByte, 1) != HAL_OK){
		return UartDriver_Status_HALError;
	}

	return UartDriver_Status_OK;
}

static UartDriver_Status_TypeDef _UartDriver_errorCallback(volatile UartDriver_TypeDef* pSelf){

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

	pSelf->errorOccuredFlag = true;

	return UartDriver_Status_OK;
}

static UartDriver_Status_TypeDef _UartDriver_transmitCompleteCallback(volatile UartDriver_TypeDef* pSelf){

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

static UartDriver_Status_TypeDef _UartDriver_initHAL(volatile UartDriver_TypeDef* pSelf, uint32_t baudRate){

	pSelf->pUartHandler->Instance							= pSelf->pUartInstance;
	pSelf->pUartHandler->Init.BaudRate						= baudRate;
	pSelf->pUartHandler->Init.WordLength					= UART_WORDLENGTH_8B;
	pSelf->pUartHandler->Init.StopBits						= UART_STOPBITS_1;
	pSelf->pUartHandler->Init.Parity						= UART_PARITY_NONE;
	pSelf->pUartHandler->Init.Mode							= UART_MODE_TX_RX;
	pSelf->pUartHandler->Init.HwFlowCtl						= UART_HWCONTROL_NONE;
	pSelf->pUartHandler->Init.OverSampling					= UART_OVERSAMPLING_16;
	pSelf->pUartHandler->Init.OneBitSampling				= UART_ONE_BIT_SAMPLE_DISABLE;
	pSelf->pUartHandler->AdvancedInit.AdvFeatureInit		= UART_ADVFEATURE_DMADISABLEONERROR_INIT;
	pSelf->pUartHandler->AdvancedInit.DMADisableonRxError	= UART_ADVFEATURE_DMA_DISABLEONRXERROR;

	if (HAL_UART_Init(pSelf->pUartHandler) != HAL_OK)
	{
		return UartDriver_Status_HALError;
	}

	if (pSelf->pUartHandler->gState != HAL_UART_STATE_READY){
		return UartDriver_Status_Error;
	}

	return UartDriver_Status_OK;
}


/****************************** Implementations of stm32f7xx_hal_uart.h __weak functions placeholders ******************************/

extern volatile UartDriver_TypeDef uartGpsDriver;

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart){
	UNUSED(huart);

	if (_UartDriver_transmitCompleteCallback(&uartGpsDriver) != UartDriver_Status_OK){
		Error_Handler();
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	UNUSED(huart);

	UartDriver_Status_TypeDef ret;
	if ((ret = _UartDriver_receivedBytesCallback(&uartGpsDriver)) != UartDriver_Status_OK){
		Error_Handler();
	}
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart){
	UNUSED(huart);

	UartDriver_Status_TypeDef ret;
	if ((ret = _UartDriver_errorCallback(&uartGpsDriver)) != UartDriver_Status_OK){
		Error_Handler();
	}
}
