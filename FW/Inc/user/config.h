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
#include "user/can_data.h"

#define	CONFIG_SIGNED_TYPE_flag		(1<<0)
#define	CONFIG_16_BIT_TYPE_flag		(1<<1)
#define	CONFIG_ON_OFF_TYPE_flag		(1<<2)
#define	CONFIG_FLAG_TYPE_flag		(1<<3)
#define	CONFIG_CUSTOM_TYPE_flag		(1<<4)

#define	CONFIG_FILENAME		"logger.aghconf"

#define	CONFIG_FILE_USED_VERSION		0
#define	CONFIG_FILE_USED_SUBVERSION		3

#define CONFIG_ID_NUMBER			2048

#define	CONFIG_MAX_DLC_VALUE	8
#define	CONFIG_MAX_NO_OF_FRAMES	(14 * 4)
#define	CONFIG_MAX_CHANNELS_PER_FRAME	8

#define	CONFIG_FILE_NAME		"config.aghconf"
#define	CONFIG_NAMES_LENGTH		21

#define	CONFIG_GPS_FRAME_ID		0x800

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

typedef enum {
	UNSIGNED_8BIT 	= (0x00),
	SIGNED_8BIT 	= (CONFIG_SIGNED_TYPE_flag),
	UNSIGNED_16BIT 	= (CONFIG_16_BIT_TYPE_flag),
	SIGNED_16BIT 	= (CONFIG_SIGNED_TYPE_flag | CONFIG_16_BIT_TYPE_flag),
	ON_OFF_8BIT 	= (CONFIG_ON_OFF_TYPE_flag),	//0x00 - OFF, 0xFF - ON
	FLAGS_8BIT 		= (CONFIG_FLAG_TYPE_flag),		//look into documentation for bits meaning definition
	FLAGS_16BIT 	= (CONFIG_FLAG_TYPE_flag | CONFIG_16_BIT_TYPE_flag),	//look into documentation for bits meaning definition
	CUSTOM_8BIT 	= (CONFIG_CUSTOM_TYPE_flag),	//look into documentation
	CUSTOM_16BIT 	= (CONFIG_CUSTOM_TYPE_flag | CONFIG_16_BIT_TYPE_flag)	//look into documentation
} ValueType_TypeDef;

typedef struct {
	ValueType_TypeDef 	valueType;
	uint16_t 			multiplier;
	uint16_t 			divider;
	uint16_t 			offset;
	char				description[CONFIG_NAMES_LENGTH];
	char				unit[CONFIG_NAMES_LENGTH];
	char				comment[CONFIG_NAMES_LENGTH];
} ConfigChannel_TypeDef;

typedef struct {
	uint16_t 				ID;
	uint8_t 				DLC;
	char					moduleName[CONFIG_NAMES_LENGTH];
	ConfigChannel_TypeDef 	channels[CONFIG_MAX_CHANNELS_PER_FRAME];
} ConfigFrame_TypeDef;

typedef struct {
	uint16_t 				version;
	uint16_t 				subversion;
	uint16_t 				numOfFrames;
	uint16_t				canSpeed;
	uint8_t					gpsFrequency;
	ConfigFrame_TypeDef 	canFrames[CONFIG_MAX_NO_OF_FRAMES];
	ConfigFrame_TypeDef* 	canFramesByID[CONFIG_ID_NUMBER];
} Config_TypeDef;


typedef enum {
	ConfigDataManager_Status_OK = 0,
	ConfigDataManager_Status_InitError,
	ConfigDataManager_Status_NoConfigFileError,
	ConfigDataManager_Status_NoSuchChannelError,
	ConfigDataManager_Status_ConfigFileWrongVersionError,
	ConfigDataManager_Status_ConfigFileDataError,
	ConfigDataManager_Status_WrongOffsetError,
	ConfigDataManager_Status_WrongIDError,
	ConfigDataManager_Status_WrongDLCError,
	ConfigDataManager_Status_NotInitialisedError,
	ConfigDataManager_Status_FrameIDPreviouslyUsedError,
	ConfigDataManager_Status_WrongGPSFrequencyError,
	ConfigDataManager_Status_WrongCANBitrateError,
	ConfigDataManager_Status_Error
} ConfigDataManager_Status_TypeDef;

typedef enum {
	ConfigDataManager_State_UnInitialized = 0,
	ConfigDataManager_State_DuringInit,
	ConfigDataManager_State_Initialized
} ConfigDataManager_State_TypeDef;

typedef struct {
	ConfigDataManager_State_TypeDef		state;
	Config_TypeDef						sConfig;
	FileSystemWrapper_File_TypeDef		sConfigFileHandler;
	FileReadingBuffer_TypeDef			sReadingBuffer;
} ConfigDataManager_TypeDef;


ConfigDataManager_Status_TypeDef ConfigDataManager_init(ConfigDataManager_TypeDef* pSelf, FileSystemWrapper_TypeDef* pFileSystem);
ConfigDataManager_Status_TypeDef ConfigDataManager_getConfigPointer(ConfigDataManager_TypeDef* pSelf, Config_TypeDef** ppRetConfig);
ConfigDataManager_Status_TypeDef ConfigDataManager_getIDsList(ConfigDataManager_TypeDef* pSelf, uint16_t* pRetIDsBuffer, uint16_t bufferSize, uint16_t* pIDsWritten);
ConfigDataManager_Status_TypeDef ConfigDataManager_checkCorrectnessData(ConfigDataManager_TypeDef* pSelf, CANData_TypeDef* pData);
ConfigDataManager_Status_TypeDef ConfigDataManager_findChannel(ConfigDataManager_TypeDef* pSelf, uint16_t ID, uint8_t offset, ConfigChannel_TypeDef** pRetChannel);



#endif /* CONFIG_H_ */
