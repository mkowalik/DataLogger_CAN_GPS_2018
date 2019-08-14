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
#include <user/rtc_driver.h>
#include "user/config.h"
#include "user/can_receiver.h"
#include "user/led_driver.h"

typedef enum {
	ActionScheduler_Status_OK = 0,
	ActionScheduler_Status_UnInitializedError,
	ActionScheduler_Status_Error
} ActionScheduler_Status_TypeDef;

#define	ACTION_SCHEDULER_IDLE_LED_ON_TIME		50
#define	ACTION_SCHEDULER_IDLE_LED_OFF_TIME		950
#define	ACTION_SCHEDULER_LOGGING_LED_ON			100
#define	ACTION_SCHEDULER_LOGGING_LED_OFF		100

#define	ACTION_SCHEDULER_RTC_SETUP_FRAME_ID		0x520
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

typedef struct {
	ActionScheduler_State_TypeDef	state;
	ConfigDataManager_TypeDef*		pConfigManager;
	DataSaver_TypeDef*				pDataSaver;
	CANReceiver_TypeDef*			pCANReceiver;
	LedDriver_TypeDef*				pStatusLedDriver;
	RTCDriver_TypeDef*				pRTCDriver;
	uint32_t						logStartMsTime;
} ActionScheduler_TypeDef;


ActionScheduler_Status_TypeDef ActionScheduler_init(ActionScheduler_TypeDef* pSelf, ConfigDataManager_TypeDef* pConfigManager,
		DataSaver_TypeDef* pDataSaver, CANReceiver_TypeDef* pCANReceiver, RTCDriver_TypeDef* pRTCDriver, LedDriver_TypeDef* pStatusLedDriver);
ActionScheduler_Status_TypeDef ActionScheduler_startScheduler(ActionScheduler_TypeDef* pSelf);
//ActionScheduler_Status_TypeDef ActionScheduler_1msElapsedCallbackHandler(ActionScheduler_TypeDef* pSelf);

#endif /* ACTION_SCHEDULER_H_ */
