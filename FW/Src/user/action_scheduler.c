/*
 * action_scheduler.c
 *
 *  Created on: 12.11.2017
 *      Author: Michal Kowalik
 */

#include "user/action_scheduler.h"
#include "stdio.h"

//< ----- Private functions prototypes ----- >//
static bool _AcionScheduler_StartLogTrigger(ActionScheduler_TypeDef* pSelf, CANData_TypeDef* pData);
static bool _AcionScheduler_StopLogTrigger(ActionScheduler_TypeDef* pSelf, CANData_TypeDef* pData);
static ActionScheduler_Status_TypeDef _ActionScheduler_SetDateAndTimeFrameHandler(ActionScheduler_TypeDef* pSelf, CANData_TypeDef* pData);
static ActionScheduler_Status_TypeDef _ActionScheduler_idleState(ActionScheduler_TypeDef* pSelf);
static ActionScheduler_Status_TypeDef _ActionScheduler_logInitState(ActionScheduler_TypeDef* pSelf);
static ActionScheduler_Status_TypeDef _ActionScheduler_loggingState(ActionScheduler_TypeDef* pSelf);
static ActionScheduler_Status_TypeDef _ActionScheduler_logCloseState(ActionScheduler_TypeDef* pSelf);

//< ----- Public functions ----- >//

ActionScheduler_Status_TypeDef ActionScheduler_init(ActionScheduler_TypeDef* pSelf, ConfigDataManager_TypeDef* pConfigManager,
		DataSaver_TypeDef* pDataSaver, CANReceiver_TypeDef* pCANReceiver, GPSDriver_TypeDef* pGPSDriver, RTCDriver_TypeDef* pRTCDriver, LedDriver_TypeDef* pStatusLedDriver) {

	if (pSelf == NULL || pConfigManager == NULL || pDataSaver == NULL || pCANReceiver == NULL || pGPSDriver == NULL || pRTCDriver == NULL || pStatusLedDriver == NULL){
		return ActionScheduler_Status_NullPointerError;
	}

	if (pSelf->state != ActionScheduler_State_UnInitialized){
		return ActionScheduler_Status_Error;
	}

	ConfigDataManager_getConfigPointer(pConfigManager, &pSelf->pConfig);
	pSelf->pDataSaver		= pDataSaver;
	pSelf->pCANReceiver		= pCANReceiver;
	pSelf->pGPSDriver		= pGPSDriver;
	pSelf->pRTCDriver		= pRTCDriver;
	pSelf->pStatusLedDriver	= pStatusLedDriver;
#if CAR_DEF == CAR_DEF_LEM_WHEEL_MODULE
	if (RTCDriver_getDateAndTime(pSelf->pRTCDriver, &(pSelf->stopLoggingTimeout)) != RTCDriver_Status_OK){
		return ActionScheduler_Status_Error;
	}
#endif

	if (LedDriver_BlinkingLed(pSelf->pStatusLedDriver, ACTION_SCHEDULER_IDLE_LED_ON_TIME, ACTION_SCHEDULER_IDLE_LED_OFF_TIME) != LedDriver_Status_OK){
		return ActionScheduler_Status_Error;
	}
	pSelf->state = ActionScheduler_State_Idle;

	return ActionScheduler_Status_OK;
}

ActionScheduler_Status_TypeDef ActionScheduler_startScheduler(ActionScheduler_TypeDef* pSelf){

	if (pSelf->state == ActionScheduler_State_UnInitialized){
		return ActionScheduler_Status_UnInitializedError;
	}

	if (CANReceiver_start(pSelf->pCANReceiver) != CANReceiver_Status_OK){
		return ActionScheduler_Status_Error;
	}

#if CAR_DEF == CAR_DEF_LEM_WHEEL_MODULE
	if (RTCDriver_getDateAndTime(pSelf->pRTCDriver, &(pSelf->stopLoggingTimeout)) != RTCDriver_Status_OK){
		return ActionScheduler_Status_Error;
	}
#endif

	pSelf->state = ActionScheduler_State_Idle;

	return ActionScheduler_Status_OK;
}

ActionScheduler_Status_TypeDef ActionScheduler_thread(ActionScheduler_TypeDef* pSelf){

	switch (pSelf->state){
	case ActionScheduler_State_UnInitialized:
	default:
		return ActionScheduler_Status_UnInitializedError;
		break;
	case ActionScheduler_State_Idle:
		return _ActionScheduler_idleState(pSelf);
		break;
	case ActionScheduler_State_LogInit:
		return _ActionScheduler_logInitState(pSelf);
		break;
	case ActionScheduler_State_Logging:
		return _ActionScheduler_loggingState(pSelf);
		break;
	case ActionScheduler_State_LogClose:
		return _ActionScheduler_logCloseState(pSelf);
		break;
	}

	return ActionScheduler_Status_Error;
}

//< ----- Private functions ----- >//

static ActionScheduler_Status_TypeDef _ActionScheduler_SetDateAndTimeFrameHandler(ActionScheduler_TypeDef* pSelf, CANData_TypeDef* pData){

	if ((pData->ID == ACTION_SCHEDULER_RTC_SETUP_FRAME_ID) && (pData->DLC == ACTION_SCHEDULER_RTC_SETUP_FRAME_DLC)){

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
	}

	return ActionScheduler_Status_OK;
}

static bool _AcionScheduler_StartLogTrigger(ActionScheduler_TypeDef* pSelf, CANData_TypeDef* pData){

#ifndef CAR_DEF
	#error "Missing CAR_DEF definition."
#elif CAR_DEF == CAR_DEF_GRAZYNA
	if ((pData->ID == 0x600) &&
		((uint16_t)(pData->Data[0] | ((pData->Data[1])<<8)) > 50)){	//TODO make it not hardcoded
		//(pData->Data[2] > 10)){
			return true;
	}
	return false;
#elif CAR_DEF == CAR_DEF_LEM
	if ((pData->ID == 0x550) &&
			pData->Data[0] != 0 &&
			pData->Data[1] != 1){
		return true;
	}
	return false;
#elif CAR_DEF == CAR_DEF_GRAZYNA_TPS
	if (pData->ID == 0x600){
		if (pData->Data[2] > 10){
			return true;
		}
	}
	return false;
#elif CAR_DEF == CAR_DEF_LEM_WHEEL_MODULE
	if (pData->ID == 0x380){
		DateTime_TypeDef actualTime;
		if (RTCDriver_getDateAndTime(pSelf->pRTCDriver, &actualTime) != RTCDriver_Status_OK){
			return false;
		}
		if (RTCDriver_addSeconds(&actualTime, 3) != RTCDriver_Status_OK){
			return false;
		}
		pSelf->stopLoggingTimeout	= actualTime;
		return true;
	}
	return false;
#else
	#error "Unexpected value of CAR_DEF definition."
#endif
}

static bool _AcionScheduler_StopLogTrigger(ActionScheduler_TypeDef* pSelf, CANData_TypeDef* pData){

#ifndef CAR_DEF
	#error "Missing CAR_DEF definition."
#elif CAR_DEF == CAR_DEF_GRAZYNA
	if ((pData->ID == 0x600) &&
		((uint16_t)(pData->Data[0] | ((pData->Data[1])<<8)) < 50)){
		//	(pData->Data[2] <5)){
			return true;
	}
	return false;
#elif CAR_DEF == CAR_DEF_LEM
	if ((pData->ID == 0x550) &&
			pData->Data[0] == 0 &&
			pData->Data[1] == 1){
		return true;
	}
	return false;
#elif CAR_DEF == CAR_DEF_GRAZYNA_TPS
	if ((pData->ID == 0x600) &&
			(pData->Data[2] <5)){
			return true;
	}
	return false;
#elif CAR_DEF == CAR_DEF_LEM_WHEEL_MODULE
	DateTime_TypeDef actualTime;
	if (pData->ID == 0x380){
		if (RTCDriver_getDateAndTime(pSelf->pRTCDriver, &actualTime) != RTCDriver_Status_OK){
			return false;
		}
		if (RTCDriver_addSeconds(&actualTime, 3) != RTCDriver_Status_OK){
			return false;
		}
		pSelf->stopLoggingTimeout	= actualTime;
		return false;
	}

	if (RTCDriver_getDateAndTime(pSelf->pRTCDriver, &actualTime) != RTCDriver_Status_OK){
		return false;
	}

	if (RTCDriver_isAfter(pSelf->stopLoggingTimeout, actualTime)) {
		return true;
	}
	return false;
#else
	#error "Unexpected value of CAR_DEF definition."
#endif

}

static ActionScheduler_Status_TypeDef _ActionScheduler_idleState(ActionScheduler_TypeDef* pSelf){

	CANData_TypeDef msg;
	CANReceiver_Status_TypeDef status;

	while(1) {

		status = CANReceiver_pullLastFrame(pSelf->pCANReceiver, &msg);

		if (status == CANReceiver_Status_OK){

			if (_AcionScheduler_StartLogTrigger(pSelf, &msg) != 0){
				pSelf->state = ActionScheduler_State_LogInit;
				pSelf->logStartMsTime = msg.msTime;
				break;
			} else {
				_ActionScheduler_SetDateAndTimeFrameHandler(pSelf, &msg);
			}
		} else if (status ==  CANReceiver_Status_Empty) {
			break;
		} else { //CANReceiver_Status_Error:
			return ActionScheduler_Status_Error;
		}
	}
	return ActionScheduler_Status_OK;
}

static ActionScheduler_Status_TypeDef _ActionScheduler_logInitState(ActionScheduler_TypeDef* pSelf){

	DateTime_TypeDef dateTime;
	if (RTCDriver_getDateAndTime(pSelf->pRTCDriver, &dateTime) != RTCDriver_Status_OK){
		return ActionScheduler_Status_Error;
	}

	if (pSelf->pConfig->gpsFrequency != Config_GPSFrequency_OFF){
		if (GPSDriver_startReceiver(pSelf->pGPSDriver) != GPSDriver_Status_OK){
			return ActionScheduler_Status_Error;
		}
	}

	if (DataSaver_startAGHLogFile(pSelf->pDataSaver, dateTime) != DataSaver_Status_OK){
		return ActionScheduler_Status_Error;
	}

	if (LedDriver_BlinkingLed(pSelf->pStatusLedDriver, ACTION_SCHEDULER_LOGGING_LED_ON, ACTION_SCHEDULER_LOGGING_LED_OFF) != LedDriver_Status_OK){
		return ActionScheduler_Status_Error;
	}
	pSelf->state = ActionScheduler_State_Logging;

	return ActionScheduler_Status_OK;
}

static ActionScheduler_Status_TypeDef _ActionScheduler_loggingState(ActionScheduler_TypeDef* pSelf){

	CANData_TypeDef					canData;
	CANReceiver_Status_TypeDef		canStatus;

	GPSData_TypeDef					gpsData;
	GPSDriver_Status_TypeDef		gpsStatus;

	ActionScheduler_Status_TypeDef	ret;

	bool							breakFlagCAN = false;
	bool							breakFlagGPS = false;

	while(breakFlagCAN == false || breakFlagGPS == false) {
		canStatus = CANReceiver_pullLastFrame(pSelf->pCANReceiver, &canData);

		if (canStatus == CANReceiver_Status_OK){

			canData.msTime -= pSelf->logStartMsTime;

			if (_AcionScheduler_StopLogTrigger(pSelf, &canData) != 0){
				pSelf->state = ActionScheduler_State_LogClose;
				breakFlagCAN = true;
			}

			if (DataSaver_writeCANData(pSelf->pDataSaver, &canData) != DataSaver_Status_OK){
				return ActionScheduler_Status_Error;
			}

			if ((ret = _ActionScheduler_SetDateAndTimeFrameHandler(pSelf, &canData)) != ActionScheduler_Status_OK){
				return ret;
			}

		} else if (canStatus ==  CANReceiver_Status_Empty) {
			breakFlagCAN = true;
		} else { //CANReceiver_Status_Error:
			return ActionScheduler_Status_Error;
		}

		if (pSelf->pConfig->gpsFrequency != Config_GPSFrequency_OFF){
			gpsStatus = GPSDriver_pullLastFrame(pSelf->pGPSDriver, &gpsData);

			if (gpsStatus == GPSDriver_Status_OK){
				if (DataSaver_writeGPSData(pSelf->pDataSaver, &gpsData) != DataSaver_Status_OK){
					return ActionScheduler_Status_Error;
				}
				breakFlagGPS = false;
			} else if (gpsStatus == GPSDriver_Status_Empty){
				breakFlagGPS = true;
			} else {
				return ActionScheduler_Status_Error;
			}
		} else {
			breakFlagGPS = true;
		}
	}

	return ActionScheduler_Status_OK;
}

static ActionScheduler_Status_TypeDef _ActionScheduler_logCloseState(ActionScheduler_TypeDef* pSelf){

	if (pSelf->pConfig->gpsFrequency != Config_GPSFrequency_OFF){
		if (GPSDriver_stopReceiver(pSelf->pGPSDriver) != GPSDriver_Status_OK){
			return ActionScheduler_Status_Error;
		}
	}

	if (DataSaver_endAGHLogFile(pSelf->pDataSaver) != DataSaver_Status_OK){
		return ActionScheduler_Status_Error;
	}
	if (LedDriver_BlinkingLed(pSelf->pStatusLedDriver, ACTION_SCHEDULER_IDLE_LED_ON_TIME, ACTION_SCHEDULER_IDLE_LED_OFF_TIME) != LedDriver_Status_OK){
		return ActionScheduler_Status_Error;
	}
	pSelf->state = ActionScheduler_State_Idle;

	return ActionScheduler_Status_OK;
}

