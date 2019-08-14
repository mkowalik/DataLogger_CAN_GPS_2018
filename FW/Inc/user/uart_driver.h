/*
 * UARTDriver.h
 *
 *  Created on: 02.11.2018
 *      Author: Michal Kowalik
 */

#ifndef UART_DRIVER_H_
#define UART_DRIVER_H_

#include <stdint.h>
#include <stdbool.h>
#include "stm32f7xx_hal.h"
#include "fifo_queue_multiread.h"
#include "ms_timer_driver.h"

#define	UART_DRIVER_TIMEOUT				500
#define UART_DRIVER_MAX_CALLBACK_NUMBER	2
#define UART_DRIVER_BUFFER_SIZE			1024 //< GPS max sentence x 5 (max whole pack) x 2

typedef enum {
	UartDriver_Status_OK = 0,
	UartDriver_Status_UnInitializedErrror,
	UartDriver_Status_BigDataSizeError,
	UartDriver_Status_TimeoutError,
	UartDriver_Status_BufferOverflowError,
	UartDriver_Status_NotReceivingErrror,
	UartDriver_Status_NotTransmitingErrror,
	UartDriver_Status_TooManyCallbacksError,
	UartDriver_Status_NullPointerError,
	UartDriver_Status_HALError,
	UartDriver_Status_Error
} UartDriver_Status_TypeDef;

typedef enum {
	UartDriver_State_UnInitialized = 0,
	UartDriver_State_DuringInit,
	UartDriver_State_Ready,
	UartDriver_State_Receiving
} UartDriver_State_TypeDef;

typedef enum {
	UartDriver_StartAndTerminationState_WaitingForStartSign = 0,
	UartDriver_StartAndTerminationState_ReceivedTerminationSign,
	UartDriver_StartAndTerminationState_WaitingForTerminationSign
} UartDriver_StartAndTerminationState_TypeDef;

typedef struct {
	uint8_t 	dataByte;
	uint32_t	msTime;
} UartDriver_FIFOElem_TypeDef;

typedef struct {
	UART_HandleTypeDef* volatile						pUartHandler;
	volatile UartDriver_State_TypeDef					state;
	MSTimerDriver_TypeDef* volatile						pMsTimerHandler;
	volatile bool										transmitInProgress;

	volatile FIFOMultiread_TypeDef						rxFifo;
	volatile UartDriver_FIFOElem_TypeDef				receiveBuffer[UART_DRIVER_BUFFER_SIZE];

	volatile uint8_t									actuallyReceivingByte;

	void											  (*callbacksByte[UART_DRIVER_MAX_CALLBACK_NUMBER])(UartDriver_FIFOElem_TypeDef byteWithTimestamp, void* pArgs);
	void* volatile										callbacksByteArgs[UART_DRIVER_MAX_CALLBACK_NUMBER];
//	volatile FIFOMultireadReaderIdentifier_TypeDef		callbacksByteReaders[UART_DRIVER_MAX_CALLBACK_NUMBER];

	void											  (*callbacksStartAndTerminationSign[UART_DRIVER_MAX_CALLBACK_NUMBER])(uint8_t* bytes, uint16_t length, uint32_t timestamp, void* pArgs);
	void* volatile										callbackStartAndTerminationSignArgs[UART_DRIVER_MAX_CALLBACK_NUMBER];
	volatile uint8_t									startSignVal[UART_DRIVER_MAX_CALLBACK_NUMBER];
	volatile uint8_t									terminationSignVal[UART_DRIVER_MAX_CALLBACK_NUMBER];
	volatile FIFOMultireadReaderIdentifier_TypeDef		callbacksStartAndTerminationSignReaders[UART_DRIVER_MAX_CALLBACK_NUMBER];
	volatile uint8_t									receivedStartSignsNumber[UART_DRIVER_MAX_CALLBACK_NUMBER];
	volatile uint8_t									receivedTerminationSignsNumber[UART_DRIVER_MAX_CALLBACK_NUMBER];
} UartDriver_TypeDef;

/*typedef enum {
	UartDriver_Parity_None = 0,
	UartDriver_Parity_OddPairty,
	UartDriver_Parity_EvenParity
} UartDriver_Parity_TypeDef;

typedef enum {
	UartDriver_StopBits_1bit = 0,
	UartDriver_StopBits_2bits
} UartDriver_StopBits_TypeDef;*/

typedef uint16_t UartDriver_CallbackIterator_TypeDef;
UartDriver_Status_TypeDef UartDriver_init(volatile UartDriver_TypeDef* pSelf, UART_HandleTypeDef* pUartHandler, USART_TypeDef* pUartInstance, MSTimerDriver_TypeDef* pMsTimerHandler, uint32_t baudRate);

UartDriver_Status_TypeDef UartDriver_getBaudRate(volatile UartDriver_TypeDef* pSelf, uint32_t* pRetBaudRate);
UartDriver_Status_TypeDef UartDriver_setBaudRate(volatile UartDriver_TypeDef* pSelf, uint32_t baudRate);

UartDriver_Status_TypeDef UartDriver_sendBytes(volatile UartDriver_TypeDef* pSelf, uint8_t* pBuffer, uint16_t bytes);

UartDriver_Status_TypeDef UartDriver_receiveBytesTerminationSign(volatile UartDriver_TypeDef* pSelf, uint8_t* pReceiveBuffer,
		uint16_t bufferSize, uint8_t terminationSign);	//TODO dorobic wersje z timeoutem
UartDriver_Status_TypeDef UartDriver_receiveNBytes(volatile UartDriver_TypeDef* pSelf, uint8_t* pReceiveBuffer, uint16_t bytesToRead);

UartDriver_Status_TypeDef UartDriver_sendAndReceiveTerminationSign(volatile UartDriver_TypeDef* pSelf, uint8_t* pSendBuffer,
		uint16_t bytesToSend, uint8_t* pReceiveBuffer, uint16_t bufferSize, uint8_t terminationSign);
UartDriver_Status_TypeDef UartDriver_sendAndReceiveNBytes(volatile UartDriver_TypeDef* pSelf, uint8_t* pSendBuffer,
		uint16_t bytesToSend, uint8_t* pReceiveBuffer, uint16_t bytesToRead);

UartDriver_Status_TypeDef UartDriver_setReceivedByteCallback(volatile UartDriver_TypeDef* pSelf,
		void (*foo)(UartDriver_FIFOElem_TypeDef byteWithTimestamp, void* pArgs),
		void* pArgs, UartDriver_CallbackIterator_TypeDef* pRetCallbackIterator);
UartDriver_Status_TypeDef UartDriver_removeReceivedByteCallback(volatile UartDriver_TypeDef* pSelf,
		UartDriver_CallbackIterator_TypeDef callbackIterator);

UartDriver_Status_TypeDef UartDriver_setReceivedBytesStartAndTerminationSignCallback(volatile UartDriver_TypeDef* pSelf, void (*foo)(uint8_t* bytes, uint16_t length, uint32_t timestamp, void* pArgs),
		void* pArgs, volatile UartDriver_CallbackIterator_TypeDef* pRetCallbackIterator, uint8_t startSign, uint8_t terminationSign);
UartDriver_Status_TypeDef UartDriver_removeReceivedBytesStartAndTerminationSignCallback(volatile UartDriver_TypeDef* pSelf, UartDriver_CallbackIterator_TypeDef callbackIterator);

UartDriver_Status_TypeDef UartDriver_startReceiver(volatile UartDriver_TypeDef* pSelf);
UartDriver_Status_TypeDef UartDriver_stopReceiver(volatile UartDriver_TypeDef* pSelf);

UartDriver_Status_TypeDef UartDriver_receivedBytesCallback(volatile UartDriver_TypeDef* pSelf);
UartDriver_Status_TypeDef UartDriver_transmitCompleteCallback(volatile UartDriver_TypeDef* pSelf);

#endif /* UART_DRIVER_H_ */
