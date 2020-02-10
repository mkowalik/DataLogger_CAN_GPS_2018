/*
 * action_scheduler.c
 *
 *  Created on: 12.11.2017
 *      Author: Michal Kowalik
 */

#include <string.h>

#include "user/action_scheduler.h"
#include "stdio.h"

//< ----- Private functions prototypes ----- >//
static ActionScheduler_Status_TypeDef _ActionScheduler_SetDateAndTimeFrameHandler(ActionScheduler_TypeDef* pSelf, const CANData_TypeDef* pData);
static ActionScheduler_Status_TypeDef _ActionScheduler_updateRecordsUsingDataFromGPS(ActionScheduler_TypeDef* pSelf, const GPSData_TypeDef* pGPSData);

static ActionScheduler_Status_TypeDef _ActionScheduler_idleState(ActionScheduler_TypeDef* pSelf);
static ActionScheduler_Status_TypeDef _ActionScheduler_logInitState(ActionScheduler_TypeDef* pSelf);
static ActionScheduler_Status_TypeDef _ActionScheduler_loggingState(ActionScheduler_TypeDef* pSelf);
static ActionScheduler_Status_TypeDef _ActionScheduler_logCloseState(ActionScheduler_TypeDef* pSelf);

static ActionScheduler_Status_TypeDef _AcionScheduler_StartLogTrigger(ActionScheduler_TypeDef* pSelf, CANData_TypeDef* pData);
static ActionScheduler_Status_TypeDef _AcionScheduler_StopLogTrigger(ActionScheduler_TypeDef* pSelf, CANData_TypeDef* pData);

static ActionScheduler_Status_TypeDef _ActionScheduler_checkFramesTimeout(ActionScheduler_TypeDef* pSelf, const Config_Trigger_TypeDef* aTriggers, uint32_t* aTriggersTimeouts, uint8_t triggersNumber);

static ActionScheduler_Status_TypeDef _getCompareOperatorFunction(const Config_TrigerCompareOperator_TypeDef compareOperator, CompareOperatorFunction* pRetOperatorFunction);
static ActionScheduler_Status_TypeDef _getFrameTimeoutValue(ActionScheduler_TypeDef* pSelf, const Config_Trigger_TypeDef* pTrigger, uint32_t* pRetFrameTimeout);

static bool _ActionScheduler_compareOperator_EQUAL(ActionScheduler_TypeDef* pSelf, const CANData_TypeDef* pCANData, const Config_Trigger_TypeDef* pTrigger, uint32_t* pTriggerTimeout);
static bool _ActionScheduler_compareOperator_NOT_EQUAL(ActionScheduler_TypeDef* pSelf, const CANData_TypeDef* pCANData, const Config_Trigger_TypeDef* pTrigger, uint32_t* pTriggerTimeout);
static bool _ActionScheduler_compareOperator_GREATER(ActionScheduler_TypeDef* pSelf, const CANData_TypeDef* pCANData, const Config_Trigger_TypeDef* pTrigger, uint32_t* pTriggerTimeout);
static bool _ActionScheduler_compareOperator_GREATER_OR_EQUAL(ActionScheduler_TypeDef* pSelf, const CANData_TypeDef* pCANData, const Config_Trigger_TypeDef* pTrigger, uint32_t* pTriggerTimeout);
static bool _ActionScheduler_compareOperator_LESS(ActionScheduler_TypeDef* pSelf, const CANData_TypeDef* pCANData, const Config_Trigger_TypeDef* pTrigger, uint32_t* pTriggerTimeout);
static bool _ActionScheduler_compareOperator_LESS_OR_EQUAL(ActionScheduler_TypeDef* pSelf, const CANData_TypeDef* pCANData, const Config_Trigger_TypeDef* pTrigger, uint32_t* pTriggerTimeout);
static bool _ActionScheduler_compareOperator_BITWISE_AND(ActionScheduler_TypeDef* pSelf, const CANData_TypeDef* pCANData, const Config_Trigger_TypeDef* pTrigger, uint32_t* pTriggerTimeout);
static bool _ActionScheduler_compareOperator_BITWISE_OR(ActionScheduler_TypeDef* pSelf, const CANData_TypeDef* pCANData, const Config_Trigger_TypeDef* pTrigger, uint32_t* pTriggerTimeout);
static bool _ActionScheduler_compareOperator_BITWISE_XOR(ActionScheduler_TypeDef* pSelf, const CANData_TypeDef* pCANData, const Config_Trigger_TypeDef* pTrigger, uint32_t* pTriggerTimeout);
static bool _ActionScheduler_compareOperator_FRAME_OCCURED(ActionScheduler_TypeDef* pSelf, const CANData_TypeDef* pCANData, const Config_Trigger_TypeDef* pTrigger, uint32_t* pTriggerTimeout);
static bool _ActionScheduler_compareOperator_FRAME_TIMEOUT_MS(ActionScheduler_TypeDef* pSelf, const CANData_TypeDef* pCANData, const Config_Trigger_TypeDef* pTrigger, uint32_t* pTriggerTimeout);

//< ----- Public functions ----- >//

ActionScheduler_Status_TypeDef ActionScheduler_init(ActionScheduler_TypeDef* pSelf, ConfigDataManager_TypeDef* pConfigManager, DataSaver_TypeDef* pDataSaver, volatile CANReceiver_TypeDef* pCANReceiver,
		GPSDriver_TypeDef* pGPSDriver, volatile MSTimerDriver_TypeDef* pMSTimerDriver, RTCDriver_TypeDef* pRTCDriver, LedDriver_TypeDef* pStatusLedDriver, LedDriver_TypeDef* pGPSLedDriver) {

	if (pSelf == NULL || pConfigManager == NULL || pDataSaver == NULL || pCANReceiver == NULL || pGPSDriver == NULL || pMSTimerDriver == NULL || pRTCDriver == NULL || pStatusLedDriver == NULL || pGPSLedDriver == NULL){
		return ActionScheduler_Status_NullPointerError;
	}

	if (pSelf->state != ActionScheduler_State_UnInitialized){
		return ActionScheduler_Status_Error;
	}

	ConfigDataManager_getConfigPointer(pConfigManager, &pSelf->pConfig);
	pSelf->pDataSaver				= pDataSaver;
	pSelf->pCANReceiver				= pCANReceiver;
	pSelf->pGPSDriver				= pGPSDriver;
	pSelf->pMSTimerDriver			= pMSTimerDriver;
	pSelf->pRTCDriver				= pRTCDriver;
	pSelf->pStatusLedDriver			= pStatusLedDriver;
	pSelf->pGPSLedDriver			= pGPSLedDriver;
	pSelf->lastGpsFixType			= GPSFixType_Unknown;
	pSelf->dateAndTimeUpdated		= false;

	for (uint8_t i=0; i<CONFIG_MAX_START_LOG_TRIGGER_NUMBER; i++){
		pSelf->startTriggersCompareOperatorFunctions[i]	= NULL;
		pSelf->startTriggerFrameTimeout[i]				= 0;
	}
	for (uint8_t i=0; i<CONFIG_MAX_STOP_LOG_TRIGGER_NUMBER; i++){
		pSelf->stopTriggersCompareOperatorFunctions[i]	= NULL;
		pSelf->stopTriggerFrameTimeout[i]				= 0;
	}

	if (LedDriver_BlinkingLed(pSelf->pStatusLedDriver, ACTION_SCHEDULER_IDLE_LED_ON_TIME, ACTION_SCHEDULER_IDLE_LED_OFF_TIME) != LedDriver_Status_OK){
		return ActionScheduler_Status_Error;
	}
	if (LedDriver_OffLed(pSelf->pGPSLedDriver) != LedDriver_Status_OK){
		return ActionScheduler_Status_Error;
	}
	pSelf->state = ActionScheduler_State_Idle;

	return ActionScheduler_Status_OK;
}

ActionScheduler_Status_TypeDef ActionScheduler_startScheduler(ActionScheduler_TypeDef* pSelf){

	if (pSelf == NULL){
		return ActionScheduler_Status_NullPointerError;
	}
	if (pSelf->state == ActionScheduler_State_UnInitialized){
		return ActionScheduler_Status_UnInitializedError;
	}
	ActionScheduler_Status_TypeDef ret = ActionScheduler_Status_OK;

	for (uint8_t i=0; i<pSelf->pConfig->startLogTriggersNumber; i++){
		if ((ret = _getCompareOperatorFunction(pSelf->pConfig->startLogTriggers[i].compareOperator, &(pSelf->startTriggersCompareOperatorFunctions[i]))) != ActionScheduler_Status_OK){
			return ret;
		}
		if ((ret = _getFrameTimeoutValue(pSelf, &(pSelf->pConfig->startLogTriggers[i]), &(pSelf->startTriggerFrameTimeout[i]))) != ActionScheduler_Status_OK){
			return ret;
		}
	}
	for (uint8_t i=0; i<pSelf->pConfig->stopLogTriggersNumber; i++){
		if ((ret = _getCompareOperatorFunction(pSelf->pConfig->stopLogTriggers[i].compareOperator, &(pSelf->stopTriggersCompareOperatorFunctions[i]))) != ActionScheduler_Status_OK){
			return ret;
		}
		if ((ret = _getFrameTimeoutValue(pSelf, &(pSelf->pConfig->stopLogTriggers[i]), &(pSelf->stopTriggerFrameTimeout[i]))) != ActionScheduler_Status_OK){
			return ret;
		}
	}

	if (CANReceiver_start(pSelf->pCANReceiver) != CANReceiver_Status_OK){
		return ActionScheduler_Status_CANReceiverError;
	}
	if (GPSDriver_startReceiver(pSelf->pGPSDriver) != GPSDriver_Status_OK){
		return ActionScheduler_Status_GPSDriverError;
	}

	pSelf->state = ActionScheduler_State_Idle;

	return ActionScheduler_Status_OK;
}

ActionScheduler_Status_TypeDef ActionScheduler_thread(ActionScheduler_TypeDef* pSelf){

	if (pSelf == NULL){
		return ActionScheduler_Status_NullPointerError;
	}

	ActionScheduler_Status_TypeDef ret = ActionScheduler_Status_OK;
	switch (pSelf->state){
	case ActionScheduler_State_UnInitialized:
	default:
		return ActionScheduler_Status_UnInitializedError;
		break;
	case ActionScheduler_State_Idle:
		if ((ret = _ActionScheduler_idleState(pSelf)) != ActionScheduler_Status_OK) {
			return ret;
		};
		break;
	case ActionScheduler_State_LogInit:
		if ((ret = _ActionScheduler_logInitState(pSelf)) != ActionScheduler_Status_OK) {
			return ret;
		};
		break;
	case ActionScheduler_State_Logging:
		if ((ret = _ActionScheduler_loggingState(pSelf)) != ActionScheduler_Status_OK) {
			return ret;
		};
		break;
	case ActionScheduler_State_LogClose:
		if ((ret = _ActionScheduler_logCloseState(pSelf)) != ActionScheduler_Status_OK) {
			return ret;
		};
		break;
	}

	return ActionScheduler_Status_OK;
}

//< ----- Private functions ----- >//

static ActionScheduler_Status_TypeDef _ActionScheduler_SetDateAndTimeFrameHandler(ActionScheduler_TypeDef* pSelf, const CANData_TypeDef* pData){

	if ((pData->ID == pSelf->pConfig->rtcConfigurationFrameID) && (pData->DLC == ACTION_SCHEDULER_RTC_SETUP_FRAME_DLC)){

		DateTime_TypeDef dateAndTime = {0};
		dateAndTime.year 	= (pData->Data[0] | ((pData->Data[1]) << 8));
		dateAndTime.month	= pData->Data[2];
		dateAndTime.day		= pData->Data[3];
		dateAndTime.hour	= pData->Data[4];
		dateAndTime.minute	= pData->Data[5];
		dateAndTime.second	= pData->Data[6];

		RTCDriver_Status_TypeDef ret = RTCDriver_setDateAndTime(pSelf->pRTCDriver, dateAndTime);

		if ((ret == RTCDriver_Status_WrongDateFormatError) || (ret == RTCDriver_Status_WrongTimeFormatError)){
			//TODO log wrong format warning
			return ActionScheduler_Status_OK;
		} else if (ret != RTCDriver_Status_OK){
			return ActionScheduler_Status_Error;
		}
		pSelf->dateAndTimeUpdated = true;
	}

	return ActionScheduler_Status_OK;
}

static ActionScheduler_Status_TypeDef _ActionScheduler_updateRecordsUsingDataFromGPS(ActionScheduler_TypeDef* pSelf, const GPSData_TypeDef* pGPSData){

	if ((pSelf->dateAndTimeUpdated == false) && (pSelf->pConfig->useDateAndTimeFromGPS)){

		DateTime_TypeDef dateToSetRTC;
		memcpy(&dateToSetRTC, &(pGPSData->gpsDateTime), sizeof(DateTime_TypeDef));

		if (pSelf->pConfig->timeZoneShift_30minsUnit > 0){
			uint32_t secondsToAdd = ((uint32_t)pSelf->pConfig->timeZoneShift_30minsUnit) * 60 * 30;
			if (RTCDriver_addSeconds(&dateToSetRTC, secondsToAdd) != RTCDriver_Status_OK){
				return ActionScheduler_Status_RTCDriverError;
			}
		} else if (pSelf->pConfig->timeZoneShift_30minsUnit < 0){
			uint32_t secondsToSubstract = ((uint32_t)-(pSelf->pConfig->timeZoneShift_30minsUnit)) * 60 * 30;
			if (RTCDriver_substractSeconds(&dateToSetRTC, secondsToSubstract) != RTCDriver_Status_OK){
				return ActionScheduler_Status_RTCDriverError;
			}
		}

		if (RTCDriver_setDateAndTime(pSelf->pRTCDriver, dateToSetRTC) != RTCDriver_Status_OK){
			return ActionScheduler_Status_RTCDriverError;
		}
		pSelf->dateAndTimeUpdated = true;
	}

	if (pSelf->lastGpsFixType != pGPSData->fixType){
		switch (pGPSData->fixType){
		case GPSFixType_Unknown:
		case GPSFixType_NoFix:
		default:
			if (LedDriver_OnLed(pSelf->pGPSLedDriver) != LedDriver_Status_OK){
				return ActionScheduler_Status_Error;
			}
			break;
		case GPSFixType_2DFix:
			if (LedDriver_BlinkingLed(pSelf->pGPSLedDriver, ACTION_SCHEDULER_GPS_LED_2DFIX_ON, ACTION_SCHEDULER_GPS_LED_2DFIX_OFF) != LedDriver_Status_OK){
				return ActionScheduler_Status_Error;
			}
			break;
		case GPSFixType_3DFix:
			if (LedDriver_BlinkingLed(pSelf->pGPSLedDriver, ACTION_SCHEDULER_GPS_LED_3DFIX_ON, ACTION_SCHEDULER_GPS_LED_3DFIX_OFF) != LedDriver_Status_OK){
				return ActionScheduler_Status_Error;
			}
			break;
		}
		pSelf->lastGpsFixType = pGPSData->fixType;
	}

	return ActionScheduler_Status_OK;
}

static ActionScheduler_Status_TypeDef _ActionScheduler_idleState(ActionScheduler_TypeDef* pSelf){


	ActionScheduler_Status_TypeDef	ret 		= ActionScheduler_Status_OK;

	#define	DATA_NOT_PRESENT_TIMESTAMP	UINT32_MAX

	uint32_t	canMsgTimestamp		= DATA_NOT_PRESENT_TIMESTAMP;
	uint32_t	canErrTimestamp		= DATA_NOT_PRESENT_TIMESTAMP;
	uint32_t	gpsDataTimestamp	= DATA_NOT_PRESENT_TIMESTAMP;

	CANReceiver_Status_TypeDef	canStatus		= CANReceiver_Status_OK;
	CANReceiver_Status_TypeDef	canErrStatus	= CANReceiver_Status_OK;
	GPSDriver_Status_TypeDef	gpsStatus		= GPSDriver_Status_OK;

	CANData_TypeDef			canMsg;
	CANErrorData_TypeDef	canErr;
	GPSData_TypeDef			gpsData;

	while(true) {

		//< ----- CAN msg handling ----- >//
		if (canMsgTimestamp == DATA_NOT_PRESENT_TIMESTAMP){
			canStatus		= CANReceiver_pullLastFrame(pSelf->pCANReceiver, &canMsg);
			if (canStatus == CANReceiver_Status_OK){
				canMsgTimestamp	= canMsg.msTimestamp;
			} else if (canStatus == CANReceiver_Status_Empty) {
				canMsgTimestamp = DATA_NOT_PRESENT_TIMESTAMP;
			} else { //CANReceiver_Status_Error:
				return ActionScheduler_Status_CANReceiverError;
			}
		}
		//< ----- CAN Errors ignore ----- >//
		if (canErrTimestamp == DATA_NOT_PRESENT_TIMESTAMP){
			canErrStatus	= CANReceiver_pullLastCANBusError(pSelf->pCANReceiver, &canErr);
			if (canErrStatus == CANReceiver_Status_OK){
				canErrTimestamp = canErr.msTimestamp;
			} else if (canErrStatus == CANReceiver_Status_Empty){
				canErrTimestamp = DATA_NOT_PRESENT_TIMESTAMP;
			} else {
				return ActionScheduler_Status_CANReceiverError;
			}
		}
		//< ----- GPS data handling ----- >//
		if ((pSelf->pConfig->gpsFrequency != Config_GPSFrequency_OFF) && (gpsDataTimestamp == DATA_NOT_PRESENT_TIMESTAMP)){
			gpsStatus		= GPSDriver_pullLastFrame(pSelf->pGPSDriver, &gpsData);
			if (gpsStatus == GPSDriver_Status_OK){
				gpsDataTimestamp = gpsData.msTimestamp;
			} else if (gpsStatus == GPSDriver_Status_Empty){
				gpsDataTimestamp = DATA_NOT_PRESENT_TIMESTAMP;
			} else {
				return ActionScheduler_Status_GPSDriverError;
			}
		}

		//< ----- Loop stop condition ----- >//
		if ((canMsgTimestamp == DATA_NOT_PRESENT_TIMESTAMP) && (canErrTimestamp == DATA_NOT_PRESENT_TIMESTAMP) && (gpsDataTimestamp == DATA_NOT_PRESENT_TIMESTAMP)){
			break;
		}

		//< ----- CAN msg handling ----- >//
		if ((canMsgTimestamp <= canErrTimestamp) && (canMsgTimestamp <= gpsDataTimestamp)){

			if ((ret = _ActionScheduler_SetDateAndTimeFrameHandler(pSelf, &canMsg)) != ActionScheduler_Status_OK){
				return ret;
			}

			ret =	_AcionScheduler_StartLogTrigger(pSelf, &canMsg);
			if (ret == ActionScheduler_Status_OK){
				pSelf->state = ActionScheduler_State_LogInit;
				pSelf->logStartMsTime = canMsg.msTimestamp;
				return ActionScheduler_Status_OK;
			} else if (ret != ActionScheduler_Status_NoTrigger){
				return ret;
			}

			canMsgTimestamp = DATA_NOT_PRESENT_TIMESTAMP;
			continue;
		}

		//< ----- CAN err handling ----- >//
		if ((canErrTimestamp <= canMsgTimestamp) && (canErrTimestamp <= gpsDataTimestamp)){
			canErrTimestamp = DATA_NOT_PRESENT_TIMESTAMP;
			continue;
		}

		//< ----- GPS data saving ----- >//
		if ((gpsDataTimestamp <= canMsgTimestamp) && (gpsDataTimestamp <= canErrTimestamp)){

			if ((ret = _ActionScheduler_updateRecordsUsingDataFromGPS(pSelf, &gpsData)) != ActionScheduler_Status_OK){
				return ret;
			}

			gpsDataTimestamp = DATA_NOT_PRESENT_TIMESTAMP;
			continue;
		}
	}

	ret = _ActionScheduler_checkFramesTimeout(pSelf, pSelf->pConfig->startLogTriggers, pSelf->startTriggerFrameTimeout, pSelf->pConfig->startLogTriggersNumber);
	if (ret == ActionScheduler_Status_OK){
		pSelf->state = ActionScheduler_State_LogInit;

		uint32_t msTime = 0;
		if (MSTimerDriver_getMSTime(pSelf->pMSTimerDriver, &msTime) != MSTimerDriver_Status_OK){
			return ActionScheduler_Status_MSTimerError;
		}
		pSelf->logStartMsTime = msTime;
	} else if (ret != ActionScheduler_Status_NoTrigger){
		return ret;
	}

	return ActionScheduler_Status_OK;
}

static ActionScheduler_Status_TypeDef _ActionScheduler_logInitState(ActionScheduler_TypeDef* pSelf){

	ActionScheduler_Status_TypeDef ret = ActionScheduler_Status_OK;

	DateTime_TypeDef dateTime;
	if (RTCDriver_getDateAndTime(pSelf->pRTCDriver, &dateTime) != RTCDriver_Status_OK){
		return ActionScheduler_Status_Error;
	}

	for (uint8_t i=0; i<pSelf->pConfig->stopLogTriggersNumber; i++){
		if ((ret = _getFrameTimeoutValue(pSelf, &(pSelf->pConfig->stopLogTriggers[i]), &(pSelf->stopTriggerFrameTimeout[i]))) != ActionScheduler_Status_OK){
			return ret;
		}
	}

	if (DataSaver_startAGHLogFile(pSelf->pDataSaver, dateTime) != DataSaver_Status_OK){
		return ActionScheduler_Status_DataSaverError;
	}

	if (LedDriver_BlinkingLed(pSelf->pStatusLedDriver, ACTION_SCHEDULER_LOGGING_LED_ON, ACTION_SCHEDULER_LOGGING_LED_OFF) != LedDriver_Status_OK){
		return ActionScheduler_Status_Error;
	}
	pSelf->state = ActionScheduler_State_Logging;

	return ActionScheduler_Status_OK;
}

static ActionScheduler_Status_TypeDef _ActionScheduler_loggingState(ActionScheduler_TypeDef* pSelf){

	ActionScheduler_Status_TypeDef	ret 		= ActionScheduler_Status_OK;

	#define	DATA_NOT_PRESENT_TIMESTAMP	UINT32_MAX

	uint32_t	canMsgTimestamp		= DATA_NOT_PRESENT_TIMESTAMP;
	uint32_t	canErrTimestamp		= DATA_NOT_PRESENT_TIMESTAMP;
	uint32_t	gpsDataTimestamp	= DATA_NOT_PRESENT_TIMESTAMP;

	CANReceiver_Status_TypeDef	canStatus		= CANReceiver_Status_OK;
	CANReceiver_Status_TypeDef	canErrStatus	= CANReceiver_Status_OK;
	GPSDriver_Status_TypeDef	gpsStatus		= GPSDriver_Status_OK;

	CANData_TypeDef			canMsg;
	CANErrorData_TypeDef	canErr;
	GPSData_TypeDef			gpsData;

	while(true) {

		//< ----- CAN msg handling ----- >//
		if (canMsgTimestamp == DATA_NOT_PRESENT_TIMESTAMP){
			canStatus		= CANReceiver_pullLastFrame(pSelf->pCANReceiver, &canMsg);
			if (canStatus == CANReceiver_Status_OK){
				canMsgTimestamp	= canMsg.msTimestamp;
			} else if (canStatus == CANReceiver_Status_Empty) {
				canMsgTimestamp = DATA_NOT_PRESENT_TIMESTAMP;
			} else { //CANReceiver_Status_Error:
				return ActionScheduler_Status_CANReceiverError;
			}
		}
		//< ----- CAN Errors data handling ----- >//
		if (canErrTimestamp == DATA_NOT_PRESENT_TIMESTAMP){
			canErrStatus	= CANReceiver_pullLastCANBusError(pSelf->pCANReceiver, &canErr);
			if (canErrStatus == CANReceiver_Status_OK){
				canErrTimestamp = canErr.msTimestamp;
			} else if (canErrStatus == CANReceiver_Status_Empty){
				canErrTimestamp = DATA_NOT_PRESENT_TIMESTAMP;
			} else {
				return ActionScheduler_Status_CANReceiverError;
			}
		}
		//< ----- GPS data handling ----- >//
		if ((pSelf->pConfig->gpsFrequency != Config_GPSFrequency_OFF) && (gpsDataTimestamp == DATA_NOT_PRESENT_TIMESTAMP)){
			gpsStatus		= GPSDriver_pullLastFrame(pSelf->pGPSDriver, &gpsData);
			if (gpsStatus == GPSDriver_Status_OK){
				gpsDataTimestamp = gpsData.msTimestamp;
			} else if (gpsStatus == GPSDriver_Status_Empty){
				gpsDataTimestamp = DATA_NOT_PRESENT_TIMESTAMP;
			} else {
				return ActionScheduler_Status_GPSDriverError;
			}
		}

		//< ----- Loop stop condition ----- >//
		if ((canMsgTimestamp == DATA_NOT_PRESENT_TIMESTAMP) && (canErrTimestamp == DATA_NOT_PRESENT_TIMESTAMP) && (gpsDataTimestamp == DATA_NOT_PRESENT_TIMESTAMP)){
			break;
		}

		//< ----- CAN msg saving ----- >//
		if ((canMsgTimestamp <= canErrTimestamp) && (canMsgTimestamp <= gpsDataTimestamp)){

			if ((ret = _ActionScheduler_SetDateAndTimeFrameHandler(pSelf, &canMsg)) != ActionScheduler_Status_OK){
				return ret;
			}

			ret =	_AcionScheduler_StopLogTrigger(pSelf, &canMsg);
			if (ret == ActionScheduler_Status_OK){
				pSelf->state = ActionScheduler_State_LogClose;
			} else if (ret != ActionScheduler_Status_NoTrigger){
				return ret;
			}

			canMsg.msTimestamp -= pSelf->logStartMsTime;
			if (DataSaver_writeCANData(pSelf->pDataSaver, &canMsg) != DataSaver_Status_OK){
				return ActionScheduler_Status_DataSaverError;
			}

			canMsgTimestamp = DATA_NOT_PRESENT_TIMESTAMP;
			continue;
		}

		//< ----- CAN err saving ----- >//
		if ((canErrTimestamp <= canMsgTimestamp) && (canErrTimestamp <= gpsDataTimestamp)){
			canErr.msTimestamp -= pSelf->logStartMsTime;
			if (DataSaver_writeCANError(pSelf->pDataSaver, &canErr) != DataSaver_Status_OK){
				return ActionScheduler_Status_DataSaverError;
			}

			canErrTimestamp = DATA_NOT_PRESENT_TIMESTAMP;
			continue;
		}

		//< ----- GPS data saving ----- >//
		if ((gpsDataTimestamp <= canMsgTimestamp) && (gpsDataTimestamp <= canErrTimestamp)){
			gpsData.msTimestamp -= pSelf->logStartMsTime;
			if (pSelf->pConfig->gpsFrequency != Config_GPSFrequency_OFF){
				if (DataSaver_writeGPSData(pSelf->pDataSaver, &gpsData) != DataSaver_Status_OK){
					return ActionScheduler_Status_DataSaverError;
				}
			}
			if ((ret = _ActionScheduler_updateRecordsUsingDataFromGPS(pSelf, &gpsData)) != ActionScheduler_Status_OK){
				return ret;
			}
			gpsDataTimestamp = DATA_NOT_PRESENT_TIMESTAMP;
			continue;
		}
	}

	//< ----- Check timeout on messages (trigger) handling ----- >//
	ret = _ActionScheduler_checkFramesTimeout(pSelf, pSelf->pConfig->stopLogTriggers, pSelf->stopTriggerFrameTimeout, pSelf->pConfig->stopLogTriggersNumber);
	if (ret == ActionScheduler_Status_OK){
		pSelf->state = ActionScheduler_State_LogClose;
	} else if (ret != ActionScheduler_Status_NoTrigger){
		return ret;
	}

	return ActionScheduler_Status_OK;
}

static ActionScheduler_Status_TypeDef _ActionScheduler_logCloseState(ActionScheduler_TypeDef* pSelf){

	if (DataSaver_endAGHLogFile(pSelf->pDataSaver) != DataSaver_Status_OK){
		return ActionScheduler_Status_DataSaverError;
	}
	if (LedDriver_BlinkingLed(pSelf->pStatusLedDriver, ACTION_SCHEDULER_IDLE_LED_ON_TIME, ACTION_SCHEDULER_IDLE_LED_OFF_TIME) != LedDriver_Status_OK){
		return ActionScheduler_Status_Error;
	}

	if (CANReceiver_clear(pSelf->pCANReceiver) != CANReceiver_Status_OK){
		return ActionScheduler_Status_CANReceiverError;
	}
	if (GPSDriver_clear(pSelf->pGPSDriver) != GPSDriver_Status_OK){
		return ActionScheduler_Status_GPSDriverError;
	}
	pSelf->state = ActionScheduler_State_Idle;

	return ActionScheduler_Status_OK;
}

static ActionScheduler_Status_TypeDef _AcionScheduler_StartLogTrigger(ActionScheduler_TypeDef* pSelf, CANData_TypeDef* pData){

	for (uint8_t i=0; i<pSelf->pConfig->startLogTriggersNumber; i++){
		if ((pSelf->pConfig->startLogTriggers[i].usedFrameID) == (pData->ID)){
			if (pSelf->startTriggersCompareOperatorFunctions[i](pSelf, pData, &(pSelf->pConfig->startLogTriggers[i]), &(pSelf->startTriggerFrameTimeout[i]))){
				return ActionScheduler_Status_OK;
			}
		}
	}

	return ActionScheduler_Status_NoTrigger;
}

static ActionScheduler_Status_TypeDef _AcionScheduler_StopLogTrigger(ActionScheduler_TypeDef* pSelf, CANData_TypeDef* pData)
{

	for (uint8_t i=0; i<pSelf->pConfig->stopLogTriggersNumber; i++){
		if ((pSelf->pConfig->stopLogTriggers[i].usedFrameID) == (pData->ID)){
			if (pSelf->stopTriggersCompareOperatorFunctions[i](pSelf, pData, &(pSelf->pConfig->stopLogTriggers[i]), &(pSelf->stopTriggerFrameTimeout[i]))){
				return ActionScheduler_Status_OK;
			}
		}
	}

	return ActionScheduler_Status_NoTrigger;
}

static ActionScheduler_Status_TypeDef _ActionScheduler_checkFramesTimeout(ActionScheduler_TypeDef* pSelf, const Config_Trigger_TypeDef* aTriggers, uint32_t* aTriggersTimeouts, uint8_t triggersNumber){

	uint32_t actualMSTime = 0;
	if (MSTimerDriver_getMSTime(pSelf->pMSTimerDriver, &actualMSTime) != MSTimerDriver_Status_OK){
		return ActionScheduler_Status_MSTimerError;
	}

	for (uint8_t i = 0; i<triggersNumber; i++){
		if ((aTriggers[i].compareOperator == Config_TrigerCompareOperator_FRAME_TIMEOUT_MS)){
			if (aTriggersTimeouts[i] < actualMSTime){
				return ActionScheduler_Status_OK;
			}
		}
	}

	return ActionScheduler_Status_NoTrigger;
}

static ActionScheduler_Status_TypeDef _getCompareOperatorFunction(const Config_TrigerCompareOperator_TypeDef compareOperator, CompareOperatorFunction* pRetOperatorFunction){

	switch (compareOperator){
	case Config_TrigerCompareOperator_EQUAL:
		(*pRetOperatorFunction) = _ActionScheduler_compareOperator_EQUAL;
		return ActionScheduler_Status_OK;
	case Config_TrigerCompareOperator_NOT_EQUAL:
		(*pRetOperatorFunction) = _ActionScheduler_compareOperator_NOT_EQUAL;
		return ActionScheduler_Status_OK;
	case Config_TrigerCompareOperator_GREATER:
		(*pRetOperatorFunction) = _ActionScheduler_compareOperator_GREATER;
		return ActionScheduler_Status_OK;
	case Config_TrigerCompareOperator_GREATER_OR_EQUAL:
		(*pRetOperatorFunction) = _ActionScheduler_compareOperator_GREATER_OR_EQUAL;
		return ActionScheduler_Status_OK;
	case Config_TrigerCompareOperator_LESS:
		(*pRetOperatorFunction) = _ActionScheduler_compareOperator_LESS;
		return ActionScheduler_Status_OK;
	case Config_TrigerCompareOperator_LESS_OR_EQUAL:
		(*pRetOperatorFunction) = _ActionScheduler_compareOperator_LESS_OR_EQUAL;
		return ActionScheduler_Status_OK;
	case Config_TrigerCompareOperator_BITWISE_AND:
		(*pRetOperatorFunction) = _ActionScheduler_compareOperator_BITWISE_AND;
		return ActionScheduler_Status_OK;
	case Config_TrigerCompareOperator_BITWISE_OR:
		(*pRetOperatorFunction) = _ActionScheduler_compareOperator_BITWISE_OR;
		return ActionScheduler_Status_OK;
	case Config_TrigerCompareOperator_BITWISE_XOR:
		(*pRetOperatorFunction) = _ActionScheduler_compareOperator_BITWISE_XOR;
		return ActionScheduler_Status_OK;
	case Config_TrigerCompareOperator_FRAME_OCCURED:
		(*pRetOperatorFunction) = _ActionScheduler_compareOperator_FRAME_OCCURED;
		return ActionScheduler_Status_OK;
	case Config_TrigerCompareOperator_FRAME_TIMEOUT_MS:
		(*pRetOperatorFunction) = _ActionScheduler_compareOperator_FRAME_TIMEOUT_MS;
		return ActionScheduler_Status_OK;
	default:
		return ActionScheduler_Status_WrongCompareOperatorError;
	}
}


static ActionScheduler_Status_TypeDef _getFrameTimeoutValue(ActionScheduler_TypeDef* pSelf, const Config_Trigger_TypeDef* pTrigger, uint32_t* pRetFrameTimeout){

	if (pTrigger->compareOperator != Config_TrigerCompareOperator_FRAME_TIMEOUT_MS){
		(*pRetFrameTimeout) = 0;
		return ActionScheduler_Status_OK;
	}

	uint32_t actualTimeMs = 0;

	if (MSTimerDriver_getMSTime(pSelf->pMSTimerDriver, &actualTimeMs) != MSTimerDriver_Status_OK){
		return ActionScheduler_Status_MSTimerError;
	}

	(*pRetFrameTimeout) = actualTimeMs + pTrigger->compareConstValue;

	return ActionScheduler_Status_OK;
}

static bool _ActionScheduler_compareOperator_EQUAL(ActionScheduler_TypeDef* pSelf, const CANData_TypeDef* pCANData, const Config_Trigger_TypeDef* pTrigger, uint32_t* pTriggerTimeout){
	UNUSED(pSelf);
	UNUSED(pTriggerTimeout);
	uint32_t signalValue = 0;
	if (pCANData->ID != pTrigger->uFrameSignal.pSignal->pParentFrame->ID){
		return false;
	}
	if (CANData_GetValueRawOfSignal(pCANData, pTrigger->uFrameSignal.pSignal, &signalValue) != CANData_Status_OK){
		return false;
	}
	return (signalValue == pTrigger->compareConstValue);
}
static bool _ActionScheduler_compareOperator_NOT_EQUAL(ActionScheduler_TypeDef* pSelf, const CANData_TypeDef* pCANData, const Config_Trigger_TypeDef* pTrigger, uint32_t* pTriggerTimeout){
	UNUSED(pSelf);
	UNUSED(pTriggerTimeout);
	uint32_t signalValue = 0;
	if (pCANData->ID != pTrigger->uFrameSignal.pSignal->pParentFrame->ID){
		return false;
	}
	if (CANData_GetValueRawOfSignal(pCANData, pTrigger->uFrameSignal.pSignal, &signalValue) != CANData_Status_OK){
		return false;
	}
	return (signalValue != pTrigger->compareConstValue);
}
static bool _ActionScheduler_compareOperator_GREATER(ActionScheduler_TypeDef* pSelf, const CANData_TypeDef* pCANData, const Config_Trigger_TypeDef* pTrigger, uint32_t* pTriggerTimeout){
	UNUSED(pSelf);
	UNUSED(pTriggerTimeout);
	uint32_t signalValue = 0;
	if (pCANData->ID != pTrigger->uFrameSignal.pSignal->pParentFrame->ID){
		return false;
	}
	if (CANData_GetValueRawOfSignal(pCANData, pTrigger->uFrameSignal.pSignal, &signalValue) != CANData_Status_OK){
		return false;
	}
	return (signalValue > pTrigger->compareConstValue);
}
static bool _ActionScheduler_compareOperator_GREATER_OR_EQUAL(ActionScheduler_TypeDef* pSelf, const CANData_TypeDef* pCANData, const Config_Trigger_TypeDef* pTrigger, uint32_t* pTriggerTimeout){
	UNUSED(pSelf);
	UNUSED(pTriggerTimeout);
	uint32_t signalValue = 0;
	if (pCANData->ID != pTrigger->uFrameSignal.pSignal->pParentFrame->ID){
		return false;
	}
	if (CANData_GetValueRawOfSignal(pCANData, pTrigger->uFrameSignal.pSignal, &signalValue) != CANData_Status_OK){
		return false;
	}
	return (signalValue >= pTrigger->compareConstValue);
}
static bool _ActionScheduler_compareOperator_LESS(ActionScheduler_TypeDef* pSelf, const CANData_TypeDef* pCANData, const Config_Trigger_TypeDef* pTrigger, uint32_t* pTriggerTimeout){
	UNUSED(pSelf);
	UNUSED(pTriggerTimeout);
	uint32_t signalValue = 0;
	if (pCANData->ID != pTrigger->uFrameSignal.pSignal->pParentFrame->ID){
		return false;
	}
	if (CANData_GetValueRawOfSignal(pCANData, pTrigger->uFrameSignal.pSignal, &signalValue) != CANData_Status_OK){
		return false;
	}
	return (signalValue < pTrigger->compareConstValue);
}
static bool _ActionScheduler_compareOperator_LESS_OR_EQUAL(ActionScheduler_TypeDef* pSelf, const CANData_TypeDef* pCANData, const Config_Trigger_TypeDef* pTrigger, uint32_t* pTriggerTimeout){
	UNUSED(pSelf);
	UNUSED(pTriggerTimeout);
	uint32_t signalValue = 0;
	if (pCANData->ID != pTrigger->uFrameSignal.pSignal->pParentFrame->ID){
		return false;
	}
	if (CANData_GetValueRawOfSignal(pCANData, pTrigger->uFrameSignal.pSignal, &signalValue) != CANData_Status_OK){
		return false;
	}
	return (signalValue <= pTrigger->compareConstValue);
}
static bool _ActionScheduler_compareOperator_BITWISE_AND(ActionScheduler_TypeDef* pSelf, const CANData_TypeDef* pCANData, const Config_Trigger_TypeDef* pTrigger, uint32_t* pTriggerTimeout){
	UNUSED(pSelf);
	UNUSED(pTriggerTimeout);
	uint32_t signalValue = 0;
	if (pCANData->ID != pTrigger->uFrameSignal.pSignal->pParentFrame->ID){
		return false;
	}
	if (CANData_GetValueRawOfSignal(pCANData, pTrigger->uFrameSignal.pSignal, &signalValue) != CANData_Status_OK){
		return false;
	}
	return ((signalValue & pTrigger->compareConstValue) != 0);
}
static bool _ActionScheduler_compareOperator_BITWISE_OR(ActionScheduler_TypeDef* pSelf, const CANData_TypeDef* pCANData, const Config_Trigger_TypeDef* pTrigger, uint32_t* pTriggerTimeout){
	UNUSED(pSelf);
	UNUSED(pTriggerTimeout);
	uint32_t signalValue = 0;
	if (pCANData->ID != pTrigger->uFrameSignal.pSignal->pParentFrame->ID){
		return false;
	}
	if (CANData_GetValueRawOfSignal(pCANData, pTrigger->uFrameSignal.pSignal, &signalValue) != CANData_Status_OK){
		return false;
	}
	return ((signalValue | pTrigger->compareConstValue) != 0);
}
static bool _ActionScheduler_compareOperator_BITWISE_XOR(ActionScheduler_TypeDef* pSelf, const CANData_TypeDef* pCANData, const Config_Trigger_TypeDef* pTrigger, uint32_t* pTriggerTimeout){
	UNUSED(pSelf);
	UNUSED(pTriggerTimeout);
	uint32_t signalValue = 0;
	if (pCANData->ID != pTrigger->uFrameSignal.pSignal->pParentFrame->ID){
		return false;
	}
	if (CANData_GetValueRawOfSignal(pCANData, pTrigger->uFrameSignal.pSignal, &signalValue) != CANData_Status_OK){
		return false;
	}
	return ((signalValue ^ pTrigger->compareConstValue) != 0);
}
static bool _ActionScheduler_compareOperator_FRAME_OCCURED(ActionScheduler_TypeDef* pSelf, const CANData_TypeDef* pCANData, const Config_Trigger_TypeDef* pTrigger, uint32_t* pTriggerTimeout){
	UNUSED(pSelf);
	UNUSED(pTriggerTimeout);
	if (pCANData->ID == pTrigger->uFrameSignal.pFrame->ID){
		return true;
	}
	return false;
}
static bool _ActionScheduler_compareOperator_FRAME_TIMEOUT_MS(ActionScheduler_TypeDef* pSelf, const CANData_TypeDef* pCANData, const Config_Trigger_TypeDef* pTrigger, uint32_t* pTriggerTimeout){
	if (pCANData->ID == pTrigger->uFrameSignal.pFrame->ID){
		_getFrameTimeoutValue(pSelf, pTrigger, pTriggerTimeout);
	}
	return false;
}

