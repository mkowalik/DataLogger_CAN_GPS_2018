/*
 * gps_driver.c
 *
 *  Created on: 25.06.2019
 *      Author: Michal Kowalik
 */

#include "string.h"
#include "main.h"
#include "user/gps_driver.h"
#include "user/string_operations.h"
#include "user/utils.h"
#include "user/fixed_point.h"

#include <stdio.h>



#define	GPS_CONFIG_TEST_CMD								"$PMTK000"
#define	GPS_CONFIG_TEST_RESPONSE_PREFIX					"$PMTK001,000"
#define	GPS_CONFIG_CHANGE_UART_SPEED_CMD				"$PMTK251"
#define	GPS_CONFIG_CHANGE_UART_SPEED_RESPONSE_PREFIX	"$PMTK001,251"
#define	GPS_CONFIG_CHANGE_INFO_PERIOD_CMD				"$PMTK220"
#define GPS_CONFIG_CHANGE_INFO_PERIOD_RESPONSE_PREFIX	"$PMTK001,220"

#define GPS_MAX_COMMAND_LENGTH	32

#define GPS_NMEA_GPGGA_PREFIX	(uint8_t*)"$GPGGA" //< GPS_ESSENTIAL_FIX_DATA: time, latitude, longitude, fix quality, no of satelites being tracked, horizontal dilution of position, altitude, mean sea level above WGS84
#define GPS_NMEA_GPGSA_PREFIX	(uint8_t*)"$GPGSA" //< GPS_SATELITE_STATUS: auto/manual seelction 2D/3D fix, PRNs of satelites used for fix, dilution of precision, horizontal dilution of precision, vertical dilution of precision
#define GPS_NMEA_GPRMC_PREFIX	(uint8_t*)"$GPRMC" //< RECOMENDED_MINIMUM_SENTENCE: time, A=active/V=void, latitude, N/S, longitude, E/W, speed in knots, track angle in degrees, date, magnetic variation

#define GPS_NMEA_START_SIGN					'$'
#define GPS_NMEA_PRE_TERMINATION_SIGN		'\r'
#define GPS_NMEA_TERMINATION_SIGN			'\n'
#define GPS_NMEA_DATA_SEPARATOR_SIGN		','
#define GPS_NMEA_DECIMAL_SEPARATOR_SIGN		'.'
#define GPS_NMEA_CHECKSUM_SEPARATOR_SIGN	'*'
#define GPS_NMEA_CHECKSUM_LENGTH			2

#define GPS_NMEA_SUFIX_LENGTH				(sizeof(GPS_NMEA_CHECKSUM_SEPARATOR_SIGN)+GPS_NMEA_CHECKSUM_LENGTH+sizeof(GPS_NMEA_PRE_TERMINATION_SIGN)+sizeof(GPS_NMEA_TERMINATION_SIGN))

#define GPS_KNOT_TO_KPH_FACTOR				FixedPoint_constrDecimalFrac(1, 852, 1000, GPS_NMEA_FIXED_POINT_FRACTIONAL_BITS)

#define GPS_UART_DEFAULT_AT_START_BAUDRATE			9600

//< ----- Private functions prototypes ----- >//

static void _GPSDriver_dataReceivedCallback(uint8_t* bytes, uint16_t length, uint32_t timestamp, void* pArgs);
static void _GPSDriver_sendCommandAndWaitForResponseTemporaryCallback(uint8_t* bytes, uint16_t length, uint32_t timestamp, void* pArgs);

static GPSDriver_Status_TypeDef _GPSDriver_addCommandSufix(volatile GPSDriver_TypeDef* pSelf, uint8_t* buffer, uint16_t* length, uint16_t bufferSize);
static GPSDriver_Status_TypeDef _GPSDriver_sendCommandAndWaitForResponse(volatile GPSDriver_TypeDef* pSelf, uint8_t* commandBuffer, uint16_t commandLength, uint8_t* expectedOKResponse, uint16_t expectedOKResponseLength, uint32_t timeoutMs);
static GPSDriver_Status_TypeDef _GPSDriver_sendCommand(volatile GPSDriver_TypeDef* pSelf, uint8_t* commandBuffer, uint16_t commandLength);
static GPSDriver_Status_TypeDef _GPSDriver_sendTestCommand(volatile GPSDriver_TypeDef* pSelf);

static GPSDriver_Status_TypeDef _GPSDriver_changeUartBaudrateCommand(volatile GPSDriver_TypeDef* pSelf, uint32_t baudRate);
static GPSDriver_Status_TypeDef _GPSDriver_changeUpdateFrequemcyCommand(volatile GPSDriver_TypeDef* pSelf, GPSDriver_Frequency_TypeDef frequency);

static GPSDriver_Status_TypeDef _GPSDriver_handleGPGGASentence(volatile GPSDriver_TypeDef* pSelf, _GPSDriver_NMEASentenceString* nmeaSentenceString);
static GPSDriver_Status_TypeDef _GPSDriver_handleGPGSASentence(volatile GPSDriver_TypeDef* pSelf, _GPSDriver_NMEASentenceString* nmeaSentenceString);
static GPSDriver_Status_TypeDef _GPSDriver_handleGPRMCSentence(volatile GPSDriver_TypeDef* pSelf, _GPSDriver_NMEASentenceString* nmeaSentenceString);

static GPSDriver_Status_TypeDef _GPSDriver_getNMEAChecksumString(const uint8_t* sentence, uint16_t length, uint8_t* retChecksumString);
static GPSDriver_Status_TypeDef _GPSDriver_checkNMEAChecksum(const uint8_t* sentence, uint16_t length);
static GPSDriver_Status_TypeDef _GPSDriver_parseTime(uint8_t* bytes, uint16_t length, volatile DateTime_TypeDef* retDateTime);
static GPSDriver_Status_TypeDef _GPSDriver_parseDate(uint8_t* bytes, uint16_t length, volatile DateTime_TypeDef* retDateTime);
static GPSDriver_Status_TypeDef _GPSDriver_parseLatitude(uint8_t* bytes, uint16_t length, volatile GPSData_TypeDef* retGPSData);
static GPSDriver_Status_TypeDef _GPSDriver_parseLongitude(uint8_t* bytes, uint16_t length, volatile GPSData_TypeDef* retGPSData);
static GPSDriver_Status_TypeDef _GPSDriver_parseFixedPoint(uint8_t* bytes, uint16_t length, volatile FixedPoint* retFixedPoint);

//< ----- Uart Callback function ----- >//

static void _GPSDriver_dataReceivedCallback(uint8_t* bytes, uint16_t length, uint32_t timestamp, void* pArgs){

	if (pArgs == NULL || bytes == NULL){
		Warning_Handler("_GPSDriver_dataReceivedCallback - null pointer.");
	}

	volatile GPSDriver_TypeDef* volatile pSelf = (GPSDriver_TypeDef*) pArgs;

	if (pSelf->state == GPSDriver_State_DuringInit){
		Warning_Handler("_GPSDriver_dataReceivedCallback - GPS Driver during init.");
	}

	if (pSelf->state == GPSDriver_State_UnInitialized){
		Warning_Handler("_GPSDriver_dataReceivedCallback - GPS Driver uninitialized.");
	}

	if (length > GPS_NMEA_MAX_SENTENCE_LENGTH_INCLUDING_CRC){
		Warning_Handler("_GPSDriver_dataReceivedCallback - NMEA sentence longer than max length.");
	}

	_GPSDriver_NMEASentenceString tmpNMEAString;
	tmpNMEAString.timestamp		 	= timestamp;
	tmpNMEAString.sentenceLength	= length;
	memcpy(tmpNMEAString.sentenceString, bytes, length);

	if (FIFOQueue_enqueue(&pSelf->nmeaSentenceStringFIFO, &tmpNMEAString) != FIFO_Status_OK){
		Warning_Handler("_GPSDriver_dataReceivedCallback - FIFOQueue_enqueue error.");
	}

}

static void _GPSDriver_sendCommandAndWaitForResponseTemporaryCallback(uint8_t* bytes, uint16_t length, uint32_t timestamp, void* pArgs){

	volatile GPSDriver_TypeDef* volatile pSelf = (GPSDriver_TypeDef*) pArgs;

	if (pSelf->awaitingResponseState != _GPSDriver_ResponseState_WaitingForResponse){
		Warning_Handler("_GPSDriver_sendCommandAndWaitForResponseTemporaryCallback - awaitingResponseState not equal to _GPSDriver_ResponseState_WaitingForResponse.");
		return;
	}

	if (length != pSelf->awaitingResponseLength){
		return;
	}

	StringOperations_Status_TypeDef retStatus = stringEqual((uint8_t*)pSelf->awaitingResponse, (uint8_t*)bytes, pSelf->awaitingResponseLength);
	if (retStatus == StringOperations_Status_OK){
		pSelf->awaitingResponseState = _GPSDriver_ResponseState_ResponseReceived;
		return;
	} else if (retStatus == StringOperations_Status_NotEqual){
		return;
	} else {
		Warning_Handler("_GPSDriver_sendCommandAndWaitForResponseTemporaryCallback - stringEqual operation error.");
	}
}

//< ----- Public functions ----- >//

GPSDriver_Status_TypeDef GPSDriver_init(volatile GPSDriver_TypeDef* pSelf, UartDriver_TypeDef* pUartHandler, MSTimerDriver_TypeDef* pMSTimer, GPSDriver_Frequency_TypeDef frequency){

	GPSDriver_Status_TypeDef ret = GPSDriver_Status_OK;
	if (pSelf == NULL || pUartHandler == NULL){
		return GPSDriver_Status_Error;
	}

	if (pSelf->state != GPSDriver_State_UnInitialized){
		return UartDriver_Status_Error;
	}

	pSelf->state					= GPSDriver_State_DuringInit;
	pSelf->pUartHandler				= pUartHandler;
	pSelf->pMSTimer					= pMSTimer;
	pSelf->pUartCallbackIterator	= 0;

	pSelf->gpggaPartialSegmentReceived	= false;
	pSelf->gpgsaPartialSegmentReceived	= false;
	pSelf->gprmcPartialSegmentReceived	= false;
	pSelf->gpggaPartialSegmentTimestamp	= 0;
	pSelf->gpgsaPartialSegmentTimestamp = 0;
	pSelf->gprmcPartialSegmentTimestamp	= 0;
	memset((void*)&pSelf->partialGPSData, 0, sizeof(GPSData_TypeDef));

	memset((void*)&pSelf->nmeaSentenseStringFIFOBuffer, 0, sizeof(_GPSDriver_NMEASentenceString) * GPS_NMEA_STRING_BUFFER_FIFO_SIZE);
	if (FIFOQueue_init(&pSelf->nmeaSentenceStringFIFO, pSelf->nmeaSentenseStringFIFOBuffer, sizeof(_GPSDriver_NMEASentenceString), GPS_NMEA_STRING_BUFFER_FIFO_SIZE) != FIFO_Status_OK){
		return GPSDriver_Status_Error;
	}

	pSelf->awaitingResponseLength	= 0;
	pSelf->awaitingResponseState	= _GPSDriver_ResponseState_Idle;
	memset((void*)&pSelf->awaitingResponse, 0, GPS_NMEA_MAX_SENTENCE_LENGTH_INCLUDING_CRC);

	uint32_t actualUartBaudrate;

	if (UartDriver_getBaudRate(pSelf->pUartHandler, &actualUartBaudrate) != UartDriver_Status_OK){
		return GPSDriver_Status_UartError;
	}

	if (actualUartBaudrate != GPS_UART_DEFAULT_AT_START_BAUDRATE){
		if (UartDriver_setBaudRate(pSelf->pUartHandler, GPS_UART_DEFAULT_AT_START_BAUDRATE) != UartDriver_Status_OK){
			return GPSDriver_Status_UartError;
		}
	}

	if (UartDriver_setReceivedBytesStartAndTerminationSignCallback(
			pSelf->pUartHandler,
			_GPSDriver_dataReceivedCallback,
			(void*)pSelf,
			&pSelf->pUartCallbackIterator,
			GPS_NMEA_START_SIGN,
			GPS_NMEA_TERMINATION_SIGN) != UartDriver_Status_OK){

		return GPSDriver_Status_UartError;
	}

	if (UartDriver_startReceiver(pSelf->pUartHandler) != UartDriver_Status_OK){
		return GPSDriver_Status_UartError;
	}

	if ((ret = _GPSDriver_changeUartBaudrateCommand(pSelf, GPS_UART_BAUDRATE)) != GPSDriver_Status_OK){
		return ret;
	}

	if ((ret = _GPSDriver_changeUpdateFrequemcyCommand(pSelf, frequency)) != GPSDriver_Status_OK){
		return ret;
	}

	if (UartDriver_stopReceiver(pSelf->pUartHandler) != UartDriver_Status_OK){
		return GPSDriver_Status_UartError;
	}

	pSelf->state = GPSDriver_State_Initialized;

	return GPSDriver_Status_OK;
}

GPSDriver_Status_TypeDef GPSDriver_setReceivedDataCallback(volatile GPSDriver_TypeDef* pSelf, void (*foo)(GPSData_TypeDef gpsData, void* pArgs),
		void* pArgs, GPSDriver_CallbackIterator_TypeDef* pRetCallbackIterator){

	if (pSelf == NULL || foo == NULL || pRetCallbackIterator == NULL){
		return GPSDriver_Status_Error;
	}

	if (pSelf->state == GPSDriver_State_UnInitialized || pSelf->state == GPSDriver_State_DuringInit){
		return GPSDriver_Status_UnInitializedError;
	}

	uint8_t i = 0;
	for (uint8_t i=0; i<GPS_DRIVER_MAX_CALLBACK_NUMBER; i++){
		if (pSelf->pReceivedDataCallbackFunctions[i] != NULL){
			pSelf->pReceivedDataCallbackFunctions[i] = foo;
			pSelf->pCallbackArguments[i] = pArgs;
		}
	}

	*pRetCallbackIterator = (GPSDriver_CallbackIterator_TypeDef)i;

	if (i == GPS_DRIVER_MAX_CALLBACK_NUMBER){
		return GPSDriver_Status_TooManyCallbacksError;
	}

	return GPSDriver_Status_OK;
}

GPSDriver_Status_TypeDef GPSDriver_removeReceivedDataCallback(volatile GPSDriver_TypeDef* pSelf, GPSDriver_CallbackIterator_TypeDef callbackIterator){

	if (pSelf == NULL){
		return GPSDriver_Status_Error;
	}

	if (pSelf->state == GPSDriver_State_UnInitialized || pSelf->state == GPSDriver_State_DuringInit){
		return GPSDriver_Status_UnInitializedError;
	}

	if (pSelf->pReceivedDataCallbackFunctions[callbackIterator] == NULL){
		return GPSDriver_Status_UartError;
	}

	pSelf->pReceivedDataCallbackFunctions[callbackIterator]	= NULL;
	pSelf->pCallbackArguments[callbackIterator]				= NULL;

	return GPSDriver_Status_OK;
}

GPSDriver_Status_TypeDef GPSDriver_startReceiver(volatile GPSDriver_TypeDef* pSelf) {

	if (pSelf == NULL){
		return GPSDriver_Status_Error;
	}

	if (pSelf->state == GPSDriver_State_UnInitialized || pSelf->state == GPSDriver_State_DuringInit){
		return GPSDriver_Status_UnInitializedError;
	}

	if (pSelf->state == GPSDriver_State_Running){
		return GPSDriver_Status_RunningError;
	}

	if (UartDriver_startReceiver(pSelf->pUartHandler) != UartDriver_Status_OK){
		return GPSDriver_Status_UartError;
	}

	return GPSDriver_Status_OK;

}

GPSDriver_Status_TypeDef GPSDriver_stopReceiver(volatile GPSDriver_TypeDef* pSelf) {

	if (pSelf == NULL){
		return GPSDriver_Status_Error;
	}

	if (pSelf->state == GPSDriver_State_UnInitialized || pSelf->state == GPSDriver_State_DuringInit){
		return GPSDriver_Status_UnInitializedError;
	}

	if (pSelf->state == GPSDriver_State_Initialized){
		return GPSDriver_Status_NotRunningError;
	}

	if (UartDriver_stopReceiver(pSelf->pUartHandler) != UartDriver_Status_OK){
		return GPSDriver_Status_UartError;
	}

	return GPSDriver_Status_OK;

}

GPSDriver_Status_TypeDef GPSDriver_thread(volatile GPSDriver_TypeDef* pSelf) {

	if (pSelf == NULL){
		return GPSDriver_Status_Error;
	}

	if (pSelf->state == GPSDriver_State_UnInitialized || pSelf->state == GPSDriver_State_DuringInit){
		return GPSDriver_Status_UnInitializedError;
	}

	GPSDriver_Status_TypeDef		ret = GPSDriver_Status_OK;
	FIFO_Status_TypeDef				fifoRet;
	_GPSDriver_NMEASentenceString	retNMEAString;

	fifoRet = FIFOQueue_dequeue(&pSelf->nmeaSentenceStringFIFO, &retNMEAString);

	if (fifoRet == FIFO_Status_Empty){
		return ret;
	} else if (fifoRet == FIFO_Status_OK){

		if (retNMEAString.sentenceLength > sizeof(GPS_NMEA_GPGGA_PREFIX) &&
				stringEqual(GPS_NMEA_GPGGA_PREFIX, retNMEAString.sentenceString, sizeof(GPS_NMEA_GPGGA_PREFIX))) {

			if ((ret = _GPSDriver_handleGPGGASentence(pSelf, &retNMEAString)) != GPSDriver_Status_OK){
				return ret;
			}
			pSelf->gpggaPartialSegmentTimestamp	= retNMEAString.timestamp;
			pSelf->gpggaPartialSegmentReceived	= true;

		} else if (retNMEAString.sentenceLength  > sizeof(GPS_NMEA_GPGSA_PREFIX) &&
				stringEqual(GPS_NMEA_GPGSA_PREFIX, retNMEAString.sentenceString, sizeof(GPS_NMEA_GPGSA_PREFIX)) ){

			if ((ret = _GPSDriver_handleGPGSASentence(pSelf, &retNMEAString)) != GPSDriver_Status_OK){
				return ret;
			}
			pSelf->gpgsaPartialSegmentTimestamp	= retNMEAString.timestamp;
			pSelf->gpgsaPartialSegmentReceived	= true;

		} else if (retNMEAString.sentenceLength  > sizeof(GPS_NMEA_GPRMC_PREFIX) &&
				stringEqual(GPS_NMEA_GPRMC_PREFIX, retNMEAString.sentenceString, sizeof(GPS_NMEA_GPRMC_PREFIX)) ){

			if ((ret = _GPSDriver_handleGPRMCSentence(pSelf, &retNMEAString)) != GPSDriver_Status_OK){
				return ret;
			}
			pSelf->gprmcPartialSegmentTimestamp	= retNMEAString.timestamp;
			pSelf->gprmcPartialSegmentReceived	= true;

		} else {
			return GPSDriver_Status_OK; //< not recognised NMEA Sentence prefix
		}
	} else {
		return GPSDriver_Status_Error;
	}

	if (pSelf->gpggaPartialSegmentReceived && pSelf->gpgsaPartialSegmentReceived && pSelf->gprmcPartialSegmentReceived){
		if (ABS(pSelf->gpggaPartialSegmentTimestamp - pSelf->gpgsaPartialSegmentTimestamp) < GPS_NMEA_MAX_SENTENCES_DELAY &&
			ABS(pSelf->gpgsaPartialSegmentTimestamp - pSelf->gprmcPartialSegmentTimestamp) < GPS_NMEA_MAX_SENTENCES_DELAY &&
			ABS(pSelf->gpggaPartialSegmentTimestamp - pSelf->gprmcPartialSegmentTimestamp) < GPS_NMEA_MAX_SENTENCES_DELAY){

			for (uint8_t i=0; i<GPS_DRIVER_MAX_CALLBACK_NUMBER; i++){
				if (pSelf->pReceivedDataCallbackFunctions[i] != NULL){
					pSelf->pReceivedDataCallbackFunctions[i](pSelf->partialGPSData, (void*)pSelf->pCallbackArguments[i]);
				}
			}
		}
	}

	return GPSDriver_Status_OK;
}

//< ----- Private functions ----- >//

static GPSDriver_Status_TypeDef _GPSDriver_addCommandSufix(volatile GPSDriver_TypeDef* pSelf, uint8_t* buffer, uint16_t* length, uint16_t bufferSize){

	GPSDriver_Status_TypeDef ret = GPSDriver_Status_OK;

	if ((*length) + GPS_NMEA_SUFIX_LENGTH > bufferSize){
		return GPSDriver_Status_BufferOverflowError;
	}

	buffer[(*length)++] = GPS_NMEA_CHECKSUM_SEPARATOR_SIGN;

	if ((ret = _GPSDriver_getNMEAChecksumString(buffer, (*length), buffer + (*length))) != GPSDriver_Status_OK){
		return ret;
	}
	(*length) += GPS_NMEA_CHECKSUM_LENGTH;

	buffer[(*length)++] = GPS_NMEA_PRE_TERMINATION_SIGN;
	buffer[(*length)++] = GPS_NMEA_TERMINATION_SIGN;

	return ret;
}

static GPSDriver_Status_TypeDef _GPSDriver_sendCommandAndWaitForResponse(volatile GPSDriver_TypeDef* pSelf, uint8_t* commandBuffer, uint16_t commandLength, uint8_t* expectedOKResponse, uint16_t expectedOKResponseLength, uint32_t timeoutMs){

	GPSDriver_Status_TypeDef ret = GPSDriver_Status_OK;
	if (pSelf->awaitingResponseState != _GPSDriver_ResponseState_Idle){
		return GPSDriver_Status_BusyError;
	}

	memcpy((void*)pSelf->awaitingResponse, expectedOKResponse, expectedOKResponseLength);
	pSelf->awaitingResponseLength	= timeoutMs;
	pSelf->awaitingResponseState	= _GPSDriver_ResponseState_WaitingForResponse;

	volatile UartDriver_CallbackIterator_TypeDef	tempCallbackIterator;

	if (	UartDriver_setReceivedBytesStartAndTerminationSignCallback
			(
				pSelf->pUartHandler,
				_GPSDriver_sendCommandAndWaitForResponseTemporaryCallback,
				(void*)pSelf,
				&tempCallbackIterator,
				expectedOKResponse[0],
				expectedOKResponse[expectedOKResponseLength-1]
			) != UartDriver_Status_OK ) {

		return GPSDriver_Status_UartError;
	}

	if (ret == GPSDriver_Status_OK && UartDriver_sendBytes(pSelf->pUartHandler, commandBuffer, commandLength) != UartDriver_Status_OK){
		return GPSDriver_Status_UartError;
	}

	uint32_t sentCommandTimestamp;
	if (ret == GPSDriver_Status_OK && MSTimerDriver_getMSTime(pSelf->pMSTimer, &sentCommandTimestamp) != MSTimerDriver_Status_OK){
		ret = GPSDriver_Status_Error;
	}

	uint32_t actualTimestamp;
	do {
		if (ret == GPSDriver_Status_OK && MSTimerDriver_getMSTime(pSelf->pMSTimer, &actualTimestamp) != MSTimerDriver_Status_OK){
			ret = GPSDriver_Status_Error;
		}
	} while (	ret == GPSDriver_Status_OK &&
				actualTimestamp < sentCommandTimestamp + GPS_COMMAND_RESPONSE_TIMEOUT_MS &&
				pSelf->awaitingResponseState != _GPSDriver_ResponseState_ResponseReceived);

	if (pSelf->awaitingResponseState != _GPSDriver_ResponseState_ResponseReceived){
		ret = GPSDriver_Status_ACKTimeoutError;
	}

	if (UartDriver_removeReceivedBytesStartAndTerminationSignCallback(pSelf->pUartHandler, tempCallbackIterator) != UartDriver_Status_OK ) {
		ret = (ret == GPSDriver_Status_OK) ? GPSDriver_Status_UartError : ret;
	}

	return ret;
}

static GPSDriver_Status_TypeDef _GPSDriver_sendCommand(volatile GPSDriver_TypeDef* pSelf, uint8_t* commandBuffer, uint16_t commandLength){

	if (UartDriver_sendBytes(pSelf->pUartHandler, commandBuffer, commandLength) != UartDriver_Status_OK){
		return GPSDriver_Status_UartError;
	}

	return GPSDriver_Status_OK;
}

static GPSDriver_Status_TypeDef _GPSDriver_sendTestCommand(volatile GPSDriver_TypeDef* pSelf){

	GPSDriver_Status_TypeDef ret	= GPSDriver_Status_OK;
	uint16_t i						= 0;
	uint8_t buffer[GPS_MAX_COMMAND_LENGTH] = {0};

	memcpy(buffer, GPS_CONFIG_TEST_CMD, sizeof(GPS_CONFIG_TEST_CMD));
	i += sizeof(GPS_CONFIG_TEST_CMD);

	if ((ret = _GPSDriver_addCommandSufix(pSelf, buffer, &i, GPS_MAX_COMMAND_LENGTH)) != GPSDriver_Status_OK){
		return ret;
	}

	return _GPSDriver_sendCommandAndWaitForResponse(pSelf, buffer, i, (uint8_t*)GPS_CONFIG_TEST_RESPONSE_PREFIX, sizeof(GPS_CONFIG_TEST_RESPONSE_PREFIX), GPS_COMMAND_RESPONSE_TIMEOUT_MS);
}

static GPSDriver_Status_TypeDef _GPSDriver_changeUartBaudrateCommand(volatile GPSDriver_TypeDef* pSelf, uint32_t baudRate){

	GPSDriver_Status_TypeDef ret = GPSDriver_Status_OK;
	if (pSelf->state == GPSDriver_State_UnInitialized){
		return GPSDriver_Status_UnInitializedError;
	}

	uint16_t i = 0;
	uint8_t buffer[GPS_MAX_COMMAND_LENGTH];
	memset(buffer, 0, GPS_MAX_COMMAND_LENGTH);

	memcpy(buffer, GPS_CONFIG_CHANGE_UART_SPEED_CMD, sizeof(GPS_CONFIG_CHANGE_UART_SPEED_CMD));
	i += sizeof(GPS_CONFIG_CHANGE_INFO_PERIOD_CMD);

	buffer[i++] = GPS_NMEA_DATA_SEPARATOR_SIGN;

	i += sprintf ((char*)(buffer+i), "%lu", baudRate);

	if ((ret = _GPSDriver_addCommandSufix(pSelf, buffer, &i, GPS_MAX_COMMAND_LENGTH)) != GPSDriver_Status_OK){
		return ret;
	}

	if ((ret = _GPSDriver_sendCommand(pSelf, buffer, i)) != GPSDriver_Status_OK){
		return ret;
	}

	if (UartDriver_setBaudRate(pSelf->pUartHandler, GPS_UART_BAUDRATE) != UartDriver_Status_OK){
		return GPSDriver_Status_UartError;
	}

	HAL_Delay(GPS_SET_BAUDRATE_DELAY);

	return _GPSDriver_sendTestCommand(pSelf);
}

static GPSDriver_Status_TypeDef _GPSDriver_changeUpdateFrequemcyCommand(volatile GPSDriver_TypeDef* pSelf, GPSDriver_Frequency_TypeDef frequency){

	if (pSelf->state == GPSDriver_State_UnInitialized){
		return GPSDriver_Status_Error;
	}

	GPSDriver_Status_TypeDef ret = GPSDriver_Status_OK;
	uint8_t i = 0;
	uint8_t buffer[GPS_MAX_COMMAND_LENGTH];
	memset(buffer, 0, GPS_MAX_COMMAND_LENGTH);

	memcpy(buffer, GPS_CONFIG_CHANGE_INFO_PERIOD_CMD, sizeof(GPS_CONFIG_CHANGE_INFO_PERIOD_CMD));
	i += sizeof(GPS_CONFIG_CHANGE_INFO_PERIOD_CMD);

	buffer[i++] = GPS_NMEA_DATA_SEPARATOR_SIGN;

	switch(frequency){
	case GPSDriver_Frequency_0_5Hz:
		memcpy(buffer+i, "2000", sizeof("2000"));
		i += sizeof("2000");
		break;
	case GPSDriver_Frequency_1Hz:
		memcpy(buffer+i, "1000", sizeof("1000"));
		i += sizeof("1000");
		break;
	case GPSDriver_Frequency_2Hz:
		memcpy(buffer+i, "500", sizeof("500"));
		i += sizeof("500");
		break;
	case GPSDriver_Frequency_5Hz:
		memcpy(buffer+i, "200", sizeof("200"));
		i += sizeof("200");
		break;
	case GPSDriver_Frequency_10Hz:
		memcpy(buffer+i, "100", sizeof("100"));
		i += sizeof("100");
		break;
	default:
		return GPSDriver_Status_Error;
	}

	buffer[i++] = GPS_NMEA_DATA_SEPARATOR_SIGN;

	if ((ret = _GPSDriver_getNMEAChecksumString(buffer, i, buffer + i)) != GPSDriver_Status_OK){
		return ret;
	}
	i+= GPS_NMEA_CHECKSUM_LENGTH;

	buffer[i++] = GPS_NMEA_PRE_TERMINATION_SIGN;
	buffer[i++] = GPS_NMEA_TERMINATION_SIGN;

	if ((ret = _GPSDriver_sendCommandAndWaitForResponse(pSelf, buffer, i, (uint8_t*)GPS_CONFIG_CHANGE_INFO_PERIOD_RESPONSE_PREFIX, sizeof(GPS_CONFIG_CHANGE_INFO_PERIOD_RESPONSE_PREFIX), GPS_COMMAND_RESPONSE_TIMEOUT_MS)) != GPSDriver_Status_OK){
		return ret;
	}

	return ret;
}

static GPSDriver_Status_TypeDef _GPSDriver_getNMEAChecksumString(const uint8_t* sentence, uint16_t length, uint8_t* retChecksumString){

	uint16_t i = 0;
	if (sentence == NULL || retChecksumString == NULL){
		return GPSDriver_Status_Error;
	}

    uint8_t checksum = 0x00;

    // The optional checksum is an XOR of all bytes between "$" and "*".
    while (i < length && *(sentence + i) != 0 && *(sentence + i) != '*'){
        checksum ^= *(sentence + i);
        i++;
    }

    if (uInt8ToString(checksum, retChecksumString) != StringOperations_Status_OK){
    	return GPSDriver_Status_NMEASentenceError;
    }

    return GPSDriver_Status_OK;
}

static GPSDriver_Status_TypeDef _GPSDriver_checkNMEAChecksum(const uint8_t* sentence, uint16_t length){

	uint16_t i = 0;
	GPSDriver_Status_TypeDef ret = GPSDriver_Status_OK;

    if (*(sentence+i) == '$'){
        i++;
    } else {
    	return GPSDriver_Status_NMEASentenceError;
    }

    uint8_t checksumBuffer[2] = {0, 0};

    if ((ret = _GPSDriver_getNMEAChecksumString(sentence+i, length-i, checksumBuffer)) != GPSDriver_Status_OK){
    	return ret;
    }

    while (i < length && sentence[i++] != GPS_NMEA_CHECKSUM_SEPARATOR_SIGN){ }

    if (i == length){
    	return GPSDriver_Status_NMEASentenceError;
    }

    if (*(sentence + i) != checksumBuffer[0] || *(sentence + i + 1) != checksumBuffer[1]){
    	return GPSDriver_Status_WrongNMEAChecksumError;
    }

    return GPSDriver_Status_OK;

}

static GPSDriver_Status_TypeDef _GPSDriver_parseTime(uint8_t* bytes, uint16_t length, volatile DateTime_TypeDef* retDateTime){

	if (length < 6){
		return GPSDriver_Status_NMEASentenceError;
	}

	uint8_t tmp;

	if (decChar2Uint8(*bytes, &tmp) != StringOperations_Status_OK){
		return GPSDriver_Status_NMEASentenceError;
	}
	retDateTime->hour	= tmp * 10;

	if (decChar2Uint8(*(bytes+1), &tmp) != StringOperations_Status_OK){
		return GPSDriver_Status_NMEASentenceError;
	}
	retDateTime->hour	+= tmp;


	if (decChar2Uint8(*(bytes+2), &tmp) != StringOperations_Status_OK){
		return GPSDriver_Status_NMEASentenceError;
	}
	retDateTime->minute	= tmp * 10;

	if (decChar2Uint8(*(bytes+3), &tmp) != StringOperations_Status_OK){
		return GPSDriver_Status_NMEASentenceError;
	}
	retDateTime->minute	+= tmp;


	if (decChar2Uint8(*(bytes+4), &tmp) != StringOperations_Status_OK){
		return GPSDriver_Status_NMEASentenceError;
	}
	retDateTime->second	= tmp * 10;

	if (decChar2Uint8(*(bytes+5), &tmp) != StringOperations_Status_OK){
		return GPSDriver_Status_NMEASentenceError;
	}
	retDateTime->second	+= tmp;

	return GPSDriver_Status_OK;
}

static GPSDriver_Status_TypeDef _GPSDriver_parseDate(uint8_t* bytes, uint16_t length, volatile DateTime_TypeDef* retDateTime){

	if (length < 6){
		return GPSDriver_Status_NMEASentenceError;
	}

	uint8_t tmp;

	if (decChar2Uint8(*bytes, &tmp) != StringOperations_Status_OK){
		return GPSDriver_Status_NMEASentenceError;
	}
	retDateTime->day		= tmp * 10;

	if (decChar2Uint8(*(bytes+1), &tmp) != StringOperations_Status_OK){
		return GPSDriver_Status_NMEASentenceError;
	}
	retDateTime->day		+= tmp;

	if (decChar2Uint8(*(bytes+2), &tmp) != StringOperations_Status_OK){
		return GPSDriver_Status_NMEASentenceError;
	}
	retDateTime->month	= tmp * 10;

	if (decChar2Uint8(*(bytes+3), &tmp) != StringOperations_Status_OK){
		return GPSDriver_Status_NMEASentenceError;
	}
	retDateTime->month	+= tmp;

	if (decChar2Uint8(*(bytes+4), &tmp) != StringOperations_Status_OK){
		return GPSDriver_Status_NMEASentenceError;
	}
	retDateTime->year	= tmp * 10;

	if (decChar2Uint8(*(bytes+5), &tmp) != StringOperations_Status_OK){
		return GPSDriver_Status_NMEASentenceError;
	}
	retDateTime->year	+= tmp;

	return GPSDriver_Status_OK;
}

static GPSDriver_Status_TypeDef _GPSDriver_parseLatitude(uint8_t* bytes, uint16_t length, volatile GPSData_TypeDef* retGPSData){

	uint16_t tmp;

	if (findChar(bytes, GPS_NMEA_DATA_SEPARATOR_SIGN, length, &tmp) != StringOperations_Status_OK){
		return GPSDriver_Status_NMEASentenceError;
	}
	uint16_t commaIndex = tmp;

	if (findChar(bytes, GPS_NMEA_DECIMAL_SEPARATOR_SIGN, length, &tmp) != StringOperations_Status_OK){
		return GPSDriver_Status_NMEASentenceError;
	}
	uint16_t dotIndex = tmp;

	if (commaIndex == length){
		return GPSDriver_Status_NMEASentenceError;
	}

	if (dotIndex == length || dotIndex > commaIndex){
		return GPSDriver_Status_NMEASentenceError;
	}

	if (*(bytes + commaIndex + 1) == 'N'){
		 if (stringToFixedPoint(bytes, commaIndex, GPS_NMEA_DECIMAL_SEPARATOR_SIGN, GPS_NMEA_FIXED_POINT_FRACTIONAL_BITS, (FixedPoint*)&retGPSData->latitude) != StringOperations_Status_OK){
			return GPSDriver_Status_NMEASentenceError;
		 }
	} else if (*(bytes + commaIndex + 1) == 'S'){
		if (stringToFixedPoint(bytes, commaIndex, GPS_NMEA_DECIMAL_SEPARATOR_SIGN, GPS_NMEA_FIXED_POINT_FRACTIONAL_BITS, (FixedPoint*)&retGPSData->latitude) != StringOperations_Status_OK){
			return GPSDriver_Status_NMEASentenceError;
		}
		retGPSData->latitude.integer *= (-1);
	} else {
		return GPSDriver_Status_NMEASentenceError;
	}

	return GPSDriver_Status_OK;
}

static GPSDriver_Status_TypeDef _GPSDriver_parseLongitude(uint8_t* bytes, uint16_t length, volatile GPSData_TypeDef* retGPSData){

	uint16_t tmp;

	if (findChar(bytes, GPS_NMEA_DATA_SEPARATOR_SIGN, length, &tmp) != StringOperations_Status_OK){
		return GPSDriver_Status_NMEASentenceError;
	}
	uint16_t commaIndex = tmp;

	if (findChar(bytes, GPS_NMEA_DECIMAL_SEPARATOR_SIGN, length, &tmp) != StringOperations_Status_OK){
		return GPSDriver_Status_NMEASentenceError;
	}
	uint16_t dotIndex = tmp;

	if (commaIndex == length){
		return GPSDriver_Status_NMEASentenceError;
	}

	if (dotIndex == length || dotIndex > commaIndex){
		return GPSDriver_Status_NMEASentenceError;
	}

	if (*(bytes + commaIndex + 1) == 'E'){
		 if (stringToFixedPoint(bytes, commaIndex, GPS_NMEA_DECIMAL_SEPARATOR_SIGN, GPS_NMEA_FIXED_POINT_FRACTIONAL_BITS, (FixedPoint*)&retGPSData->longitude) != StringOperations_Status_OK){
			return GPSDriver_Status_NMEASentenceError;
		 }
	} else if (*(bytes + commaIndex + 1) == 'W'){
		if (stringToFixedPoint(bytes, commaIndex, GPS_NMEA_DECIMAL_SEPARATOR_SIGN, GPS_NMEA_FIXED_POINT_FRACTIONAL_BITS, (FixedPoint*)&retGPSData->longitude) != StringOperations_Status_OK){
			return GPSDriver_Status_NMEASentenceError;
		}
		retGPSData->latitude.integer *= (-1);
	} else {
		return GPSDriver_Status_NMEASentenceError;
	}

	return GPSDriver_Status_OK;
}

static GPSDriver_Status_TypeDef _GPSDriver_parseFixedPoint(uint8_t* bytes, uint16_t length, volatile FixedPoint* retFixedPoint){

	uint16_t tmp;

	if (findChar(bytes, GPS_NMEA_DECIMAL_SEPARATOR_SIGN, length, &tmp) != StringOperations_Status_OK){
		return GPSDriver_Status_NMEASentenceError;
	}
	uint16_t dotIndex = tmp;

	if (dotIndex == length){
		return GPSDriver_Status_NMEASentenceError;
	}

	uint32_t decimalPart, fractionalNumerator, fractionalDenominator = 1;
	if (string2UInt32(bytes, dotIndex, &decimalPart) != StringOperations_Status_OK){ //< Number of satellites being tracked
		return GPSDriver_Status_NMEASentenceError;
	}
	bytes += dotIndex + sizeof(GPS_NMEA_DECIMAL_SEPARATOR_SIGN);
	length -= dotIndex + sizeof(GPS_NMEA_DECIMAL_SEPARATOR_SIGN);
	if (string2UInt32(bytes, length, &fractionalNumerator) != StringOperations_Status_OK){ //TODO czy sizeof tu dobrze zadziala?
		return GPSDriver_Status_NMEASentenceError;
	}

	for (uint8_t i=0; i<length; i++){
		fractionalDenominator *= 10;
	}

	*retFixedPoint = FixedPoint_constrDecimalFrac(decimalPart, fractionalNumerator, fractionalDenominator, GPS_NMEA_FIXED_POINT_FRACTIONAL_BITS);

	return GPSDriver_Status_OK;
}

static GPSDriver_Status_TypeDef _GPSDriver_handleGPGGASentence(volatile GPSDriver_TypeDef* pSelf, _GPSDriver_NMEASentenceString* nmeaSentenceString){

	GPSDriver_Status_TypeDef	ret = GPSDriver_Status_OK;
	uint16_t					tmp_u16;
	uint32_t					tmp_u32;

	if ((ret = _GPSDriver_checkNMEAChecksum(nmeaSentenceString->sentenceString, nmeaSentenceString->sentenceLength)) != GPSDriver_Status_OK){
		return ret;
	}

	uint8_t*					it = nmeaSentenceString->sentenceString + sizeof(GPS_NMEA_GPGGA_PREFIX) + sizeof(',');

	//< ----- Parse fix time ----- >//
	if (findChar(it, GPS_NMEA_DATA_SEPARATOR_SIGN, nmeaSentenceString->sentenceString + nmeaSentenceString->sentenceLength - it, &tmp_u16) != StringOperations_Status_OK){
		return GPSDriver_Status_NMEASentenceError;
	}
	if ((ret = _GPSDriver_parseTime(it, tmp_u16, &pSelf->partialGPSData.dateTime)) != GPSDriver_Status_OK){
		return ret;
	}
	it += tmp_u16 + sizeof(',');

	//< ----- Parse latitude ----- >//
	if (findChar(it, GPS_NMEA_DATA_SEPARATOR_SIGN, nmeaSentenceString->sentenceString + nmeaSentenceString->sentenceLength - it, &tmp_u16) != StringOperations_Status_OK){
		return GPSDriver_Status_NMEASentenceError;
	}
	if ((ret = _GPSDriver_parseLatitude(it, tmp_u16 + 2/*comma and N/S*/, &pSelf->partialGPSData)) != GPSDriver_Status_OK){
		return ret;
	}

	it += tmp_u16 + sizeof(',');
	if (findChar(it, GPS_NMEA_DATA_SEPARATOR_SIGN, nmeaSentenceString->sentenceString + nmeaSentenceString->sentenceLength - it, &tmp_u16) != StringOperations_Status_OK){
		return GPSDriver_Status_NMEASentenceError;
	}
	it += tmp_u16 + sizeof(','); //< N/S and comma

	//< ----- Parse longitude ----- >//
	if (findChar(it, GPS_NMEA_DATA_SEPARATOR_SIGN, nmeaSentenceString->sentenceString + nmeaSentenceString->sentenceLength - it, &tmp_u16) != StringOperations_Status_OK){
		return GPSDriver_Status_NMEASentenceError;
	}
	if ((ret = _GPSDriver_parseLongitude(it, tmp_u16 + 2/*comma and E/W*/, &pSelf->partialGPSData)) != GPSDriver_Status_OK){
		return ret;
	}

	it += tmp_u16 + sizeof(',');
	if (findChar(it, GPS_NMEA_DATA_SEPARATOR_SIGN, nmeaSentenceString->sentenceString + nmeaSentenceString->sentenceLength - it, &tmp_u16) != StringOperations_Status_OK){
		return GPSDriver_Status_NMEASentenceError;
	}
	it += tmp_u16 + sizeof(','); //< comma and E/W

	//< ----- Fix quality ----- >//
	if (findChar(it, GPS_NMEA_DATA_SEPARATOR_SIGN, nmeaSentenceString->sentenceString + nmeaSentenceString->sentenceLength - it, &tmp_u16) != StringOperations_Status_OK){
		return GPSDriver_Status_NMEASentenceError;
	}
	it += tmp_u16 + sizeof(','); //< ignore fix quality

	//< ----- Parse number of satellites being tracked ----- >//
	if (findChar(it, GPS_NMEA_DATA_SEPARATOR_SIGN, nmeaSentenceString->sentenceString + nmeaSentenceString->sentenceLength - it, &tmp_u16) != StringOperations_Status_OK){
		return GPSDriver_Status_NMEASentenceError;
	}
	if (string2UInt32(it, tmp_u16, &tmp_u32) != StringOperations_Status_OK){ //< Number of satellites being tracked
		return GPSDriver_Status_NMEASentenceError;
	}
	if (tmp_u32 > 0xFF){
		return GPSDriver_Status_NMEASentenceError;
	} else {
		pSelf->partialGPSData.nSatellites = (uint8_t) tmp_u32;
	}
	it += tmp_u16 + sizeof(',');

	//< ----- Parse horizontal dilution of position ----- >//
	if (findChar(it, GPS_NMEA_DATA_SEPARATOR_SIGN, nmeaSentenceString->sentenceString + nmeaSentenceString->sentenceLength - it, &tmp_u16) != StringOperations_Status_OK){
		return GPSDriver_Status_NMEASentenceError;
	}
	if ((ret = _GPSDriver_parseFixedPoint(it, tmp_u16, &pSelf->partialGPSData.horizontalPrecision)) != GPSDriver_Status_OK){ //< Horizontal dilution of position
		return ret;
	}
	it += tmp_u16 + sizeof(',');

	//< ----- Parse altitude ----- >//
	if (findChar(it, GPS_NMEA_DATA_SEPARATOR_SIGN, nmeaSentenceString->sentenceString + nmeaSentenceString->sentenceLength - it, &tmp_u16) != StringOperations_Status_OK){
		return GPSDriver_Status_NMEASentenceError;
	}
	if ((ret = _GPSDriver_parseFixedPoint(it, tmp_u16, &pSelf->partialGPSData.altitude)) != GPSDriver_Status_OK){ //< Altitude, Meters, above mean sea level
		return ret;
	}
	it += tmp_u16 + sizeof(',');

	// Rest of fields is ignored
	return ret;
}

static GPSDriver_Status_TypeDef _GPSDriver_handleGPGSASentence(volatile GPSDriver_TypeDef* pSelf, _GPSDriver_NMEASentenceString* nmeaSentenceString){

	GPSDriver_Status_TypeDef	ret = GPSDriver_Status_OK;
	uint16_t					tmp_u16;
	uint32_t					tmp_u32;

	if ((ret = _GPSDriver_checkNMEAChecksum(nmeaSentenceString->sentenceString, nmeaSentenceString->sentenceLength)) != GPSDriver_Status_OK){
		return ret;
	}

	uint8_t*					it = nmeaSentenceString->sentenceString + sizeof(GPS_NMEA_GPGGA_PREFIX) + sizeof(',');

	//< ----- Parse fix type ----- >//
	if (findChar(it, GPS_NMEA_DATA_SEPARATOR_SIGN, nmeaSentenceString->sentenceString + nmeaSentenceString->sentenceLength - it, &tmp_u16) != StringOperations_Status_OK){
		return GPSDriver_Status_NMEASentenceError;
	}
	if (string2UInt32(it, tmp_u16, &tmp_u32) != StringOperations_Status_OK){ //< Number of satellites being tracked
		return GPSDriver_Status_NMEASentenceError;
	}

	switch (tmp_u16){
	case 1:
		pSelf->partialGPSData.fixType = GPSFixType_NoFix;
		break;
	case 2:
		pSelf->partialGPSData.fixType = GPSFixType_2DFix;
		break;
	case 3:
		pSelf->partialGPSData.fixType = GPSFixType_3DFix;
		break;
	default:
		return GPSDriver_Status_NMEASentenceError;
	}
	it += tmp_u16 + sizeof(',');

	//< ----- Parse/ignore PRNs of satellites used for fix (space for 12) and PDOP (dilution of precision) ----- >//
	#define GPS_GPGSA_FIELDS_TO_IGNORE	13 //< PRNs of satellites used for fix (space for 12)  and PDOP (dilution of precision)
	for (uint8_t i=0; i<13; i++){ //TODO mocno do sprawdzenia
		if (findChar(it, GPS_NMEA_DATA_SEPARATOR_SIGN, nmeaSentenceString->sentenceString + nmeaSentenceString->sentenceLength - it, &tmp_u16) != StringOperations_Status_OK){
			return GPSDriver_Status_NMEASentenceError;
		}
		it += tmp_u16 + sizeof(',');
	}

	//< ----- Parse horizontal dilution of precision (HDOP) ----- >//
	if (findChar(it, GPS_NMEA_DATA_SEPARATOR_SIGN, nmeaSentenceString->sentenceString + nmeaSentenceString->sentenceLength - it, &tmp_u16) != StringOperations_Status_OK){
		return GPSDriver_Status_NMEASentenceError;
	}
	if ((ret = _GPSDriver_parseFixedPoint(it, tmp_u16, &pSelf->partialGPSData.horizontalPrecision)) != GPSDriver_Status_OK){ //< Horizontal dilution of position
		return ret;
	}
	it += tmp_u16 + sizeof(',');

	//< ----- Parse Vertical dilution of precision (HDOP) ----- >//
	if (findChar(it, GPS_NMEA_DATA_SEPARATOR_SIGN, nmeaSentenceString->sentenceString + nmeaSentenceString->sentenceLength - it, &tmp_u16) != StringOperations_Status_OK){
		return GPSDriver_Status_NMEASentenceError;
	}
	if ((ret = _GPSDriver_parseFixedPoint(it, tmp_u16, &pSelf->partialGPSData.verticalPrecision)) != GPSDriver_Status_OK){ //< Horizontal dilution of position
		return ret;
	}
	it += tmp_u16 + sizeof(',');

	return ret;
}

static GPSDriver_Status_TypeDef _GPSDriver_handleGPRMCSentence(volatile GPSDriver_TypeDef* pSelf, _GPSDriver_NMEASentenceString* nmeaSentenceString){

	GPSDriver_Status_TypeDef	ret = GPSDriver_Status_OK;
	uint16_t					tmp_u16;

	if ((ret = _GPSDriver_checkNMEAChecksum(nmeaSentenceString->sentenceString, nmeaSentenceString->sentenceLength)) != GPSDriver_Status_OK){
		return ret;
	}

	uint8_t*					it = nmeaSentenceString->sentenceString + sizeof(GPS_NMEA_GPGGA_PREFIX) + sizeof(',');

	//< ----- Parse fix time ----- >//
	if (findChar(it, GPS_NMEA_DATA_SEPARATOR_SIGN, nmeaSentenceString->sentenceString + nmeaSentenceString->sentenceLength - it, &tmp_u16) != StringOperations_Status_OK){
		return GPSDriver_Status_NMEASentenceError;
	}
	if ((ret = _GPSDriver_parseTime(it, tmp_u16, &pSelf->partialGPSData.dateTime)) != GPSDriver_Status_OK){
		return ret;
	}
	it += tmp_u16 + sizeof(',');

	//< ----- Parse status (Active/Void) ----- >//
	if (findChar(it, GPS_NMEA_DATA_SEPARATOR_SIGN, nmeaSentenceString->sentenceString + nmeaSentenceString->sentenceLength - it, &tmp_u16) != StringOperations_Status_OK){
		return GPSDriver_Status_NMEASentenceError;
	}
	it += tmp_u16 + sizeof(','); //< ignore field

	//< ----- Parse latitude ----- >//
	if (findChar(it, GPS_NMEA_DATA_SEPARATOR_SIGN, nmeaSentenceString->sentenceString + nmeaSentenceString->sentenceLength - it, &tmp_u16) != StringOperations_Status_OK){
		return GPSDriver_Status_NMEASentenceError;
	}
	if ((ret = _GPSDriver_parseLatitude(it, tmp_u16 + 2/*comma and N/S*/, &pSelf->partialGPSData)) != GPSDriver_Status_OK){
		return ret;
	}

	it += tmp_u16 + sizeof(',');
	if (findChar(it, GPS_NMEA_DATA_SEPARATOR_SIGN, nmeaSentenceString->sentenceString + nmeaSentenceString->sentenceLength - it, &tmp_u16) != StringOperations_Status_OK){
		return GPSDriver_Status_NMEASentenceError;
	}
	it += tmp_u16 + sizeof(','); //< N/S and comma

	//< ----- Parse longitude ----- >//
	if (findChar(it, GPS_NMEA_DATA_SEPARATOR_SIGN, nmeaSentenceString->sentenceString + nmeaSentenceString->sentenceLength - it, &tmp_u16) != StringOperations_Status_OK){
		return GPSDriver_Status_NMEASentenceError;
	}
	if ((ret = _GPSDriver_parseLongitude(it, tmp_u16 + 2/*comma and E/W*/, &pSelf->partialGPSData)) != GPSDriver_Status_OK){
		return ret;
	}

	it += tmp_u16 + sizeof(',');
	if (findChar(it, GPS_NMEA_DATA_SEPARATOR_SIGN, nmeaSentenceString->sentenceString + nmeaSentenceString->sentenceLength - it, &tmp_u16) != StringOperations_Status_OK){
		return GPSDriver_Status_NMEASentenceError;
	}
	it += tmp_u16 + sizeof(','); //< comma and E/W


	//< ----- Parse speed over the ground in knots ----- >//
	if (findChar(it, GPS_NMEA_DATA_SEPARATOR_SIGN, nmeaSentenceString->sentenceString + nmeaSentenceString->sentenceLength - it, &tmp_u16) != StringOperations_Status_OK){
		return GPSDriver_Status_NMEASentenceError;
	}
	if ((ret = _GPSDriver_parseFixedPoint(it, tmp_u16, &pSelf->partialGPSData.speed)) != GPSDriver_Status_OK){ //< Horizontal dilution of position
		return ret;
	}
	pSelf->partialGPSData.speed = FixedPoint_a_mult_b(pSelf->partialGPSData.speed, GPS_KNOT_TO_KPH_FACTOR, GPS_NMEA_FIXED_POINT_FRACTIONAL_BITS);

	it += tmp_u16 + sizeof(',');

	//< ----- Parse track angle in degrees ----- >//
	if (findChar(it, GPS_NMEA_DATA_SEPARATOR_SIGN, nmeaSentenceString->sentenceString + nmeaSentenceString->sentenceLength - it, &tmp_u16) != StringOperations_Status_OK){
		return GPSDriver_Status_NMEASentenceError;
	}
	if ((ret = _GPSDriver_parseFixedPoint(it, tmp_u16, &pSelf->partialGPSData.speed)) != GPSDriver_Status_OK){ //< Horizontal dilution of position
		return ret;
	}
	it += tmp_u16 + sizeof(',');

	//< ----- Parse date ----- >//
	if (findChar(it, GPS_NMEA_DATA_SEPARATOR_SIGN, nmeaSentenceString->sentenceString + nmeaSentenceString->sentenceLength - it, &tmp_u16) != StringOperations_Status_OK){
		return GPSDriver_Status_NMEASentenceError;
	}
	if ((ret = _GPSDriver_parseDate(it, tmp_u16, &pSelf->partialGPSData.dateTime)) != GPSDriver_Status_OK){
		return ret;
	}
	it += tmp_u16 + sizeof(',');

	return ret;
}


