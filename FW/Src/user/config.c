/*
 * config_data.c
 *
 *  Created on: 12.11.2017
 *      Author: Michal Kowalik
 */

#include "user/file_system_wrapper.h"
#include "user/config.h"
#include "user/utils.h"

#include "string.h"
#include "stdbool.h"

//< ----- Private functions prototypes ----- >//

static ConfigDataManager_Status_TypeDef _ConfigDataManager_openConfigFile(ConfigDataManager_TypeDef* pSelf);
static ConfigDataManager_Status_TypeDef _ConfigDataManager_validateCorrectFrameID(ConfigDataManager_TypeDef* pSelf, uint16_t id);
static ConfigDataManager_Status_TypeDef _ConfigDataManager_validateFrameDLC(ConfigDataManager_TypeDef* pSelf, uint8_t dlc);
static ConfigDataManager_Status_TypeDef _ConfigDataManager_validateSignalDefinition(ConfigDataManager_TypeDef* pSelf, ConfigFrame_TypeDef* pFrame, ConfigSignal_TypeDef* pSignal);
static ConfigDataManager_Status_TypeDef _ConfigDataManager_validateGPSFrequency(ConfigDataManager_TypeDef* pSelf);
static ConfigDataManager_Status_TypeDef _ConfigDataManager_validateCANSpeed(ConfigDataManager_TypeDef* pSelf);
static bool								_ConfigDataManager_isSignalUsedForOperator(Config_TrigerCompareOperator_TypeDef compareOperator);
static bool								_ConfigDataManager_isConstCompareValueUsedForOperator(Config_TrigerCompareOperator_TypeDef compareOperator);
static ConfigDataManager_Status_TypeDef _ConfigDataManager_validateTrigger(ConfigDataManager_TypeDef* pSelf, Config_Trigger_TypeDef* pTrigger);

static ConfigDataManager_Status_TypeDef _ConfigDataManager_getFreeSignalListElem(ConfigDataManager_TypeDef* pSelf, ConfigSignalListElem_TypeDef** ppRetSignalListElem);

static ConfigDataManager_Status_TypeDef _ConfigDataManager_readConfigFilePreambule(ConfigDataManager_TypeDef* pSelf);
static ConfigDataManager_Status_TypeDef _ConfigDataManager_readFrameDefinition(ConfigDataManager_TypeDef* pSelf, ConfigFrame_TypeDef* pFrame);
static ConfigDataManager_Status_TypeDef _ConfigDataManager_readSignalDefinition(ConfigDataManager_TypeDef* pSelf, ConfigFrame_TypeDef* pFrame, ConfigSignal_TypeDef* pSignal);
static ConfigDataManager_Status_TypeDef _ConfigDataManager_readSingleTriggerDefinition(ConfigDataManager_TypeDef* pSelf, Config_Trigger_TypeDef* pTrigger);
static ConfigDataManager_Status_TypeDef _ConfigDataManager_readTriggersDefinitions(ConfigDataManager_TypeDef* pSelf);

static ConfigDataManager_Status_TypeDef _ConfigDataManager_writeConfigFilePreambule(ConfigDataManager_TypeDef* pSelf, FileWritingBuffer_TypeDef* pWritingBuffer);
static ConfigDataManager_Status_TypeDef _ConfigDataManager_writeFrameDefinition(ConfigDataManager_TypeDef* pSelf, FileWritingBuffer_TypeDef* pWritingBuffer, ConfigFrame_TypeDef* pFrame);
static ConfigDataManager_Status_TypeDef _ConfigDataManager_writeSignalDefinition(ConfigDataManager_TypeDef* pSelf, FileWritingBuffer_TypeDef* pWritingBuffer, ConfigSignal_TypeDef* pSignal);
static ConfigDataManager_Status_TypeDef _ConfigDataManager_writeSingleTriggerDefinition(ConfigDataManager_TypeDef* pSelf, FileWritingBuffer_TypeDef* pWritingBuffer, Config_Trigger_TypeDef* pTrigger);
static ConfigDataManager_Status_TypeDef _ConfigDataManager_writeTriggersDefinitions(ConfigDataManager_TypeDef* pSelf, FileWritingBuffer_TypeDef* pWritingBuffer);

//< ----- Public functions ----- >//

ConfigDataManager_Status_TypeDef ConfigDataManager_init(ConfigDataManager_TypeDef* pSelf, FileSystemWrapper_TypeDef* pFileSystem){

	if (pSelf == NULL || pFileSystem == NULL){
		return ConfigDataManager_Status_NullPointerError;
	}

	pSelf->state			= ConfigDataManager_State_DuringInit;
	pSelf->pFileSystem		= pFileSystem;
	pSelf->signalsCounter	= 0;


	memset(&pSelf->sConfig, 0, sizeof(pSelf->sConfig));
	memset(&pSelf->sConfigFileHandler, 0, sizeof(pSelf->sConfigFileHandler));
	memset(&pSelf->sReadingBuffer, 0, sizeof(pSelf->sReadingBuffer));
	memset(&pSelf->signalsListElemsPoll, 0, sizeof(pSelf->signalsListElemsPoll));

	pSelf->state 			= ConfigDataManager_State_Initialized;

	return ConfigDataManager_Status_OK;
}

ConfigDataManager_Status_TypeDef ConfigDataManager_readConfig(ConfigDataManager_TypeDef* pSelf){
	ConfigDataManager_Status_TypeDef	ret		= ConfigDataManager_Status_OK;

	if (pSelf == NULL){
		return ConfigDataManager_Status_NullPointerError;
	}

	if (pSelf->state == ConfigDataManager_State_UnInitialized || pSelf->state == ConfigDataManager_State_DuringInit){
		return ConfigDataManager_Status_NotInitialisedError;
	}

	if ((ret = _ConfigDataManager_openConfigFile(pSelf)) != ConfigDataManager_Status_OK){
		return ret;
	}

	pSelf->state =	ConfigDataManager_State_DuringConfigReading;

	if ((ret = _ConfigDataManager_readConfigFilePreambule(pSelf)) != ConfigDataManager_Status_OK){
		return ret;
	}

	for (uint16_t frameNr=0; frameNr<pSelf->sConfig.numOfFrames; frameNr++){

		ConfigFrame_TypeDef* pFrame	= &(pSelf->sConfig.canFrames[frameNr]);

		if ((ret = _ConfigDataManager_readFrameDefinition(pSelf, pFrame)) != ConfigDataManager_Status_OK){
			return ret;
		}

		pSelf->sConfig.canFramesByID[pFrame->ID] = pFrame;
	}

	if ((ret = _ConfigDataManager_readTriggersDefinitions(pSelf)) != ConfigDataManager_Status_OK){
		return ret;
	}

	pSelf->state	= ConfigDataManager_State_ConfigRead;

	return ConfigDataManager_Status_OK;
}

ConfigDataManager_Status_TypeDef ConfigDataManager_getConfigPointer(ConfigDataManager_TypeDef* pSelf, Config_TypeDef** ppRetConfig){

	if (pSelf == NULL || ppRetConfig == NULL){
		return ConfigDataManager_Status_NullPointerError;
	}

	if (pSelf->state == ConfigDataManager_State_UnInitialized || pSelf->state == ConfigDataManager_State_DuringInit){
		return ConfigDataManager_Status_NotInitialisedError;
	}

	if (pSelf->state != ConfigDataManager_State_ConfigRead){
		return ConfigDataManager_Status_ConfigNotReadError;
	}

	*(ppRetConfig) = &(pSelf->sConfig);

	return ConfigDataManager_Status_OK;
}

ConfigDataManager_Status_TypeDef ConfigDataManager_getIDsList(ConfigDataManager_TypeDef* pSelf, uint16_t* pRetIDsBuffer, uint16_t bufferSize, uint16_t* pIDsWritten){

	if (pSelf == NULL || pRetIDsBuffer == NULL || pIDsWritten == NULL){
		return ConfigDataManager_Status_NullPointerError;
	}

	if (pSelf->state == ConfigDataManager_State_UnInitialized || pSelf->state == ConfigDataManager_State_DuringInit){
		return ConfigDataManager_Status_NotInitialisedError;
	}

	if (pSelf->state != ConfigDataManager_State_ConfigRead){
		return ConfigDataManager_Status_ConfigNotReadError;
	}

	for (uint16_t i=0; i<MIN(bufferSize, pSelf->sConfig.numOfFrames); i++){

		pRetIDsBuffer[i] = pSelf->sConfig.canFrames[i].ID;

	}

	(*pIDsWritten) = MIN(bufferSize, pSelf->sConfig.numOfFrames);

	return ConfigDataManager_Status_OK;

}

ConfigDataManager_Status_TypeDef ConfigDataManager_findFrmae(ConfigDataManager_TypeDef* pSelf, uint16_t frameID, ConfigFrame_TypeDef** pRetFrame){

	if (pSelf == NULL || pRetFrame == NULL){
		return ConfigDataManager_Status_NullPointerError;
	}

	if (pSelf->state == ConfigDataManager_State_UnInitialized || pSelf->state == ConfigDataManager_State_DuringInit){
		return ConfigDataManager_Status_NotInitialisedError;
	}

	if ((pSelf->state != ConfigDataManager_State_ConfigRead) && (pSelf->state != ConfigDataManager_State_DuringConfigReading)){
		return ConfigDataManager_Status_ConfigNotReadError;
	}

	if (_ConfigDataManager_validateCorrectFrameID(pSelf, frameID) != ConfigDataManager_Status_FrameIDPreviouslyUsed){
		return ConfigDataManager_Status_WrongFrameIDError;
	}

	(*pRetFrame) = pSelf->sConfig.canFramesByID[frameID];

	return ConfigDataManager_Status_OK;
}


ConfigDataManager_Status_TypeDef ConfigDataManager_findSignal(ConfigDataManager_TypeDef* pSelf, uint16_t frameID, uint16_t signalID, ConfigSignal_TypeDef** pRetSignal){

	if (pSelf == NULL || pRetSignal == NULL){
		return ConfigDataManager_Status_NullPointerError;
	}

	if (pSelf->state == ConfigDataManager_State_UnInitialized || pSelf->state == ConfigDataManager_State_DuringInit){
		return ConfigDataManager_Status_NotInitialisedError;
	}

	if ((pSelf->state != ConfigDataManager_State_ConfigRead) && (pSelf->state != ConfigDataManager_State_DuringConfigReading)){
		return ConfigDataManager_Status_ConfigNotReadError;
	}

	if (_ConfigDataManager_validateCorrectFrameID(pSelf, frameID) != ConfigDataManager_Status_FrameIDPreviouslyUsed){
		return ConfigDataManager_Status_WrongFrameIDError;
	}

	ConfigFrame_TypeDef*	pFrame = pSelf->sConfig.canFramesByID[frameID];
	if (pFrame == NULL){
		return ConfigDataManager_Status_WrongFrameIDError;
	}

	ConfigSignalListElem_TypeDef* pNextSignalListElem = pFrame->pSignalsListHead;
	while (pNextSignalListElem != NULL && pNextSignalListElem->signal.signalID != signalID){
		pNextSignalListElem = pNextSignalListElem->pNext;
	}

	if (pNextSignalListElem == NULL){
		return ConfigDataManager_Status_WrongSignalIDError;
	} else {
		*pRetSignal = &(pNextSignalListElem->signal);
	}

	return ConfigDataManager_Status_OK;
}

ConfigDataManager_Status_TypeDef ConfigDataManager_writeConfig(ConfigDataManager_TypeDef* pSelf, FileWritingBuffer_TypeDef* pWritingBuffer){
	ConfigDataManager_Status_TypeDef ret = ConfigDataManager_Status_OK;

	if ((ret = _ConfigDataManager_writeConfigFilePreambule(pSelf, pWritingBuffer)) != ConfigDataManager_Status_OK){
		return ret;
	}

	for (uint16_t i = 0; i<pSelf->sConfig.numOfFrames; i++){
		if ((ret = _ConfigDataManager_writeFrameDefinition(pSelf, pWritingBuffer, &(pSelf->sConfig.canFrames[i]))) != ConfigDataManager_Status_OK){
			return ret;
		}
	}

	if ((ret = _ConfigDataManager_writeTriggersDefinitions(pSelf, pWritingBuffer)) != ConfigDataManager_Status_OK){
		return ret;
	}

	return ConfigDataManager_Status_OK;
}

//< ----- Private functions ----- >//


static ConfigDataManager_Status_TypeDef _ConfigDataManager_openConfigFile(ConfigDataManager_TypeDef* pSelf){

	FileSystemWrapper_Status_TypeDef	status	= FileSystemWrapper_Status_OK;

	status = FileSystemWrapper_open(pSelf->pFileSystem, &(pSelf->sConfigFileHandler), CONFIG_FILENAME);

	if (status == FileSystemWrapper_Status_NO_FILE){
		return ConfigDataManager_Status_NoConfigFileError;
	} else if (status != FileSystemWrapper_Status_OK){
		return ConfigDataManager_Status_Error;
	}

	if (FileReadingBuffer_init(&pSelf->sReadingBuffer, &pSelf->sConfigFileHandler) != FileReadingBuffer_Status_OK){
		return ConfigDataManager_Status_FileReadingBufferError;
	}

	return ConfigDataManager_Status_OK;
}

static ConfigDataManager_Status_TypeDef _ConfigDataManager_validateCorrectFrameID(ConfigDataManager_TypeDef* pSelf, uint16_t id){

	if (id >= CONFIG_ID_NUMBER){
		return ConfigDataManager_Status_WrongFrameIDError;
	}

	if (pSelf->sConfig.canFramesByID[id] != NULL){
		return ConfigDataManager_Status_FrameIDPreviouslyUsed;
	}

	return ConfigDataManager_Status_OK;
}


static ConfigDataManager_Status_TypeDef _ConfigDataManager_validateFrameDLC(ConfigDataManager_TypeDef* pSelf, uint8_t dlc){

	if (dlc > CONFIG_MAX_FRAME_DLC_VALUE){
		return ConfigDataManager_Status_WrongFrameDLCError;
	}

	return ConfigDataManager_Status_OK;
}

static ConfigDataManager_Status_TypeDef _ConfigDataManager_validateSignalDefinition(ConfigDataManager_TypeDef* pSelf, ConfigFrame_TypeDef* pFrame, ConfigSignal_TypeDef* pSignal){

	if (pSignal->pFrame != pFrame){
		return ConfigDataManager_Status_WrongSignalDefinitionError;
	}

	ConfigSignalListElem_TypeDef* pNextSignalListElem = pFrame->pSignalsListHead;
	while (pNextSignalListElem != NULL && pNextSignalListElem->signal.signalID != pSignal->signalID){
		pNextSignalListElem = pNextSignalListElem->pNext;
	}
	if (pNextSignalListElem != NULL){
		return ConfigDataManager_Status_WrongSignalIDError;
	}

	if (pSignal->lengthBits > CONFIG_MAX_SIGNAL_LENGTH_BITS_VALUE){
		return ConfigDataManager_Status_WrongSignalLengthError;
	}

	if (pSignal->startBit + pSignal->lengthBits > (pFrame->expectedDLC * 8)){
		return ConfigDataManager_Status_WrongSignalLengthError;
	}

	if (pSignal->valueType_unused != 0){
		return ConfigDataManager_Status_WrongSignalDefinitionError;
	}

	return ConfigDataManager_Status_OK;
}


static ConfigDataManager_Status_TypeDef _ConfigDataManager_validateGPSFrequency(ConfigDataManager_TypeDef* pSelf){

	switch (pSelf->sConfig.gpsFrequency){
	case Config_GPSFrequency_OFF:
	case Config_GPSFrequency_0_5Hz:
	case Config_GPSFrequency_1Hz:
	case Config_GPSFrequency_2Hz:
	case Config_GPSFrequency_5Hz:
	case Config_GPSFrequency_10Hz:
		return ConfigDataManager_Status_OK;
	default:
		return ConfigDataManager_Status_WrongGPSFrequencyError;
	}
}


static ConfigDataManager_Status_TypeDef _ConfigDataManager_validateCANSpeed(ConfigDataManager_TypeDef* pSelf){

	switch (pSelf->sConfig.canBitrate){
	case Config_CANBitrate_50kbps:
	case Config_CANBitrate_125kbps:
	case Config_CANBitrate_250kbps:
	case Config_CANBitrate_500kbps:
	case Config_CANBitrate_1Mbps:
		return ConfigDataManager_Status_OK;
	default:
		return ConfigDataManager_Status_WrongCANBitrateError;
	}
}

static bool _ConfigDataManager_isSignalUsedForOperator(Config_TrigerCompareOperator_TypeDef compareOperator){
	return ((compareOperator != Config_TrigerCompareOperator_FRAME_OCCURED) && (compareOperator != Config_TrigerCompareOperator_FRAME_TIMEOUT_MS));
}

static bool _ConfigDataManager_isConstCompareValueUsedForOperator(Config_TrigerCompareOperator_TypeDef compareOperator){
	return (compareOperator != Config_TrigerCompareOperator_FRAME_TIMEOUT_MS);
}

static ConfigDataManager_Status_TypeDef _ConfigDataManager_validateTrigger(ConfigDataManager_TypeDef* pSelf, Config_Trigger_TypeDef* pTrigger){

	if (_ConfigDataManager_isSignalUsedForOperator(pTrigger->compareOperator)){
		if (pTrigger->pSignal == NULL){
			return ConfigDataManager_Status_WrongTriggerDefinitionError;
		}
	} else {
		if (pTrigger->pFrame == NULL){
			return ConfigDataManager_Status_WrongTriggerDefinitionError;
		}
	}

	switch (pTrigger->compareOperator){
	case Config_TrigerCompareOperator_EQUAL:
	case Config_TrigerCompareOperator_NOT_EQUAL:
	case Config_TrigerCompareOperator_GREATER:
	case Config_TrigerCompareOperator_GREATER_OR_EQUAL:
	case Config_TrigerCompareOperator_LESS:
	case Config_TrigerCompareOperator_LESS_OR_EQUAL:
	case Config_TrigerCompareOperator_BITWISE_AND:
	case Config_TrigerCompareOperator_BITWISE_OR:
	case Config_TrigerCompareOperator_BITWISE_XOR:
	case Config_TrigerCompareOperator_FRAME_OCCURED:
	case Config_TrigerCompareOperator_FRAME_TIMEOUT_MS:
		break;
	default:
		return ConfigDataManager_Status_WrongTriggerDefinitionError;
	}

	return ConfigDataManager_Status_OK;
}


static ConfigDataManager_Status_TypeDef _ConfigDataManager_getFreeSignalListElem(ConfigDataManager_TypeDef* pSelf, ConfigSignalListElem_TypeDef** ppRetSignalListElem){

	if (pSelf->signalsCounter < CONFIG_MAX_SIGNALS_NUMBER){
		(*ppRetSignalListElem)		= &(pSelf->signalsListElemsPoll[pSelf->signalsCounter++]);
	} else {
		return ConfigDataManager_Status_SignalsBufferOverflowError;
	}
	(*ppRetSignalListElem)->pNext	= NULL;

	return ConfigDataManager_Status_OK;
}


static ConfigDataManager_Status_TypeDef _ConfigDataManager_readConfigFilePreambule(ConfigDataManager_TypeDef* pSelf){
	ConfigDataManager_Status_TypeDef	ret		= ConfigDataManager_Status_OK;

	if (FileReadingBuffer_readUInt16(&pSelf->sReadingBuffer, &pSelf->sConfig.version) != FileReadingBuffer_Status_OK){
		return ConfigDataManager_Status_FileReadingBufferError;
	}
	if (FileReadingBuffer_readUInt16(&pSelf->sReadingBuffer, &pSelf->sConfig.subversion) != FileReadingBuffer_Status_OK){
		return ConfigDataManager_Status_FileReadingBufferError;
	}

	if ((pSelf->sConfig.version != CONFIG_FILE_USED_VERSION) || (pSelf->sConfig.subversion != CONFIG_FILE_USED_SUBVERSION)){
		return ConfigDataManager_Status_ConfigFileWrongVersionError;
	}

	if (FileReadingBuffer_readString(&pSelf->sReadingBuffer, pSelf->sConfig.logFileName, CONFIG_NAMES_LENGTH) != FileReadingBuffer_Status_OK){
		return ConfigDataManager_Status_FileReadingBufferError;
	}

	if (FileReadingBuffer_readUInt16(&pSelf->sReadingBuffer, &pSelf->sConfig.canBitrate) != FileReadingBuffer_Status_OK){
		return ConfigDataManager_Status_FileReadingBufferError;
	}
	if ((ret = _ConfigDataManager_validateCANSpeed(pSelf)) != ConfigDataManager_Status_OK){
		return ret;
	}

	if (FileReadingBuffer_readUInt8(&pSelf->sReadingBuffer, &pSelf->sConfig.gpsFrequency) != FileReadingBuffer_Status_OK){
		return ConfigDataManager_Status_FileReadingBufferError;
	}
	if ((ret = _ConfigDataManager_validateGPSFrequency(pSelf)) != ConfigDataManager_Status_OK){
		return ret;
	}

	if (FileReadingBuffer_readUInt16(&pSelf->sReadingBuffer, &pSelf->sConfig.rtcConfigurationFrameID) != FileReadingBuffer_Status_OK){
		return ConfigDataManager_Status_FileReadingBufferError;
	}
	if ((ret = _ConfigDataManager_validateCorrectFrameID(pSelf, pSelf->sConfig.rtcConfigurationFrameID)) != ConfigDataManager_Status_OK){
		return ret;
	}

	if (FileReadingBuffer_readUInt16(&pSelf->sReadingBuffer, &pSelf->sConfig.numOfFrames) != FileReadingBuffer_Status_OK){
		return ConfigDataManager_Status_FileReadingBufferError;
	}

	return ConfigDataManager_Status_OK;
}


static ConfigDataManager_Status_TypeDef _ConfigDataManager_readFrameDefinition(ConfigDataManager_TypeDef* pSelf, ConfigFrame_TypeDef* pFrame){
	ConfigDataManager_Status_TypeDef	ret		= ConfigDataManager_Status_OK;

	if (FileReadingBuffer_readUInt16(&pSelf->sReadingBuffer, &(pFrame->ID)) != FileReadingBuffer_Status_OK){
		return ConfigDataManager_Status_FileReadingBufferError;
	}
	if (FileReadingBuffer_readUInt8(&pSelf->sReadingBuffer, &(pFrame->expectedDLC)) != FileReadingBuffer_Status_OK){
		return ConfigDataManager_Status_FileReadingBufferError;
	}
	if (FileReadingBuffer_readString(&pSelf->sReadingBuffer, pFrame->frameName, CONFIG_NAMES_LENGTH) != FileReadingBuffer_Status_OK){
		return ConfigDataManager_Status_FileReadingBufferError;
	}

	if ((ret = _ConfigDataManager_validateCorrectFrameID(pSelf, pFrame->ID)) != ConfigDataManager_Status_OK){
		return ret;
	}
	if ((ret = _ConfigDataManager_validateFrameDLC(pSelf, pFrame->expectedDLC)) != ConfigDataManager_Status_OK){
		return ret;
	}
	uint16_t signalsNumber = 0;

	if (FileReadingBuffer_readUInt16(&pSelf->sReadingBuffer, &signalsNumber) != FileReadingBuffer_Status_OK){
		return ConfigDataManager_Status_FileReadingBufferError;
	}

	ConfigSignalListElem_TypeDef* pLastSignalListElem = pFrame->pSignalsListHead;

	for (uint16_t signalIt=0; signalIt < signalsNumber; signalIt++){

		ConfigSignalListElem_TypeDef*	pSignalListElem	= NULL;
		if ((ret = _ConfigDataManager_getFreeSignalListElem(pSelf, &pSignalListElem)) != ConfigDataManager_Status_OK){
			return ret;
		}

		if ((ret = _ConfigDataManager_readSignalDefinition(pSelf, pFrame, &(pSignalListElem->signal))) != ConfigDataManager_Status_OK){
			return ret;
		}

		if (pFrame->pSignalsListHead == NULL){
			pFrame->pSignalsListHead	= pSignalListElem;
			pLastSignalListElem			= pSignalListElem;
		} else {
			pLastSignalListElem->pNext	= pSignalListElem;
			pLastSignalListElem			= pSignalListElem;
		}
	}

	return ConfigDataManager_Status_OK;
}


static ConfigDataManager_Status_TypeDef _ConfigDataManager_readSignalDefinition(ConfigDataManager_TypeDef* pSelf, ConfigFrame_TypeDef* pFrame, ConfigSignal_TypeDef* pSignal){
	ConfigDataManager_Status_TypeDef	ret		= ConfigDataManager_Status_OK;

	pSignal->pFrame	= pFrame;

	if (FileReadingBuffer_readUInt16(&pSelf->sReadingBuffer, &(pSignal->signalID)) != FileReadingBuffer_Status_OK){
		return ConfigDataManager_Status_FileReadingBufferError;
	}
	if (FileReadingBuffer_readUInt8(&pSelf->sReadingBuffer, &(pSignal->startBit)) != FileReadingBuffer_Status_OK){
		return ConfigDataManager_Status_FileReadingBufferError;
	}
	if (FileReadingBuffer_readUInt8(&pSelf->sReadingBuffer, &(pSignal->lengthBits)) != FileReadingBuffer_Status_OK){
		return ConfigDataManager_Status_FileReadingBufferError;
	}
	if (FileReadingBuffer_readUInt8(&pSelf->sReadingBuffer, &(pSignal->valueType_raw)) != FileReadingBuffer_Status_OK){
		return ConfigDataManager_Status_FileReadingBufferError;
	}

	if (FileReadingBuffer_readUInt16(&pSelf->sReadingBuffer, &(pSignal->multiplier)) != FileReadingBuffer_Status_OK){
		return ConfigDataManager_Status_FileReadingBufferError;
	}
	if (FileReadingBuffer_readUInt16(&pSelf->sReadingBuffer, &(pSignal->divider)) != FileReadingBuffer_Status_OK){
		return ConfigDataManager_Status_FileReadingBufferError;
	}
	if (FileReadingBuffer_readUInt16(&pSelf->sReadingBuffer, &(pSignal->offset)) != FileReadingBuffer_Status_OK){
		return ConfigDataManager_Status_FileReadingBufferError;
	}

	if (FileReadingBuffer_readString(&pSelf->sReadingBuffer, pSignal->description, CONFIG_NAMES_LENGTH) != FileReadingBuffer_Status_OK){
		return ConfigDataManager_Status_FileReadingBufferError;
	}
	if (FileReadingBuffer_readString(&pSelf->sReadingBuffer, pSignal->unit, CONFIG_NAMES_LENGTH) != FileReadingBuffer_Status_OK){
		return ConfigDataManager_Status_FileReadingBufferError;
	}
	if (FileReadingBuffer_readString(&pSelf->sReadingBuffer, pSignal->comment, CONFIG_NAMES_LENGTH) != FileReadingBuffer_Status_OK){
		return ConfigDataManager_Status_FileReadingBufferError;
	}

	if ((ret = _ConfigDataManager_validateSignalDefinition(pSelf, pFrame, pSignal)) != ConfigDataManager_Status_OK){
		return ret;
	}

	return ConfigDataManager_Status_OK;
}

static ConfigDataManager_Status_TypeDef _ConfigDataManager_readSingleTriggerDefinition(ConfigDataManager_TypeDef* pSelf, Config_Trigger_TypeDef* pTrigger){
	ConfigDataManager_Status_TypeDef	ret		= ConfigDataManager_Status_OK;

	uint16_t frameId	= 0;
	uint16_t signalId	= 0;

	ConfigSignal_TypeDef* pSignal = NULL;

	if (FileReadingBuffer_readString(&pSelf->sReadingBuffer, pTrigger->triggerName, CONFIG_NAMES_LENGTH) != FileReadingBuffer_Status_OK){
		return ConfigDataManager_Status_FileReadingBufferError;
	}

	if (FileReadingBuffer_readUInt16(&pSelf->sReadingBuffer, &frameId) != FileReadingBuffer_Status_OK){
		return ConfigDataManager_Status_FileReadingBufferError;
	}

	if (FileReadingBuffer_readUInt16(&pSelf->sReadingBuffer, &signalId) != FileReadingBuffer_Status_OK){
		return ConfigDataManager_Status_FileReadingBufferError;
	}

	if (FileReadingBuffer_readUInt32(&pSelf->sReadingBuffer, &(pTrigger->compareConstValue)) != FileReadingBuffer_Status_OK){
		return ConfigDataManager_Status_FileReadingBufferError;
	}

	if (FileReadingBuffer_readUInt8(&pSelf->sReadingBuffer, &(pTrigger->compareOperator)) != FileReadingBuffer_Status_OK){
		return ConfigDataManager_Status_FileReadingBufferError;
	}

	if (_ConfigDataManager_isSignalUsedForOperator(pTrigger->compareOperator)){
		if ((ret = ConfigDataManager_findSignal(pSelf, frameId, signalId, &(pTrigger->pSignal))) != ConfigDataManager_Status_OK){
			return ret;
		}
	} else {
		if ((ret = ConfigDataManager_findFrmae(pSelf, frameId, &(pTrigger->pFrame))) != ConfigDataManager_Status_OK){
			return ret;
		}
	}
	if ((ret = _ConfigDataManager_validateTrigger(pSelf, pTrigger)) != ConfigDataManager_Status_OK){
		return ret;
	}

	return ConfigDataManager_Status_OK;
}

static ConfigDataManager_Status_TypeDef _ConfigDataManager_readTriggersDefinitions(ConfigDataManager_TypeDef* pSelf){
	ConfigDataManager_Status_TypeDef	ret		= ConfigDataManager_Status_OK;

	if (FileReadingBuffer_readUInt8(&pSelf->sReadingBuffer, &(pSelf->sConfig.startLogTriggersNumber)) != FileReadingBuffer_Status_OK){
		return ConfigDataManager_Status_FileReadingBufferError;
	}
	if (FileReadingBuffer_readUInt8(&pSelf->sReadingBuffer, &(pSelf->sConfig.stopLogTriggersNumber)) != FileReadingBuffer_Status_OK){
		return ConfigDataManager_Status_FileReadingBufferError;
	}

	for (uint8_t i = 0; i < pSelf->sConfig.startLogTriggersNumber; i++){
		if ((ret = _ConfigDataManager_readSingleTriggerDefinition(pSelf, &(pSelf->sConfig.startLogTriggers[i]))) != ConfigDataManager_Status_OK){
			return ret;
		}
	}
	for (uint8_t i = 0; i < pSelf->sConfig.stopLogTriggersNumber; i++){
		if ((ret = _ConfigDataManager_readSingleTriggerDefinition(pSelf, &(pSelf->sConfig.stopLogTriggers[i]))) != ConfigDataManager_Status_OK){
			return ret;
		}
	}

	return ConfigDataManager_Status_OK;
}



static ConfigDataManager_Status_TypeDef _ConfigDataManager_writeConfigFilePreambule(ConfigDataManager_TypeDef* pSelf, FileWritingBuffer_TypeDef* pWritingBuffer){

	if (FileWritingBuffer_writeUInt16(pWritingBuffer, pSelf->sConfig.version) != FileWritingBuffer_Status_OK){
		return ConfigDataManager_Status_FileWritingBufferError;
	}
	if (FileWritingBuffer_writeUInt16(pWritingBuffer, pSelf->sConfig.subversion) != FileWritingBuffer_Status_OK){
		return ConfigDataManager_Status_FileWritingBufferError;
	}

	if (FileWritingBuffer_writeString(pWritingBuffer, pSelf->sConfig.logFileName, CONFIG_NAMES_LENGTH) != FileWritingBuffer_Status_OK){
		return ConfigDataManager_Status_FileWritingBufferError;
	}

	if (FileWritingBuffer_writeUInt16(pWritingBuffer, pSelf->sConfig.canBitrate) != FileWritingBuffer_Status_OK){
		return ConfigDataManager_Status_FileWritingBufferError;
	}

	if (FileWritingBuffer_writeUInt8(pWritingBuffer, pSelf->sConfig.gpsFrequency) != FileWritingBuffer_Status_OK){
		return ConfigDataManager_Status_FileWritingBufferError;
	}

	if (FileWritingBuffer_writeUInt16(pWritingBuffer, pSelf->sConfig.rtcConfigurationFrameID) != FileWritingBuffer_Status_OK){
		return ConfigDataManager_Status_FileWritingBufferError;
	}

	if (FileWritingBuffer_writeUInt16(pWritingBuffer, pSelf->sConfig.numOfFrames) != FileWritingBuffer_Status_OK){
		return ConfigDataManager_Status_FileWritingBufferError;
	}

	return ConfigDataManager_Status_OK;
}

static ConfigDataManager_Status_TypeDef _ConfigDataManager_writeFrameDefinition(ConfigDataManager_TypeDef* pSelf, FileWritingBuffer_TypeDef* pWritingBuffer, ConfigFrame_TypeDef* pFrame){
	ConfigDataManager_Status_TypeDef	ret		= ConfigDataManager_Status_OK;

	if (FileWritingBuffer_writeUInt16(pWritingBuffer, pFrame->ID) != FileWritingBuffer_Status_OK){
		return ConfigDataManager_Status_FileWritingBufferError;
	}
	if (FileWritingBuffer_writeUInt8(pWritingBuffer, pFrame->expectedDLC) != FileWritingBuffer_Status_OK){
		return ConfigDataManager_Status_FileWritingBufferError;
	}
	if (FileWritingBuffer_writeString(pWritingBuffer, pFrame->frameName, CONFIG_NAMES_LENGTH) != FileWritingBuffer_Status_OK){
		return ConfigDataManager_Status_FileWritingBufferError;
	}

	uint16_t signalsNumber = 0;

	ConfigSignalListElem_TypeDef* pNextSignalListElem = pFrame->pSignalsListHead;
	while (pNextSignalListElem != NULL){
		signalsNumber++;
		pNextSignalListElem = pNextSignalListElem->pNext;
	}

	if (FileWritingBuffer_writeUInt16(pWritingBuffer, signalsNumber) != FileWritingBuffer_Status_OK){
		return ConfigDataManager_Status_FileWritingBufferError;
	}

	pNextSignalListElem = pFrame->pSignalsListHead;
	while (pNextSignalListElem != NULL){

		if ((ret =_ConfigDataManager_writeSignalDefinition(pSelf, pWritingBuffer, &(pNextSignalListElem->signal))) != ConfigDataManager_Status_OK){
			return ret;
		}

		pNextSignalListElem = pNextSignalListElem->pNext;
	}

	return ConfigDataManager_Status_OK;
}

static ConfigDataManager_Status_TypeDef _ConfigDataManager_writeSignalDefinition(ConfigDataManager_TypeDef* pSelf, FileWritingBuffer_TypeDef* pWritingBuffer, ConfigSignal_TypeDef* pSignal){

	if (FileWritingBuffer_writeUInt16(pWritingBuffer, (pSignal->signalID)) != FileWritingBuffer_Status_OK){
		return ConfigDataManager_Status_FileWritingBufferError;
	}
	if (FileWritingBuffer_writeUInt8(pWritingBuffer, (pSignal->startBit)) != FileWritingBuffer_Status_OK){
		return ConfigDataManager_Status_FileWritingBufferError;
	}
	if (FileWritingBuffer_writeUInt8(pWritingBuffer, (pSignal->lengthBits)) != FileWritingBuffer_Status_OK){
		return ConfigDataManager_Status_FileWritingBufferError;
	}
	if (FileWritingBuffer_writeUInt8(pWritingBuffer, (pSignal->valueType_raw)) != FileWritingBuffer_Status_OK){
		return ConfigDataManager_Status_FileWritingBufferError;
	}

	if (FileWritingBuffer_writeUInt16(pWritingBuffer, (pSignal->multiplier)) != FileWritingBuffer_Status_OK){
		return ConfigDataManager_Status_FileWritingBufferError;
	}
	if (FileWritingBuffer_writeUInt16(pWritingBuffer, (pSignal->divider)) != FileWritingBuffer_Status_OK){
		return ConfigDataManager_Status_FileWritingBufferError;
	}
	if (FileWritingBuffer_writeUInt16(pWritingBuffer, (pSignal->offset)) != FileWritingBuffer_Status_OK){
		return ConfigDataManager_Status_FileWritingBufferError;
	}

	if (FileWritingBuffer_writeString(pWritingBuffer, pSignal->description, CONFIG_NAMES_LENGTH) != FileWritingBuffer_Status_OK){
		return ConfigDataManager_Status_FileWritingBufferError;
	}
	if (FileWritingBuffer_writeString(pWritingBuffer, pSignal->unit, CONFIG_NAMES_LENGTH) != FileWritingBuffer_Status_OK){
		return ConfigDataManager_Status_FileWritingBufferError;
	}
	if (FileWritingBuffer_writeString(pWritingBuffer, pSignal->comment, CONFIG_NAMES_LENGTH) != FileWritingBuffer_Status_OK){
		return ConfigDataManager_Status_FileWritingBufferError;
	}

	return ConfigDataManager_Status_OK;
}

static ConfigDataManager_Status_TypeDef _ConfigDataManager_writeTriggersDefinitions(ConfigDataManager_TypeDef* pSelf, FileWritingBuffer_TypeDef* pWritingBuffer){
	ConfigDataManager_Status_TypeDef	ret		= ConfigDataManager_Status_OK;

	if (FileWritingBuffer_writeUInt8(pWritingBuffer, pSelf->sConfig.startLogTriggersNumber) != FileWritingBuffer_Status_OK){
		return ConfigDataManager_Status_FileWritingBufferError;
	}
	if (FileWritingBuffer_writeUInt8(pWritingBuffer, pSelf->sConfig.stopLogTriggersNumber) != FileWritingBuffer_Status_OK){
		return ConfigDataManager_Status_FileWritingBufferError;
	}

	for (uint8_t i = 0; i < pSelf->sConfig.startLogTriggersNumber; i++){
		if ((ret = _ConfigDataManager_writeSingleTriggerDefinition(pSelf, pWritingBuffer, &(pSelf->sConfig.startLogTriggers[i]))) != ConfigDataManager_Status_OK){
			return ret;
		}
	}
	for (uint8_t i = 0; i < pSelf->sConfig.stopLogTriggersNumber; i++){
		if ((ret = _ConfigDataManager_writeSingleTriggerDefinition(pSelf, pWritingBuffer, &(pSelf->sConfig.stopLogTriggers[i]))) != ConfigDataManager_Status_OK){
			return ret;
		}
	}

	return ConfigDataManager_Status_OK;
}

static ConfigDataManager_Status_TypeDef _ConfigDataManager_writeSingleTriggerDefinition(ConfigDataManager_TypeDef* pSelf, FileWritingBuffer_TypeDef* pWritingBuffer, Config_Trigger_TypeDef* pTrigger){


	if (FileWritingBuffer_writeString(pWritingBuffer, pTrigger->triggerName, CONFIG_NAMES_LENGTH) != FileWritingBuffer_Status_OK){
		return ConfigDataManager_Status_FileWritingBufferError;
	}
	if (_ConfigDataManager_isSignalUsedForOperator(pTrigger->compareOperator)){
		if (FileWritingBuffer_writeUInt16(pWritingBuffer, pTrigger->pSignal->pFrame->ID) != FileWritingBuffer_Status_OK){
			return ConfigDataManager_Status_FileWritingBufferError;
		}
		if (FileWritingBuffer_writeUInt16(pWritingBuffer, pTrigger->pSignal->signalID) != FileWritingBuffer_Status_OK){
			return ConfigDataManager_Status_FileWritingBufferError;
		}
	} else {
		if (FileWritingBuffer_writeUInt16(pWritingBuffer, pTrigger->pFrame->ID) != FileWritingBuffer_Status_OK){
			return ConfigDataManager_Status_FileWritingBufferError;
		}
		if (FileWritingBuffer_writeUInt16(pWritingBuffer, 0) != FileWritingBuffer_Status_OK){
			return ConfigDataManager_Status_FileWritingBufferError;
		}
	}

	if (FileWritingBuffer_writeUInt32(pWritingBuffer, (pTrigger->compareConstValue)) != FileWritingBuffer_Status_OK){
		return ConfigDataManager_Status_FileWritingBufferError;
	}

	if (FileWritingBuffer_writeUInt8(pWritingBuffer, (uint8_t)(pTrigger->compareOperator)) != FileWritingBuffer_Status_OK){
		return ConfigDataManager_Status_FileWritingBufferError;
	}

	return ConfigDataManager_Status_OK;
}
