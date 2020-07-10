/*
 * can_data.c
 *
 *  Created on: 02.01.2020
 *      Author: kowal
 */

#include "user/can_data.h"
#include "user/utils.h"

CANData_Status_TypeDef CANData_GetValueRawOfSignal(const CANData_TypeDef* pData, const ConfigSignal_TypeDef* pSignal, uint32_t* pRetSignalRawVal){

	if (pData == NULL || pSignal == NULL || pRetSignalRawVal == NULL){
		return CANData_Status_NullPointerError;
	}

	if (pSignal->pParentFrame->ID != pData->ID){
		return CANData_Status_SignalNotInGivenFrameError;
	}

	if ((pData->DLC * 8) < (pSignal->startBit + pSignal->lengthBits)){
		return CANData_Status_DataTooShortError;
	}

	uint8_t bitsLeft	= pSignal->lengthBits;
	uint8_t bitsShiftRaw= pSignal->startBit % 8;
	uint8_t bitIt		= pSignal->startBit;
	*pRetSignalRawVal		= 0;

	if (pSignal->valueType_BigEndian != 0){ //< Big Endian
		while (bitsLeft > 0) {
			uint8_t actualByte	= pData->Data[(bitIt/8)];
			uint8_t nextByte	= 0;
			if ((bitIt/8)+1 < pData->DLC) {
				nextByte = pData->Data[(bitIt/8)+1];
			}
			actualByte <<= bitsShiftRaw;
			actualByte |= (nextByte >> (8u - bitsShiftRaw));

			if (bitsLeft % 8 != 0){
				actualByte >>= (8u - (bitsLeft % 8));
				actualByte &= (0xFF >> (8u - (bitsLeft % 8)));
			}
			*pRetSignalRawVal |= (((uint32_t)actualByte) << (bitsLeft - (((bitsLeft % 8u) == 0) ? 8u : (bitsLeft % 8u))) );
			bitIt += ((bitsLeft % 8u) == 0) ? 8u : (bitsLeft % 8u);
			bitsShiftRaw = (bitsShiftRaw + bitsLeft) % 8u;
			bitsLeft -= ((bitsLeft % 8u) == 0) ? 8u : (bitsLeft % 8u);
		}
	} else { //< Little Endian
		while (bitsLeft > 0) {
			uint8_t actualByte	= pData->Data[(bitIt/8)];
			uint8_t nextByte	= 0;
			if ((bitIt/8)+1 < pData->DLC) {
				nextByte = pData->Data[(bitIt/8)+1];
			}
			actualByte <<= bitsShiftRaw;
			actualByte |= (nextByte >> (8u - bitsShiftRaw));

			if (bitsLeft < 8){
				actualByte >>= (8u - bitsLeft);
				actualByte &= (0xFF >> (8u - bitsLeft));
			}
			*pRetSignalRawVal |= (((uint32_t)actualByte) << (((bitIt - pSignal->startBit)/8) * 8u));
			bitIt += MIN(8U, bitsLeft);
			bitsLeft -= MIN(8U, bitsLeft);
		}
	}

	return CANData_Status_OK;
}

#ifdef  USE_FULL_ASSERT

#include <stdbool.h>
#include <string.h>

void CANData_UnitTests_GetValueRawOfSignal();
bool CANData_GetValueRawOfSignal_ParametricTestCase(uint8_t,uint8_t,bool,uint32_t);

void CANData_UnitTests(){
	CANData_UnitTests_GetValueRawOfSignal();
}

void CANData_UnitTests_GetValueRawOfSignal(){

	//< ----- Little Endian ----- >//
	assert_param(CANData_GetValueRawOfSignal_ParametricTestCase(4, 8, false, 0x51u));
	assert_param(CANData_GetValueRawOfSignal_ParametricTestCase(4, 4, false, 0x5u));
	assert_param(CANData_GetValueRawOfSignal_ParametricTestCase(4, 16, false, 0x6A51u));
	assert_param(CANData_GetValueRawOfSignal_ParametricTestCase(4, 12, false, 0x651u));
	assert_param(CANData_GetValueRawOfSignal_ParametricTestCase(5, 10, false, 0b1110100010u));
	assert_param(CANData_GetValueRawOfSignal_ParametricTestCase(0, 32, false, 0x3AA31615u));
	assert_param(CANData_GetValueRawOfSignal_ParametricTestCase(2, 2, false, 0b01u));
	assert_param(CANData_GetValueRawOfSignal_ParametricTestCase(7, 2, false, 0b10u));

	//< ----- Big Endian ----- >//
	assert_param(CANData_GetValueRawOfSignal_ParametricTestCase(4, 8, true, 0x51u));
	assert_param(CANData_GetValueRawOfSignal_ParametricTestCase(4, 16, true, 0x516Au));
	assert_param(CANData_GetValueRawOfSignal_ParametricTestCase(4, 12, true, 0x516u));
	assert_param(CANData_GetValueRawOfSignal_ParametricTestCase(5, 10, true, 0b1010001011u));
	assert_param(CANData_GetValueRawOfSignal_ParametricTestCase(0, 32, true, 0x1516A33Au));
	assert_param(CANData_GetValueRawOfSignal_ParametricTestCase(2, 2, true, 0b01u));
	assert_param(CANData_GetValueRawOfSignal_ParametricTestCase(7, 2, true, 0b10u));

}

void CANData_Test_PrepareFrameSignalData(
		ConfigFrame_TypeDef* pFrame,
		ConfigSignal_TypeDef* pSignal,
		CANData_TypeDef* pData,
		uint8_t startBit,
		uint8_t lengthBits,
		bool isBigEndian)
{

#define	FRAME_ID	(0x100)
#define	IS_SIGNED	false
#define	FRAME_NAME	"testFrame"

    const uint8_t rawData[] = {0x15, 0x16, 0xA3, 0x3A};

	pFrame->ID 						= FRAME_ID;
	pFrame->expectedDLC				= sizeof(rawData);
	strcpy(pFrame->frameName, FRAME_NAME);

	pSignal->pParentFrame			= pFrame;
	pSignal->signalID				= 0;
	pSignal->startBit				= startBit;
	pSignal->lengthBits				= lengthBits;
	pSignal->valueType_BigEndian	= isBigEndian ? 1 : 0;
	pSignal->valueType_Signed		= IS_SIGNED ? 1 : 0;

	pData->ID						= FRAME_ID;
	pData->DLC						= sizeof(rawData);
	memcpy(pData->Data, rawData, sizeof(rawData));
}

bool CANData_GetValueRawOfSignal_ParametricTestCase(
		uint8_t		startBit,
		uint8_t		lengthBits,
		bool		isBigEndian,
		uint32_t	expectedResult )
{
	CANData_TypeDef			data	= {0};
	ConfigFrame_TypeDef		frame	= {0};
	ConfigSignal_TypeDef	signal	= {0};

	uint32_t	retRawValue		= 0;
	CANData_Test_PrepareFrameSignalData(&frame, &signal, &data, startBit, lengthBits, isBigEndian);
	if (CANData_GetValueRawOfSignal(&data, &signal, &retRawValue) != CANData_Status_OK){
		return false;
	}
	assert_param(retRawValue = expectedResult);
	return true;
}

#endif //USE_FULL_ASSERT
