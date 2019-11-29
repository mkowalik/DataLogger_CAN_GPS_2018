/*
 * gps_driver.c
 *
 *  Created on: 25.06.2019
 *      Author: Michal Kowalik
 */

#include "main.h"

#if (defined(USED_GPS) && (USED_GPS == UBLOX_M8_GPS))

#include "string.h"
#include "user/ublox_m8_gps_driver.h"
#include "user/string_operations.h"
#include "user/utils.h"
#include "user/fixed_point.h"

#include <stdio.h>

#define	GPS_CONFIG_TEST_RESPONSE_PREFIX					"$PMTK001,0,3"
#define	GPS_CONFIG_CHANGE_UART_SPEED_CMD_PREFIX			"$PMTK251"
#define	GPS_CONFIG_CHANGE_INFO_PERIOD_CMD_PREFIX		"$PMTK220"
#define GPS_CONFIG_CHANGE_INFO_PERIOD_RESPONSE_PREFIX	"$PMTK001,220,3"

#define GPS_MAX_COMMAND_LENGTH	32

#define GPS_NMEA_GPGGA_PREFIX	("$GNGGA") //< GPS_ESSENTIAL_FIX_DATA: time, latitude, longitude, fix quality, no of satelites being tracked, horizontal dilution of position, altitude, mean sea level above WGS84
#define GPS_NMEA_GPGSA_PREFIX	("$GNGSA") //< GPS_SATELITE_STATUS: auto/manual seelction 2D/3D fix, PRNs of satelites used for fix, dilution of precision, horizontal dilution of precision, vertical dilution of precision
#define GPS_NMEA_GPRMC_PREFIX	("$GNRMC") //< RECOMENDED_MINIMUM_SENTENCE: time, A=active/V=void, latitude, N/S, longitude, E/W, speed in knots, track angle in degrees, date, magnetic variation

#define GPS_NMEA_START_SIGN					((char)'$')
#define GPS_NMEA_PRE_TERMINATION_SIGN		((char)'\r')
#define GPS_NMEA_TERMINATION_SIGN			((char)'\n')
#define GPS_NMEA_DATA_SEPARATOR_SIGN		((char)',')
#define GPS_NMEA_DECIMAL_SEPARATOR_SIGN		((char)'.')
#define GPS_NMEA_CHECKSUM_SEPARATOR_SIGN	((char)'*')
#define GPS_NMEA_CHECKSUM_LENGTH			2

#define GPS_NMEA_SUFIX_LENGTH				(sizeof(GPS_NMEA_CHECKSUM_SEPARATOR_SIGN)+GPS_NMEA_CHECKSUM_LENGTH+sizeof(GPS_NMEA_PRE_TERMINATION_SIGN)+sizeof(GPS_NMEA_TERMINATION_SIGN))

#define GPS_KNOT_TO_KPH_FACTOR				FixedPoint_constrDecimalFrac(1, 852, 1000, GPS_NMEA_FIXED_POINT_FRACTIONAL_BITS)

#define GPS_UART_DEFAULT_AT_START_BAUDRATE	115200

//< ----- Private functions prototypes ----- >//

static GPSDriver_Status_TypeDef _GPSDriver_getNMEAChecksumValue(const uint8_t* pSentence, uint8_t* retChecksumValue);
static GPSDriver_Status_TypeDef _GPSDriver_checkNMEAChecksum(const uint8_t* pSentence);
static GPSDriver_Status_TypeDef _GPSDriver_appendNMEAChecksumString(uint8_t* pSentence, uint16_t bufferSize);
static GPSDriver_Status_TypeDef _GPSDriver_appendCommandSufix(volatile Ublox8MGPSDriver_TypeDef* pSelf, uint8_t* pBuffer, uint16_t bufferSize);

static GPSDriver_Status_TypeDef _GPSDriver_sendCommandAndWaitForResponse(volatile Ublox8MGPSDriver_TypeDef* pSelf, uint8_t* pCommandBuffer, uint8_t* pExpectedOKResponseBuffer);
static GPSDriver_Status_TypeDef _GPSDriver_sendCommand(volatile Ublox8MGPSDriver_TypeDef* pSelf, uint8_t* pCommandBuffer);

static GPSDriver_Status_TypeDef _GPSDriver_sendUbxNavTimeUTCCommand(volatile Ublox8MGPSDriver_TypeDef* pSelf);

static GPSDriver_Status_TypeDef _GPSDriver_changeUartBaudrateCommand(volatile Ublox8MGPSDriver_TypeDef* pSelf, uint32_t baudRate);
static GPSDriver_Status_TypeDef _GPSDriver_changeUpdateFrequemcyCommand(volatile Ublox8MGPSDriver_TypeDef* pSelf, Config_GPSFrequency_TypeDef frequency);

static GPSDriver_Status_TypeDef _GPSDriver_parseTime(uint8_t* pSentence, uint16_t length, volatile DateTime_TypeDef* pRetDateTime);
static GPSDriver_Status_TypeDef _GPSDriver_parseDate(uint8_t* pSentence, uint16_t length, volatile DateTime_TypeDef* pRetDateTime);
static GPSDriver_Status_TypeDef _GPSDriver_parseLatitude(uint8_t* pSentence, uint16_t length, volatile GPSData_TypeDef* pRetGPSData);
static GPSDriver_Status_TypeDef _GPSDriver_parseLongitude(uint8_t* pSentence, uint16_t length, volatile GPSData_TypeDef* pRetGPSData);
static GPSDriver_Status_TypeDef _GPSDriver_parseFixedPoint(uint8_t* pSentence, uint16_t length, volatile FixedPoint* pRetFixedPoint);

static GPSDriver_Status_TypeDef _GPSDriver_handleGNGGASentence(volatile Ublox8MGPSDriver_TypeDef* pSelf, _GPSDriver_NMEASentenceString* pNmeaSentenceString);
static GPSDriver_Status_TypeDef _GPSDriver_handleGNGSASentence(volatile Ublox8MGPSDriver_TypeDef* pSelf, _GPSDriver_NMEASentenceString* pNmeaSentenceString);
static GPSDriver_Status_TypeDef _GPSDriver_handleGNRMCSentence(volatile Ublox8MGPSDriver_TypeDef* pSelf, _GPSDriver_NMEASentenceString* pNmeaSentenceString);

//< ----- Public functions ----- >//

GPSDriver_Status_TypeDef GPSDriver_init(
		volatile Ublox8MGPSDriver_TypeDef* pSelf,
		UartDriver_TypeDef* pUartDriverHandler,
		UartReceiverStartTerm_TypeDef* pUartReceiverHandler,
		MSTimerDriver_TypeDef* pMSTimer,
		DODriver_TypeDef* pDOResetDriver,
		Config_GPSFrequency_TypeDef frequency) {

	GPSDriver_Status_TypeDef ret = GPSDriver_Status_OK;
	if (pSelf == NULL || pUartDriverHandler == NULL || pUartReceiverHandler == NULL || pMSTimer == NULL || pDOResetDriver == NULL){
		return GPSDriver_Status_NullPointerError;
	}

	pSelf->state					= GPSDriver_State_DuringInit;
	pSelf->pUartDriverHandler		= pUartDriverHandler;
	pSelf->pUartReceiverHandler		= pUartReceiverHandler;
	pSelf->pMSTimer					= pMSTimer;
	pSelf->pDOResetDriver			= pDOResetDriver;
	pSelf->uartReaderIterator		= 0;

	pSelf->gpggaPartialSegmentReceived	= false;
	pSelf->gpgsaPartialSegmentReceived	= false;
	pSelf->gprmcPartialSegmentReceived	= false;
	pSelf->gpggaPartialSegmentTimestamp	= 0;
	pSelf->gpgsaPartialSegmentTimestamp = 0;
	pSelf->gprmcPartialSegmentTimestamp	= 0;
	memset((void*)&pSelf->partialGPSData, 0, sizeof(GPSData_TypeDef));

	if (DODriver_SetLow(pSelf->pDOResetDriver) != DODriver_Status_OK){
		ret = GPSDriver_Status_DOResetError;
	}

	if (frequency == Config_GPSFrequency_OFF){
		pSelf->state = GPSDriver_State_OFF;
		return GPSDriver_Status_OK;
	}

	uint32_t actualUartBaudrate;
	if (UartDriver_getBaudRate(pSelf->pUartDriverHandler, &actualUartBaudrate) != UartDriver_Status_OK){
		return GPSDriver_Status_UartDriverError;
	}

	/*if (actualUartBaudrate != GPS_UART_DEFAULT_AT_START_BAUDRATE){
		if (UartDriver_setBaudRate(pSelf->pUartDriverHandler, GPS_UART_DEFAULT_AT_START_BAUDRATE) != UartDriver_Status_OK){
			return GPSDriver_Status_UartDriverError;
		}
	}*/

	if (UartReceiverStartTerm_registerReader(
			pSelf->pUartReceiverHandler,
			&pSelf->uartReaderIterator,
			GPS_NMEA_START_SIGN,
			GPS_NMEA_TERMINATION_SIGN
		) != UartReceiverStartTerm_Status_OK){

		return GPSDriver_Status_UartReceiverError;
	}
	pSelf->uartReaderIteratorSet	= true;

	if (DODriver_SetHigh(pSelf->pDOResetDriver) != DODriver_Status_OK){
		ret = GPSDriver_Status_DOResetError;
	}

	HAL_Delay(GPS_DEVICE_RESET_TIME_MS);	//< setting GPS in RESET state few lines above

	if (UartDriver_startReceiver(pSelf->pUartDriverHandler) != UartDriver_Status_OK){
		return GPSDriver_Status_UartDriverError;
	}

	if (UartReceiverStartTerm_start(pSelf->pUartReceiverHandler) != UartReceiverStartTerm_Status_OK){
		ret = GPSDriver_Status_UartReceiverError;
	}

	if (DODriver_SetHigh(pSelf->pDOResetDriver) != DODriver_Status_OK){
		ret = GPSDriver_Status_DOResetError;
	}

	HAL_Delay(GPS_DEVICE_START_TIME_MS);
/*
	if (ret == GPSDriver_Status_OK){
		ret = _GPSDriver_changeUartBaudrateCommand(pSelf, GPS_UART_BAUDRATE);
	}

	if (ret == GPSDriver_Status_OK){
		ret = _GPSDriver_changeUpdateFrequemcyCommand(pSelf, frequency);
	}
*/
	if (UartReceiverStartTerm_stop(pSelf->pUartReceiverHandler) != UartReceiverStartTerm_Status_OK){
		ret = (ret == GPSDriver_Status_OK) ? GPSDriver_Status_UartReceiverError : ret;
	}

	if (ret == GPSDriver_Status_OK){
		pSelf->state = GPSDriver_State_Initialized;
	}

	return ret;
}

GPSDriver_Status_TypeDef GPSDriver_startReceiver(volatile Ublox8MGPSDriver_TypeDef* pSelf) {

	if (pSelf == NULL){
		return GPSDriver_Status_NullPointerError;
	}

	if (pSelf->state == GPSDriver_State_OFF){
		return GPSDriver_Status_OK;
	}

	if (pSelf->state == GPSDriver_State_UnInitialized || pSelf->state == GPSDriver_State_DuringInit){
		return GPSDriver_Status_UnInitializedError;
	}

	if (pSelf->state == GPSDriver_State_Running){
		return GPSDriver_Status_RunningError;
	}

	if (UartDriver_startReceiver(pSelf->pUartDriverHandler) != UartDriver_Status_OK){
		return GPSDriver_Status_UartDriverError;
	}

	if (UartReceiverStartTerm_start(pSelf->pUartReceiverHandler) != UartReceiverStartTerm_Status_OK){
		return GPSDriver_Status_UartReceiverError;
	}
	pSelf->state = GPSDriver_State_Running;

	return GPSDriver_Status_OK;

}

GPSDriver_Status_TypeDef GPSDriver_stopReceiver(volatile Ublox8MGPSDriver_TypeDef* pSelf) {

	if (pSelf == NULL){
		return GPSDriver_Status_NullPointerError;
	}

	if (pSelf->state == GPSDriver_State_OFF){
		return GPSDriver_Status_OK;
	}

	if (pSelf->state == GPSDriver_State_UnInitialized || pSelf->state == GPSDriver_State_DuringInit){
		return GPSDriver_Status_UnInitializedError;
	}

	if (pSelf->state == GPSDriver_State_Initialized){
		return GPSDriver_Status_NotRunningError;
	}

	if (UartDriver_stopReceiver(pSelf->pUartDriverHandler) != UartDriver_Status_OK){
		return GPSDriver_Status_UartDriverError;
	}

	if (UartReceiverStartTerm_stop(pSelf->pUartReceiverHandler) != UartReceiverStartTerm_Status_OK){
		return GPSDriver_Status_UartReceiverError;
	}
	pSelf->state = GPSDriver_State_Initialized;

	return GPSDriver_Status_OK;

}

GPSDriver_Status_TypeDef GPSDriver_pullLastFrame(volatile Ublox8MGPSDriver_TypeDef* pSelf, GPSData_TypeDef* pRetGPSData) {

	if (pSelf == NULL || pRetGPSData == NULL){
		return GPSDriver_Status_NullPointerError;
	}

	if (pSelf->state == GPSDriver_State_OFF){
		return GPSDriver_Status_Empty;
	}

	if (pSelf->state != GPSDriver_State_Running){
		return GPSDriver_Status_NotRunningError;
	}

	GPSDriver_Status_TypeDef		ret						= GPSDriver_Status_OK;;
	_GPSDriver_NMEASentenceString	nmeaRxSentenceString;
	memset((void*)&nmeaRxSentenceString, 0, sizeof(_GPSDriver_NMEASentenceString));
	UartReceiverStartTerm_Status_TypeDef		retUR					= UartReceiverStartTerm_Status_OK;

	while (1) {

		retUR = UartReceiverStartTerm_pullLastSentence(pSelf->pUartReceiverHandler, pSelf->uartReaderIterator, nmeaRxSentenceString.sentenceString, &nmeaRxSentenceString.sentenceLength, &nmeaRxSentenceString.timestamp); //TODO dodac buffer size
		if (retUR == UartReceiverStartTerm_Status_Empty){
			return GPSDriver_Status_Empty;
		} else if (retUR != UartReceiverStartTerm_Status_OK){
			return GPSDriver_Status_UartReceiverError;
		}

		if (nmeaRxSentenceString.sentenceLength > strlen(GPS_NMEA_GPGGA_PREFIX) &&
				stringEqual((uint8_t*)GPS_NMEA_GPGGA_PREFIX, nmeaRxSentenceString.sentenceString, strlen(GPS_NMEA_GPGGA_PREFIX)) == StringOperations_Status_OK) {

			ret = _GPSDriver_handleGNGGASentence(pSelf, &nmeaRxSentenceString);
			if (ret == GPSDriver_Status_OK){
				pSelf->gpggaPartialSegmentTimestamp	= nmeaRxSentenceString.timestamp;
				pSelf->gpggaPartialSegmentReceived	= true;
			} else if (ret == GPSDriver_Status_NMEASentenceError || ret == GPSDriver_Status_WrongNMEAChecksumError){
				Warning_Handler("GPSDriver_Status_NMEASentenceError or GPSDriver_Status_WrongNMEAChecksumError error while parsing GPGGA Sentence.");
			} else {
				return ret;
			}

		} else if (nmeaRxSentenceString.sentenceLength  > strlen(GPS_NMEA_GPGSA_PREFIX) &&
				stringEqual((uint8_t*)GPS_NMEA_GPGSA_PREFIX, nmeaRxSentenceString.sentenceString, strlen(GPS_NMEA_GPGSA_PREFIX)) == StringOperations_Status_OK){

			ret = _GPSDriver_handleGNGSASentence(pSelf, &nmeaRxSentenceString);

			if (ret == GPSDriver_Status_OK){
				pSelf->gpgsaPartialSegmentTimestamp	= nmeaRxSentenceString.timestamp;
				pSelf->gpgsaPartialSegmentReceived	= true;
			} else if (ret == GPSDriver_Status_NMEASentenceError || ret == GPSDriver_Status_WrongNMEAChecksumError){
				Warning_Handler("GPSDriver_Status_NMEASentenceError or GPSDriver_Status_WrongNMEAChecksumError error while parsing GPGSA Sentence.");
			} else {
				return ret;
			}

		} else if (nmeaRxSentenceString.sentenceLength  > strlen(GPS_NMEA_GPRMC_PREFIX) &&
				stringEqual((uint8_t*)GPS_NMEA_GPRMC_PREFIX, nmeaRxSentenceString.sentenceString, strlen(GPS_NMEA_GPRMC_PREFIX)) == StringOperations_Status_OK){

			ret = _GPSDriver_handleGNRMCSentence(pSelf, &nmeaRxSentenceString);

			if (ret == GPSDriver_Status_OK){
				pSelf->gprmcPartialSegmentTimestamp	= nmeaRxSentenceString.timestamp;
				pSelf->gprmcPartialSegmentReceived	= true;
			} else if (ret == GPSDriver_Status_NMEASentenceError || ret == GPSDriver_Status_WrongNMEAChecksumError){
				Warning_Handler("GPSDriver_Status_NMEASentenceError or GPSDriver_Status_WrongNMEAChecksumError error while parsing GPRMC Sentence.");
			} else {
				return ret;
			}
		}

		if (pSelf->gpggaPartialSegmentReceived && pSelf->gpgsaPartialSegmentReceived && pSelf->gprmcPartialSegmentReceived){
			if (ABS_DIFF(pSelf->gpggaPartialSegmentTimestamp, pSelf->gpgsaPartialSegmentTimestamp) < GPS_NMEA_MAX_SENTENCES_DELAY &&
					ABS_DIFF(pSelf->gpgsaPartialSegmentTimestamp, pSelf->gprmcPartialSegmentTimestamp) < GPS_NMEA_MAX_SENTENCES_DELAY &&
					ABS_DIFF(pSelf->gpggaPartialSegmentTimestamp, pSelf->gprmcPartialSegmentTimestamp) < GPS_NMEA_MAX_SENTENCES_DELAY){

				*pRetGPSData = pSelf->partialGPSData;

				return GPSDriver_Status_OK;
			}
		}
	}
	return GPSDriver_Status_Error;
}

GPSDriver_Status_TypeDef GPSDriver_getState(volatile Ublox8MGPSDriver_TypeDef* pSelf, GPSDriver_State_TypeDef* retState){
	if (pSelf == NULL || retState == NULL){
		return GPSDriver_Status_NullPointerError;
	}

	(*retState) = pSelf->state;
	return GPSDriver_Status_OK;
}

GPSDriver_Status_TypeDef GPSDriver_setOFF(volatile Ublox8MGPSDriver_TypeDef* pSelf){

	GPSDriver_Status_TypeDef ret = GPSDriver_Status_OK;
	if (pSelf == NULL){
		return GPSDriver_Status_NullPointerError;
	}

	if (pSelf->uartReaderIteratorSet){
		if (UartReceiverStartTerm_unregisterReader(pSelf->pUartReceiverHandler, pSelf->uartReaderIterator) != UartReceiverStartTerm_Status_OK){
			ret = GPSDriver_Status_UartReceiverError;
		}
	}

	if (pSelf->state == GPSDriver_State_Running){

		if (UartDriver_stopReceiver(pSelf->pUartDriverHandler) != UartDriver_Status_OK){
			ret = (ret == GPSDriver_Status_OK) ? GPSDriver_Status_UartDriverError : ret;
		}

		if (UartReceiverStartTerm_stop(pSelf->pUartReceiverHandler) != UartReceiverStartTerm_Status_OK){
			ret = (ret == GPSDriver_Status_OK) ? GPSDriver_Status_UartReceiverError : ret;
		}
	}

	if (DODriver_SetLow(pSelf->pDOResetDriver) != DODriver_Status_OK){
		ret = GPSDriver_Status_DOResetError;
	}

	pSelf->state = GPSDriver_State_OFF;

	return ret;
}

//< ----- Private functions ----- >//

static GPSDriver_Status_TypeDef _GPSDriver_getNMEAChecksumValue(const uint8_t* pSentence, uint8_t* retChecksumValue){

	uint16_t i = 0;
    *retChecksumValue = 0;
    if (pSentence[i] == GPS_NMEA_START_SIGN){
    	i++;
    }
    // The optional checksum is an XOR of all bytes between "$" and "*".
    while (pSentence[i] != 0 && pSentence[i] != GPS_NMEA_CHECKSUM_SEPARATOR_SIGN){
        *retChecksumValue ^= pSentence[i++];
    }
    return GPSDriver_Status_OK;
}

static GPSDriver_Status_TypeDef _GPSDriver_checkNMEAChecksum(const uint8_t* pSentence){

	GPSDriver_Status_TypeDef ret = GPSDriver_Status_OK;

    if (pSentence[0] != GPS_NMEA_START_SIGN){
    	return GPSDriver_Status_NMEASentenceError;
    }

    uint8_t checksumVal = 0;
    if ((ret = _GPSDriver_getNMEAChecksumValue(pSentence+1, &checksumVal)) != GPSDriver_Status_OK){
    	return ret;
    }

    uint8_t checksumStr[3];
    if (uInt8ToHexStringMinDigits(checksumStr, checksumVal, true, 2) != StringOperations_Status_OK){
    	return GPSDriver_Status_Error;
    }

    uint16_t i = 0;
    uint16_t length = strlen((char*)pSentence);
    while (i < length && pSentence[i++] != GPS_NMEA_CHECKSUM_SEPARATOR_SIGN){ }

    if (i == length){
    	return GPSDriver_Status_NMEASentenceError;
    }

    if (*(pSentence + i) != checksumStr[0] || *(pSentence + i + 1) != checksumStr[1]){
    	return GPSDriver_Status_WrongNMEAChecksumError;
    }

    return GPSDriver_Status_OK;
}

static GPSDriver_Status_TypeDef _GPSDriver_appendNMEAChecksumString(uint8_t* pSentence, uint16_t bufferSize){

	GPSDriver_Status_TypeDef ret = GPSDriver_Status_OK;
    uint16_t length = strlen((char*)pSentence);
	if (pSentence[length-1] != GPS_NMEA_CHECKSUM_SEPARATOR_SIGN){
		if (length + 1 > bufferSize){
			return GPSDriver_Status_BufferOverflowError;
		}
		strCharCat(pSentence, GPS_NMEA_CHECKSUM_SEPARATOR_SIGN); //append '*' if not at the end of the pSentence
	}

	if (length + GPS_NMEA_CHECKSUM_LENGTH > bufferSize){
		return GPSDriver_Status_BufferOverflowError;
	}

    uint8_t checksum = 0x00;

    if ((ret = _GPSDriver_getNMEAChecksumValue(pSentence, &checksum)) != GPSDriver_Status_OK){
    	return ret;
    }

    if (appendUint8ToHexString(pSentence, checksum, bufferSize, true) != StringOperations_Status_OK){
    	return GPSDriver_Status_NMEASentenceError;
    }

    return GPSDriver_Status_OK;
}

static GPSDriver_Status_TypeDef _GPSDriver_appendCommandSufix(volatile Ublox8MGPSDriver_TypeDef* pSelf, uint8_t* pBuffer, uint16_t bufferSize){

	GPSDriver_Status_TypeDef ret = GPSDriver_Status_OK;

	if (strlen((char*)pBuffer) + GPS_NMEA_SUFIX_LENGTH > bufferSize){
		return GPSDriver_Status_BufferOverflowError;
	}

	strCharCat(pBuffer, (uint8_t)GPS_NMEA_CHECKSUM_SEPARATOR_SIGN);

	if ((ret = _GPSDriver_appendNMEAChecksumString(pBuffer, bufferSize)) != GPSDriver_Status_OK){
		return ret;
	}

	strCharCat(pBuffer, (uint8_t)GPS_NMEA_PRE_TERMINATION_SIGN);
	strCharCat(pBuffer, (uint8_t)GPS_NMEA_TERMINATION_SIGN);

	return ret;
}

static GPSDriver_Status_TypeDef _GPSDriver_sendCommandAndWaitForResponse(volatile Ublox8MGPSDriver_TypeDef* pSelf, uint8_t* pCommandBuffer, uint8_t* pExpectedOKResponseBuffer){

	GPSDriver_Status_TypeDef ret = GPSDriver_Status_OK;

	uint16_t expectedResponseLength = strlen((char*)pExpectedOKResponseBuffer);

	UartReceiverStartTerm_ReaderIterator_TypeDef	tempReaderIterator;
	uint8_t								tempSentenceBuffer[GPS_NMEA_MAX_SENTENCE_LENGTH_INCLUDING_CRC];
	uint16_t							tempSentenceLength;
	uint32_t							tempSentenceTimestamp;
	UartReceiverStartTerm_Status_TypeDef			retUR	= UartReceiverStartTerm_Status_OK;

	if (	UartReceiverStartTerm_registerReader
			(
				pSelf->pUartReceiverHandler,
				&tempReaderIterator,
				pExpectedOKResponseBuffer[0],
				pExpectedOKResponseBuffer[expectedResponseLength-1]
			) != UartReceiverStartTerm_Status_OK ) {

		return GPSDriver_Status_UartReceiverError;
	}

	ret = _GPSDriver_sendCommand(pSelf, pCommandBuffer);

	uint32_t sentCommandTimestamp;
	if (ret == GPSDriver_Status_OK && MSTimerDriver_getMSTime(pSelf->pMSTimer, &sentCommandTimestamp) != MSTimerDriver_Status_OK){
		ret = GPSDriver_Status_MSTimerError;
	}

	while (ret == GPSDriver_Status_OK){

		retUR = UartReceiverStartTerm_pullLastSentence(pSelf->pUartReceiverHandler, tempReaderIterator, tempSentenceBuffer, &tempSentenceLength, &tempSentenceTimestamp);
		if (retUR != UartReceiverStartTerm_Status_OK && retUR != UartReceiverStartTerm_Status_Empty){
			ret = GPSDriver_Status_UartReceiverError;
		}

		if (retUR != UartReceiverStartTerm_Status_Empty && tempSentenceLength == expectedResponseLength){
			StringOperations_Status_TypeDef retStatus = stringEqual(tempSentenceBuffer, pExpectedOKResponseBuffer, tempSentenceLength);
			if (retStatus == StringOperations_Status_OK){
				break;
			} else if (retStatus == StringOperations_Status_NotEqual){
				continue;
			} else {
				ret = GPSDriver_Status_StringOperationsError;
			}
		}

		uint32_t actualTimestamp;
		if (MSTimerDriver_getMSTime(pSelf->pMSTimer, &actualTimestamp) != MSTimerDriver_Status_OK){
			ret = GPSDriver_Status_MSTimerError;
		}
		if (ret == GPSDriver_Status_OK && actualTimestamp - sentCommandTimestamp > GPS_COMMAND_RESPONSE_TIMEOUT_MS){
			ret = GPSDriver_Status_ACKTimeoutError;
		}
	}

	if (UartReceiverStartTerm_unregisterReader(pSelf->pUartReceiverHandler, tempReaderIterator) != UartReceiverStartTerm_Status_OK ) {
		ret = (ret == GPSDriver_Status_OK) ? GPSDriver_Status_UartReceiverError : ret;
	}

	return ret;
}

static GPSDriver_Status_TypeDef _GPSDriver_sendCommand(volatile Ublox8MGPSDriver_TypeDef* pSelf, uint8_t* pCommandBuffer){

	if (UartDriver_sendBytesDMA(pSelf->pUartDriverHandler, pCommandBuffer, strlen((char*)pCommandBuffer), GPS_TX_TIMEOUT_MS) != UartDriver_Status_OK){
		return GPSDriver_Status_UartDriverError;
	}

	UartDriver_Status_TypeDef ret = UartDriver_waitForTxTimeout(pSelf->pUartDriverHandler);

	if (ret == UartDriver_Status_TxTimeoutError){
		return GPSDriver_Status_TXTimeoutError;
	} else if (ret != UartDriver_Status_OK){
		return GPSDriver_Status_UartDriverError;
	}

	return GPSDriver_Status_OK;
}

static GPSDriver_Status_TypeDef _GPSDriver_sendUBXCommandAndWaitForResponse(
		volatile Ublox8MGPSDriver_TypeDef* pSelf,
		uint8_t* pCommandBuffer,
		uint16_t bufferSize,
		uint8_t* pExpectedOKResponseBuffer){

	GPSDriver_Status_TypeDef ret = GPSDriver_Status_OK;

	uint16_t expectedResponseLength = strlen((char*)pExpectedOKResponseBuffer);

	UartReceiverStartTerm_ReaderIterator_TypeDef	tempReaderIterator;
	uint8_t								tempSentenceBuffer[GPS_NMEA_MAX_SENTENCE_LENGTH_INCLUDING_CRC];
	uint16_t							tempSentenceLength;
	uint32_t							tempSentenceTimestamp;
	UartReceiverStartTerm_Status_TypeDef			retUR	= UartReceiverStartTerm_Status_OK;

	if (	UartReceiverStartTerm_registerReader
			(
				pSelf->pUartReceiverHandler,
				&tempReaderIterator,
				pExpectedOKResponseBuffer[0],
				pExpectedOKResponseBuffer[expectedResponseLength-1]
			) != UartReceiverStartTerm_Status_OK ) {

		return GPSDriver_Status_UartReceiverError;
	}

	ret = _GPSDriver_sendCommand(pSelf, pCommandBuffer);

	uint32_t sentCommandTimestamp;
	if (ret == GPSDriver_Status_OK && MSTimerDriver_getMSTime(pSelf->pMSTimer, &sentCommandTimestamp) != MSTimerDriver_Status_OK){
		ret = GPSDriver_Status_MSTimerError;
	}

	while (ret == GPSDriver_Status_OK){

		retUR = UartReceiverStartTerm_pullLastSentence(pSelf->pUartReceiverHandler, tempReaderIterator, tempSentenceBuffer, &tempSentenceLength, &tempSentenceTimestamp);
		if (retUR != UartReceiverStartTerm_Status_OK && retUR != UartReceiverStartTerm_Status_Empty){
			ret = GPSDriver_Status_UartReceiverError;
		}

		if (retUR != UartReceiverStartTerm_Status_Empty && tempSentenceLength == expectedResponseLength){
			StringOperations_Status_TypeDef retStatus = stringEqual(tempSentenceBuffer, pExpectedOKResponseBuffer, tempSentenceLength);
			if (retStatus == StringOperations_Status_OK){
				break;
			} else if (retStatus == StringOperations_Status_NotEqual){
				continue;
			} else {
				ret = GPSDriver_Status_StringOperationsError;
			}
		}

		uint32_t actualTimestamp;
		if (MSTimerDriver_getMSTime(pSelf->pMSTimer, &actualTimestamp) != MSTimerDriver_Status_OK){
			ret = GPSDriver_Status_MSTimerError;
		}
		if (ret == GPSDriver_Status_OK && actualTimestamp - sentCommandTimestamp > GPS_COMMAND_RESPONSE_TIMEOUT_MS){
			ret = GPSDriver_Status_ACKTimeoutError;
		}
	}

	if (UartReceiverStartTerm_unregisterReader(pSelf->pUartReceiverHandler, tempReaderIterator) != UartReceiverStartTerm_Status_OK ) {
		ret = (ret == GPSDriver_Status_OK) ? GPSDriver_Status_UartReceiverError : ret;
	}

	return ret;
}

static GPSDriver_Status_TypeDef _GPSDriver_getUBXChecksumValue(volatile Ublox8MGPSDriver_TypeDef* pSelf, uint8_t* pCommandBuffer, uint16_t commandLength, uint8_t* pRetCRC) {

	pRetCRC[0] = 0;
	pRetCRC[1] = 0;
	for(uint16_t i = 0; i < commandLength; i++)
	{
		pRetCRC[0] = pRetCRC[0] + pCommandBuffer[i];;
		pRetCRC[1] = pRetCRC[1] + pRetCRC[0];
	}

	return GPSDriver_Status_OK;
}

#define UBX_PREAMBULE_BYTE0	0xB5
#define UBX_PREAMBULE_BYTE1	0x62

#define UBX_CLASS_NAV		0x05
#define UBX_ID_TIMEUTC		0x21

static GPSDriver_Status_TypeDef _GPSDriver_sendUbxNavTimeUTCCommand(volatile Ublox8MGPSDriver_TypeDef* pSelf){
/*
	GPSDriver_Status_TypeDef ret = GPSDriver_Status_OK;
	uint8_t buffer[GPS_MAX_COMMAND_LENGTH];
	memset(buffer, 0, GPS_MAX_COMMAND_LENGTH);

	buffer[0] = UBX_PREAMBULE_BYTE0; //< 0xB5
	buffer[1] = UBX_PREAMBULE_BYTE1; //< 0x62

	buffer[2] = UBX_CLASS_NAV; //< 0x05
	buffer[3] = UBX_ID_TIMEUTC; //< 0x21

	buffer[4] = 0; //< length
	buffer[5] = 0; //< length

	if ((ret = _GPSDriver_getUBXChecksumValue(pSelf, buffer, 6, buffer + 6)) != GPSDriver_Status_OK){
		return ret;
	}

	_GPSDriver_sendUBXCommandAndWaitForResponse(pSelf, buffer, 8, buffer);


*/
}

static GPSDriver_Status_TypeDef _GPSDriver_changeUpdateFrequemcyCommand(volatile Ublox8MGPSDriver_TypeDef* pSelf, Config_GPSFrequency_TypeDef frequency){
/*
	if (pSelf->state == GPSDriver_State_UnInitialized){
		return GPSDriver_Status_Error;
	}

	GPSDriver_Status_TypeDef ret = GPSDriver_Status_OK;

	uint8_t commandBuffer[GPS_MAX_COMMAND_LENGTH];
	uint8_t responseBuffer[GPS_MAX_COMMAND_LENGTH];

	memset(commandBuffer, 0, GPS_MAX_COMMAND_LENGTH);
	memset(responseBuffer, 0, GPS_MAX_COMMAND_LENGTH);

	strcpy((char*)commandBuffer, GPS_CONFIG_CHANGE_INFO_PERIOD_CMD_PREFIX);
	strCharCat(commandBuffer, GPS_NMEA_DATA_SEPARATOR_SIGN);

	switch(frequency){
	case Config_GPSFrequency_OFF:
		break;
	case Config_GPSFrequency_0_5Hz:
		strcat((char*)commandBuffer, "2000");
		break;
	case Config_GPSFrequency_1Hz:
		strcat((char*)commandBuffer, "1000");
		break;
	case Config_GPSFrequency_2Hz:
		strcat((char*)commandBuffer, "500");
		break;
	case Config_GPSFrequency_5Hz:
		strcat((char*)commandBuffer, "200");
		break;
	case Config_GPSFrequency_10Hz:
		strcat((char*)commandBuffer, "100");
		break;
	default:
		return GPSDriver_Status_Error;
	}
	if ((ret = _GPSDriver_appendCommandSufix(pSelf, commandBuffer, GPS_MAX_COMMAND_LENGTH)) != GPSDriver_Status_OK){
		return ret;
	}

	strcpy((char*)responseBuffer, GPS_CONFIG_CHANGE_INFO_PERIOD_RESPONSE_PREFIX);
	if ((ret = _GPSDriver_appendCommandSufix(pSelf, responseBuffer, GPS_MAX_COMMAND_LENGTH)) != GPSDriver_Status_OK){
		return ret;
	}

	return _GPSDriver_sendCommandAndWaitForResponse(pSelf, commandBuffer, responseBuffer);*/
}

static GPSDriver_Status_TypeDef _GPSDriver_parseTime(uint8_t* pSentence, uint16_t length, volatile DateTime_TypeDef* pRetDateTime){

	if (length < 6){
		return GPSDriver_Status_NMEASentenceError;
	}

	uint8_t tmp;
	uint16_t dotIndex;

	if (decChar2Uint8(*pSentence, &tmp) != StringOperations_Status_OK){
		return GPSDriver_Status_NMEASentenceError;
	}
	pRetDateTime->hour	= tmp * 10;

	if (decChar2Uint8(*(pSentence+1), &tmp) != StringOperations_Status_OK){
		return GPSDriver_Status_NMEASentenceError;
	}
	pRetDateTime->hour	+= tmp;


	if (decChar2Uint8(*(pSentence+2), &tmp) != StringOperations_Status_OK){
		return GPSDriver_Status_NMEASentenceError;
	}
	pRetDateTime->minute	= tmp * 10;

	if (decChar2Uint8(*(pSentence+3), &tmp) != StringOperations_Status_OK){
		return GPSDriver_Status_NMEASentenceError;
	}
	pRetDateTime->minute	+= tmp;


	if (decChar2Uint8(*(pSentence+4), &tmp) != StringOperations_Status_OK){
		return GPSDriver_Status_NMEASentenceError;
	}
	pRetDateTime->second	= tmp * 10;

	if (decChar2Uint8(*(pSentence+5), &tmp) != StringOperations_Status_OK){
		return GPSDriver_Status_NMEASentenceError;
	}
	pRetDateTime->second	+= tmp;

	pRetDateTime->miliseconds	= 0;
	StringOperations_Status_TypeDef strOperStat = findChar(pSentence, GPS_NMEA_DECIMAL_SEPARATOR_SIGN, length, &dotIndex);
	if (strOperStat == StringOperations_Status_CharNotFoundError){
		return GPSDriver_Status_OK;
	} else if (strOperStat != StringOperations_Status_OK){
		return GPSDriver_Status_NMEASentenceError;
	}

	uint16_t factor				= 100;
	for (uint16_t i = dotIndex+1; i < length && factor > 0; i++){
		if (decChar2Uint8(*(pSentence+i), &tmp) != StringOperations_Status_OK){
			return GPSDriver_Status_NMEASentenceError;
		}
		pRetDateTime->miliseconds += (factor * tmp);
		factor /= 10;
	}

	return GPSDriver_Status_OK;
}

static GPSDriver_Status_TypeDef _GPSDriver_parseDate(uint8_t* pSentence, uint16_t length, volatile DateTime_TypeDef* pRetDateTime){

	if (length < 6){
		return GPSDriver_Status_NMEASentenceError;
	}

	uint8_t tmp;

	if (decChar2Uint8(*pSentence, &tmp) != StringOperations_Status_OK){
		return GPSDriver_Status_NMEASentenceError;
	}
	pRetDateTime->day		= tmp * 10;

	if (decChar2Uint8(*(pSentence+1), &tmp) != StringOperations_Status_OK){
		return GPSDriver_Status_NMEASentenceError;
	}
	pRetDateTime->day		+= tmp;

	if (decChar2Uint8(*(pSentence+2), &tmp) != StringOperations_Status_OK){
		return GPSDriver_Status_NMEASentenceError;
	}
	pRetDateTime->month	= tmp * 10;

	if (decChar2Uint8(*(pSentence+3), &tmp) != StringOperations_Status_OK){
		return GPSDriver_Status_NMEASentenceError;
	}
	pRetDateTime->month	+= tmp;

	if (decChar2Uint8(*(pSentence+4), &tmp) != StringOperations_Status_OK){
		return GPSDriver_Status_NMEASentenceError;
	}
	pRetDateTime->year	= tmp * 10;

	if (decChar2Uint8(*(pSentence+5), &tmp) != StringOperations_Status_OK){
		return GPSDriver_Status_NMEASentenceError;
	}
	pRetDateTime->year	+= tmp;

	return GPSDriver_Status_OK;
}

static GPSDriver_Status_TypeDef _GPSDriver_parseLatitude(uint8_t* pSentence, uint16_t length, volatile GPSData_TypeDef* pRetGPSData){

	uint16_t tmp;

	if (findChar(pSentence, GPS_NMEA_DATA_SEPARATOR_SIGN, length, &tmp) != StringOperations_Status_OK){
		return GPSDriver_Status_NMEASentenceError;
	}
	uint16_t commaIndex = tmp;

	if (findChar(pSentence, GPS_NMEA_DECIMAL_SEPARATOR_SIGN, length, &tmp) != StringOperations_Status_OK){
		return GPSDriver_Status_NMEASentenceError;
	}
	uint16_t dotIndex = tmp;

	if (commaIndex == length){
		return GPSDriver_Status_NMEASentenceError;
	}

	if (dotIndex == length || dotIndex > commaIndex){
		return GPSDriver_Status_NMEASentenceError;
	}

	if (*(pSentence + commaIndex + 1) == 'N'){
		 if (stringToFixedPoint(pSentence, commaIndex, GPS_NMEA_DECIMAL_SEPARATOR_SIGN, GPS_NMEA_FIXED_POINT_FRACTIONAL_BITS, (FixedPoint*)&pRetGPSData->latitude) != StringOperations_Status_OK){
			return GPSDriver_Status_NMEASentenceError;
		 }
	} else if (*(pSentence + commaIndex + 1) == 'S'){
		if (stringToFixedPoint(pSentence, commaIndex, GPS_NMEA_DECIMAL_SEPARATOR_SIGN, GPS_NMEA_FIXED_POINT_FRACTIONAL_BITS, (FixedPoint*)&pRetGPSData->latitude) != StringOperations_Status_OK){
			return GPSDriver_Status_NMEASentenceError;
		}
		pRetGPSData->latitude.integer *= (-1);
	} else {
		return GPSDriver_Status_NMEASentenceError;
	}

	return GPSDriver_Status_OK;
}

static GPSDriver_Status_TypeDef _GPSDriver_parseLongitude(uint8_t* pSentence, uint16_t length, volatile GPSData_TypeDef* pRetGPSData){

	uint16_t tmp;

	if (findChar(pSentence, GPS_NMEA_DATA_SEPARATOR_SIGN, length, &tmp) != StringOperations_Status_OK){
		return GPSDriver_Status_NMEASentenceError;
	}
	uint16_t commaIndex = tmp;

	if (findChar(pSentence, GPS_NMEA_DECIMAL_SEPARATOR_SIGN, length, &tmp) != StringOperations_Status_OK){
		return GPSDriver_Status_NMEASentenceError;
	}
	uint16_t dotIndex = tmp;

	if (commaIndex == length){
		return GPSDriver_Status_NMEASentenceError;
	}

	if (dotIndex == length || dotIndex > commaIndex){
		return GPSDriver_Status_NMEASentenceError;
	}

	if (*(pSentence + commaIndex + 1) == 'E'){
		 if (stringToFixedPoint(pSentence, commaIndex, GPS_NMEA_DECIMAL_SEPARATOR_SIGN, GPS_NMEA_FIXED_POINT_FRACTIONAL_BITS, (FixedPoint*)&pRetGPSData->longitude) != StringOperations_Status_OK){
			return GPSDriver_Status_NMEASentenceError;
		 }
	} else if (*(pSentence + commaIndex + 1) == 'W'){
		if (stringToFixedPoint(pSentence, commaIndex, GPS_NMEA_DECIMAL_SEPARATOR_SIGN, GPS_NMEA_FIXED_POINT_FRACTIONAL_BITS, (FixedPoint*)&pRetGPSData->longitude) != StringOperations_Status_OK){
			return GPSDriver_Status_NMEASentenceError;
		}
		pRetGPSData->latitude.integer *= (-1);
	} else {
		return GPSDriver_Status_NMEASentenceError;
	}

	return GPSDriver_Status_OK;
}

static GPSDriver_Status_TypeDef _GPSDriver_parseFixedPoint(uint8_t* pSentence, uint16_t length, volatile FixedPoint* pRetFixedPoint){

	uint16_t tmp;

	if (findChar(pSentence, GPS_NMEA_DECIMAL_SEPARATOR_SIGN, length, &tmp) != StringOperations_Status_OK){
		return GPSDriver_Status_NMEASentenceError;
	}
	uint16_t dotIndex = tmp;

	if (dotIndex == length){
		return GPSDriver_Status_NMEASentenceError;
	}

	uint32_t decimalPart, fractionalNumerator, fractionalDenominator = 1;
	if (string2UInt32(pSentence, dotIndex, &decimalPart) != StringOperations_Status_OK){ //< Number of satellites being tracked
		return GPSDriver_Status_NMEASentenceError;
	}
	pSentence += dotIndex + sizeof(GPS_NMEA_DECIMAL_SEPARATOR_SIGN);
	length -= dotIndex + sizeof(GPS_NMEA_DECIMAL_SEPARATOR_SIGN);
	if (string2UInt32(pSentence, length, &fractionalNumerator) != StringOperations_Status_OK){ //TODO czy sizeof tu dobrze zadziala?
		return GPSDriver_Status_NMEASentenceError;
	}

	for (uint8_t i=0; i<length; i++){
		fractionalDenominator *= 10;
	}

	*pRetFixedPoint = FixedPoint_constrDecimalFrac(decimalPart, fractionalNumerator, fractionalDenominator, GPS_NMEA_FIXED_POINT_FRACTIONAL_BITS);

	return GPSDriver_Status_OK;
}

static GPSDriver_Status_TypeDef _GPSDriver_handleGNGGASentence(volatile Ublox8MGPSDriver_TypeDef* pSelf, _GPSDriver_NMEASentenceString* pNmeaSentenceString){

	GPSDriver_Status_TypeDef	ret = GPSDriver_Status_OK;
	uint16_t					tmp_u16;
	uint32_t					tmp_u32;

	if ((ret = _GPSDriver_checkNMEAChecksum(pNmeaSentenceString->sentenceString)) != GPSDriver_Status_OK){
		return ret;
	}

	uint32_t	tmp1 = sizeof(GPS_NMEA_DATA_SEPARATOR_SIGN);
	uint32_t	tmp2 = strlen(GPS_NMEA_GPGGA_PREFIX);
	uint8_t*					it = pNmeaSentenceString->sentenceString + tmp2 + tmp1;

	//< ----- Parse fix time ----- >//
	if (findChar(it, GPS_NMEA_DATA_SEPARATOR_SIGN, pNmeaSentenceString->sentenceString + pNmeaSentenceString->sentenceLength - it, &tmp_u16) != StringOperations_Status_OK){
		return GPSDriver_Status_NMEASentenceError;
	}
	if ((ret = _GPSDriver_parseTime(it, tmp_u16, &pSelf->partialGPSData.gpsDateTime)) != GPSDriver_Status_OK){
		return ret;
	}
	it += tmp_u16 + sizeof(GPS_NMEA_DATA_SEPARATOR_SIGN);

	//< ----- Parse latitude ----- >//
	if (findChar(it, GPS_NMEA_DATA_SEPARATOR_SIGN, pNmeaSentenceString->sentenceString + pNmeaSentenceString->sentenceLength - it, &tmp_u16) != StringOperations_Status_OK){
		return GPSDriver_Status_NMEASentenceError;
	}
	if ((ret = _GPSDriver_parseLatitude(it, tmp_u16 + 2/*comma and N/S*/, &pSelf->partialGPSData)) != GPSDriver_Status_OK){
		return ret;
	}

	it += tmp_u16 + sizeof(GPS_NMEA_DATA_SEPARATOR_SIGN);
	if (findChar(it, GPS_NMEA_DATA_SEPARATOR_SIGN, pNmeaSentenceString->sentenceString + pNmeaSentenceString->sentenceLength - it, &tmp_u16) != StringOperations_Status_OK){
		return GPSDriver_Status_NMEASentenceError;
	}
	it += tmp_u16 + sizeof(GPS_NMEA_DATA_SEPARATOR_SIGN); //< N/S and comma

	//< ----- Parse longitude ----- >//
	if (findChar(it, GPS_NMEA_DATA_SEPARATOR_SIGN, pNmeaSentenceString->sentenceString + pNmeaSentenceString->sentenceLength - it, &tmp_u16) != StringOperations_Status_OK){
		return GPSDriver_Status_NMEASentenceError;
	}
	if ((ret = _GPSDriver_parseLongitude(it, tmp_u16 + 2/*comma and E/W*/, &pSelf->partialGPSData)) != GPSDriver_Status_OK){
		return ret;
	}

	it += tmp_u16 + sizeof(GPS_NMEA_DATA_SEPARATOR_SIGN);
	if (findChar(it, GPS_NMEA_DATA_SEPARATOR_SIGN, pNmeaSentenceString->sentenceString + pNmeaSentenceString->sentenceLength - it, &tmp_u16) != StringOperations_Status_OK){
		return GPSDriver_Status_NMEASentenceError;
	}
	it += tmp_u16 + sizeof(GPS_NMEA_DATA_SEPARATOR_SIGN); //< comma and E/W

	//< ----- Fix quality ----- >//
	if (findChar(it, GPS_NMEA_DATA_SEPARATOR_SIGN, pNmeaSentenceString->sentenceString + pNmeaSentenceString->sentenceLength - it, &tmp_u16) != StringOperations_Status_OK){
		return GPSDriver_Status_NMEASentenceError;
	}
	it += tmp_u16 + sizeof(GPS_NMEA_DATA_SEPARATOR_SIGN); //< ignore fix quality

	//< ----- Parse number of satellites being tracked ----- >//
	if (findChar(it, GPS_NMEA_DATA_SEPARATOR_SIGN, pNmeaSentenceString->sentenceString + pNmeaSentenceString->sentenceLength - it, &tmp_u16) != StringOperations_Status_OK){
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
	it += tmp_u16 + sizeof(GPS_NMEA_DATA_SEPARATOR_SIGN);

	//< ----- Parse horizontal dilution of position ----- >//
	if (findChar(it, GPS_NMEA_DATA_SEPARATOR_SIGN, pNmeaSentenceString->sentenceString + pNmeaSentenceString->sentenceLength - it, &tmp_u16) != StringOperations_Status_OK){
		return GPSDriver_Status_NMEASentenceError;
	}
	if ((ret = _GPSDriver_parseFixedPoint(it, tmp_u16, &pSelf->partialGPSData.horizontalPrecision)) != GPSDriver_Status_OK){ //< Horizontal dilution of position
		return ret;
	}
	it += tmp_u16 + sizeof(GPS_NMEA_DATA_SEPARATOR_SIGN);

	//< ----- Parse altitude ----- >//
	if (findChar(it, GPS_NMEA_DATA_SEPARATOR_SIGN, pNmeaSentenceString->sentenceString + pNmeaSentenceString->sentenceLength - it, &tmp_u16) != StringOperations_Status_OK){
		return GPSDriver_Status_NMEASentenceError;
	}
	if ((ret = _GPSDriver_parseFixedPoint(it, tmp_u16, &pSelf->partialGPSData.altitude)) != GPSDriver_Status_OK){ //< Altitude, Meters, above mean sea level
		return ret;
	}
	it += tmp_u16 + sizeof(GPS_NMEA_DATA_SEPARATOR_SIGN);

	// Rest of fields is ignored
	return ret;
}

static GPSDriver_Status_TypeDef _GPSDriver_handleGNGSASentence(volatile Ublox8MGPSDriver_TypeDef* pSelf, _GPSDriver_NMEASentenceString* pNmeaSentenceString){

	GPSDriver_Status_TypeDef	ret = GPSDriver_Status_OK;
	uint16_t					tmp_u16;
	uint32_t					tmp_u32;

	if ((ret = _GPSDriver_checkNMEAChecksum(pNmeaSentenceString->sentenceString)) != GPSDriver_Status_OK){
		return ret;
	}

	uint8_t*					it = pNmeaSentenceString->sentenceString + strlen(GPS_NMEA_GPGGA_PREFIX) + sizeof(GPS_NMEA_DATA_SEPARATOR_SIGN);

	//< ----- Auto selection of 2D or 3D fix ----- >//
	if (findChar(it, GPS_NMEA_DATA_SEPARATOR_SIGN, pNmeaSentenceString->sentenceString + pNmeaSentenceString->sentenceLength - it, &tmp_u16) != StringOperations_Status_OK){
		return GPSDriver_Status_NMEASentenceError;
	}
	it += tmp_u16 + sizeof(GPS_NMEA_DATA_SEPARATOR_SIGN); //< Ignore auto selection of 2D or 3D fix

	//< ----- Parse fix type ----- >//
	if (findChar(it, GPS_NMEA_DATA_SEPARATOR_SIGN, pNmeaSentenceString->sentenceString + pNmeaSentenceString->sentenceLength - it, &tmp_u16) != StringOperations_Status_OK){
		return GPSDriver_Status_NMEASentenceError;
	}
	if (string2UInt32(it, tmp_u16, &tmp_u32) != StringOperations_Status_OK){ //< Number of satellites being tracked
		return GPSDriver_Status_NMEASentenceError;
	}

	switch (tmp_u32){
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
	it += tmp_u16 + sizeof(GPS_NMEA_DATA_SEPARATOR_SIGN);

	//< ----- Parse/ignore PRNs of satellites used for fix (space for 12) and PDOP (dilution of precision) ----- >//
	#define GPS_GPGSA_FIELDS_TO_IGNORE	13 //< PRNs of satellites used for fix (space for 12)  and PDOP (dilution of precision)
	for (uint8_t i=0; i<GPS_GPGSA_FIELDS_TO_IGNORE; i++){ //TODO mocno do sprawdzenia
		if (findChar(it, GPS_NMEA_DATA_SEPARATOR_SIGN, pNmeaSentenceString->sentenceString + pNmeaSentenceString->sentenceLength - it, &tmp_u16) != StringOperations_Status_OK){
			return GPSDriver_Status_NMEASentenceError;
		}
		it += tmp_u16 + sizeof(GPS_NMEA_DATA_SEPARATOR_SIGN);
	}

	//< ----- Parse horizontal dilution of precision (HDOP) ----- >//
	if (findChar(it, GPS_NMEA_DATA_SEPARATOR_SIGN, pNmeaSentenceString->sentenceString + pNmeaSentenceString->sentenceLength - it, &tmp_u16) != StringOperations_Status_OK){
		return GPSDriver_Status_NMEASentenceError;
	}
	if ((ret = _GPSDriver_parseFixedPoint(it, tmp_u16, &pSelf->partialGPSData.horizontalPrecision)) != GPSDriver_Status_OK){ //< Horizontal dilution of position
		return ret;
	}
	it += tmp_u16 + sizeof(GPS_NMEA_DATA_SEPARATOR_SIGN);

	//< ----- Parse Vertical dilution of precision (HDOP) ----- >//
	if (findChar(it, GPS_NMEA_CHECKSUM_SEPARATOR_SIGN, pNmeaSentenceString->sentenceString + pNmeaSentenceString->sentenceLength - it, &tmp_u16) != StringOperations_Status_OK){
		return GPSDriver_Status_NMEASentenceError;
	}
	if ((ret = _GPSDriver_parseFixedPoint(it, tmp_u16, &pSelf->partialGPSData.verticalPrecision)) != GPSDriver_Status_OK){ //< Horizontal dilution of position
		return ret;
	}
	it += tmp_u16 + sizeof(GPS_NMEA_DATA_SEPARATOR_SIGN);

	return ret;
}

static GPSDriver_Status_TypeDef _GPSDriver_handleGNRMCSentence(volatile Ublox8MGPSDriver_TypeDef* pSelf, _GPSDriver_NMEASentenceString* pNmeaSentenceString){

	GPSDriver_Status_TypeDef	ret = GPSDriver_Status_OK;
	uint16_t					tmp_u16;

	if ((ret = _GPSDriver_checkNMEAChecksum(pNmeaSentenceString->sentenceString)) != GPSDriver_Status_OK){
		return ret;
	}

	uint8_t*					it = pNmeaSentenceString->sentenceString + strlen(GPS_NMEA_GPGGA_PREFIX) + sizeof(GPS_NMEA_DATA_SEPARATOR_SIGN);

	//< ----- Parse fix time ----- >//
	if (findChar(it, GPS_NMEA_DATA_SEPARATOR_SIGN, pNmeaSentenceString->sentenceString + pNmeaSentenceString->sentenceLength - it, &tmp_u16) != StringOperations_Status_OK){
		return GPSDriver_Status_NMEASentenceError;
	}
	if ((ret = _GPSDriver_parseTime(it, tmp_u16, &pSelf->partialGPSData.gpsDateTime)) != GPSDriver_Status_OK){
		return ret;
	}
	it += tmp_u16 + sizeof(GPS_NMEA_DATA_SEPARATOR_SIGN);

	//< ----- Parse status (Active/Void) ----- >//
	if (findChar(it, GPS_NMEA_DATA_SEPARATOR_SIGN, pNmeaSentenceString->sentenceString + pNmeaSentenceString->sentenceLength - it, &tmp_u16) != StringOperations_Status_OK){
		return GPSDriver_Status_NMEASentenceError;
	}
	it += tmp_u16 + sizeof(GPS_NMEA_DATA_SEPARATOR_SIGN); //< ignore field

	//< ----- Parse latitude ----- >//
	if (findChar(it, GPS_NMEA_DATA_SEPARATOR_SIGN, pNmeaSentenceString->sentenceString + pNmeaSentenceString->sentenceLength - it, &tmp_u16) != StringOperations_Status_OK){
		return GPSDriver_Status_NMEASentenceError;
	}
	if ((ret = _GPSDriver_parseLatitude(it, tmp_u16 + 2/*comma and N/S*/, &pSelf->partialGPSData)) != GPSDriver_Status_OK){
		return ret;
	}

	it += tmp_u16 + sizeof(GPS_NMEA_DATA_SEPARATOR_SIGN);
	if (findChar(it, GPS_NMEA_DATA_SEPARATOR_SIGN, pNmeaSentenceString->sentenceString + pNmeaSentenceString->sentenceLength - it, &tmp_u16) != StringOperations_Status_OK){
		return GPSDriver_Status_NMEASentenceError;
	}
	it += tmp_u16 + sizeof(GPS_NMEA_DATA_SEPARATOR_SIGN); //< N/S and comma

	//< ----- Parse longitude ----- >//
	if (findChar(it, GPS_NMEA_DATA_SEPARATOR_SIGN, pNmeaSentenceString->sentenceString + pNmeaSentenceString->sentenceLength - it, &tmp_u16) != StringOperations_Status_OK){
		return GPSDriver_Status_NMEASentenceError;
	}
	if ((ret = _GPSDriver_parseLongitude(it, tmp_u16 + 2/*comma and E/W*/, &pSelf->partialGPSData)) != GPSDriver_Status_OK){
		return ret;
	}

	it += tmp_u16 + sizeof(GPS_NMEA_DATA_SEPARATOR_SIGN);
	if (findChar(it, GPS_NMEA_DATA_SEPARATOR_SIGN, pNmeaSentenceString->sentenceString + pNmeaSentenceString->sentenceLength - it, &tmp_u16) != StringOperations_Status_OK){
		return GPSDriver_Status_NMEASentenceError;
	}
	it += tmp_u16 + sizeof(GPS_NMEA_DATA_SEPARATOR_SIGN); //< comma and E/W


	//< ----- Parse speed over the ground in knots ----- >//
	if (findChar(it, GPS_NMEA_DATA_SEPARATOR_SIGN, pNmeaSentenceString->sentenceString + pNmeaSentenceString->sentenceLength - it, &tmp_u16) != StringOperations_Status_OK){
		return GPSDriver_Status_NMEASentenceError;
	}
	if ((ret = _GPSDriver_parseFixedPoint(it, tmp_u16, &pSelf->partialGPSData.speed)) != GPSDriver_Status_OK){ //< Horizontal dilution of position
		return ret;
	}
	pSelf->partialGPSData.speed = FixedPoint_a_mult_b(pSelf->partialGPSData.speed, GPS_KNOT_TO_KPH_FACTOR, GPS_NMEA_FIXED_POINT_FRACTIONAL_BITS);

	it += tmp_u16 + sizeof(GPS_NMEA_DATA_SEPARATOR_SIGN);

	//< ----- Parse track angle in degrees ----- >//
	if (findChar(it, GPS_NMEA_DATA_SEPARATOR_SIGN, pNmeaSentenceString->sentenceString + pNmeaSentenceString->sentenceLength - it, &tmp_u16) != StringOperations_Status_OK){
		return GPSDriver_Status_NMEASentenceError;
	}
	if (tmp_u16 == 0){
		pSelf->partialGPSData.trackAngle = FixedPoint_constrDecimalFrac(0, 0, 1, GPS_NMEA_FIXED_POINT_FRACTIONAL_BITS); //< zero
	} else if ((ret = _GPSDriver_parseFixedPoint(it, tmp_u16, &pSelf->partialGPSData.trackAngle)) != GPSDriver_Status_OK){ //< Horizontal dilution of position
		return ret;
	}
	it += tmp_u16 + sizeof(GPS_NMEA_DATA_SEPARATOR_SIGN);

	//< ----- Parse date ----- >//
	if (findChar(it, GPS_NMEA_DATA_SEPARATOR_SIGN, pNmeaSentenceString->sentenceString + pNmeaSentenceString->sentenceLength - it, &tmp_u16) != StringOperations_Status_OK){
		return GPSDriver_Status_NMEASentenceError;
	}
	if ((ret = _GPSDriver_parseDate(it, tmp_u16, &pSelf->partialGPSData.gpsDateTime)) != GPSDriver_Status_OK){
		return ret;
	}
	it += tmp_u16 + sizeof(GPS_NMEA_DATA_SEPARATOR_SIGN);

	return ret;
}

//< ----- test functions ----- >//

bool GPSDriver_testGNGGA1(volatile Ublox8MGPSDriver_TypeDef* pSelf){

	GPSDriver_Status_TypeDef		ret;
	_GPSDriver_NMEASentenceString	nmeaString;
	strcpy((char*)nmeaString.sentenceString, "$GNGGA,012219.40,5005.00694,N,02000.64285,E,2,10,1.17,260.7,M,39.8,M,,0000*4F\r\n");
	nmeaString.sentenceLength	= strlen((char*)nmeaString.sentenceString);
	nmeaString.timestamp		= 100;
	if ((ret = _GPSDriver_handleGNGGASentence(pSelf, &nmeaString)) != GPSDriver_Status_OK){
		return false;
	}
	assert_param(pSelf->partialGPSData.gpsDateTime.hour				== 1);
	assert_param(pSelf->partialGPSData.gpsDateTime.minute			== 22);
	assert_param(pSelf->partialGPSData.gpsDateTime.second			== 19);
	assert_param(pSelf->partialGPSData.gpsDateTime.miliseconds		== 400);
	assert_param(pSelf->partialGPSData.latitude.integer				== ((5005 << 12) + ((694<<12)/100000)) );
	assert_param(pSelf->partialGPSData.longitude.integer			== ((2000 << 12) + ((64285<<12)/100000)) );
	assert_param(pSelf->partialGPSData.nSatellites					== 10);
	assert_param(pSelf->partialGPSData.horizontalPrecision.integer	== ((1 << 12) + ((17<<12)/100)) );
	assert_param(pSelf->partialGPSData.altitude.integer				== ((260 << 12) + ((7<<12)/10)) );
	return true;
}

bool GPSDriver_testGNGSA1(volatile Ublox8MGPSDriver_TypeDef* pSelf){

	GPSDriver_Status_TypeDef		ret;
	_GPSDriver_NMEASentenceString	nmeaString;
	strcpy((char*)nmeaString.sentenceString, "$GNGSA,A,3,21,26,29,20,27,05,31,10,,,,,2.44,1.58,1.87*11\r\n");
	nmeaString.sentenceLength	= strlen((char*)nmeaString.sentenceString);
	nmeaString.timestamp		= 100;
	if ((ret = _GPSDriver_handleGNGSASentence(pSelf, &nmeaString)) != GPSDriver_Status_OK){
		return false;
	}
	assert_param(pSelf->partialGPSData.fixType						== GPSFixType_3DFix);
	assert_param(pSelf->partialGPSData.horizontalPrecision.integer	== ((1 << 12) + ((58<<12)/100)) );
	assert_param(pSelf->partialGPSData.verticalPrecision.integer	== ((1 << 12) + ((87<<12)/100)) );
	return true;
}

bool GPSDriver_testGNRMC1(volatile Ublox8MGPSDriver_TypeDef* pSelf){

	GPSDriver_Status_TypeDef		ret;
	_GPSDriver_NMEASentenceString	nmeaString;
	strcpy((char*)nmeaString.sentenceString, "$GNRMC,012219.60,A,5005.00694,N,02000.64286,E,0.087,,291119,,,D*6F\r\n");
	nmeaString.sentenceLength	= strlen((char*)nmeaString.sentenceString);
	nmeaString.timestamp		= 100;
	if ((ret = _GPSDriver_handleGNRMCSentence(pSelf, &nmeaString)) != GPSDriver_Status_OK){
		return false;
	}
	assert_param(pSelf->partialGPSData.gpsDateTime.hour				== 1);
	assert_param(pSelf->partialGPSData.gpsDateTime.minute			== 22);
	assert_param(pSelf->partialGPSData.gpsDateTime.second			== 19);
	assert_param(pSelf->partialGPSData.gpsDateTime.miliseconds		== 600);
	assert_param(pSelf->partialGPSData.latitude.integer				== ((5005 << 12) + ((694<<12)/100000)) );
	assert_param(pSelf->partialGPSData.longitude.integer			== ((2000 << 12) + ((64286<<12)/100000)) );
	assert_param(pSelf->partialGPSData.speed.integer				== 659);
	assert_param(pSelf->partialGPSData.trackAngle.integer			== 0);
	assert_param(pSelf->partialGPSData.gpsDateTime.day				== 29);
	assert_param(pSelf->partialGPSData.gpsDateTime.month			== 11);
	assert_param(pSelf->partialGPSData.gpsDateTime.year				== 19);
	return true;
}

void GPSDriver_test(){
	Ublox8MGPSDriver_TypeDef self;
	assert_param(GPSDriver_testGNGGA1(&self));
	assert_param(GPSDriver_testGNGSA1(&self));
	assert_param(GPSDriver_testGNRMC1(&self));
}

#endif
