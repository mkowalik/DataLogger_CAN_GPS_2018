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

#define	UART_DRIVER_TX_TIMEOUT					250
#define UART_DRIVER_MAX_CALLBACKS_NUMBER		3
#define UART_DRIVER_MAX_RX_START_RETRY_NUMBER	3

typedef enum {
	UartDriver_Status_OK = 0,
	UartDriver_Status_UnInitializedErrror,
	UartDriver_Status_BigDataSizeError,
	UartDriver_Status_TxTimeoutError,
	UartDriver_Status_RxTimeoutError,
	UartDriver_Status_BufferOverflowError,
	UartDriver_Status_NotReceivingErrror,
	UartDriver_Status_NotTransmitingErrror,
	UartDriver_Status_TooManyCallbacksError,
	UartDriver_Status_NullPointerError,
	UartDriver_Status_HALError,
	UartDriver_Status_MSTimerError,
	UartDriver_Status_FIFOError,
	UartDriver_Status_RxStartMaxRetryCountError,
	UartDriver_Status_Error
} UartDriver_Status_TypeDef;

typedef enum {
	UartDriver_State_UnInitialized = 0,
	UartDriver_State_DuringInit,
	UartDriver_State_Ready,
	UartDriver_State_Receiving
} UartDriver_State_TypeDef;


typedef struct {
	volatile UartDriver_State_TypeDef					state;
	UART_HandleTypeDef* volatile						pUartHandler;
	USART_TypeDef* volatile								pUartInstance;
	MSTimerDriver_TypeDef* volatile						pMsTimerHandler;

	volatile bool										errorOccuredFlag;

	volatile uint32_t									transmitTimeoutTimestamp;
	volatile bool										transmitInProgress;

	volatile uint8_t									actuallyReceivingByte;

	volatile uint16_t									synchronousReceivedBytesCounter;
	volatile uint16_t									synchronousReceiverBufferSize;
	volatile uint8_t*									pSynchronousReceiverReceiveBuffer;

	void											  (*callbacksByte[UART_DRIVER_MAX_CALLBACKS_NUMBER])(uint8_t dataByte, uint32_t timestamp, void* pArgs);
	void* volatile										callbacksByteArgs[UART_DRIVER_MAX_CALLBACKS_NUMBER];
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

typedef uint16_t UartDriver_ByteReceivedCallbackIterator_TypeDef;

UartDriver_Status_TypeDef UartDriver_init(volatile UartDriver_TypeDef* pSelf, UART_HandleTypeDef* pUartHandler, USART_TypeDef* pUartInstance, MSTimerDriver_TypeDef* pMsTimerHandler, uint32_t baudRate);

UartDriver_Status_TypeDef UartDriver_getBaudRate(volatile UartDriver_TypeDef* pSelf, uint32_t* pRetBaudRate);
UartDriver_Status_TypeDef UartDriver_setBaudRate(volatile UartDriver_TypeDef* pSelf, uint32_t baudRate);

UartDriver_Status_TypeDef UartDriver_sendBytesDMA(volatile UartDriver_TypeDef* pSelf, uint8_t* pBuffer, uint16_t length, uint32_t timeout);

UartDriver_Status_TypeDef UartDriver_receiveBytesTerminationSign(volatile UartDriver_TypeDef* pSelf, uint8_t* pReceiveBuffer,
		uint16_t bufferSize, uint8_t terminationSign, uint32_t timeout);
UartDriver_Status_TypeDef UartDriver_receiveNBytes(volatile UartDriver_TypeDef* pSelf, volatile uint8_t* pReceiveBuffer, uint16_t bytesToRead, uint32_t timeout);

UartDriver_Status_TypeDef UartDriver_sendAndReceiveTerminationSign(volatile UartDriver_TypeDef* pSelf, uint8_t* pSendBuffer,
		uint16_t bytesToSend, uint8_t* pReceiveBuffer, uint16_t bufferSize, uint8_t terminationSign, uint32_t timeout);
UartDriver_Status_TypeDef UartDriver_sendAndReceiveNBytes(volatile UartDriver_TypeDef* pSelf, uint8_t* pSendBuffer,
		uint16_t bytesToSend, uint8_t* pReceiveBuffer, uint16_t bytesToRead, uint32_t timeout);

UartDriver_Status_TypeDef UartDriver_setReceivedByteCallback(
		volatile UartDriver_TypeDef* pSelf,
		void (*foo)(uint8_t dataByte, uint32_t timestamp, void* pArgs),
		void* pArgs,
		UartDriver_ByteReceivedCallbackIterator_TypeDef* pRetCallbackIterator);
UartDriver_Status_TypeDef UartDriver_removeReceivedByteCallback(volatile UartDriver_TypeDef* pSelf,
		UartDriver_ByteReceivedCallbackIterator_TypeDef callbackIterator);

UartDriver_Status_TypeDef UartDriver_startReceiver(volatile UartDriver_TypeDef* pSelf);
UartDriver_Status_TypeDef UartDriver_stopReceiver(volatile UartDriver_TypeDef* pSelf);

UartDriver_Status_TypeDef UartDriver_stopTransmitter(volatile UartDriver_TypeDef* pSelf);

UartDriver_Status_TypeDef UartDriver_waitForTxTimeout(volatile UartDriver_TypeDef* pSelf);

UartDriver_Status_TypeDef UartDriver_getState(volatile UartDriver_TypeDef* pSelf, UartDriver_State_TypeDef* pRetState);

#endif /* UART_DRIVER_H_ */
