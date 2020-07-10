/*
 * data_saver.h
 *
 *  Created on: 12.11.2017
 *      Author: Michal Kowalik
 */

#ifndef DATA_SAVER_H_
#define DATA_SAVER_H_

#include "user/rtc_driver.h"
#include "user/file_writing_buffer.h"
#include "user/file_system_wrapper.h"
#include "user/can_data.h"
#include "user/gps_data.h"
#include "user/fifo_queue.h"
#include "user/config.h"


#define MEMORY_MSG_QUEUE_SIZE	128

#define	LOG_FILE_EXTENSION			"aghlog"
#define	LOG_FILENAME_MAX_LENGTH		50

#define	GPS_DATA_ID					0x800


typedef enum {
	DataSaver_State_UnInitialized = 0,
	DataSaver_State_Initialized,
	DataSaver_State_OpenedFile
} DataSaver_State_TypeDef;

typedef struct {
	FileSystemWrapper_TypeDef*		pFileSystemHandler;
	FileSystemWrapper_File_TypeDef	sDataFile;
	FileWritingBuffer_TypeDef		sWritingBuffer;

	ConfigDataManager_TypeDef*		pConfigManager;

	DataSaver_State_TypeDef			state;
} DataSaver_TypeDef;

typedef enum {
	DataSaver_Status_OK = 0,
	DataSaver_Status_MemoryFullError,
	DataSaver_Status_FileNotOpenedError,
	DataSaver_Status_NoMemoryDeviceError,
	DataSaver_Status_UnInitializedError,
	DataSaver_Status_AlreadyOpenedFileError,
	DataSaver_Status_WrongNameError,
	DataSaver_Status_ConfigError,
	DataSaver_Status_NullPointerError,
	DataSaver_Status_Error
} DataSaver_Status_TypeDef;

DataSaver_Status_TypeDef DataSaver_init				(DataSaver_TypeDef* pSelf, ConfigDataManager_TypeDef* pConfigManager, FileSystemWrapper_TypeDef* pFileSystemHandler);
DataSaver_Status_TypeDef DataSaver_startAGHLogFile	(DataSaver_TypeDef* pSelf, DateTime_TypeDef dateTime);
DataSaver_Status_TypeDef DataSaver_endAGHLogFile	(DataSaver_TypeDef* pSelf);
DataSaver_Status_TypeDef DataSaver_writeCANData		(DataSaver_TypeDef* pSelf, const CANData_TypeDef* pData);
DataSaver_Status_TypeDef DataSaver_writeCANError	(DataSaver_TypeDef* pSelf, const CANErrorData_TypeDef* pCANErrData);
DataSaver_Status_TypeDef DataSaver_writeGPSData		(DataSaver_TypeDef* pSelf, const GPSData_TypeDef* pData);

#endif /* DATA_SAVER_H_ */
