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

#define GPS_NMEA_GPGGA_PREFIX	("$GNGGA") //< GPS_ESSENTIAL_FIX_DATA: time, latitude, longitude, fix quality, no of satelites being tracked, horizontal dilution of position, altitude, mean sea level above WGS84
#define GPS_NMEA_GPGSA_PREFIX	("$GNGSA") //< GPS_SATELITE_STATUS: auto/manual seelction 2D/3D fix, PRNs of satelites used for fix, dilution of precision, horizontal dilution of precision, vertical dilution of precision
#define GPS_NMEA_GPRMC_PREFIX	("$GNRMC") //< RECOMENDED_MINIMUM_SENTENCE: time, A=active/V=void, latitude, N/S, longitude, E/W, speed in knots, track angle in degrees, date, magnetic variation

#define GPS_NMEA_START_SIGN						((char)'$')
#define GPS_NMEA_PRE_TERMINATION_SIGN			((char)'\r')
#define GPS_NMEA_TERMINATION_SIGN				((char)'\n')
#define GPS_NMEA_DATA_SEPARATOR_SIGN			((char)',')
#define GPS_NMEA_DECIMAL_SEPARATOR_SIGN			((char)'.')
#define GPS_NMEA_CHECKSUM_SEPARATOR_SIGN		((char)'*')
#define GPS_NMEA_CHECKSUM_LENGTH				2

#define GPS_NMEA_SUFIX_LENGTH					(sizeof(GPS_NMEA_CHECKSUM_SEPARATOR_SIGN)+GPS_NMEA_CHECKSUM_LENGTH+sizeof(GPS_NMEA_PRE_TERMINATION_SIGN)+sizeof(GPS_NMEA_TERMINATION_SIGN))

#define GPS_KNOT_TO_KPH_FACTOR					FixedPoint_constrDecimalFrac(1U, 852U, 1000U, GPS_NMEA_FIXED_POINT_FRACTIONAL_BITS)

#define GPS_UART_DEFAULT_AT_START_BAUDRATE		115200U

#define GPS_UBX_MAX_COMMAND_LENGTH				60U

#define	GPS_UBX_STATIC_PARTS_LENGTH				8U
#define	GPS_UBX_HEADER_LENGTH					6U

#define	GPS_UBX_SYNC_CHAR_1						0xB5U
#define	GPS_UBX_SYNC_CHAR_2						0x62U

#define	GPS_UBX_ACK_ACK_CLASS					0x05U
#define	GPS_UBX_ACK_ACK_ID						0x01U
#define	GPS_UBX_ACK_NAK_ID						0x00U
#define	GPS_UBX_ACK_ANSWER_PAYLOAD_LENGTH		2U

#define	GPS_UBX_CFG_CLASS						0x06U
#define	GPS_UBX_CFG_RATE_ID						0x08U
#define GPS_UBX_CFG_RATE_REQUEST_PAYLOAD_LENGTH	6U

static const uint8_t ubxCfgRateACKAnswerPrefix[]	= {
		GPS_UBX_SYNC_CHAR_1,
		GPS_UBX_SYNC_CHAR_2,
		GPS_UBX_ACK_ACK_CLASS,
		GPS_UBX_ACK_ACK_ID,
		U16_LSB(GPS_UBX_ACK_ANSWER_PAYLOAD_LENGTH),
		U16_MSB(GPS_UBX_ACK_ANSWER_PAYLOAD_LENGTH),
		GPS_UBX_CFG_CLASS,
		GPS_UBX_CFG_RATE_ID
};

static const uint8_t ubxCfgRateNAKAnswerPrefix[]	= {
		GPS_UBX_SYNC_CHAR_1,
		GPS_UBX_SYNC_CHAR_2,
		GPS_UBX_ACK_ACK_CLASS,
		GPS_UBX_ACK_NAK_ID,
		U16_LSB(GPS_UBX_ACK_ANSWER_PAYLOAD_LENGTH),
		U16_MSB(GPS_UBX_ACK_ANSWER_PAYLOAD_LENGTH),
		GPS_UBX_CFG_CLASS,
		GPS_UBX_CFG_RATE_ID
};

//< ----- Private functions prototypes ----- >//

static GPSDriver_Status_TypeDef _GPSDriver_sendCommand(volatile Ublox8MGPSDriver_TypeDef* pSelf, uint8_t* pCommandBuffer, uint16_t length);

static GPSDriver_Status_TypeDef _GPSDriver_getNMEAChecksumValue(const uint8_t* pSentence, uint8_t* retChecksumValue);
static GPSDriver_Status_TypeDef _GPSDriver_checkNMEAChecksum(const uint8_t* pSentence);

static GPSDriver_Status_TypeDef _GPSDriver_sendUBXCommandAndWaitForResponse(
		volatile Ublox8MGPSDriver_TypeDef* pSelf,
		uint8_t* pCommandBuffer,
		uint16_t bufferSize,
		UartReceiverStartLength_ReaderIterator_TypeDef ackAnswerReaderIterator,
		UartReceiverStartLength_ReaderIterator_TypeDef nakAnswerReaderIterator,
		uint8_t* pAnswerBuffer);

static GPSDriver_Status_TypeDef _GPSDriver_sendUbxCfgRateUTCCommand(volatile Ublox8MGPSDriver_TypeDef* pSelf, Config_GPSFrequency_TypeDef frequency);

static GPSDriver_Status_TypeDef _GPSDriver_parseGPSFixType(uint8_t* pSentence, uint16_t length, volatile GPSFixType* pRetFixType);
static GPSDriver_Status_TypeDef _GPSDriver_parseTime(uint8_t* pSentence, uint16_t length, volatile DateTime_TypeDef* pRetDateTime);
static GPSDriver_Status_TypeDef _GPSDriver_parseDate(uint8_t* pSentence, uint16_t length, volatile DateTime_TypeDef* pRetDateTime);
static GPSDriver_Status_TypeDef _GPSDriver_parseLatitude(uint8_t* pSentence, uint16_t length, volatile GPSData_TypeDef* pRetGPSData);
static GPSDriver_Status_TypeDef _GPSDriver_parseLongitude(uint8_t* pSentence, uint16_t length, volatile GPSData_TypeDef* pRetGPSData);
static GPSDriver_Status_TypeDef _GPSDriver_parseFixedPoint(uint8_t* pSentence, uint16_t length, volatile FixedPoint* pRetFixedPoint);

static GPSDriver_Status_TypeDef _GPSDriver_handleGNGGASentence(volatile Ublox8MGPSDriver_TypeDef* pSelf, _GPSDriver_NMEASentenceString* pNmeaSentenceString);
static GPSDriver_Status_TypeDef _GPSDriver_handleGNGSASentence(volatile Ublox8MGPSDriver_TypeDef* pSelf, _GPSDriver_NMEASentenceString* pNmeaSentenceString);
static GPSDriver_Status_TypeDef _GPSDriver_handleGNRMCSentence(volatile Ublox8MGPSDriver_TypeDef* pSelf, _GPSDriver_NMEASentenceString* pNmeaSentenceString);

static void _GPSDriver_FixTypeRawUARTCharHandler(uint8_t dataByte, uint32_t timestamp, void* pArgs);

//< ----- Public functions ----- >//

GPSDriver_Status_TypeDef GPSDriver_init(
		volatile Ublox8MGPSDriver_TypeDef* pSelf,
		UartDriver_TypeDef* pUartDriverHandler,
		UartReceiverStartTerm_TypeDef* pUartNMEAReceiverHandler,
		UartReceiverStartLength_TypeDef* pUartUBXReceiverHandler,
		MSTimerDriver_TypeDef* pMSTimer,
		DODriver_TypeDef* pDOResetDriver,
		Config_GPSFrequency_TypeDef frequency) {

	GPSDriver_Status_TypeDef ret = GPSDriver_Status_OK;
	if (pSelf == NULL || pUartDriverHandler == NULL || pUartNMEAReceiverHandler == NULL || pUartUBXReceiverHandler == NULL || pMSTimer == NULL || pDOResetDriver == NULL){
		return GPSDriver_Status_NullPointerError;
	}

	pSelf->state						= GPSDriver_State_DuringInit;
	pSelf->pUartDriverHandler			= pUartDriverHandler;
	pSelf->pUartNMEAReceiverHandler		= pUartNMEAReceiverHandler;
	pSelf->pUartUBXReceiverHandler		= pUartUBXReceiverHandler;
	pSelf->pMSTimer						= pMSTimer;
	pSelf->pDOResetDriver				= pDOResetDriver;
	pSelf->uartNMEAReaderIterator		= 0;

	pSelf->gpggaPartialSegmentReceived	= false;
	pSelf->gpgsaPartialSegmentReceived	= false;
	pSelf->gprmcPartialSegmentReceived	= false;
	pSelf->gpggaPartialSegmentTimestamp	= 0;
	pSelf->gpgsaPartialSegmentTimestamp = 0;
	pSelf->gprmcPartialSegmentTimestamp	= 0;
	pSelf->fixType						= GPSFixType_NoFix;
	memset((void*)&pSelf->partialGPSData, 0, sizeof(GPSData_TypeDef));
	if (frequency == Config_GPSFrequency_OFF){
		pSelf->state = GPSDriver_State_OFF;
		return GPSDriver_Status_OK;
	}

	if (UartReceiverStartTerm_registerReader(
			pSelf->pUartNMEAReceiverHandler,
			&pSelf->uartNMEAReaderIterator,
			GPS_NMEA_START_SIGN,
			GPS_NMEA_TERMINATION_SIGN) != UartReceiverStartTerm_Status_OK)
	{
		return GPSDriver_Status_UartReceiverStartTermError;
	}
	pSelf->uartNMEAReaderIteratorSet	= true;

	if (DODriver_SetLow(pSelf->pDOResetDriver) != DODriver_Status_OK){
		ret = GPSDriver_Status_DOResetError;
	}
	HAL_Delay(GPS_DEVICE_RESET_TIME_MS);

	if (DODriver_SetHigh(pSelf->pDOResetDriver) != DODriver_Status_OK){
		ret = GPSDriver_Status_DOResetError;
	}
	HAL_Delay(GPS_DEVICE_START_TIME_MS);

	if (ret == GPSDriver_Status_OK){
		ret = _GPSDriver_sendUbxCfgRateUTCCommand(pSelf, frequency);
	}

	if (ret == GPSDriver_Status_OK){
		pSelf->state = GPSDriver_State_Initialized;
	}

	if (ret == GPSDriver_Status_OK){
		if ( UartDriver_setReceivedByteCallback(
				pSelf->pUartDriverHandler,
				_GPSDriver_FixTypeRawUARTCharHandler,
				(void*)pSelf,
				&(pSelf->fixTypeRawUARTCharHandlerCallbackIt) ) != UartDriver_Status_OK)
		{
			ret = GPSDriver_Status_UartDriverError;
		}
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

	UartDriver_State_TypeDef uartDriverState;
	if (UartDriver_getState(pSelf->pUartDriverHandler, &uartDriverState) != UartDriver_Status_OK){
		return GPSDriver_Status_UartDriverError;
	}

	if (uartDriverState != UartDriver_State_Receiving){
		return GPSDriver_Status_UartDriverNotStatrtedError;
	}

	if (UartReceiverStartTerm_start(pSelf->pUartNMEAReceiverHandler) != UartReceiverStartTerm_Status_OK){
		return GPSDriver_Status_UartReceiverStartTermError;
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

	if (UartReceiverStartTerm_stop(pSelf->pUartNMEAReceiverHandler) != UartReceiverStartTerm_Status_OK){
		return GPSDriver_Status_UartReceiverStartTermError;
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

	GPSDriver_Status_TypeDef ret						= GPSDriver_Status_OK;;
	_GPSDriver_NMEASentenceString nmeaRxSentenceString;
	memset((void*)&nmeaRxSentenceString, 0, sizeof(_GPSDriver_NMEASentenceString));
	UartReceiverStartTerm_Status_TypeDef retUR			= UartReceiverStartTerm_Status_OK;

	while (1) {

		retUR = UartReceiverStartTerm_pullLastSentence(pSelf->pUartNMEAReceiverHandler, pSelf->uartNMEAReaderIterator, nmeaRxSentenceString.sentenceString, &nmeaRxSentenceString.sentenceLength, &nmeaRxSentenceString.timestamp); //TODO dodac buffer size
		if (retUR == UartReceiverStartTerm_Status_Empty){
			return GPSDriver_Status_Empty;
		} else if (retUR != UartReceiverStartTerm_Status_OK){
			return GPSDriver_Status_UartReceiverStartTermError;
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

				pSelf->partialGPSData.msTime		= MIN(pSelf->gpggaPartialSegmentTimestamp, MIN(pSelf->gpgsaPartialSegmentTimestamp, pSelf->gprmcPartialSegmentTimestamp));

				*pRetGPSData 						= pSelf->partialGPSData;

				pSelf->gpggaPartialSegmentReceived	= false;
				pSelf->gpgsaPartialSegmentReceived	= false;
				pSelf->gprmcPartialSegmentReceived	= false;

				return GPSDriver_Status_OK;
			}
		}
	}
	return GPSDriver_Status_Error;
}

GPSDriver_Status_TypeDef GPSDriver_getState(volatile Ublox8MGPSDriver_TypeDef* pSelf, GPSDriver_State_TypeDef* pRetState){
	if (pSelf == NULL || pRetState == NULL){
		return GPSDriver_Status_NullPointerError;
	}

	(*pRetState) = pSelf->state;
	return GPSDriver_Status_OK;
}

GPSDriver_Status_TypeDef GPSDriver_getFixType(volatile Ublox8MGPSDriver_TypeDef* pSelf, GPSFixType* pRetFixType){
	if (pSelf == NULL || pRetFixType == NULL){
		return GPSDriver_Status_NullPointerError;
	}

	(*pRetFixType) = pSelf->fixType;

	return GPSDriver_Status_OK;
}

GPSDriver_Status_TypeDef GPSDriver_setOFF(volatile Ublox8MGPSDriver_TypeDef* pSelf){

	GPSDriver_Status_TypeDef ret = GPSDriver_Status_OK;
	if (pSelf == NULL){
		return GPSDriver_Status_NullPointerError;
	}

	if (pSelf->state == GPSDriver_State_Running){
		if (UartReceiverStartTerm_stop(pSelf->pUartNMEAReceiverHandler) != UartReceiverStartTerm_Status_OK){
			ret = (ret == GPSDriver_Status_OK) ? GPSDriver_Status_UartReceiverStartTermError : ret;
		}
		if (UartReceiverStartLength_stop(pSelf->pUartUBXReceiverHandler) != UartReceiverStartLength_Status_OK){
			ret = (ret == GPSDriver_Status_OK) ? GPSDriver_Status_UartReceiverStartTermError : ret;
		}
	}

	if (pSelf->uartNMEAReaderIteratorSet){
		if (UartReceiverStartTerm_unregisterReader(pSelf->pUartNMEAReceiverHandler, pSelf->uartNMEAReaderIterator) != UartReceiverStartTerm_Status_OK){
			ret = GPSDriver_Status_UartReceiverStartTermError;
		}
	}

	if (DODriver_SetLow(pSelf->pDOResetDriver) != DODriver_Status_OK){
		ret = GPSDriver_Status_DOResetError;
	}

	pSelf->state = GPSDriver_State_OFF;

	return ret;
}

//< ----- Private functions ----- >//

static GPSDriver_Status_TypeDef _GPSDriver_sendCommand(volatile Ublox8MGPSDriver_TypeDef* pSelf, uint8_t* pCommandBuffer, uint16_t length){

	if (UartDriver_sendBytesDMA(pSelf->pUartDriverHandler, pCommandBuffer, length, GPS_TX_TIMEOUT_MS) != UartDriver_Status_OK){
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

static GPSDriver_Status_TypeDef _GPSDriver_sendUBXCommandAndWaitForResponse(
		volatile Ublox8MGPSDriver_TypeDef* pSelf,
		uint8_t* pCommandBuffer,
		uint16_t bufferSize,
		UartReceiverStartLength_ReaderIterator_TypeDef ackAnswerReaderIterator,
		UartReceiverStartLength_ReaderIterator_TypeDef nakAnswerReaderIterator,
		uint8_t* pAnswerBuffer)
{

	GPSDriver_Status_TypeDef				ret		= GPSDriver_Status_OK;
	UartReceiverStartLength_Status_TypeDef	retUR	= UartReceiverStartLength_Status_OK;

	UartDriver_State_TypeDef uartDriverState;
	if (UartDriver_getState(pSelf->pUartDriverHandler, &uartDriverState) != UartDriver_Status_OK){
		return GPSDriver_Status_UartDriverError;
	}

	if (UartReceiverStartLength_start(pSelf->pUartUBXReceiverHandler) != UartReceiverStartLength_Status_OK){
		return GPSDriver_Status_UartReceiverStartLengthError;
	}

	uint32_t	answerTimestamp = 0;

	ret = _GPSDriver_sendCommand(pSelf, pCommandBuffer, bufferSize);

	uint32_t sentCommandTimestamp;
	if (ret == GPSDriver_Status_OK && MSTimerDriver_getMSTime(pSelf->pMSTimer, &sentCommandTimestamp) != MSTimerDriver_Status_OK){
		ret = GPSDriver_Status_MSTimerError;
	}

	while (ret == GPSDriver_Status_OK || ret == GPSDriver_Status_NAKAnswer){

		retUR = UartReceiverStartLength_pullLastSentence(pSelf->pUartUBXReceiverHandler, ackAnswerReaderIterator, pAnswerBuffer, &answerTimestamp);
		if (retUR != UartReceiverStartLength_Status_OK && retUR != UartReceiverStartLength_Status_Empty){
			ret = GPSDriver_Status_UartReceiverStartLengthError;
		}
		if (retUR == UartReceiverStartLength_Status_OK){
			ret = GPSDriver_Status_OK;
			break;
		}

		retUR = UartReceiverStartLength_pullLastSentence(pSelf->pUartUBXReceiverHandler, nakAnswerReaderIterator, pAnswerBuffer, &answerTimestamp);
		if (retUR != UartReceiverStartLength_Status_OK && retUR != UartReceiverStartLength_Status_Empty){
			ret = GPSDriver_Status_UartReceiverStartLengthError;
		}
		if (retUR == UartReceiverStartLength_Status_OK){
			ret = GPSDriver_Status_NAKAnswer;
			break;
		}

		uint32_t actualTimestamp;
		if (MSTimerDriver_getMSTime(pSelf->pMSTimer, &actualTimestamp) != MSTimerDriver_Status_OK){
			ret = GPSDriver_Status_MSTimerError;
		}
		if (ret == GPSDriver_Status_OK && actualTimestamp - sentCommandTimestamp > GPS_COMMAND_RESPONSE_TIMEOUT_MS){
			ret = GPSDriver_Status_ACKTimeoutError;
		}
	}

	if (UartReceiverStartLength_stop(pSelf->pUartUBXReceiverHandler) != UartReceiverStartLength_Status_OK){
		ret = (ret == GPSDriver_Status_OK) ? GPSDriver_Status_UartReceiverStartLengthError : ret;
	}

	return ret;
}

static GPSDriver_Status_TypeDef _GPSDriver_getUBXChecksumValue(volatile Ublox8MGPSDriver_TypeDef* pSelf, uint8_t* pCommandBuffer, uint16_t commandLength, uint8_t* pRetCRC) {
	UNUSED(pSelf);

	pRetCRC[0] = 0;
	pRetCRC[1] = 0;
	for(uint16_t i = 0; i < commandLength; i++)
	{
		pRetCRC[0] = pRetCRC[0] + pCommandBuffer[i];;
		pRetCRC[1] = pRetCRC[1] + pRetCRC[0];
	}

	return GPSDriver_Status_OK;
}

static GPSDriver_Status_TypeDef _GPSDriver_sendUbxCfgRateUTCCommand(volatile Ublox8MGPSDriver_TypeDef* pSelf, Config_GPSFrequency_TypeDef frequency){

	GPSDriver_Status_TypeDef ret = GPSDriver_Status_OK;

	uint16_t measRate	= 0;
	uint16_t navRate	= 1;
	uint16_t timeReg	= 1;

	switch(frequency){
	case Config_GPSFrequency_OFF:
		break;
	case Config_GPSFrequency_0_5Hz:
		measRate = 2000;
		break;
	case Config_GPSFrequency_1Hz:
		measRate = 1000;
		break;
	case Config_GPSFrequency_2Hz:
		measRate = 500;
		break;
	case Config_GPSFrequency_5Hz:
		measRate = 200;
		break;
	case Config_GPSFrequency_10Hz:
		measRate = 100;
		break;
	default:
		return GPSDriver_Status_Error;
	}

	uint8_t buffer[GPS_UBX_MAX_COMMAND_LENGTH];
	memset(buffer, 0, GPS_UBX_MAX_COMMAND_LENGTH);

	buffer[0] = GPS_UBX_SYNC_CHAR_1; //< 0xB5
	buffer[1] = GPS_UBX_SYNC_CHAR_2; //< 0x62

	buffer[2] = GPS_UBX_CFG_CLASS; //< 0x06
	buffer[3] = GPS_UBX_CFG_RATE_ID; //< 0x08

	buffer[4] = U16_LSB(GPS_UBX_CFG_RATE_REQUEST_PAYLOAD_LENGTH); //< length
	buffer[5] = U16_MSB(GPS_UBX_CFG_RATE_REQUEST_PAYLOAD_LENGTH); //< length

	buffer[6] = U16_LSB(measRate);
	buffer[7] = U16_MSB(measRate);

	buffer[8] = U16_LSB(navRate);
	buffer[9] = U16_MSB(navRate);

	buffer[10] = U16_LSB(timeReg);
	buffer[11] = U16_MSB(timeReg);

	if ((ret = _GPSDriver_getUBXChecksumValue(pSelf, buffer+2, 10, buffer + 12)) != GPSDriver_Status_OK){
		return ret;
	}

	UartReceiverStartLength_ReaderIterator_TypeDef	ackReaderIterator;
	UartReceiverStartLength_ReaderIterator_TypeDef	nakReaderIterator;
	bool											ackReaderSet = false;
	bool											nakReaderSet = false;

	if (UartReceiverStartLength_registerReader(
			pSelf->pUartUBXReceiverHandler,
			&ackReaderIterator,
			ARRAY_LENGTH(ubxCfgRateACKAnswerPrefix),
			ubxCfgRateACKAnswerPrefix,
			GPS_UBX_STATIC_PARTS_LENGTH + GPS_UBX_ACK_ANSWER_PAYLOAD_LENGTH) != UartReceiverStartLength_Status_OK)
	{
		ret = GPSDriver_Status_UartReceiverStartLengthError;
	} else {
		ackReaderSet	= true;
	}

	if (UartReceiverStartLength_registerReader(
			pSelf->pUartUBXReceiverHandler,
			&nakReaderIterator,
			ARRAY_LENGTH(ubxCfgRateNAKAnswerPrefix),
			ubxCfgRateNAKAnswerPrefix,
			GPS_UBX_STATIC_PARTS_LENGTH + GPS_UBX_ACK_ANSWER_PAYLOAD_LENGTH) != UartReceiverStartLength_Status_OK)
	{
		ret = GPSDriver_Status_UartReceiverStartLengthError;
	} else {
		nakReaderSet	= true;
	}

	if (ret == GPSDriver_Status_OK){
		ret =  _GPSDriver_sendUBXCommandAndWaitForResponse(
				pSelf,
				buffer,
				GPS_UBX_STATIC_PARTS_LENGTH + GPS_UBX_CFG_RATE_REQUEST_PAYLOAD_LENGTH,
				ackReaderIterator,
				nakReaderIterator,
				buffer);
	}

	if (ackReaderSet){
		if (UartReceiverStartLength_unregisterReader(pSelf->pUartUBXReceiverHandler, ackReaderIterator) != UartReceiverStartLength_Status_OK){
			ret = GPSDriver_Status_UartReceiverStartLengthError;
		}
	}
	if (nakReaderSet){
		if (UartReceiverStartLength_unregisterReader(pSelf->pUartUBXReceiverHandler, nakReaderIterator) != UartReceiverStartLength_Status_OK)
		{
			ret = GPSDriver_Status_UartReceiverStartLengthError;
		}
	}

	return ret;
}

static GPSDriver_Status_TypeDef _GPSDriver_parseGPSFixType(uint8_t* pSentence, uint16_t length, volatile GPSFixType* pRetFixType) {

	if (length != 1){
		return GPSDriver_Status_NMEASentenceError;
	}

	uint8_t tmp;
	if (decChar2Uint8(*pSentence, &tmp) != StringOperations_Status_OK){
		return GPSDriver_Status_NMEASentenceError;
	}

	switch (tmp){
	case 1:
		(*pRetFixType) = GPSFixType_NoFix;
		return GPSDriver_Status_OK;
	case 2:
		(*pRetFixType) = GPSFixType_2DFix;
		return GPSDriver_Status_OK;
	case 3:
		(*pRetFixType) = GPSFixType_3DFix;
		return GPSDriver_Status_OK;
	default:
		return GPSDriver_Status_NMEASentenceError;
	}
	return GPSDriver_Status_NMEASentenceError;
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
	if ((ret = _GPSDriver_parseGPSFixType(it, tmp_u16, &(pSelf->partialGPSData.fixType))) != GPSDriver_Status_OK){ //< Horizontal dilution of position
		return ret;
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

static void _GPSDriver_FixTypeRawUARTCharHandler(uint8_t dataByte, uint32_t timestamp, void* pArgs){
	UNUSED(timestamp);

	Ublox8MGPSDriver_TypeDef* pSelf = (Ublox8MGPSDriver_TypeDef*) pArgs;

	switch (pSelf->fixTypeParserState){
	case _GPSFixTypeParser_State_WaitingFor$:
		if (dataByte == ((uint8_t)'$')){
			pSelf->fixTypeParserState = _GPSFixTypeParser_State_WaitingFor1stG;
		} else {
			pSelf->fixTypeParserState = _GPSFixTypeParser_State_WaitingFor$;
		}
		return;
	case _GPSFixTypeParser_State_WaitingFor1stG:
		if (dataByte == ((uint8_t)'G')){
			pSelf->fixTypeParserState = _GPSFixTypeParser_State_WaitingForN;
		} else {
			pSelf->fixTypeParserState = _GPSFixTypeParser_State_WaitingFor$;
		}
		return;
	case _GPSFixTypeParser_State_WaitingForN:
		if (dataByte == ((uint8_t)'N')){
			pSelf->fixTypeParserState = _GPSFixTypeParser_State_WaitingFor2ndG;
		} else {
			pSelf->fixTypeParserState = _GPSFixTypeParser_State_WaitingFor$;
		}
		return;
	case _GPSFixTypeParser_State_WaitingFor2ndG:
		if (dataByte == ((uint8_t)'G')){
			pSelf->fixTypeParserState = _GPSFixTypeParser_State_WaitingForS;
		} else {
			pSelf->fixTypeParserState = _GPSFixTypeParser_State_WaitingFor$;
		}
		return;
	case _GPSFixTypeParser_State_WaitingForS:
		if (dataByte == ((uint8_t)'S')){
			pSelf->fixTypeParserState = _GPSFixTypeParser_State_WaitingForA;
		} else {
			pSelf->fixTypeParserState = _GPSFixTypeParser_State_WaitingFor$;
		}
		return;
	case _GPSFixTypeParser_State_WaitingForA:
		if (dataByte == ((uint8_t)'A')){
			pSelf->fixTypeParserState = _GPSFixTypeParser_State_WaitingFor1stComma;
		} else {
			pSelf->fixTypeParserState = _GPSFixTypeParser_State_WaitingFor$;
		}
		return;
	case _GPSFixTypeParser_State_WaitingFor1stComma:
		if (dataByte == ((uint8_t)',')){
			pSelf->fixTypeParserState = _GPSFixTypeParser_State_WaitingForAutoManualFixSelectionSign;
		} else {
			pSelf->fixTypeParserState = _GPSFixTypeParser_State_WaitingFor$;
		}
		return;
	case _GPSFixTypeParser_State_WaitingForAutoManualFixSelectionSign:
		if ((dataByte == ((uint8_t)'A')) || (dataByte == ((uint8_t)'M'))){
			pSelf->fixTypeParserState = _GPSFixTypeParser_State_WaitingFor2ndComma;
		} else {
			pSelf->fixTypeParserState = _GPSFixTypeParser_State_WaitingFor$;
		}
		return;
	case _GPSFixTypeParser_State_WaitingFor2ndComma:
		if (dataByte == ((uint8_t)',')){
			pSelf->fixTypeParserState = _GPSFixTypeParser_State_WaitingForFixTypeSign;
		} else {
			pSelf->fixTypeParserState = _GPSFixTypeParser_State_WaitingFor$;
		}
		return;
	case _GPSFixTypeParser_State_WaitingForFixTypeSign:
		if (_GPSDriver_parseGPSFixType(&dataByte, 1, &(pSelf->fixType)) != GPSDriver_Status_OK){
			Warning_Handler("_GPSDriver_parseGPSFixType returned error");
		}
		pSelf->fixTypeParserState = _GPSFixTypeParser_State_WaitingFor$;
		return;
	default:
		pSelf->fixTypeParserState = _GPSFixTypeParser_State_WaitingFor$;
	}
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
#ifdef  USE_FULL_ASSERT
	Ublox8MGPSDriver_TypeDef self;
#endif
	assert_param(GPSDriver_testGNGGA1(&self));
	assert_param(GPSDriver_testGNGSA1(&self));
	assert_param(GPSDriver_testGNRMC1(&self));
}

#endif
