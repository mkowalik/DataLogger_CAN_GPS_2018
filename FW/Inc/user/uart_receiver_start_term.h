/*
 * uart_receiver.h
 *
 *  Created on: 20.08.2019
 *      Author: kowal
 */

#include "user/uart_driver.h"

#ifndef USER_UART_RECEIVER_START_TERM_H_
#define USER_UART_RECEIVER_START_TERM_H_

#define UART_RECEIVER_START_TERM_MAX_READERS_NUMBER	3
#define UART_RECEIVER_START_TERM_BUFFER_SIZE		520 //< GPS max sentence x 10 (max whole pack) x 4

typedef enum {
	UartReceiverStartTerm_State_UnInitialized = 0,
	UartReceiverStartTerm_State_DuringInit,
	UartReceiverStartTerm_State_Initialized,
	UartReceiverStartTerm_State_Receiving
} UartReceiverStartTerm_State_TypeDef;

typedef struct {
	uint8_t 	dataByte;
	uint32_t	msTime;
} UartReceiverStartTerm_FIFOElem_TypeDef;

typedef struct {

	volatile UartDriver_TypeDef*						pUartDriver;
	volatile UartReceiverStartTerm_State_TypeDef		state;

	UartDriver_ByteReceivedCallbackIterator_TypeDef		uartDriverCallbackIterator;

	volatile FIFOMultiread_TypeDef						rxFifo;
	volatile UartReceiverStartTerm_FIFOElem_TypeDef		receiveBuffer[UART_RECEIVER_START_TERM_BUFFER_SIZE];

	volatile bool										readerRegistered[UART_RECEIVER_START_TERM_MAX_READERS_NUMBER];
	volatile FIFOMultireadReader_TypeDef				startTerminationSignFIFOReaders[UART_RECEIVER_START_TERM_MAX_READERS_NUMBER];
	volatile uint8_t									startSignVal[UART_RECEIVER_START_TERM_MAX_READERS_NUMBER];
	volatile uint8_t									terminationSignVal[UART_RECEIVER_START_TERM_MAX_READERS_NUMBER];
	volatile uint8_t									receivedStartSignsNumber[UART_RECEIVER_START_TERM_MAX_READERS_NUMBER];
	volatile uint8_t									receivedTerminationSignsNumber[UART_RECEIVER_START_TERM_MAX_READERS_NUMBER];

} UartReceiverStartTerm_TypeDef;

typedef enum {
	UartReceiverStartTerm_Status_OK = 0,
	UartReceiverStartTerm_Status_Empty,
	UartReceiverStartTerm_Status_UnInitializedErrror,
	UartReceiverStartTerm_Status_NullPointerError,
	UartReceiverStartTerm_Status_TooManyReadersError,
	UartReceiverStartTerm_Status_NotRegisteredReaderError,
	UartReceiverStartTerm_Status_FIFOError,
	UartReceiverStartTerm_Status_UartDriverError,
	UartReceiverStartTerm_Status_UartDriverNotStartedError,
	UartReceiverStartTerm_Status_InvalidArgumentsError,
	UartReceiverStartTerm_Status_ReceivedAlreadyStartedError,
	UartReceiverStartTerm_Status_ReceiverNotReceivingError,
	UartReceiverStartTerm_Status_Error
} UartReceiverStartTerm_Status_TypeDef;

typedef uint16_t UartReceiverStartTerm_ReaderIterator_TypeDef;

UartReceiverStartTerm_Status_TypeDef UartReceiverStartTerm_init(UartReceiverStartTerm_TypeDef* pSelf, UartDriver_TypeDef* pUartDriver);

UartReceiverStartTerm_Status_TypeDef UartReceiverStartTerm_registerReader(volatile UartReceiverStartTerm_TypeDef* pSelf, volatile UartReceiverStartTerm_ReaderIterator_TypeDef* pRetReaderIterator,
		uint8_t startSign, uint8_t terminationSign);
UartReceiverStartTerm_Status_TypeDef UartReceiverStartTerm_unregisterReader(volatile UartReceiverStartTerm_TypeDef* pSelf, UartReceiverStartTerm_ReaderIterator_TypeDef readerIterator);

UartReceiverStartTerm_Status_TypeDef UartReceiverStartTerm_start(volatile UartReceiverStartTerm_TypeDef* pSelf);
UartReceiverStartTerm_Status_TypeDef UartReceiverStartTerm_stop(volatile UartReceiverStartTerm_TypeDef* pSelf);

UartReceiverStartTerm_Status_TypeDef UartReceiverStartTerm_pullLastSentence(volatile UartReceiverStartTerm_TypeDef* pSelf, UartReceiverStartTerm_ReaderIterator_TypeDef readerIt, uint8_t* pRetSentence, uint16_t* pRetLength, uint32_t* pRetTimestamp);


#endif /* USER_UART_RECEIVER_START_TERM_H_ */
