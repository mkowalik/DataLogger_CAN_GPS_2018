/*
 * string_operations.h
 *
 *  Created on: 14.07.2019
 *      Author: Michal Kowalik
 */

#ifndef USER_STRING_OPERATIONS_H_
#define USER_STRING_OPERATIONS_H_

#include <stdint.h>
#include <stdbool.h>
#include "user/fixed_point.h"

typedef enum {
	StringOperations_Status_OK = 0,
	StringOperations_Status_NotEqual,
	StringOperations_Status_NotDecimalCharError,
	StringOperations_Status_CharNotFoundError,
	StringOperations_Status_NullPointerError,
	StringOperations_Status_Error
} StringOperations_Status_TypeDef;


StringOperations_Status_TypeDef	findChar(uint8_t* buffer, uint8_t charToFind, uint16_t bufferSize, uint16_t* pRetIndex);
StringOperations_Status_TypeDef	stringEqual(uint8_t* stringA, uint8_t* stringB, uint16_t length);
StringOperations_Status_TypeDef	stringToFixedPoint(uint8_t* sentence, uint16_t length, uint8_t decimalSeparator, uint8_t fractionalBits, FixedPoint* pRetFixedPoint);
StringOperations_Status_TypeDef	decChar2Uint8(uint8_t c, uint8_t* pRetInt);
StringOperations_Status_TypeDef	string2UInt32(uint8_t* sentence, uint8_t length, uint32_t* pRetInt);
StringOperations_Status_TypeDef	hexChar2Uint8(uint8_t c, uint8_t* pRetInt);
StringOperations_Status_TypeDef	uInt8ToString(uint8_t val, uint8_t* pRetString);
bool							isDecimalChar(uint8_t c);

#endif /* USER_STRING_OPERATIONS_H_ */
