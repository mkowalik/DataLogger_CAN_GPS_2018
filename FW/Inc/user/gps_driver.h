/*
 * gps_driver.h
 *
 *  Created on: 17.06.2019
 *      Author: Michal Kowalik
 */

#ifndef USER_GPS_DRIVER_H_
#define USER_GPS_DRIVER_H_

#include "stdint.h"

#include "user/gps_data.h"
#include "user/uart_driver.h"
#include "user/uart_receiver.h"
#include "user/fifo_queue.h"
#include "user/config.h"

#define	GPS_DRIVER_MAX_CALLBACK_NUMBER				3

#define GPS_UART_BAUDRATE							115200

#define GPS_NMEA_FIXED_POINT_FRACTIONAL_BITS		12

#define GPS_TX_TIMEOUT_MS							100
#define GPS_COMMAND_RESPONSE_TIMEOUT_MS				1000

#define GPS_DEVICE_START_TIME_MS					600

#define GPS_SET_BAUDRATE_DELAY						400

#define GPS_NMEA_MAX_SENTENCES_DELAY				50

#define GPS_NMEA_MAX_SENTENCE_LENGTH_INCLUDING_CRC	83

#define GPS_NMEA_STRING_BUFFER_FIFO_SIZE			256

typedef enum {
	GPSDriver_Status_OK = 0,
	GPSDriver_Status_Empty,
	GPSDriver_Status_UnInitializedError,
	GPSDriver_Status_RunningError,
	GPSDriver_Status_NotRunningError,
	GPSDriver_Status_NMEASentenceError,
	GPSDriver_Status_WrongNMEAChecksumError,
	GPSDriver_Status_TooManyCallbacksError,
	GPSDriver_Status_BufferOverflowError,
	GPSDriver_Status_BusyError,
	GPSDriver_Status_UartDriverError,
	GPSDriver_Status_UartReceiverError,
	GPSDriver_Status_MSTimerError,
	GPSDriver_Status_StringOperationsError,
	GPSDriver_Status_TXTimeoutError,
	GPSDriver_Status_ACKTimeoutError,
	GPSDriver_Status_NullPointerError,
	GPSDriver_Status_Error
} GPSDriver_Status_TypeDef;

typedef enum {
	GPSDriver_State_UnInitialized = 0,
	GPSDriver_State_DuringInit,
	GPSDriver_State_Initialized,
	GPSDriver_State_Running
} GPSDriver_State_TypeDef;

typedef enum {
	_GPSDriver_ResponseState_Idle = 0,
	_GPSDriver_ResponseState_WaitingForResponse,
	_GPSDriver_ResponseState_ResponseReceived
} _GPSDriver_ResponseState;

typedef struct {
	uint32_t	timestamp;
	uint16_t	sentenceLength;
	uint8_t		sentenceString[GPS_NMEA_MAX_SENTENCE_LENGTH_INCLUDING_CRC];
} _GPSDriver_NMEASentenceString;

typedef uint16_t GPSDriver_Reader_TypeDef;

typedef struct {
	volatile GPSDriver_State_TypeDef				state;
	volatile UartDriver_TypeDef* volatile			pUartDriverHandler;
	volatile UartReceiver_TypeDef* volatile			pUartReceiverHandler;
	volatile MSTimerDriver_TypeDef* volatile 		pMSTimer;
	volatile UartReceiver_ReaderIterator_TypeDef	pUartReaderIterator;

	volatile bool									gpggaPartialSegmentReceived;
	volatile bool									gpgsaPartialSegmentReceived;
	volatile bool									gprmcPartialSegmentReceived;
	volatile uint32_t								gpggaPartialSegmentTimestamp;
	volatile uint32_t								gpgsaPartialSegmentTimestamp;
	volatile uint32_t								gprmcPartialSegmentTimestamp;
	volatile GPSData_TypeDef						partialGPSData;
} GPSDriver_TypeDef;

typedef uint16_t GPSDriver_CallbackIterator_TypeDef;

GPSDriver_Status_TypeDef GPSDriver_init(volatile GPSDriver_TypeDef* pSelf, UartDriver_TypeDef* pUartDriverHandler, UartReceiver_TypeDef* pUartReceiverHandler, MSTimerDriver_TypeDef* pMSTimer, Config_GPSFrequency_TypeDef frequency);

GPSDriver_Status_TypeDef GPSDriver_startReceiver(volatile GPSDriver_TypeDef* pSelf);
GPSDriver_Status_TypeDef GPSDriver_stopReceiver(volatile GPSDriver_TypeDef* pSelf);

GPSDriver_Status_TypeDef GPSDriver_pullLastFrame(volatile GPSDriver_TypeDef* pSelf, GPSData_TypeDef* pRetGPSData);

#endif /* USER_GPS_DRIVER_H_ */
