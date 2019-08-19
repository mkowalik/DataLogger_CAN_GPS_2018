/*
 * action_scheduler.c
 *
 *  Created on: 12.11.2017
 *      Author: Michal Kowalik
 */


#include "user/action_scheduler.h"
#include "stdio.h"

static ActionScheduler_Status_TypeDef ActionScheduler_mainLoop(ActionScheduler_TypeDef* pSelf);
static uint8_t AcionScheduler_StartLogTrigger(ActionScheduler_TypeDef* pSelf, CANData_TypeDef* pData);
static uint8_t AcionScheduler_StopLogTrigger(ActionScheduler_TypeDef* pSelf, CANData_TypeDef* pData);
static ActionScheduler_Status_TypeDef ActionScheduler_idleState(ActionScheduler_TypeDef* pSelf);
static ActionScheduler_Status_TypeDef ActionScheduler_logInitState(ActionScheduler_TypeDef* pSelf);
static ActionScheduler_Status_TypeDef ActionScheduler_loggingState(ActionScheduler_TypeDef* pSelf);
static ActionScheduler_Status_TypeDef ActionScheduler_logCloseState(ActionScheduler_TypeDef* pSelf);

ActionScheduler_Status_TypeDef ActionScheduler_init(ActionScheduler_TypeDef* pSelf, ConfigDataManager_TypeDef* pConfigManager,
		DataSaver_TypeDef* pDataSaver, CANReceiver_TypeDef* pCANReceiver, RTCDriver_TypeDef* pRTCDriver, LedDriver_TypeDef* pStatusLedDriver){

	if (pSelf->state != ActionScheduler_State_UnInitialized){
		return ActionScheduler_Status_Error;
	}

	pSelf->pConfigManager	= pConfigManager;
	pSelf->pDataSaver		= pDataSaver;
	pSelf->pCANReceiver		= pCANReceiver;
	pSelf->pRTCDriver		= pRTCDriver;
	pSelf->pStatusLedDriver	= pStatusLedDriver;

	if (LedDriver_BlinkingLed(pSelf->pStatusLedDriver, ACTION_SCHEDULER_IDLE_LED_ON_TIME, ACTION_SCHEDULER_IDLE_LED_OFF_TIME) != LedDriver_Status_OK){
		return ActionScheduler_Status_Error;
	}
	pSelf->state = ActionScheduler_State_Idle;

	return ActionScheduler_Status_OK;

}

static ActionScheduler_Status_TypeDef ActionScheduler_mainLoop(ActionScheduler_TypeDef* pSelf){

	switch (pSelf->state){
	case ActionScheduler_State_UnInitialized:
	default:
		return ActionScheduler_Status_UnInitializedError;
		break;
	case ActionScheduler_State_Idle:
		return ActionScheduler_idleState(pSelf);
		break;
	case ActionScheduler_State_LogInit:
		return ActionScheduler_logInitState(pSelf);
		break;
	case ActionScheduler_State_Logging:
		return ActionScheduler_loggingState(pSelf);
		break;
	case ActionScheduler_State_LogClose:
		return ActionScheduler_logCloseState(pSelf);
		break;
	}

	return ActionScheduler_Status_Error;
}

static uint8_t AcionScheduler_StartLogTrigger(ActionScheduler_TypeDef* pSelf, CANData_TypeDef* pData){

//	if (pSelf->state == ActionScheduler_State_UnInitialized){
//		return ActionScheduler_Status_UnInitializedError;
//	}

#ifndef CAR_DEF
	#error "Missing CAR_DEF definition."
#elif CAR_DEF == CAR_DEF_GRAZYNA
	if ((pData->ID == 0x600) &&
		((uint16_t)(pData->Data[0] | ((pData->Data[1])<<8)) > 50)){	//TODO make it not hardcoded
		//(pData->Data[2] > 10)){
			return 1;
	}
	return 0;
#elif CAR_DEF == CAR_DEF_LEM
	if ((pData->ID == 0x550) &&
			pData->Data[0] != 0 &&
			pData->Data[1] != 1){
		return 1;
	}
	return 0;
#else
	#error "Unexpected value of CAR_DEF definition."
#endif

}

static ActionScheduler_Status_TypeDef ActionScheduler_SetDateAndTime(ActionScheduler_TypeDef* pSelf, CANData_TypeDef* pData){

	if (pSelf->state == ActionScheduler_State_UnInitialized){
		return ActionScheduler_Status_UnInitializedError;
	}

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

static uint8_t AcionScheduler_StopLogTrigger(ActionScheduler_TypeDef* pSelf, CANData_TypeDef* pData){

//	if (pSelf->state == ActionScheduler_State_UnInitialized){
//		return ActionScheduler_Status_UnInitializedError;
//	}
#ifndef CAR_DEF
	#error "Missing CAR_DEF definition."
#elif CAR_DEF == CAR_DEF_GRAZYNA
	if ((pData->ID == 0x600) &&
		((uint16_t)(pData->Data[0] | ((pData->Data[1])<<8)) < 50)){
		//	(pData->Data[2] <5)){
			return 1;
	}
	return 0;
#elif CAR_DEF == CAR_DEF_LEM
	if ((pData->ID == 0x550) &&
			pData->Data[0] == 0 &&
			pData->Data[1] == 1){
		return 1;
	}
	return 0;
#else
	#error "Unexpected value of CAR_DEF definition."
#endif

}

static ActionScheduler_Status_TypeDef ActionScheduler_idleState(ActionScheduler_TypeDef* pSelf){

	if (pSelf->state == ActionScheduler_State_UnInitialized){
		return ActionScheduler_Status_UnInitializedError;
	}

	CANData_TypeDef msg;
	CANReceiver_Status_TypeDef status;

	do {

		status = CANReceiver_pullLastFrame(pSelf->pCANReceiver, &msg);

		if (status == CANReceiver_Status_OK){

			if (AcionScheduler_StartLogTrigger(pSelf, &msg) != 0){
				pSelf->state = ActionScheduler_State_LogInit;
				pSelf->logStartMsTime = msg.msTime;
				break;
			} else {
				ActionScheduler_SetDateAndTime(pSelf, &msg);
			}

		} else if (status ==  CANReceiver_Status_Empty) {

			break;

		} else { //CANReceiver_Status_Error:

			return ActionScheduler_Status_Error;

		}

	} while(1);

	return ActionScheduler_Status_OK;

}

static ActionScheduler_Status_TypeDef ActionScheduler_logInitState(ActionScheduler_TypeDef* pSelf){

	if (pSelf->state == ActionScheduler_State_UnInitialized){
		return ActionScheduler_Status_UnInitializedError;
	}

	DateTime_TypeDef dateTime;
	if (RTCDriver_getDateAndTime(pSelf->pRTCDriver, &dateTime) != RTCDriver_Status_OK){
		return ActionScheduler_Status_Error;
	}

	if (DataSaver_startLogging(pSelf->pDataSaver, dateTime) != DataSaver_Status_OK){
		return ActionScheduler_Status_Error;
	}

	if (LedDriver_BlinkingLed(pSelf->pStatusLedDriver, ACTION_SCHEDULER_LOGGING_LED_ON, ACTION_SCHEDULER_LOGGING_LED_OFF) != LedDriver_Status_OK){
		return ActionScheduler_Status_Error;
	}
	pSelf->state = ActionScheduler_State_Logging;

	return ActionScheduler_Status_OK;

}

static ActionScheduler_Status_TypeDef ActionScheduler_loggingState(ActionScheduler_TypeDef* pSelf){

	if (pSelf->state == ActionScheduler_State_UnInitialized){
		return ActionScheduler_Status_UnInitializedError;
	}

	CANData_TypeDef msg;
	CANReceiver_Status_TypeDef status;

	do {

		status = CANReceiver_pullLastFrame(pSelf->pCANReceiver, &msg);

		if (status == CANReceiver_Status_OK){

			msg.msTime -= pSelf->logStartMsTime;

			if (AcionScheduler_StopLogTrigger(pSelf, &msg) != 0){
				pSelf->state = ActionScheduler_State_LogClose;
				break;
			}

			if (DataSaver_writeCANData(pSelf->pDataSaver, &msg) != DataSaver_Status_OK){
				return ActionScheduler_Status_Error;
			}

		} else if (status ==  CANReceiver_Status_Empty) {

			break;

		} else { //CANReceiver_Status_Error:

			return ActionScheduler_Status_Error;

		}

	} while(1);

	return ActionScheduler_Status_OK;

}

static ActionScheduler_Status_TypeDef ActionScheduler_logCloseState(ActionScheduler_TypeDef* pSelf){

	if (pSelf->state == ActionScheduler_State_UnInitialized){
		return ActionScheduler_Status_UnInitializedError;
	}

	if (DataSaver_stopLogging(pSelf->pDataSaver) != DataSaver_Status_OK){
		return ActionScheduler_Status_Error;
	}
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

	pSelf->state = ActionScheduler_State_Idle;

	while (ActionScheduler_mainLoop(pSelf) == ActionScheduler_Status_OK);

	return ActionScheduler_Status_Error;
}
/*
ActionScheduler_Status_TypeDef ActionScheduler_1msElapsedCallbackHandler(ActionScheduler_TypeDef* pSelf){

	if (pSelf->state != ActionScheduler_State_Idle){
		return ActionScheduler_mainLoop(pSelf);
	}

	return ActionScheduler_Status_OK;
}
*/
