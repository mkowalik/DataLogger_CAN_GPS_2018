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
#include "user/fifo_queue.h"

#define	GPS_DRIVER_MAX_CALLBACK_NUMBER				3

#define GPS_UART_BAUDRATE							115200

#define GPS_NMEA_FIXED_POINT_FRACTIONAL_BITS		12
#define GPS_COMMAND_RESPONSE_TIMEOUT_MS				500

#define GPS_NMEA_MAX_SENTENCES_DELAY				50

#define GPS_NMEA_MAX_SENTENCE_LENGTH_INCLUDING_CRC	83

#define GPS_NMEA_STRING_BUFFER_FIFO_SIZE			128

#define GPS_SET_BAUDRATE_DELAY						500

typedef enum {
	GPSDriver_Status_OK = 0,
	GPSDriver_Status_UnInitializedError,
	GPSDriver_Status_RunningError,
	GPSDriver_Status_NotRunningError,
	GPSDriver_Status_NMEASentenceError,
	GPSDriver_Status_WrongNMEAChecksumError,
	GPSDriver_Status_TooManyCallbacksError,
	GPSDriver_Status_BufferOverflowError,
	GPSDriver_Status_BusyError,
	GPSDriver_Status_UartError,
	GPSDriver_Status_ACKTimeoutError,
	GPSDriver_Status_Error
} GPSDriver_Status_TypeDef;

typedef enum {
	GPSDriver_State_UnInitialized = 0,
	GPSDriver_State_DuringInit,
	GPSDriver_State_Initialized,
	GPSDriver_State_Running
} GPSDriver_State_TypeDef;

typedef enum {
	GPSDriver_Frequency_0_5Hz = 1,
	GPSDriver_Frequency_1Hz,
	GPSDriver_Frequency_2Hz,
	GPSDriver_Frequency_5Hz,
	GPSDriver_Frequency_10Hz,
} GPSDriver_Frequency_TypeDef;

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

typedef uint16_t GPSDriver_CallbackIterator_TypeDef;

typedef struct {
	volatile GPSDriver_State_TypeDef				state;
	volatile UartDriver_TypeDef* volatile			pUartHandler;
	volatile MSTimerDriver_TypeDef* volatile 		pMSTimer;
	volatile UartDriver_CallbackIterator_TypeDef	pUartCallbackIterator;

	volatile bool									gpggaPartialSegmentReceived;
	volatile bool									gpgsaPartialSegmentReceived;
	volatile bool									gprmcPartialSegmentReceived;
	volatile uint32_t								gpggaPartialSegmentTimestamp;
	volatile uint32_t								gpgsaPartialSegmentTimestamp;
	volatile uint32_t								gprmcPartialSegmentTimestamp;
	volatile GPSData_TypeDef						partialGPSData;

	volatile FIFOQueue_TypeDef						nmeaSentenceStringFIFO;
	volatile _GPSDriver_NMEASentenceString			nmeaSentenseStringFIFOBuffer[GPS_NMEA_STRING_BUFFER_FIFO_SIZE];

	volatile uint16_t								awaitingResponseLength;
	volatile _GPSDriver_ResponseState				awaitingResponseState;
	volatile uint8_t								awaitingResponse[GPS_NMEA_MAX_SENTENCE_LENGTH_INCLUDING_CRC];

	void										  (*pReceivedDataCallbackFunctions[GPS_DRIVER_MAX_CALLBACK_NUMBER])(GPSData_TypeDef gpsData, void* pArgs);
	volatile void* volatile 						pCallbackArguments[GPS_DRIVER_MAX_CALLBACK_NUMBER];
} GPSDriver_TypeDef;

typedef uint16_t GPSDriver_CallbackIterator_TypeDef;

GPSDriver_Status_TypeDef GPSDriver_init(volatile GPSDriver_TypeDef* pSelf, UartDriver_TypeDef* pUartHandler, MSTimerDriver_TypeDef* pMSTimer, GPSDriver_Frequency_TypeDef frequency);

GPSDriver_Status_TypeDef GPSDriver_setReceivedDataCallback(volatile GPSDriver_TypeDef* pSelf, void (*foo)(GPSData_TypeDef gpsData, void* pArgs),
		void* pArgs, UartDriver_CallbackIterator_TypeDef* pRetCallbackIterator);
GPSDriver_Status_TypeDef GPSDriver_removeReceivedDataCallback(volatile GPSDriver_TypeDef* pSelf, GPSDriver_CallbackIterator_TypeDef callbackIterator);

GPSDriver_Status_TypeDef GPSDriver_startReceiver(volatile GPSDriver_TypeDef* pSelf);
GPSDriver_Status_TypeDef GPSDriver_stopReceiver(volatile GPSDriver_TypeDef* pSelf);

GPSDriver_Status_TypeDef GPSDriver_thread(volatile GPSDriver_TypeDef* pSelf);

#endif /* USER_GPS_DRIVER_H_ */
