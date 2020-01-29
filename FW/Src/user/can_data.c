/*
 * can_data.c
 *
 *  Created on: 02.01.2020
 *      Author: kowal
 */

#include "user/can_data.h"
#include "user/utils.h"

CANData_Status_TypeDef CANData_GetValueRawOfSignal(CANData_TypeDef* pData, ConfigSignal_TypeDef* pSignal, uint32_t* pRetSignalRawVal){

	if (pData == NULL || pSignal == NULL || pRetSignalRawVal == NULL){
		return CANData_Status_NullPointerError;
	}

	if (pSignal->pFrame->ID != pData->ID){
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
			*pRetSignalRawVal |= (((uint32_t)actualByte) << (((bitIt - pSignal->startBit)/8) * 8u)); //TODO Tu raczej blad
			bitIt += MIN(8U, bitsLeft);
			bitsLeft -= MIN(8U, bitsLeft);
		}
	}

	return CANData_Status_Error;
}
