/*
 * config.h
 *
 *  Created on: 12.11.2017
 *      Author: Michal Kowalik
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include "user/file_system_wrapper.h"
#include "user/file_reading_buffer.h"
#include "user/file_writing_buffer.h"

#define	CONFIG_FILENAME		"logger.aghconf"
#define	CONFIG_NAMES_LENGTH		21

#define	CONFIG_FILE_USED_VERSION		0
#define	CONFIG_FILE_USED_SUBVERSION		4

#define CONFIG_ID_NUMBER			2048

#define	CONFIG_MAX_FRAME_DLC_VALUE			8
#define	CONFIG_MAX_SIGNAL_LENGTH_BITS_VALUE	64
#define	CONFIG_MAX_FRAMES_NUMBER			(14 * 4)
#define	CONFIG_MAX_SIGNALS_NUMBER			256

#define	CONFIG_GPS_FRAME_ID		0x800

#define	CONFIG_MAX_START_LOG_TRIGGER_NUMBER	5
#define	CONFIG_MAX_STOP_LOG_TRIGGER_NUMBER	5

typedef enum {
	Config_GPSFrequency_OFF   = 0,
	Config_GPSFrequency_0_5Hz = 1,
	Config_GPSFrequency_1Hz   = 2,
	Config_GPSFrequency_2Hz   = 3,
	Config_GPSFrequency_5Hz   = 4,
	Config_GPSFrequency_10Hz  = 5,
} Config_GPSFrequency_TypeDef;

typedef enum {
	Config_CANBitrate_50kbps	= 50,
	Config_CANBitrate_125kbps	= 125,
	Config_CANBitrate_250kbps	= 250,
	Config_CANBitrate_500kbps	= 500,
	Config_CANBitrate_1Mbps		= 1000
} Config_CANBitrate_TypeDef;

#define	CONFIG_VALUE_TYPE_SIGNED_TYPE_flag			(1<<0)
#define	CONFIG_VALUE_TYPE_BIG_ENDIAN_TYPE_flag		(1<<1)

typedef struct _ConfigFrame_TypeDef ConfigFrame_TypeDef;

typedef struct _ConfigSignal_TypeDef {
	ConfigFrame_TypeDef*		pFrame;
	uint16_t					signalID;
	uint8_t						startBit;
	uint8_t						lengthBits;
	uint8_t						valueType;
	uint16_t 					multiplier;
	uint16_t 					divider;
	uint16_t 					offset;
	char						description[CONFIG_NAMES_LENGTH];
	char						unit[CONFIG_NAMES_LENGTH];
	char						comment[CONFIG_NAMES_LENGTH];
} ConfigSignal_TypeDef;

typedef struct _ConfigSignalListElem_TypeDef{
	ConfigSignal_TypeDef					signal;
	struct _ConfigSignalListElem_TypeDef*	pNext;
} ConfigSignalListElem_TypeDef;

typedef struct _ConfigFrame_TypeDef {
	uint16_t 						ID;
	char							frameName[CONFIG_NAMES_LENGTH];
	ConfigSignalListElem_TypeDef*	pSignalsListHead;
} ConfigFrame_TypeDef;

typedef enum {
	Config_TrigerCompareOperator_EQUAL				= 0x01,
	Config_TrigerCompareOperator_NOT_EQUAL			= 0x02,
	Config_TrigerCompareOperator_GREATER			= 0x03,
	Config_TrigerCompareOperator_GREATER_OR_EQUAL	= 0x04,
	Config_TrigerCompareOperator_LESS				= 0x05,
	Config_TrigerCompareOperator_LESS_OR_EQUAL		= 0x06,
	Config_TrigerCompareOperator_BITWISE_AND		= 0x07,
	Config_TrigerCompareOperator_BITWISE_OR			= 0x08,
	Config_TrigerCompareOperator_BITWISE_XOR		= 0x09,
	Config_TrigerCompareOperator_FRAME_OCCURED		= 0x20,
	Config_TrigerCompareOperator_FRAME_TIMEOUT_MS	= 0x21
} Config_TrigerCompareOperator_TypeDef;

typedef struct {
	ConfigSignal_TypeDef*					pSignal;
	uint64_t								compareConstValue;
	Config_TrigerCompareOperator_TypeDef	operator;
} Config_Trigger_TypeDef;

typedef struct {
	uint16_t 						version;
	uint16_t 						subversion;
	char							logFileName[CONFIG_NAMES_LENGTH];
	uint16_t 						numOfFrames;
	Config_CANBitrate_TypeDef		canSpeed;
	Config_GPSFrequency_TypeDef		gpsFrequency;
	uint16_t						rtcConfigurationFrameID;
	uint8_t							startLogTriggersNumber;
	uint8_t							stopLogTriggersNumber;

	Config_Trigger_TypeDef			startLogTriggers[CONFIG_MAX_START_LOG_TRIGGER_NUMBER];
	Config_Trigger_TypeDef			stopLogTriggers[CONFIG_MAX_START_LOG_TRIGGER_NUMBER];

	ConfigFrame_TypeDef 			canFrames[CONFIG_MAX_FRAMES_NUMBER];
	ConfigFrame_TypeDef* 			canFramesByID[CONFIG_ID_NUMBER];
} Config_TypeDef;


typedef enum {
	ConfigDataManager_Status_OK = 0,
	ConfigDataManager_Status_NullPointerError,
	ConfigDataManager_Status_ConfigNotReadError,
	ConfigDataManager_Status_SignalsBufferOverflowError,
	ConfigDataManager_Status_InitError,
	ConfigDataManager_Status_NoConfigFileError,
	ConfigDataManager_Status_ConfigFileWrongVersionError,
	ConfigDataManager_Status_FileReadingBufferError,
	ConfigDataManager_Status_FileWritingBufferError,
	ConfigDataManager_Status_WrongOffsetError,
	ConfigDataManager_Status_WrongFrameIDError,
	ConfigDataManager_Status_WrongSignalIDError,
	ConfigDataManager_Status_WrongFrameDLCError,
	ConfigDataManager_Status_WrongSignalLengthError,
	ConfigDataManager_Status_WrongSignalDefinitionError,
	ConfigDataManager_Status_NotInitialisedError,
	ConfigDataManager_Status_FrameIDPreviouslyUsed,
	ConfigDataManager_Status_WrongGPSFrequencyError,
	ConfigDataManager_Status_WrongCANBitrateError,
	ConfigDataManager_Status_WrongTriggerDefinitionError,
	ConfigDataManager_Status_Error
} ConfigDataManager_Status_TypeDef;

typedef enum {
	ConfigDataManager_State_UnInitialized = 0,
	ConfigDataManager_State_DuringInit,
	ConfigDataManager_State_Initialized,
	ConfigDataManager_State_ReadConfig
} ConfigDataManager_State_TypeDef;

typedef struct {
	ConfigDataManager_State_TypeDef		state;
	FileSystemWrapper_TypeDef* 			pFileSystem;
	Config_TypeDef						sConfig;
	FileSystemWrapper_File_TypeDef		sConfigFileHandler;
	FileReadingBuffer_TypeDef			sReadingBuffer;

	uint16_t							signalsCounter;
	ConfigSignalListElem_TypeDef		signalsListElemsPoll[CONFIG_MAX_SIGNALS_NUMBER];
} ConfigDataManager_TypeDef;


ConfigDataManager_Status_TypeDef ConfigDataManager_init(ConfigDataManager_TypeDef* pSelf, FileSystemWrapper_TypeDef* pFileSystem);
ConfigDataManager_Status_TypeDef ConfigDataManager_readConfig(ConfigDataManager_TypeDef* pSelf);
ConfigDataManager_Status_TypeDef ConfigDataManager_getConfigPointer(ConfigDataManager_TypeDef* pSelf, Config_TypeDef** ppRetConfig);
ConfigDataManager_Status_TypeDef ConfigDataManager_getIDsList(ConfigDataManager_TypeDef* pSelf, uint16_t* pRetIDsBuffer, uint16_t bufferSize, uint16_t* pIDsWritten);
ConfigDataManager_Status_TypeDef ConfigDataManager_findSignal(ConfigDataManager_TypeDef* pSelf, uint16_t frameID, uint16_t signalID, ConfigSignal_TypeDef** ppRetSignal);

ConfigDataManager_Status_TypeDef ConfigDataManager_writeConfig(ConfigDataManager_TypeDef* pSelf, FileWritingBuffer_TypeDef* pWritingBuffer);

#endif /* CONFIG_H_ */
