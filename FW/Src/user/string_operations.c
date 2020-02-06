/*
 * string_operations.c
 *
 *  Created on: 14.07.2019
 *      Author: Michal Kowalik
 */

#include <stddef.h>
#include <stdio.h>
#include "user/string_operations.h"
#include <string.h>

StringOperations_Status_TypeDef findChar(uint8_t* buffer, uint8_t charToFind, uint16_t bufferSize, uint16_t* pRetIndex){

	if (buffer == NULL || pRetIndex == NULL){
		return StringOperations_Status_OK;
	}

	for (uint16_t i=0; i<bufferSize; i++){

		if (buffer[i] == charToFind){
			*pRetIndex = i;
			return StringOperations_Status_OK;
		}
	}

	*pRetIndex = bufferSize;
	return StringOperations_Status_CharNotFoundError;
}

StringOperations_Status_TypeDef stringEqual(uint8_t* stringA, uint8_t* stringB, uint16_t length){

	if (stringA == NULL || stringB == NULL){
		return StringOperations_Status_OK;
	}

	for (uint16_t i=0; i<length; i++){
		if (stringA[i] != stringB[i]){
			return StringOperations_Status_NotEqual;
		}
	}

	return StringOperations_Status_OK;
}

StringOperations_Status_TypeDef stringToFixedPoint(uint8_t* sentence, uint16_t length, uint8_t decimalSeparator, uint8_t fractionalBits, FixedPoint* pRetFixedPoint){

	if (sentence == NULL || pRetFixedPoint == NULL){
		return StringOperations_Status_NullPointerError;
	}

	StringOperations_Status_TypeDef ret;
	uint16_t dotPosition;

	if ((ret = findChar(sentence, decimalSeparator, length, &dotPosition)) != StringOperations_Status_OK){
		return ret;
	}

	if (dotPosition == length){
		return StringOperations_Status_Error;
	}

	uint32_t	decimalPart;
	uint32_t	fractionalNumerator;
	uint32_t	fractionalDenominator = 1;

	if ((ret = string2UInt32(sentence, dotPosition, &decimalPart)) != StringOperations_Status_OK){
		return ret;
	}
	if ((ret = string2UInt32(sentence + dotPosition + sizeof(decimalSeparator), length - dotPosition - sizeof(decimalSeparator), &fractionalNumerator)) != StringOperations_Status_OK){
		return ret;
	}

	for (uint32_t i=0; i<length - dotPosition - sizeof(decimalSeparator); i++){
		fractionalDenominator *= 10;
	}

	*pRetFixedPoint = FixedPoint_constrDecimalFrac(decimalPart, fractionalNumerator, fractionalDenominator, fractionalBits);

	return StringOperations_Status_OK;
}

StringOperations_Status_TypeDef decChar2Uint8(uint8_t c, uint8_t* pRetInt){

	if (pRetInt == NULL){
		return StringOperations_Status_NullPointerError;
	}
	if (c >= '0' && c <= '9'){
		*pRetInt = c - '0';
		return StringOperations_Status_OK;
	}
	return StringOperations_Status_NotDecimalCharError;
}


StringOperations_Status_TypeDef string2UInt32(uint8_t* sentence, uint16_t length, uint32_t* pRetInt){

	if (sentence == NULL || pRetInt == NULL){
		return StringOperations_Status_NullPointerError;
	}

	StringOperations_Status_TypeDef ret;
	*pRetInt = 0;
	uint32_t multiplier = 1;

	for (int16_t i = length-1; i >= 0; i--){
		uint8_t tmp;
		if ((ret = decChar2Uint8(*(sentence + i), &tmp)) != StringOperations_Status_OK){
			return ret;
		}
		*pRetInt	+= (tmp * multiplier);
		multiplier	*= 10;
	}

	return StringOperations_Status_OK;
}


StringOperations_Status_TypeDef hexChar2Uint8(uint8_t c, uint8_t* pRetInt){

	if (pRetInt == NULL){
		return StringOperations_Status_NullPointerError;
	}

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

StringOperations_Status_TypeDef	uInt8ToString(uint8_t* pRetBuffer, uint8_t val){

	if (pRetBuffer == NULL){
		return StringOperations_Status_NullPointerError;
	}

	sprintf((char*)pRetBuffer, "%u", val);
	return StringOperations_Status_OK;
}

StringOperations_Status_TypeDef appendUInt8ToString(uint8_t* pRetBuffer, uint8_t val, uint16_t bufferSize){
	if (pRetBuffer == NULL){
		return StringOperations_Status_NullPointerError;
	}
	if ((size_t)(sprintf((char*)(pRetBuffer + strlen((char*)pRetBuffer)), "%u", val)) > bufferSize - strlen((char*)pRetBuffer)){
		return StringOperations_Status_BufferOverflowError;
	}
	return StringOperations_Status_OK;
}

StringOperations_Status_TypeDef uInt8ToHexString(uint8_t* pRetBuffer, uint8_t val, bool upperCase){
	if (pRetBuffer == NULL){
		return StringOperations_Status_NullPointerError;
	}
	sprintf((char*)pRetBuffer, (upperCase == true) ? ("%X") : ("%x"), val);
	return StringOperations_Status_OK;
}

StringOperations_Status_TypeDef uInt8ToHexStringMinDigits(uint8_t* pRetBuffer, uint8_t val, bool upperCase, uint8_t minDigitsNumber){
	if (pRetBuffer == NULL){
		return StringOperations_Status_NullPointerError;
	}
	char formatStr[20];
	memset(formatStr, 0, 20);
	sprintf (formatStr, (upperCase == true) ? ("%%0%uX") : ("%%0%ux"), ((unsigned int)minDigitsNumber)); //< prepare format string

	sprintf ((char*)pRetBuffer, formatStr, val);

	return StringOperations_Status_OK;
}

StringOperations_Status_TypeDef appendUint8ToHexString(uint8_t* pRetBuffer, uint8_t val, uint16_t bufferSize, bool upperCase){
	if (pRetBuffer == NULL){
		return StringOperations_Status_NullPointerError;
	}

	if ((size_t)(sprintf((char*)(pRetBuffer + strlen((char*)pRetBuffer)), (upperCase == true) ? ("%X") : ("%x"), val)) > bufferSize - strlen((char*)pRetBuffer)){
		return StringOperations_Status_BufferOverflowError;
	}
	return StringOperations_Status_OK;

}

StringOperations_Status_TypeDef	uInt32ToString(uint32_t val, uint8_t* pRetBuffer){
	if (pRetBuffer == NULL){
		return StringOperations_Status_NullPointerError;
	}

	sprintf((char*)pRetBuffer, "%lu", val);
	return StringOperations_Status_OK;
}

StringOperations_Status_TypeDef appendUInt32ToString(uint8_t* pRetBuffer, uint32_t val, uint16_t bufferSize){
	if (pRetBuffer == NULL){
		return StringOperations_Status_NullPointerError;
	}

	if ((size_t)(sprintf((char*)(pRetBuffer + strlen((char*)pRetBuffer)), "%lu", val)) > bufferSize - strlen((char*)pRetBuffer)){
		return StringOperations_Status_BufferOverflowError;
	}
	return StringOperations_Status_OK;
}

bool isDecimalChar(uint8_t c){
	if (c >= '0' && c <= '9'){
		return true;
	}
	return false;
}

uint8_t* strCharCat(uint8_t* str, uint8_t c){
	uint8_t str2[2] = {c, 0};
	return (uint8_t*)strcat((char*)str, (char*)str2);
}
