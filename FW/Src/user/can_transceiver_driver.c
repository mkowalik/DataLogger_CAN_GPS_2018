/*
 * can_transceiver_driver.c
 *
 *  Created on: 28.06.2018
 *      Author: Michal Kowalik
 */

#include "user/can_transceiver_driver.h"
#include "can.h"
#include "string.h"

#define	CAN_FRAMES_PER_FILTER_BANK	4
#define	CAN_BANKS_NUMBER			14 //TODO 14 czy 28? 14 bo przy uzyciu jednego CANA mozna tylko 14, ale moze odpalic 2-go CANa i go nie uzywac?
#define CAN_ID_MASK_bp				5

#define	CAN_SYNC_JUMP_WIDTH		CAN_SJW_3TQ


static CANTransceiverDriver_Status_TypeDef CANTransceiverDriver_HALCANInit(CAN_HandleTypeDef* pHcan, uint32_t prescalerValue, CAN_TypeDef* pCANInstance, uint32_t timeSeg1, uint32_t timeSeg2){

	pHcan->Instance = pCANInstance;
	pHcan->Init.Prescaler = prescalerValue;
	pHcan->Init.Mode = CAN_MODE_NORMAL;
	pHcan->Init.SyncJumpWidth = CAN_SYNC_JUMP_WIDTH;
	pHcan->Init.TimeSeg1 = timeSeg1;
	pHcan->Init.TimeSeg2 = timeSeg2;
	pHcan->Init.TimeTriggeredMode = DISABLE;
	pHcan->Init.AutoBusOff = DISABLE;
	pHcan->Init.AutoWakeUp = ENABLE;
	pHcan->Init.AutoRetransmission = DISABLE;
	pHcan->Init.ReceiveFifoLocked = DISABLE;
	pHcan->Init.TransmitFifoPriority = DISABLE;

	if (HAL_CAN_Init(pHcan) != HAL_OK)
	{
		return CANTransceiverDriver_Status_Error;
	}
	return CANTransceiverDriver_Status_OK;

}

CANTransceiverDriver_Status_TypeDef CANTransceiverDriver_init(CANTransceiverDriver_TypeDef* pSelf, Config_TypeDef* pConfig, CAN_HandleTypeDef* pHcan, CAN_TypeDef* pCANInstance){

	if (pHcan == NULL){
		return CANTransceiverDriver_Status_Error;
	}
	CANTransceiverDriver_Status_TypeDef ret = CANTransceiverDriver_Status_OK;

	pSelf->pHcan = pHcan;

	uint32_t prescalerValue;

	switch (pConfig->canSpeed){
	case 1000:
		prescalerValue = 6;
		break;
	case 500:
		prescalerValue = 12;
		break;
	case 250:
		prescalerValue = 24;
		break;
	case 125:
		prescalerValue = 48;
		break;
	case 50:
		prescalerValue = 120;
		break;
	default:
		return CANTransceiverDriver_Status_IncorrectCANBitrateValueError;
	}
	if ((ret = CANTransceiverDriver_HALCANInit(pSelf->pHcan, prescalerValue, pCANInstance, CAN_BS1_6TQ, CAN_BS2_1TQ)) != CANTransceiverDriver_Status_OK){//TODO pass proper prescaller
		return ret;
	}

	for (uint16_t i=0; i<CAN_MAX_CALLBACK_NUMBER; i++){
		pSelf->pRxCallbackFunctions[i] = NULL;
		pSelf->pRxCallbackArguemnts[i] = NULL;
	}

	CAN_FilterTypeDef filterConfig;

	filterConfig.FilterIdHigh 			= 0x00;
	filterConfig.FilterIdLow			= 0x00;
	filterConfig.FilterMaskIdHigh		= 0x00;
	filterConfig.FilterMaskIdLow		= 0x00;

	filterConfig.FilterFIFOAssignment	= CAN_FILTER_FIFO0;
	filterConfig.FilterBank				= 0;
	filterConfig.FilterMode 			= CAN_FILTERMODE_IDMASK;
	filterConfig.FilterScale 			= CAN_FILTERSCALE_32BIT;
	filterConfig.FilterActivation		= ENABLE;

	if (HAL_CAN_ConfigFilter(pSelf->pHcan, &filterConfig) != HAL_OK) {
		return CANTransceiverDriver_Status_Error;
	}

	return CANTransceiverDriver_Status_OK;

}

CANTransceiverDriver_Status_TypeDef CANTransceiverDriver_configFiltering(CANTransceiverDriver_TypeDef* pSelf, uint16_t* pFilters, uint16_t filtersNumber){

	//TODO zrobic obsluge wiekszej ilosci filtrow niz 14*4

	CAN_FilterTypeDef filterConfig;

	filterConfig.FilterMode 			= CAN_FILTERMODE_IDLIST;
	filterConfig.FilterScale 			= CAN_FILTERSCALE_16BIT;
	filterConfig.FilterActivation		= ENABLE;

	for(uint16_t i=0; i<filtersNumber; i+=CAN_FRAMES_PER_FILTER_BANK){

		if (i > (CAN_BANKS_NUMBER * CAN_FRAMES_PER_FILTER_BANK)){
			return CANTransceiverDriver_Status_TooManyFramesIDsError;
		}

		filterConfig.FilterFIFOAssignment	= (i%2 == 0) ? CAN_FILTER_FIFO0 : CAN_FILTER_FIFO1;
		filterConfig.FilterBank				= i % CAN_BANKS_NUMBER;

		filterConfig.FilterIdLow			= pFilters[i] << CAN_ID_MASK_bp;
		filterConfig.FilterIdHigh			= (i+1 < filtersNumber) ? (pFilters[i+1] << CAN_ID_MASK_bp) : pFilters[i];
		filterConfig.FilterMaskIdLow		= (i+2 < filtersNumber) ? (pFilters[i+2] << CAN_ID_MASK_bp) : pFilters[i];
		filterConfig.FilterMaskIdHigh		= (i+3 < filtersNumber) ? (pFilters[i+3] << CAN_ID_MASK_bp) : pFilters[i];

		if (HAL_CAN_ConfigFilter(pSelf->pHcan, &filterConfig) != HAL_OK) {
			return CANTransceiverDriver_Status_Error;
		}

	}

	return CANTransceiverDriver_Status_OK;

}

CANTransceiverDriver_Status_TypeDef CANTransceiverDriver_receivedMsgCallbackHandler(CANTransceiverDriver_TypeDef* pSelf, uint16_t ID, uint8_t DLC, uint8_t aData[8], uint16_t timestamp){

	CANData_TypeDef canData;
	canData.DLC = DLC;
	canData.ID = ID;
	memcpy(canData.Data, aData, 8);
	canData.msTime = timestamp; //TODO !!!!! to jest zle, ale niech bedzie na razie

	for (uint16_t i=0; i<CAN_MAX_CALLBACK_NUMBER; i++){

		if (pSelf->pRxCallbackFunctions[i] == NULL){
			break;
		}

		pSelf->pRxCallbackFunctions[i](&canData, pSelf->pRxCallbackArguemnts[i]);
	}

	return CANTransceiverDriver_Status_OK;

}

CANTransceiverDriver_Status_TypeDef CANTransceiverDriver_registerReceiveCallbackToCall(CANTransceiverDriver_TypeDef* pSelf, void (*pCallbackFunction) (CANData_TypeDef* pData, void* arg), void* pArgument){

	uint16_t i;

	for (i=0; i<CAN_MAX_CALLBACK_NUMBER; i++){
		if (pSelf->pRxCallbackFunctions[i] != NULL){
			continue;
		} else {
			pSelf->pRxCallbackFunctions[i] = pCallbackFunction;
			pSelf->pRxCallbackArguemnts[i] = pArgument;
			break;
		}
	}

	if (i >= CAN_MAX_CALLBACK_NUMBER){
		return CANTransceiverDriver_Status_Error;
	}

	return CANTransceiverDriver_Status_OK;

}

static void checkErrorcode(uint32_t errorcodeHAL, uint32_t errorMask, CANTransceiverDriver_ErrorCode_TypeDef* errorcodeTransceiverOut, CANTransceiverDriver_ErrorCode_TypeDef errorTracsceiverMaskOut){

	if ((errorcodeHAL & errorMask) != 0){
		*errorcodeTransceiverOut |= errorTracsceiverMaskOut;
	}
}

CANTransceiverDriver_Status_TypeDef CANTransceiverDriver_errorCallbackHandler(CANTransceiverDriver_TypeDef* pSelf){

	uint32_t errorcodeHAL = HAL_CAN_GetError(pSelf->pHcan);

	CANTransceiverDriver_ErrorCode_TypeDef	errorcodeTransceiverOut = CANTransceiverDriver_ErrorCode_None;

	checkErrorcode(errorcodeHAL, HAL_CAN_ERROR_EWG, &errorcodeTransceiverOut, CANTransceiverDriver_ErrorCode_ProtocolErrWarn);
	checkErrorcode(errorcodeHAL, HAL_CAN_ERROR_EPV, &errorcodeTransceiverOut, CANTransceiverDriver_ErrorCode_ErrPassive);
	checkErrorcode(errorcodeHAL, HAL_CAN_ERROR_BOF, &errorcodeTransceiverOut, CANTransceiverDriver_ErrorCode_BusOff);
	checkErrorcode(errorcodeHAL, HAL_CAN_ERROR_STF, &errorcodeTransceiverOut, CANTransceiverDriver_ErrorCode_BitStuffingError);
	checkErrorcode(errorcodeHAL, HAL_CAN_ERROR_FOR, &errorcodeTransceiverOut, CANTransceiverDriver_ErrorCode_FormError);
	checkErrorcode(errorcodeHAL, HAL_CAN_ERROR_ACK, &errorcodeTransceiverOut, CANTransceiverDriver_ErrorCode_ACKError);
	checkErrorcode(errorcodeHAL, HAL_CAN_ERROR_BR,  &errorcodeTransceiverOut, CANTransceiverDriver_ErrorCode_BitRecessiveError);
	checkErrorcode(errorcodeHAL, HAL_CAN_ERROR_BD,  &errorcodeTransceiverOut, CANTransceiverDriver_ErrorCode_BitDominantError);
	checkErrorcode(errorcodeHAL, HAL_CAN_ERROR_CRC, &errorcodeTransceiverOut, CANTransceiverDriver_ErrorCode_CRCError);
	checkErrorcode(errorcodeHAL,
			HAL_CAN_ERROR_RX_FOV0 |
			HAL_CAN_ERROR_RX_FOV1, &errorcodeTransceiverOut, CANTransceiverDriver_ErrorCode_RxFIFOError);
	checkErrorcode(errorcodeHAL,
			HAL_CAN_ERROR_TX_ALST0 |
			HAL_CAN_ERROR_TX_TERR0 |
			HAL_CAN_ERROR_TX_ALST1 |
			HAL_CAN_ERROR_TX_TERR1 |
			HAL_CAN_ERROR_TX_ALST2 |
			HAL_CAN_ERROR_TX_TERR2, &errorcodeTransceiverOut, CANTransceiverDriver_ErrorCode_TxMailboxError);
	checkErrorcode(errorcodeHAL, HAL_CAN_ERROR_TIMEOUT, &errorcodeTransceiverOut, CANTransceiverDriver_ErrorCode_TimeoutError);
	checkErrorcode(errorcodeHAL,
			HAL_CAN_ERROR_NOT_INITIALIZED |
			HAL_CAN_ERROR_NOT_READY |
			HAL_CAN_ERROR_NOT_STARTED |
			HAL_CAN_ERROR_PARAM, &errorcodeTransceiverOut, CANTransceiverDriver_ErrorCode_OtherError);

	for (uint16_t i=0; i<CAN_MAX_CALLBACK_NUMBER; i++){

		if (pSelf->pErrorCallbackFunctions[i] == NULL){
			break;
		}

		pSelf->pErrorCallbackFunctions[i](errorcodeTransceiverOut, pSelf->pErrorCallbackArguemnts[i]);
	}

	if (HAL_CAN_ResetError(pSelf->pHcan) != HAL_OK){
		Error_Handler();
	}

	return CANTransceiverDriver_Status_OK;

}

CANTransceiverDriver_Status_TypeDef CANTransceiverDriver_registerErrorCallbackToCall(CANTransceiverDriver_TypeDef* pSelf, void (*pCallbackFunction) (uint16_t errorcode, void* arg), void* pArgument){

	uint16_t i;

	for (i=0; i<CAN_MAX_CALLBACK_NUMBER; i++){
		if (pSelf->pErrorCallbackFunctions[i] != NULL){
			continue;
		} else {
			pSelf->pErrorCallbackFunctions[i] = pCallbackFunction;
			pSelf->pErrorCallbackArguemnts[i] = pArgument;
			break;
		}
	}

	if (i >= CAN_MAX_CALLBACK_NUMBER){
		return CANTransceiverDriver_Status_Error;
	}

	return CANTransceiverDriver_Status_OK;

}

CANTransceiverDriver_Status_TypeDef CANTransceiverDriver_start(CANTransceiverDriver_TypeDef* pSelf){

	if (HAL_CAN_ActivateNotification(pSelf->pHcan,	CAN_IT_RX_FIFO0_MSG_PENDING |
													CAN_IT_RX_FIFO1_MSG_PENDING |
													CAN_IT_ERROR_WARNING |
													CAN_IT_ERROR_PASSIVE |
													CAN_IT_BUSOFF |
													CAN_IT_LAST_ERROR_CODE |
													CAN_IT_ERROR) != HAL_OK){
		return CANTransceiverDriver_Status_StartError;
	}

	if (HAL_CAN_Start(pSelf->pHcan) != HAL_OK){
		return CANTransceiverDriver_Status_StartError;
	}

	return CANTransceiverDriver_Status_OK;

}

CANTransceiverDriver_Status_TypeDef CANTransceiverDriver_transmitMsg(CANTransceiverDriver_TypeDef* pSelf, CANData_TypeDef* pData){
	//TODO
	UNUSED(pSelf);
	UNUSED(pData);

	return CANTransceiverDriver_Status_Error;
}

/****************************** Implementations of stm32f7xx_hal_can.h __weak functions placeholders ******************************/

extern CANTransceiverDriver_TypeDef canTransceiverDriver;

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan){

	if (hcan != &hcan1){
		Error_Handler();
	}

	CAN_RxHeaderTypeDef	header;
	uint8_t				data[8];

	if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &header, data) != HAL_OK){
		Error_Handler();
	}

	if (CANTransceiverDriver_receivedMsgCallbackHandler(&canTransceiverDriver, header.StdId, header.DLC, data, header.Timestamp) != CANTransceiverDriver_Status_OK){
		Error_Handler();
	}

}

void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan){

	if (hcan != &hcan1){
		Error_Handler();
	}

	CAN_RxHeaderTypeDef	header;
	uint8_t				data[8];

	if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO1, &header, data) != HAL_OK){
		Error_Handler();
	}

	if (CANTransceiverDriver_receivedMsgCallbackHandler(&canTransceiverDriver, header.StdId, header.DLC, data, header.Timestamp) != CANTransceiverDriver_Status_OK){
		Error_Handler();
	}

}

void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan){

	if (hcan != &hcan1){
		Error_Handler();
	}

	if (CANTransceiverDriver_errorCallbackHandler(&canTransceiverDriver) != CANTransceiverDriver_Status_OK){
		Error_Handler();
	}

}
