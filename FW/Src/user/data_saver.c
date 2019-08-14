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

DataSaver_Status_TypeDef DataSaver_init(DataSaver_TypeDef* pSelf, Config_TypeDef* pConfig, FileSystemWrapper_TypeDef* pFileSystemHandler){

	if (pSelf->state != DataSaver_State_UnInitialized){
		return DataSaver_Status_Error;
	}

	pSelf->pConfig				= pConfig;
	pSelf->pFileSystemHandler	= pFileSystemHandler;

	pSelf->state				= DataSaver_State_Initialized;

	return DataSaver_Status_OK;

}

DataSaver_Status_TypeDef DataSaver_startLogging(DataSaver_TypeDef* pSelf, DateTime_TypeDef dateTime){

	if (pSelf->state == DataSaver_State_OpenedFile){
		return DataSaver_Status_AlreadyOpenedFileError;
	}

	if (pSelf->state != DataSaver_State_Initialized){
		return DataSaver_Status_UnInitializedError;
	}

	//prepare name of file
	uint16_t offset = 0;
	char filename[LOG_FILENAME_MAX_LENGTH];
	memset(filename, 0, LOG_FILENAME_MAX_LENGTH);
	strcpy(filename, LOG_FILE_PREFIX);
	offset += strlen(LOG_FILE_PREFIX);
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

DataSaver_Status_TypeDef DataSaver_stopLogging(DataSaver_TypeDef* pSelf){

	if (pSelf->state != DataSaver_State_OpenedFile){
		return DataSaver_Status_FileNotOpenedError;
	}

	if (FileWritingBuffer_flush(&pSelf->sWritingBuffer) != FileWritingBuffer_Status_OK){
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

	for (uint8_t i=0; i<pData->DLC; i++){
		if (FileWritingBuffer_writeUInt8(&pSelf->sWritingBuffer, pData->Data[i]) != FileWritingBuffer_Status_OK){
			return DataSaver_Status_Error;
		}
	}

	if (FileWritingBuffer_flush(&pSelf->sWritingBuffer) != FileWritingBuffer_Status_OK){
		return DataSaver_Status_Error;
	}

	return DataSaver_Status_OK;
}

DataSaver_Status_TypeDef DataSaver_writeGPSData(DataSaver_TypeDef* pSelf, GPSData_TypeDef* pData){

}

#define	DATA_SAVER_HEADER_BUFFER_SIZE	16
#define	DATA_SAVER_VERSION_BYTES_COUNT	16

static DataSaver_Status_TypeDef DataSaver_saveHeader(DataSaver_TypeDef* pSelf, DateTime_TypeDef dateTime){

	if (FileWritingBuffer_writeUInt16(&pSelf->sWritingBuffer, LOG_FILE_VERSION) != FileWritingBuffer_Status_OK){
		return DataSaver_Status_Error;
	}
	if (FileWritingBuffer_writeUInt16(&pSelf->sWritingBuffer, LOG_FILE_SUBVERSION) != FileWritingBuffer_Status_OK){
		return DataSaver_Status_Error;;
	}
	if (FileWritingBuffer_writeUInt16(&pSelf->sWritingBuffer, pSelf->pConfig->num_of_frames) != FileWritingBuffer_Status_OK){
		return DataSaver_Status_Error;;
	}

	for (uint16_t frameNr=0; frameNr < pSelf->pConfig->num_of_frames; frameNr++){

		if (FileWritingBuffer_writeUInt16(&pSelf->sWritingBuffer, pSelf->pConfig->frames[frameNr].ID) != FileWritingBuffer_Status_OK){
			return DataSaver_Status_Error;
		}

		if (FileWritingBuffer_writeUInt8(&pSelf->sWritingBuffer, pSelf->pConfig->frames[frameNr].DLC) != FileWritingBuffer_Status_OK){
			return DataSaver_Status_Error;
		}

		if (FileWritingBuffer_writeString(&pSelf->sWritingBuffer, pSelf->pConfig->frames[frameNr].moduleName, CONFIG_NAMES_LENGTH) != FileWritingBuffer_Status_OK){
			return DataSaver_Status_Error;
		}

		for (uint8_t bytesCounter=0, channelNo=0; bytesCounter<pSelf->pConfig->frames[frameNr].DLC; channelNo++){

			if (FileWritingBuffer_writeUInt8(&pSelf->sWritingBuffer, pSelf->pConfig->frames[frameNr].channels[channelNo].valueType) != FileWritingBuffer_Status_OK){
				return DataSaver_Status_Error;
			}
			if (FileWritingBuffer_writeUInt16(&pSelf->sWritingBuffer, pSelf->pConfig->frames[frameNr].channels[channelNo].multiplier) != FileWritingBuffer_Status_OK){
				return DataSaver_Status_Error;
			}
			if (FileWritingBuffer_writeUInt16(&pSelf->sWritingBuffer, pSelf->pConfig->frames[frameNr].channels[channelNo].divider) != FileWritingBuffer_Status_OK){
				return DataSaver_Status_Error;
			}
			if (FileWritingBuffer_writeUInt16(&pSelf->sWritingBuffer, pSelf->pConfig->frames[frameNr].channels[channelNo].offset) != FileWritingBuffer_Status_OK){
				return DataSaver_Status_Error;
			}

			if (FileWritingBuffer_writeString(&pSelf->sWritingBuffer, pSelf->pConfig->frames[frameNr].channels[channelNo].description, CONFIG_NAMES_LENGTH) != FileWritingBuffer_Status_OK){
				return DataSaver_Status_Error;
			}
			if (FileWritingBuffer_writeString(&pSelf->sWritingBuffer, pSelf->pConfig->frames[frameNr].channels[channelNo].unit, CONFIG_NAMES_LENGTH) != FileWritingBuffer_Status_OK){
				return DataSaver_Status_Error;
			}
			if (FileWritingBuffer_writeString(&pSelf->sWritingBuffer, pSelf->pConfig->frames[frameNr].channels[channelNo].comment, CONFIG_NAMES_LENGTH) != FileWritingBuffer_Status_OK){
				return DataSaver_Status_Error;
			}


			if ((pSelf->pConfig->frames[frameNr].channels[channelNo].valueType & CONFIG_16_BIT_TYPE_flag) == 0){
				bytesCounter += 1;
			} else {
				bytesCounter += 2;
			}
		}
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
	if (FileWritingBuffer_flush(&pSelf->sWritingBuffer) != FileWritingBuffer_Status_OK){
		return DataSaver_Status_Error;
	}

	return DataSaver_Status_OK;

}
