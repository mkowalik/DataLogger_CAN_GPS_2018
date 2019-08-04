/*
 * string_operations.c
 *
 *  Created on: 14.07.2019
 *      Author: Michal Kowalik
 */

#include <stddef.h>
#include "user/string_operations.h"

StringOperations_Status_TypeDef findChar(uint8_t* buffer, uint8_t charToFind, uint16_t bufferSize, uint16_t* pRetIndex){

	if (buffer == NULL){
		return StringOperations_Status_OK;
	}

	for (uint16_t i=0; i<bufferSize; i++){

		if (buffer[i] == charToFind){
			*pRetIndex = i;
			return StringOperations_Status_OK;
		}
	}

	*pRetIndex = bufferSize;
	return StringOperations_Status_CharNotFound;
}

StringOperations_Status_TypeDef stringEqual(uint8_t* stringA, uint8_t* stringB, uint16_t length){

	for (uint16_t i=0; i<length; i++){
		if (stringA[i] != stringB[i]){
			return StringOperations_Status_NotEqual;
		}
	}

	return StringOperations_Status_OK;

}

StringOperations_Status_TypeDef stringToFixedPoint(uint8_t* sentence, uint16_t length, uint8_t decimalSeparator, uint8_t fractionalBits, FixedPoint* pRetFixedPoint){

	StringOperations_Status_TypeDef ret;
	uint16_t dotPosition;

	if ((ret = findChar(sentence, decimalSeparator, length, &dotPosition)) != StringOperations_Status_OK){
		return ret;
	}

	if (dotPosition == length){
		return StringOperations_Status_Error;
	}

//TODO

	return StringOperations_Status_Error;
}

StringOperations_Status_TypeDef decChar2Uint8(uint8_t c, uint8_t* pRetInt){
	if (c >= '0' && c <= '9'){
		*pRetInt = c - '0';
		return StringOperations_Status_OK;
	}
	return StringOperations_Status_NotDecimalCharError;
}


StringOperations_Status_TypeDef string2UInt32(uint8_t* sentence, uint8_t length, uint32_t* pRetInt){

	StringOperations_Status_TypeDef ret;
	*pRetInt = 0;
	uint32_t multiplier = 1;

	for (int16_t i = length-1; i >= 0; i--){
		uint8_t tmp;
		if ((ret = decChar2Uint8(*(sentence + i) * multiplier, &tmp)) != StringOperations_Status_OK){
			return ret;
		}
		*pRetInt	+= tmp;
		multiplier	*= 10;
	}

	return StringOperations_Status_OK;
}


StringOperations_Status_TypeDef hexChar2Uint8(uint8_t c, uint8_t* pRetInt){
    if (c >= '0' && c <= '9'){
    	*pRetInt = c - '0';
    	return StringOperations_Status_OK;
    } if (c >= 'A' && c <= 'F'){
    	*pRetInt = c - 'A' + 10;
    	return StringOperations_Status_OK;
    } if (c >= 'a' && c <= 'f'){
    	*pRetInt = c - 'a' + 10;
    	return StringOperations_Status_OK;
    }
	return StringOperations_Status_NotDecimalCharError;
}

StringOperations_Status_TypeDef	uInt8ToString(uint8_t val, uint8_t* pRetString){
 //TODO
}

bool isDecimalChar(uint8_t c){
	if (c >= '0' && c <= '9'){
		return true;
	}
	return false;
}
