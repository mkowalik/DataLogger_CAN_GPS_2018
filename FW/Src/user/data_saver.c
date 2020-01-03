/*
 * data_saver.c
 *
 *  Created on: 12.11.2017
 *      Author: Michal Kowalik
 */

#include "user/data_saver.h"
#include "stdint.h"
#include "stdio.h"
#include "string.h"

static DataSaver_Status_TypeDef DataSaver_saveHeader(DataSaver_TypeDef* pSelf, DateTime_TypeDef dateTime);

DataSaver_Status_TypeDef DataSaver_init(DataSaver_TypeDef* pSelf, ConfigDataManager_TypeDef* pConfigManager, FileSystemWrapper_TypeDef* pFileSystemHandler){

	if (pSelf == NULL || pConfigManager == NULL || pFileSystemHandler == NULL){
		return DataSaver_Status_NullPointerError;
	}

	if (pSelf->state != DataSaver_State_UnInitialized){
		return DataSaver_Status_UnInitializedError;
	}

	pSelf->pConfigManager		= pConfigManager;
	pSelf->pFileSystemHandler	= pFileSystemHandler;

	pSelf->state				= DataSaver_State_Initialized;

	return DataSaver_Status_OK;

}

DataSaver_Status_TypeDef DataSaver_startAGHLogFile(DataSaver_TypeDef* pSelf, DateTime_TypeDef dateTime){

	if (pSelf == NULL){
		return DataSaver_Status_NullPointerError;
	}

	if (pSelf->state != DataSaver_State_Initialized){
		return DataSaver_Status_UnInitializedError;
	}

	if (pSelf->state == DataSaver_State_OpenedFile){
		return DataSaver_Status_AlreadyOpenedFileError;
	}

	Config_TypeDef*	pConfig = NULL;
	if (ConfigDataManager_getConfigPointer(pSelf->pConfigManager, &pConfig) != ConfigDataManager_Status_OK){
		return DataSaver_Status_ConfigError;
	}

	//prepare name of file
	uint16_t offset = 0;
	char filename[LOG_FILENAME_MAX_LENGTH];
	memset(filename, 0, LOG_FILENAME_MAX_LENGTH);
	strcpy(filename, pConfig->logFileName);
	offset += strlen(pConfig->logFileName);
	offset += sprintf(filename+offset, "_%04d-%02d-%02d_%02d_%02d_%02d.", dateTime.year, dateTime.month, dateTime.day, dateTime.hour, dateTime.minute, dateTime.second);
	strcpy(filename+offset, LOG_FILE_EXTENSION);
	offset += strlen(LOG_FILE_EXTENSION);

	if (offset > LOG_FILENAME_MAX_LENGTH){
		return DataSaver_Status_WrongNameError;
	}

	if (FileSystemWrapper_open(pSelf->pFileSystemHandler, &pSelf->sDataFile, filename) != FileSystemWrapper_Status_OK){
		return DataSaver_Status_FileNotOpenedError;
	}

	if (FileWritingBuffer_init(&pSelf->sWritingBuffer, &pSelf->sDataFile) != FileWritingBuffer_Status_OK){
		return DataSaver_Status_FileNotOpenedError;
	}

	DataSaver_Status_TypeDef status;

	if((status = DataSaver_saveHeader(pSelf, dateTime)) != DataSaver_Status_OK){
		return status;
	}

	pSelf->state = DataSaver_State_OpenedFile;

	return DataSaver_Status_OK;

}

DataSaver_Status_TypeDef DataSaver_endAGHLogFile(DataSaver_TypeDef* pSelf){

	if (pSelf->state != DataSaver_State_OpenedFile){
		return DataSaver_Status_FileNotOpenedError;
	}

	if (FileWritingBuffer_writeToFileSystem(&pSelf->sWritingBuffer) != FileWritingBuffer_Status_OK){
		return DataSaver_Status_Error;
	}
	if (FileWritingBuffer_deInit(&pSelf->sWritingBuffer) != FileWritingBuffer_Status_OK){
		return DataSaver_Status_Error;
	}

	if (FileSystemWrapper_close(&pSelf->sDataFile) != FileSystemWrapper_Status_OK){
		return DataSaver_Status_Error;
	}

	pSelf->state = DataSaver_State_Initialized;

	return DataSaver_Status_OK;

}

DataSaver_Status_TypeDef DataSaver_writeCANData(DataSaver_TypeDef* pSelf, CANData_TypeDef* pData){

	if (pSelf->state != DataSaver_State_OpenedFile){
		return DataSaver_Status_FileNotOpenedError;
	}

	if (FileWritingBuffer_writeUInt32(&pSelf->sWritingBuffer, pData->msTime) != FileWritingBuffer_Status_OK){
		return DataSaver_Status_Error;
	}
	if (FileWritingBuffer_writeUInt16(&pSelf->sWritingBuffer, pData->ID) != FileWritingBuffer_Status_OK){
		return DataSaver_Status_Error;
	}
	if (FileWritingBuffer_writeUInt8(&pSelf->sWritingBuffer, pData->DLC) != FileWritingBuffer_Status_OK){
		return DataSaver_Status_Error;
	}

	for (uint8_t i=0; i<pData->DLC; i++){
		if (FileWritingBuffer_writeUInt8(&pSelf->sWritingBuffer, pData->Data[i]) != FileWritingBuffer_Status_OK){
			return DataSaver_Status_Error;
		}
	}

	if (FileWritingBuffer_writeToFileSystem(&pSelf->sWritingBuffer) != FileWritingBuffer_Status_OK){
		return DataSaver_Status_Error;
	}

	return DataSaver_Status_OK;
}

DataSaver_Status_TypeDef DataSaver_writeGPSData(DataSaver_TypeDef* pSelf, GPSData_TypeDef* pData){

	if (pSelf->state != DataSaver_State_OpenedFile){
		return DataSaver_Status_FileNotOpenedError;
	}

	if (FileWritingBuffer_writeUInt32(&pSelf->sWritingBuffer, pData->msTime) != FileWritingBuffer_Status_OK){
		return DataSaver_Status_Error;
	}
	if (FileWritingBuffer_writeUInt16(&pSelf->sWritingBuffer, CONFIG_GPS_FRAME_ID) != FileWritingBuffer_Status_OK){
		return DataSaver_Status_Error;
	}

	if (FileWritingBuffer_writeUInt16(&pSelf->sWritingBuffer, pData->gpsDateTime.year) != FileWritingBuffer_Status_OK){
		return DataSaver_Status_Error;
	}
	if (FileWritingBuffer_writeUInt8(&pSelf->sWritingBuffer, pData->gpsDateTime.month) != FileWritingBuffer_Status_OK){
		return DataSaver_Status_Error;
	}
	if (FileWritingBuffer_writeUInt8(&pSelf->sWritingBuffer, pData->gpsDateTime.day) != FileWritingBuffer_Status_OK){
		return DataSaver_Status_Error;
	}
	if (FileWritingBuffer_writeUInt8(&pSelf->sWritingBuffer, pData->gpsDateTime.hour) != FileWritingBuffer_Status_OK){
		return DataSaver_Status_Error;
	}
	if (FileWritingBuffer_writeUInt8(&pSelf->sWritingBuffer, pData->gpsDateTime.minute) != FileWritingBuffer_Status_OK){
		return DataSaver_Status_Error;
	}
	if (FileWritingBuffer_writeUInt8(&pSelf->sWritingBuffer, pData->gpsDateTime.second) != FileWritingBuffer_Status_OK){
		return DataSaver_Status_Error;
	}

	if (FileWritingBuffer_writeFixedPoint32(&pSelf->sWritingBuffer, pData->longitude) != FileWritingBuffer_Status_OK){
		return DataSaver_Status_Error;
	}
	if (FileWritingBuffer_writeFixedPoint32(&pSelf->sWritingBuffer, pData->latitude) != FileWritingBuffer_Status_OK){
		return DataSaver_Status_Error;
	}
	if (FileWritingBuffer_writeUInt8(&pSelf->sWritingBuffer, pData->nSatellites) != FileWritingBuffer_Status_OK){
		return DataSaver_Status_Error;
	}
	if (FileWritingBuffer_writeFixedPoint32(&pSelf->sWritingBuffer, pData->altitude) != FileWritingBuffer_Status_OK){
		return DataSaver_Status_Error;
	}
	if (FileWritingBuffer_writeFixedPoint32(&pSelf->sWritingBuffer, pData->speed) != FileWritingBuffer_Status_OK){
		return DataSaver_Status_Error;
	}
	if (FileWritingBuffer_writeFixedPoint32(&pSelf->sWritingBuffer, pData->trackAngle) != FileWritingBuffer_Status_OK){
		return DataSaver_Status_Error;
	}
	if (FileWritingBuffer_writeUInt8(&pSelf->sWritingBuffer, pData->fixType) != FileWritingBuffer_Status_OK){
		return DataSaver_Status_Error;
	}
	if (FileWritingBuffer_writeFixedPoint32(&pSelf->sWritingBuffer, pData->horizontalPrecision) != FileWritingBuffer_Status_OK){
		return DataSaver_Status_Error;
	}
	if (FileWritingBuffer_writeFixedPoint32(&pSelf->sWritingBuffer, pData->verticalPrecision) != FileWritingBuffer_Status_OK){
		return DataSaver_Status_Error;
	}

	if (FileWritingBuffer_writeToFileSystem(&pSelf->sWritingBuffer) != FileWritingBuffer_Status_OK){
		return DataSaver_Status_Error;
	}

	return DataSaver_Status_OK;
}

static DataSaver_Status_TypeDef DataSaver_saveHeader(DataSaver_TypeDef* pSelf, DateTime_TypeDef dateTime){

	if (ConfigDataManager_writeConfig(pSelf->pConfigManager, &(pSelf->sWritingBuffer)) != ConfigDataManager_Status_OK){
		return DataSaver_Status_ConfigError;
	}

	if (FileWritingBuffer_writeUInt16(&pSelf->sWritingBuffer, dateTime.year) != FileWritingBuffer_Status_OK){
		return DataSaver_Status_Error;
	}
	if (FileWritingBuffer_writeUInt8(&pSelf->sWritingBuffer, dateTime.month) != FileWritingBuffer_Status_OK){
		return DataSaver_Status_Error;
	}
	if (FileWritingBuffer_writeUInt8(&pSelf->sWritingBuffer, dateTime.day) != FileWritingBuffer_Status_OK){
		return DataSaver_Status_Error;
	}
	if (FileWritingBuffer_writeUInt8(&pSelf->sWritingBuffer, dateTime.hour) != FileWritingBuffer_Status_OK){
		return DataSaver_Status_Error;
	}
	if (FileWritingBuffer_writeUInt8(&pSelf->sWritingBuffer, dateTime.minute) != FileWritingBuffer_Status_OK){
		return DataSaver_Status_Error;
	}
	if (FileWritingBuffer_writeUInt8(&pSelf->sWritingBuffer, dateTime.second) != FileWritingBuffer_Status_OK){
		return DataSaver_Status_Error;
	}
	if (FileWritingBuffer_writeToFileSystem(&pSelf->sWritingBuffer) != FileWritingBuffer_Status_OK){
		return DataSaver_Status_Error;
	}

	return DataSaver_Status_OK;

}
