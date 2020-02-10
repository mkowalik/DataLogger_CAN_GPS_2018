/*
 * action_scheduler.h
 *
 *  Created on: 12.11.2017
 *      Author: Michal Kowalik
 */

#ifndef ACTION_SCHEDULER_H_
#define ACTION_SCHEDULER_H_

#include <user/data_saver.h>
#include <user/ms_timer_driver.h>
#include "user/config.h"
#include "user/can_receiver.h"
#include "user/led_driver.h"
#include <user/gps_driver.h>

typedef enum {
	ActionScheduler_Status_OK = 0,
	ActionScheduler_Status_NoTrigger,
	ActionScheduler_Status_NullPointerError,
	ActionScheduler_Status_UnInitializedError,
	ActionScheduler_Status_MSTimerError,
	ActionScheduler_Status_WrongCompareOperatorError,
	ActionScheduler_Status_CANReceiverError,
	ActionScheduler_Status_GPSDriverError,
	ActionScheduler_Status_DataSaverError,
	ActionScheduler_Status_RTCDriverError,
	ActionScheduler_Status_Error
} ActionScheduler_Status_TypeDef;

#define	ACTION_SCHEDULER_IDLE_LED_ON_TIME		200
#define	ACTION_SCHEDULER_IDLE_LED_OFF_TIME		800
#define	ACTION_SCHEDULER_LOGGING_LED_ON			100
#define	ACTION_SCHEDULER_LOGGING_LED_OFF		100

#define	ACTION_SCHEDULER_GPS_LED_2DFIX_ON	500
#define	ACTION_SCHEDULER_GPS_LED_2DFIX_OFF	500
#define	ACTION_SCHEDULER_GPS_LED_3DFIX_ON	50
#define ACTION_SCHEDULER_GPS_LED_3DFIX_OFF	950

#define	ACTION_SCHEDULER_RTC_SETUP_FRAME_DLC	7

/**
 * UnInitialized --> SystemConfig --> Idle --> LogInit --> Logging --> LogClose
 * 									  /\									|
 * 						 			  |										|
 * 						 			  |_____________________________________|
 */
typedef enum {
	ActionScheduler_State_UnInitialized = 0,
	ActionScheduler_State_Idle,
	ActionScheduler_State_LogInit,
	ActionScheduler_State_Logging,
	ActionScheduler_State_LogClose
} ActionScheduler_State_TypeDef;

typedef	struct _ActionScheduler_TypeDef	ActionScheduler_TypeDef;

typedef bool (*CompareOperatorFunction)(ActionScheduler_TypeDef* pSelf, const CANData_TypeDef* pCANData, const Config_Trigger_TypeDef* pTrigger, uint32_t* pTriggerTimeout);

typedef struct _ActionScheduler_TypeDef {
	ActionScheduler_State_TypeDef	state;
	Config_TypeDef*					pConfig;
	DataSaver_TypeDef*				pDataSaver;
	CANReceiver_TypeDef*			pCANReceiver;
	GPSDriver_TypeDef*				pGPSDriver;
	MSTimerDriver_TypeDef*			pMSTimerDriver;
	RTCDriver_TypeDef*				pRTCDriver;
	LedDriver_TypeDef*				pStatusLedDriver;
	LedDriver_TypeDef*				pGPSLedDriver;
	uint32_t						logStartMsTime;
	GPSFixType						lastGpsFixType;
	bool							dateAndTimeUpdated;
	CompareOperatorFunction			startTriggersCompareOperatorFunctions[CONFIG_MAX_START_LOG_TRIGGER_NUMBER];
	uint32_t						startTriggerFrameTimeout[CONFIG_MAX_START_LOG_TRIGGER_NUMBER];
	CompareOperatorFunction			stopTriggersCompareOperatorFunctions[CONFIG_MAX_START_LOG_TRIGGER_NUMBER];
	uint32_t						stopTriggerFrameTimeout[CONFIG_MAX_START_LOG_TRIGGER_NUMBER];
} ActionScheduler_TypeDef;


ActionScheduler_Status_TypeDef ActionScheduler_init(ActionScheduler_TypeDef* pSelf, ConfigDataManager_TypeDef* pConfigManager, DataSaver_TypeDef* pDataSaver, CANReceiver_TypeDef* pCANReceiver,
		GPSDriver_TypeDef* pGPSDriver, MSTimerDriver_TypeDef* pMSTimerDriver, RTCDriver_TypeDef* pRTCDriver, LedDriver_TypeDef* pStatusLedDriver, LedDriver_TypeDef* pGPSLedDriver);
ActionScheduler_Status_TypeDef ActionScheduler_startScheduler(ActionScheduler_TypeDef* pSelf);
ActionScheduler_Status_TypeDef ActionScheduler_thread(ActionScheduler_TypeDef* pSelf);

#endif /* ACTION_SCHEDULER_H_ */
