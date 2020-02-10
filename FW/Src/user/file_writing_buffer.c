/*
 * file_writing_buffer.c
 *
 *  Created on: 17.07.2018
 *      Author: Michal Kowalik
 */


#include "user/file_writing_buffer.h"
#include "string.h"

//< ----- Static functions prototypes ----- >//

static FileWritingBuffer_Status_TypeDef _FileWritingBuffer_checkIfFull(FileWritingBuffer_TypeDef* pSelf);

//< ----- Static functions implementations ----- >//

static FileWritingBuffer_Status_TypeDef _FileWritingBuffer_checkIfFull(FileWritingBuffer_TypeDef* pSelf){

	if (pSelf->bytesLeft == 0){

		FileWritingBuffer_Status_TypeDef status;
		if ((status = FileWritingBuffer_writeToFileSystem(pSelf)) != FileWritingBuffer_Status_OK){
			return status;
		}
	}

	return FileWritingBuffer_Status_OK;
}

//< ----- Public functions ----- >//

FileWritingBuffer_Status_TypeDef FileWritingBuffer_init(FileWritingBuffer_TypeDef* pSelf, FileSystemWrapper_File_TypeDef* pFile){

	if ((pSelf == NULL) || (pFile == NULL)) {
		return FileWritingBuffer_Status_NullPointerError;
	}

	pSelf->pFile			= pFile;
	pSelf->bytesBuffered	= 0;
	pSelf->bytesLeft		= 0;

	pSelf->state = FileWritingBuffer_State_Initialized;

	return FileWritingBuffer_Status_OK;
}

FileWritingBuffer_Status_TypeDef FileWritingBuffer_deInit(FileWritingBuffer_TypeDef* pSelf){

	if (pSelf == NULL){
		return FileWritingBuffer_Status_NullPointerError;
	}

	if (pSelf->state == FileWritingBuffer_State_UnInitialized){
		return FileWritingBuffer_Status_UnInitializedError;
	}

	pSelf->state 			= FileWritingBuffer_State_UnInitialized;
	pSelf->pFile			= NULL;
	pSelf->bytesBuffered	= 0;
	pSelf->bytesLeft		= 0;
	memset(pSelf->buffer, 0, FILE_WRITING_BUFFER_SIZE);

	return FileWritingBuffer_Status_OK;
}
FileWritingBuffer_Status_TypeDef FileWritingBuffer_resetBuffer(FileWritingBuffer_TypeDef* pSelf){

	if (pSelf == NULL){
		return FileWritingBuffer_Status_NullPointerError;
	}

	if (pSelf->state == FileWritingBuffer_State_UnInitialized){
		return FileWritingBuffer_Status_UnInitializedError;
	}

	pSelf->bytesBuffered	= 0;
	pSelf->bytesLeft		= 0;

	return FileWritingBuffer_Status_OK;
}

FileWritingBuffer_Status_TypeDef FileWritingBuffer_writeUInt8(FileWritingBuffer_TypeDef* pSelf, uint8_t value){

	if (pSelf == NULL){
		return FileWritingBuffer_Status_NullPointerError;
	}

	if (pSelf->state == FileWritingBuffer_State_UnInitialized){
		return FileWritingBuffer_Status_UnInitializedError;
	}

	FileWritingBuffer_Status_TypeDef status;
	if ((status = _FileWritingBuffer_checkIfFull(pSelf)) != FileWritingBuffer_Status_OK){
		return status;
	}

	pSelf->buffer[pSelf->bytesBuffered] = value;
	pSelf->bytesLeft--;
	pSelf->bytesBuffered++;

	return FileWritingBuffer_Status_OK;
}

FileWritingBuffer_Status_TypeDef FileWritingBuffer_writeUInt16(FileWritingBuffer_TypeDef* pSelf, uint16_t value){

	if (pSelf == NULL){
		return FileWritingBuffer_Status_NullPointerError;
	}

	if (pSelf->state == FileWritingBuffer_State_UnInitialized){
		return FileWritingBuffer_Status_UnInitializedError;
	}

	FileWritingBuffer_Status_TypeDef status;
	if ((status = FileWritingBuffer_writeUInt8(pSelf, (value     ) & 0xFFU)) != FileWritingBuffer_Status_OK){
		return status;
	}
	if ((status = FileWritingBuffer_writeUInt8(pSelf, (value >> 8) & 0xFFU)) != FileWritingBuffer_Status_OK){
		return status;
	}

	return FileWritingBuffer_Status_OK;
}

FileWritingBuffer_Status_TypeDef FileWritingBuffer_writeUInt32(FileWritingBuffer_TypeDef* pSelf, uint32_t value){

	if (pSelf == NULL){
		return FileWritingBuffer_Status_NullPointerError;
	}

	if (pSelf->state == FileWritingBuffer_State_UnInitialized){
		return FileWritingBuffer_Status_UnInitializedError;
	}

	FileWritingBuffer_Status_TypeDef status;
	if ((status = FileWritingBuffer_writeUInt16(pSelf, (value      ) & 0xFFFFU)) != FileWritingBuffer_Status_OK){
		return status;
	}
	if ((status = FileWritingBuffer_writeUInt16(pSelf, (value >> 16) & 0xFFFFU)) != FileWritingBuffer_Status_OK){
		return status;
	}

	return FileWritingBuffer_Status_OK;
}

FileWritingBuffer_Status_TypeDef FileWritingBuffer_writeUInt64(FileWritingBuffer_TypeDef* pSelf, uint64_t value){

	if (pSelf == NULL){
		return FileWritingBuffer_Status_NullPointerError;
	}

	if (pSelf->state == FileWritingBuffer_State_UnInitialized){
		return FileWritingBuffer_Status_UnInitializedError;
	}

	FileWritingBuffer_Status_TypeDef status;
	if ((status = FileWritingBuffer_writeUInt32(pSelf, (value      ) & 0xFFFFFFFFU)) != FileWritingBuffer_Status_OK){
		return status;
	}
	if ((status = FileWritingBuffer_writeUInt32(pSelf, (value >> 32) & 0xFFFFFFFFU)) != FileWritingBuffer_Status_OK){
		return status;
	}

	return FileWritingBuffer_Status_OK;
}

FileWritingBuffer_Status_TypeDef FileWritingBuffer_writeInt8(FileWritingBuffer_TypeDef* pSelf, int8_t value){

	if (pSelf == NULL){
		return FileWritingBuffer_Status_NullPointerError;
	}

	if (pSelf->state == FileWritingBuffer_State_UnInitialized){
		return FileWritingBuffer_Status_UnInitializedError;
	}

	FileWritingBuffer_Status_TypeDef status;
	if ((status = _FileWritingBuffer_checkIfFull(pSelf)) != FileWritingBuffer_Status_OK){
		return status;
	}

	pSelf->buffer[pSelf->bytesBuffered] = *((uint8_t*)&value); //< reinterpret_cast like
	pSelf->bytesLeft--;
	pSelf->bytesBuffered++;

	return FileWritingBuffer_Status_OK;
}

FileWritingBuffer_Status_TypeDef FileWritingBuffer_writeChar(FileWritingBuffer_TypeDef* pSelf, char value){

	if (pSelf == NULL){
		return FileWritingBuffer_Status_NullPointerError;
	}

	if (pSelf->state == FileWritingBuffer_State_UnInitialized){
		return FileWritingBuffer_Status_UnInitializedError;
	}

	FileWritingBuffer_Status_TypeDef status;
	if ((status = FileWritingBuffer_writeUInt8(pSelf, (char)value)) != FileWritingBuffer_Status_OK){
		return status;
	}

	return FileWritingBuffer_Status_OK;
}

FileWritingBuffer_Status_TypeDef FileWritingBuffer_writeString(FileWritingBuffer_TypeDef* pSelf, const char* pStringToWrite, uint16_t stringLength){

	if ((pSelf == NULL) || (pStringToWrite == NULL)){
		return FileWritingBuffer_Status_NullPointerError;
	}

	if (pSelf->state == FileWritingBuffer_State_UnInitialized){
		return FileWritingBuffer_Status_UnInitializedError;
	}

	FileWritingBuffer_Status_TypeDef status;

	for(uint32_t i = 0; i < stringLength; i++){

		if ((status = FileWritingBuffer_writeChar(pSelf, pStringToWrite[i])) != FileWritingBuffer_Status_OK){
			return status;
		}
	}

	return FileWritingBuffer_Status_OK;
}

FileWritingBuffer_Status_TypeDef FileWritingBuffer_writeFixedPoint32(FileWritingBuffer_TypeDef* pSelf, FixedPoint value){

	if (pSelf == NULL){
		return FileWritingBuffer_Status_NullPointerError;
	}

	return FileWritingBuffer_writeUInt32(pSelf, value.integer);
}

FileWritingBuffer_Status_TypeDef FileWritingBuffer_writeToFileSystem(FileWritingBuffer_TypeDef* pSelf){

	if (pSelf == NULL){
		return FileWritingBuffer_Status_NullPointerError;
	}

	if (pSelf->state == FileWritingBuffer_State_UnInitialized){
		return FileWritingBuffer_Status_UnInitializedError;
	}

	if (pSelf->state == FileWritingBuffer_State_Flushing){
		return FileWritingBuffer_Status_OK;
	}

	uint32_t							bytesWrittern;
	if (FileSystemWrapper_writeBinaryData(pSelf->pFile, pSelf->buffer, pSelf->bytesBuffered, &bytesWrittern) != FileSystemWrapper_Status_OK){
		return FileWritingBuffer_Status_Error;
	}
	if (bytesWrittern != pSelf->bytesBuffered){
		return FileWritingBuffer_Status_Error;
	}

	pSelf->bytesBuffered	= 0;
	pSelf->bytesLeft		= FILE_WRITING_BUFFER_SIZE;

	return FileWritingBuffer_Status_OK;
}

FileWritingBuffer_Status_TypeDef FileWritingBuffer_flush(FileWritingBuffer_TypeDef* pSelf) {

	if (pSelf == NULL){
		return FileWritingBuffer_Status_NullPointerError;
	}

	if (pSelf->state == FileWritingBuffer_State_UnInitialized) {
		return FileWritingBuffer_Status_OK;
	}

	if (pSelf->state == FileWritingBuffer_State_Flushing){
		return FileWritingBuffer_Status_FlushingError;
	}

	FileWritingBuffer_Status_TypeDef ret	= FileWritingBuffer_Status_OK;
	if ((ret = FileWritingBuffer_writeToFileSystem(pSelf)) != FileWritingBuffer_Status_OK){
		return ret;
	}

	pSelf->state = FileWritingBuffer_State_Flushing;

	if (FileSystemWrapper_sync(pSelf->pFile) != FileSystemWrapper_Status_OK){
		return FileWritingBuffer_Status_FileSystemError;
	}

	pSelf->state = FileWritingBuffer_State_Initialized;

	return FileWritingBuffer_Status_OK;
}
