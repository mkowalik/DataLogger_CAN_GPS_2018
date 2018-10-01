/*
 * config_data.c
 *
 *  Created on: 12.11.2017
 *      Author: Kowalik
 */

#include <user/file_system_wrapper.h>
#include "user/config.h"

#define MIN(A,B)  (((A) < (B)) ? (A) : (B))

uint8_t assertCorrectFrame(ConfigDataManager_TypeDef* pSelf, uint8_t id, uint16_t dlc){

	if (id >= CONFIG_ID_NUMBER){
		return 0;
	}

	if (pSelf->sConfig.framesByID[id] != NULL){
		return 0;
	}

	if (dlc > CONFIG_MAX_DLC_VALUE){
		return 0;
	}

	return 1;

}

//TODO dopisac w dokumentacji, ze file system musi byc init, lub zrobic ze init wywolw
ConfigDataManager_Status_TypeDef ConfigDataManager_init(ConfigDataManager_TypeDef* pSelf, FileSystemWrapper_TypeDef* pFileSystem){

	if (pSelf->state != ConfigDataManager_State_UnInitialized){
		return ConfigDataManager_Status_InitError;
	}

	FileSystemWrapper_Status_TypeDef	status;

	status = FileSystemWrapper_open(pFileSystem, &(pSelf->sConfigFileHandler), CONFIG_FILENAME);

	if (status == FileSystemWrapper_Status_NO_FILE){
		return ConfigDataManager_Status_NoConfigFileError;
	} else if (status != FileSystemWrapper_Status_OK){
		return ConfigDataManager_Status_Error;
	}

	if (FileReadingBuffer_init(&pSelf->sReadingBuffer, &pSelf->sConfigFileHandler) != FileReadingBuffer_Status_OK){
		return ConfigDataManager_Status_ConfigFileDataError;
	}

	if (FileReadingBuffer_readUInt16(&pSelf->sReadingBuffer, &pSelf->sConfig.version) != FileReadingBuffer_Status_OK){
		return ConfigDataManager_Status_ConfigFileDataError;
	}
	if (FileReadingBuffer_readUInt16(&pSelf->sReadingBuffer, &pSelf->sConfig.subversion) != FileReadingBuffer_Status_OK){
		return ConfigDataManager_Status_ConfigFileDataError;
	}

	if ((pSelf->sConfig.version != CONFIG_FILE_USED_VERSION) || (pSelf->sConfig.subversion != CONFIG_FILE_USED_SUBVERSION)){
		return ConfigDataManager_Status_ConfigFileWrongVersionError;
	}

	if (FileReadingBuffer_readUInt16(&pSelf->sReadingBuffer, &pSelf->sConfig.num_of_frames) != FileReadingBuffer_Status_OK){
		return ConfigDataManager_Status_ConfigFileDataError;
	}

	for (uint16_t frameNr=0; frameNr<pSelf->sConfig.num_of_frames; frameNr++){

		if (FileReadingBuffer_readUInt16(&pSelf->sReadingBuffer, &pSelf->sConfig.frames[frameNr].ID) != FileReadingBuffer_Status_OK){
			return ConfigDataManager_Status_ConfigFileDataError;
		}
		if (FileReadingBuffer_readUInt8(&pSelf->sReadingBuffer, &pSelf->sConfig.frames[frameNr].DLC) != FileReadingBuffer_Status_OK){
			return ConfigDataManager_Status_ConfigFileDataError;
		}
		if (FileReadingBuffer_readString(&pSelf->sReadingBuffer, pSelf->sConfig.frames[frameNr].moduleName, CONFIG_NAMES_LENGTH) != FileReadingBuffer_Status_OK){
			return ConfigDataManager_Status_ConfigFileDataError;
		}

		if (assertCorrectFrame(pSelf, pSelf->sConfig.frames[frameNr].ID, pSelf->sConfig.frames[frameNr].DLC) == 0){
			return ConfigDataManager_Status_ConfigFileDataError;
		}

		pSelf->sConfig.framesByID[pSelf->sConfig.frames[frameNr].ID] = &(pSelf->sConfig.frames[frameNr]);

		for (uint8_t bytesCounter=0, channelNo=0; bytesCounter<pSelf->sConfig.frames[frameNr].DLC; channelNo++){

			if (FileReadingBuffer_readUInt8(&pSelf->sReadingBuffer, &pSelf->sConfig.frames[frameNr].channels[channelNo].valueType) != FileReadingBuffer_Status_OK){
				return ConfigDataManager_Status_ConfigFileDataError;
			}

			if (FileReadingBuffer_readUInt16(&pSelf->sReadingBuffer, &pSelf->sConfig.frames[frameNr].channels[channelNo].multiplier) != FileReadingBuffer_Status_OK){
				return ConfigDataManager_Status_ConfigFileDataError;
			}
			if (FileReadingBuffer_readUInt16(&pSelf->sReadingBuffer, &pSelf->sConfig.frames[frameNr].channels[channelNo].divider) != FileReadingBuffer_Status_OK){
				return ConfigDataManager_Status_ConfigFileDataError;
			}
			if (FileReadingBuffer_readUInt16(&pSelf->sReadingBuffer, &pSelf->sConfig.frames[frameNr].channels[channelNo].offset) != FileReadingBuffer_Status_OK){
				return ConfigDataManager_Status_ConfigFileDataError;
			}

			if (FileReadingBuffer_readString(&pSelf->sReadingBuffer, pSelf->sConfig.frames[frameNr].channels[channelNo].description, CONFIG_NAMES_LENGTH) != FileReadingBuffer_Status_OK){ //skip channel descritpion
				return ConfigDataManager_Status_ConfigFileDataError;
			}
			if (FileReadingBuffer_readString(&pSelf->sReadingBuffer, pSelf->sConfig.frames[frameNr].channels[channelNo].unit, CONFIG_NAMES_LENGTH) != FileReadingBuffer_Status_OK){ //skip channel unit
				return ConfigDataManager_Status_ConfigFileDataError;
			}
			if (FileReadingBuffer_readString(&pSelf->sReadingBuffer, pSelf->sConfig.frames[frameNr].channels[channelNo].comment, CONFIG_NAMES_LENGTH) != FileReadingBuffer_Status_OK){ //skip channel comment
				return ConfigDataManager_Status_ConfigFileDataError;
			}

			if ((pSelf->sConfig.frames[frameNr].channels[channelNo].valueType & CONFIG_16_BIT_TYPE_flag) == 0){
				bytesCounter += 1;
			} else {
				bytesCounter += 2;
			}
		}
	}

	pSelf->state = ConfigDataManager_State_Initialized;

	return ConfigDataManager_Status_OK;

}

ConfigDataManager_Status_TypeDef ConfigDataManager_getConfigPointer(ConfigDataManager_TypeDef* pSelf, Config_TypeDef** ppRetConfig){

	if (pSelf->state != ConfigDataManager_State_Initialized){
		return ConfigDataManager_Status_NotInitialisedError;
	}

	*(ppRetConfig) = &(pSelf->sConfig);

	return ConfigDataManager_Status_OK;
}

ConfigDataManager_Status_TypeDef ConfigDataManager_getIDsList(ConfigDataManager_TypeDef* pSelf, uint16_t* pRetIDsBuffer, uint16_t bufferSize, uint16_t* pIDsWritten){

	if (pSelf->state != ConfigDataManager_State_Initialized){
		return ConfigDataManager_Status_NotInitialisedError;
	}

	for (uint16_t i=0; i<MIN(bufferSize, pSelf->sConfig.num_of_frames); i++){

		pRetIDsBuffer[i] = pSelf->sConfig.frames[i].ID;

	}

	(*pIDsWritten) = MIN(bufferSize, pSelf->sConfig.num_of_frames);

	return ConfigDataManager_Status_OK;

}

ConfigDataManager_Status_TypeDef ConfigDataManager_checkCorrectnessData(ConfigDataManager_TypeDef* pSelf, CANData_TypeDef* pData){

	if (pSelf->state != ConfigDataManager_State_Initialized){
		return ConfigDataManager_Status_NotInitialisedError;
	}

	if (pSelf->sConfig.framesByID[pData->ID] == NULL){
		return ConfigDataManager_Status_WrongIDError;
	}

	if (pSelf->sConfig.framesByID[pData->ID]->DLC != pData->DLC){
		return ConfigDataManager_Status_WrongDLCError;
	}

	return ConfigDataManager_Status_OK;

}

ConfigDataManager_Status_TypeDef ConfigDataManager_findChannel(ConfigDataManager_TypeDef* pSelf, uint16_t ID, uint8_t offset, ConfigChannel_TypeDef* pRetChannel){

	if (pSelf->state != ConfigDataManager_State_Initialized){
		return ConfigDataManager_Status_NotInitialisedError;
	}

	if (offset > (CONFIG_MAX_DLC_VALUE-1)){
		return ConfigDataManager_Status_WrongOffsetError;
	}

	ConfigFrame_TypeDef* pFrame = pSelf->sConfig.framesByID[ID];

	if (pFrame == NULL){
		return ConfigDataManager_Status_WrongIDError;
	}

	for (uint8_t i=0; i<pFrame->DLC; ){

		if (i > offset){
			return ConfigDataManager_Status_WrongOffsetError;
		} else if (i == offset){
			pRetChannel = &(pFrame->channels[i]);
			return ConfigDataManager_Status_OK;
		}

		if ((pFrame->channels[i].valueType & CONFIG_16_BIT_TYPE_flag) == 0){
			i += 1;
		} else {
			i += 2;
		}
	}

	return ConfigDataManager_Status_WrongOffsetError;

}
