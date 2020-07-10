/*
 * uart_receiver_start_length.h
 *
 *  Created on: 25.11.2019
 *      Author: kowal
 */

#ifndef USER_UART_RECEIVER_START_LENGTH_H_
#define USER_UART_RECEIVER_START_LENGTH_H_

#include "user/uart_driver.h"

#define	UART_RECEIVER_START_LENGTH_MAX_READERS_NUMBER		2
#define	UART_RECEIVER_START_LENGTH_BUFFER_SIZE				520
#define	UART_RECEIVER_START_LENGTH_MAX_START_PATERN_LENGTH	10

typedef enum {
	UartReceiverStartLength_State_UnInitialized = 0,
	UartReceiverStartLength_State_DuringInit,
	UartReceiverStartLength_State_Initialized,
	UartReceiverStartLength_State_Receiving
} UartReceiverStartLength_State_TypeDef;

typedef struct {
	uint8_t 	dataByte;
	uint32_t	msTime;
} UartReceiverStartLength_FIFOElem_TypeDef;

typedef struct {

	volatile UartDriver_TypeDef* volatile				pUartDriver;
	volatile UartReceiverStartLength_State_TypeDef		state;

	UartDriver_ByteReceivedCallbackIterator_TypeDef		uartDriverCallbackIterator;

	volatile FIFOMultiread_TypeDef						rxFifo;
	volatile UartReceiverStartLength_FIFOElem_TypeDef	receiveBuffer[UART_RECEIVER_START_LENGTH_BUFFER_SIZE];

	volatile bool										readerRegistered[UART_RECEIVER_START_LENGTH_MAX_READERS_NUMBER];
	volatile uint8_t									startPatternLength[UART_RECEIVER_START_LENGTH_MAX_READERS_NUMBER];
	volatile uint16_t									sentenceLength[UART_RECEIVER_START_LENGTH_MAX_READERS_NUMBER];
	volatile uint8_t									startPattern[UART_RECEIVER_START_LENGTH_MAX_READERS_NUMBER][UART_RECEIVER_START_LENGTH_MAX_START_PATERN_LENGTH];
	volatile FIFOMultireadReader_TypeDef				startLengthFIFOReaders[UART_RECEIVER_START_LENGTH_MAX_READERS_NUMBER];
	volatile uint8_t									receivedStartSignsNumber[UART_RECEIVER_START_LENGTH_MAX_READERS_NUMBER];

} UartReceiverStartLength_TypeDef;


typedef enum {
	UartReceiverStartLength_Status_OK = 0,
	UartReceiverStartLength_Status_Empty,
	UartReceiverStartLength_Status_UnInitializedError,
	UartReceiverStartLength_Status_NullPointerError,
	UartReceiverStartLength_Status_TooManyReadersError,
	UartReceiverStartLength_Status_NotRegisteredReaderError,
	UartReceiverStartLength_Status_FIFOError,
	UartReceiverStartLength_Status_UartDriverError,
	UartReceiverStartLength_Status_InvalidArgumentsError,
	UartReceiverStartLength_Status_ReceiverAlreadyStartedError,
	UartReceiverStartLength_Status_ReceiverNotReceivingError,
	UartReceiverStartLength_Status_Error
} UartReceiverStartLength_Status_TypeDef;

typedef uint16_t UartReceiverStartLength_ReaderIterator_TypeDef;

UartReceiverStartLength_Status_TypeDef UartReceiverStartLength_init(UartReceiverStartLength_TypeDef* pSelf, volatile UartDriver_TypeDef* pUartDriver);
UartReceiverStartLength_Status_TypeDef UartReceiverStartLength_clear(volatile UartReceiverStartLength_TypeDef* pSelf);

UartReceiverStartLength_Status_TypeDef UartReceiverStartLength_registerReader(
		volatile UartReceiverStartLength_TypeDef* pSelf,
		volatile UartReceiverStartLength_ReaderIterator_TypeDef* pRetReaderIterator,
		uint8_t startPatternLength,
		const uint8_t* startPattern,
		uint16_t sentenceLength);
UartReceiverStartLength_Status_TypeDef UartReceiverStartLength_unregisterReader(volatile UartReceiverStartLength_TypeDef* pSelf, UartReceiverStartLength_ReaderIterator_TypeDef readerIterator);

UartReceiverStartLength_Status_TypeDef UartReceiverStartLength_start(volatile UartReceiverStartLength_TypeDef* pSelf);
UartReceiverStartLength_Status_TypeDef UartReceiverStartLength_stop(volatile UartReceiverStartLength_TypeDef* pSelf);

UartReceiverStartLength_Status_TypeDef UartReceiverStartLength_pullLastSentence(volatile UartReceiverStartLength_TypeDef* pSelf, UartReceiverStartLength_ReaderIterator_TypeDef readerIt, uint8_t* pRetSentence, uint32_t* pRetTimestamp);


#endif /* USER_UART_RECEIVER_START_LENGTH_H_ */
