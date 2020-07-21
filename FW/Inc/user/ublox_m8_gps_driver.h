/*
 * gps_driver.h
 *
 *  Created on: 17.06.2019
 *      Author: Michal Kowalik
 */

#include "main.h"

#if (defined(USED_GPS) && (USED_GPS == UBLOX_M8_GPS))

#ifndef USER_UBLOX_M8_GPS_DRIVER_H_
#define USER_UBLOX_M8_GPS_DRIVER_H_

#include "stdint.h"

#include "user/do_driver.h"
#include "user/gps_data.h"
#include "user/uart_driver.h"
#include <user/uart_receiver_start_term.h>
#include <user/uart_receiver_start_length.h>
#include "user/fifo_queue.h"
#include "user/config.h"

#define	GPS_DRIVER_MAX_CALLBACK_NUMBER				3U

#define GPS_UART_BAUDRATE							115200U

#define GPS_NMEA_FIXED_POINT_FRACTIONAL_BITS		12U

#define GPS_TX_TIMEOUT_MS							100U
#define GPS_COMMAND_RESPONSE_TIMEOUT_MS				1000U

#define GPS_DEVICE_RESET_TIME_MS					100U
#define GPS_DEVICE_START_TIME_MS					1000U

#define GPS_SET_BAUDRATE_DELAY						400U

#define GPS_NMEA_MAX_SENTENCES_DELAY				50U

#define GPS_NMEA_MAX_SENTENCE_LENGTH_INCLUDING_CRC	83U

#define GPS_NMEA_STRING_BUFFER_FIFO_SIZE			256U

typedef enum {
	GPSDriver_Status_OK = 0,
	GPSDriver_Status_NAKAnswer = 0,
	GPSDriver_Status_TXTimeoutError,
	GPSDriver_Status_ACKTimeoutError,
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
	GPSDriver_Status_UartDriverNotStatrtedError,
	GPSDriver_Status_UartReceiverStartTermError,
	GPSDriver_Status_UartReceiverStartLengthError,
	GPSDriver_Status_MSTimerError,
	GPSDriver_Status_DOResetError,
	GPSDriver_Status_StringOperationsError,
	GPSDriver_Status_NullPointerError,
	GPSDriver_Status_Error
} GPSDriver_Status_TypeDef;

typedef enum {
	GPSDriver_State_UnInitialized = 0,
	GPSDriver_State_DuringInit,
	GPSDriver_State_Initialized,
	GPSDriver_State_Running,
	GPSDriver_State_OFF,
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
	volatile GPSDriver_State_TypeDef						state;
	volatile UartDriver_TypeDef* volatile					pUartDriverHandler;
	volatile UartReceiverStartTerm_TypeDef* volatile		pUartNMEAReceiverHandler;
	volatile UartReceiverStartLength_TypeDef* volatile		pUartUBXReceiverHandler;
	volatile MSTimerDriver_TypeDef* volatile				pMSTimer;
	volatile DODriver_TypeDef*								pDOResetDriver;

	volatile UartReceiverStartTerm_ReaderIterator_TypeDef	uartNMEAReaderIterator;
	volatile bool											uartNMEAReaderIteratorSet;

	volatile bool											gpggaPartialSegmentReceived;
	volatile bool											gpgsaPartialSegmentReceived;
	volatile bool											gprmcPartialSegmentReceived;
	volatile uint32_t										gpggaPartialSegmentTimestamp;
	volatile uint32_t										gpgsaPartialSegmentTimestamp;
	volatile uint32_t										gprmcPartialSegmentTimestamp;
	volatile GPSData_TypeDef								partialGPSData;
} Ublox8MGPSDriver_TypeDef;

typedef uint16_t GPSDriver_CallbackIterator_TypeDef;

GPSDriver_Status_TypeDef GPSDriver_init (
		volatile Ublox8MGPSDriver_TypeDef*	pSelf,
		volatile UartDriver_TypeDef*		pUartDriverHandler,
		UartReceiverStartTerm_TypeDef*		pUartReceiverTermHandler,
		UartReceiverStartLength_TypeDef*	pUartReceiverLengthHandler,
		volatile MSTimerDriver_TypeDef*		pMSTimer,
		DODriver_TypeDef*					pDOResetDriver,
		Config_GPSFrequency_TypeDef			frequency
	);
GPSDriver_Status_TypeDef GPSDriver_clear(volatile Ublox8MGPSDriver_TypeDef*	pSelf);

GPSDriver_Status_TypeDef GPSDriver_startReceiver(volatile Ublox8MGPSDriver_TypeDef* pSelf);
GPSDriver_Status_TypeDef GPSDriver_stopReceiver(volatile Ublox8MGPSDriver_TypeDef* pSelf);

GPSDriver_Status_TypeDef GPSDriver_pullLastFrame(volatile Ublox8MGPSDriver_TypeDef* pSelf, GPSData_TypeDef* pRetGPSData);

GPSDriver_Status_TypeDef GPSDriver_getState(volatile Ublox8MGPSDriver_TypeDef* pSelf, GPSDriver_State_TypeDef* pRetState);

GPSDriver_Status_TypeDef GPSDriver_setOFF(volatile Ublox8MGPSDriver_TypeDef* pSelf);


//< ----- Unit-test-like functions ----- >//

#ifdef USE_FULL_ASSERT
void GPSDriver_UnitTests();
#endif //USE_FULL_ASSERT

#endif /* USER_UBLOX_M8_GPS_DRIVER_H_ */
#endif
