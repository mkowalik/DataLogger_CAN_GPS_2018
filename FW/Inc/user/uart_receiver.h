/*
 * uart_receiver.h
 *
 *  Created on: 20.08.2019
 *      Author: kowal
 */

#include "user/uart_driver.h"

#ifndef USER_UART_RECEIVER_H_
#define USER_UART_RECEIVER_H_

#define UART_RECEIVER_MAX_READERS_NUMBER	3
#define UART_RECEIVER_BUFFER_SIZE			512 //< GPS max sentence x 10 (max whole pack) x 4

typedef enum {
	UartReceiver_State_UnInitialized = 0,
	UartReceiver_State_DuringInit,
	UartReceiver_State_Initialized
} UartReceiver_State_TypeDef;

typedef struct {
	uint8_t 	dataByte;
	uint32_t	msTime;
} UartReceiver_FIFOElem_TypeDef;

typedef struct {

	volatile UartDriver_TypeDef*						pUartDriver;
	volatile UartReceiver_State_TypeDef					state;

	UartDriver_ByteReceivedCallbackIterator_TypeDef					uartDriverCallbackIterator;

	volatile FIFOMultiread_TypeDef						rxFifo;
	volatile UartReceiver_FIFOElem_TypeDef				receiveBuffer[UART_RECEIVER_BUFFER_SIZE];

	volatile bool										readerRegistered[UART_RECEIVER_MAX_READERS_NUMBER];
	volatile FIFOMultireadReaderIdentifier_TypeDef		startAndTerminationSignFIFOReaders[UART_RECEIVER_MAX_READERS_NUMBER];
	volatile uint8_t									startSignVal[UART_RECEIVER_MAX_READERS_NUMBER];
	volatile uint8_t									terminationSignVal[UART_RECEIVER_MAX_READERS_NUMBER];
	volatile uint8_t									receivedStartSignsNumber[UART_RECEIVER_MAX_READERS_NUMBER];
	volatile uint8_t									receivedTerminationSignsNumber[UART_RECEIVER_MAX_READERS_NUMBER];

} UartReceiver_TypeDef;

typedef enum {
	UartReceiver_Status_OK = 0,
	UartReceiver_Status_Empty,
	UartReceiver_Status_UnInitializedErrror,
	UartReceiver_Status_NullPointerError,
	UartReceiver_Status_TooManyReadersError,
	UartReceiver_Status_NotRegisteredReaderError,
	UartReceiver_Status_FIFOError,
	UartReceiver_Status_UartDriverError,
	UartReceiver_Status_Error
} UartReceiver_Status_TypeDef;

typedef uint16_t UartReceiver_ReaderIterator_TypeDef;

UartReceiver_Status_TypeDef UartReceiver_init(UartReceiver_TypeDef* pSelf, UartDriver_TypeDef* pUartDriver);

UartReceiver_Status_TypeDef UartReceiver_registerStartAndTerminationSignReader(volatile UartReceiver_TypeDef* pSelf, volatile UartReceiver_ReaderIterator_TypeDef* pRetReaderIterator,
		uint8_t startSign, uint8_t terminationSign);
UartReceiver_Status_TypeDef UartReceiver_removeStartAndTerminationSignReader(volatile UartReceiver_TypeDef* pSelf, UartReceiver_ReaderIterator_TypeDef readerIterator);

UartReceiver_Status_TypeDef UartReceiver_start(volatile UartReceiver_TypeDef* pSelf);
UartReceiver_Status_TypeDef UartReceiver_stop(volatile UartReceiver_TypeDef* pSelf);

UartReceiver_Status_TypeDef UartReceiver_pullLastSentence(volatile UartReceiver_TypeDef* pSelf, UartReceiver_ReaderIterator_TypeDef readerIt, uint8_t* pRetSentence, uint16_t* pRetLength, uint32_t* pRetTimestamp);


#endif /* USER_UART_RECEIVER_H_ */
