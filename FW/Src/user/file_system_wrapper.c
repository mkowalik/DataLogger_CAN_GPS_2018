/*
 * file_system_wrapper.c
 *
 *  Created on: 27.01.2018
 *      Author: Michal Kowalik
 */

#include "user/file_system_wrapper.h"
#include "string.h"

static FileSystemWrapper_Status_TypeDef remapResult(FRESULT res){

	switch (res){
	case FR_OK: 					return FileSystemWrapper_Status_OK;
	case FR_DISK_ERR:				return FileSystemWrapper_Status_DISK_ERR;
	case FR_INT_ERR:				return FileSystemWrapper_Status_INT_ERR;
	case FR_NOT_READY:				return FileSystemWrapper_Status_NOT_READY;
	case FR_NO_FILE:				return FileSystemWrapper_Status_NO_FILE;
	case FR_NO_PATH:				return FileSystemWrapper_Status_NO_PATH;
	case FR_INVALID_NAME:			return FileSystemWrapper_Status_INVALID_NAME;
	case FR_DENIED:					return FileSystemWrapper_Status_DENIED;
	case FR_EXIST:					return FileSystemWrapper_Status_EXIST;
	case FR_INVALID_OBJECT:			return FileSystemWrapper_Status_INVALID_OBJECT;
	case FR_WRITE_PROTECTED:		return FileSystemWrapper_Status_WRITE_PROTECTED;
	case FR_INVALID_DRIVE:			return FileSystemWrapper_Status_INVALID_DRIVE;
	case FR_NOT_ENABLED:			return FileSystemWrapper_Status_NOT_ENABLED;
	case FR_NO_FILESYSTEM:			return FileSystemWrapper_Status_NO_FILESYSTEM;
	case FR_MKFS_ABORTED:			return FileSystemWrapper_Status_MKFS_ABORTED;
	case FR_TIMEOUT:				return FileSystemWrapper_Status_TIMEOUT;
	case FR_LOCKED:					return FileSystemWrapper_Status_LOCKED;
	case FR_NOT_ENOUGH_CORE:		return FileSystemWrapper_Status_NOT_ENOUGH_CORE;
	case FR_TOO_MANY_OPEN_FILES:	return FileSystemWrapper_Status_TOO_MANY_OPEN_FILES;
	case FR_INVALID_PARAMETER:		return FileSystemWrapper_Status_INVALID_PARAMETER;
	default:						return FileSystemWrapper_Status_DISK_ERR;
	}

}

FileSystemWrapper_Status_TypeDef FileSystemWrapper_init(FileSystemWrapper_TypeDef* pSelf){

	if (pSelf == NULL){
		return FileSystemWrapper_Status_NullPointerError;
	}

	FRESULT res = f_mount( (FATFS*) &(pSelf->sFatFS), "", FILESYSTEM_MOUNT_IMMEDIATELY);
	if (res == FR_OK){
		pSelf->state = FileSystemWrapper_State_Initialized;
	} else {
		pSelf->state = FileSystemWrapper_State_NotInitialized;
	}
	return remapResult(res);
}

FileSystemWrapper_Status_TypeDef FileSystemWrapper_open(FileSystemWrapper_TypeDef* pSelf, FileSystemWrapper_File_TypeDef* pRetFile, const char* pFilename){

	if ((pSelf == NULL) || (pRetFile == NULL) || (pFilename == NULL)){
		return FileSystemWrapper_Status_NullPointerError;
	}
	if (pSelf->state == FileSystemWrapper_State_NotInitialized){
		return FileSystemWrapper_Status_NotInitializedError;
	}

	pRetFile->pFileSystem = pSelf;

	FRESULT res = f_open( (FIL*) &(pRetFile->sFile), (char*)pFilename, FA_READ | FA_WRITE | FA_OPEN_ALWAYS);
	return remapResult(res);
}

FileSystemWrapper_Status_TypeDef FileSystemWrapper_writeBinaryData(FileSystemWrapper_File_TypeDef* pFile, const void* pBuffer, uint32_t uiBytesToWrite, uint32_t* pBytesWritten){

	if ((pFile == NULL) || (pBuffer == NULL)){
		return FileSystemWrapper_Status_NullPointerError;
	}

	if (pFile->pFileSystem->state == FileSystemWrapper_State_NotInitialized){
		return FileSystemWrapper_Status_NotInitializedError;
	}

	FRESULT res = f_write ( (FIL*) &(pFile->sFile), pBuffer, uiBytesToWrite, (UINT*) pBytesWritten);
	return remapResult(res);
}

FileSystemWrapper_Status_TypeDef FileSystemWrapper_readData(FileSystemWrapper_File_TypeDef* pFile, const void* pBuffer, uint32_t uiBytesToRead, uint32_t* pRetBytesRead){

	if ((pFile == NULL) || (pBuffer == NULL) || (pRetBytesRead == NULL)){
		return FileSystemWrapper_Status_NullPointerError;
	}

	if (pFile->pFileSystem->state == FileSystemWrapper_State_NotInitialized){
		return FileSystemWrapper_Status_NotInitializedError;
	}

	FRESULT res = f_read( (FIL*) &(pFile->sFile), (void*)pBuffer, uiBytesToRead, (UINT*) pRetBytesRead);
	return remapResult(res);
}

FileSystemWrapper_Status_TypeDef FileSystemWrapper_putString(FileSystemWrapper_File_TypeDef* pFile, const char* pBuffer){

	if ((pFile == NULL) || (pBuffer == NULL)){
		return FileSystemWrapper_Status_NullPointerError;
	}

	if (pFile->pFileSystem->state == FileSystemWrapper_State_NotInitialized){
		return FileSystemWrapper_Status_NotInitializedError;
	}

	if ((size_t)(f_puts (pBuffer, (FIL*) &(pFile->sFile))) != strlen(pBuffer)){
		return FileSystemWrapper_Status_DISK_ERR;
	}
	return FileSystemWrapper_Status_OK;
}

FileSystemWrapper_Status_TypeDef FileSystemWrapper_close(FileSystemWrapper_File_TypeDef* pFile){

	if (pFile == NULL){
		return FileSystemWrapper_Status_NullPointerError;
	}

	if (pFile->pFileSystem->state == FileSystemWrapper_State_NotInitialized){
		return FileSystemWrapper_Status_NotInitializedError;
	}

	FRESULT res = f_close( (FIL*) &(pFile->sFile));
	return remapResult(res);
}

FileSystemWrapper_Status_TypeDef FileSystemWrapper_sync(FileSystemWrapper_File_TypeDef* pFile){

	if (pFile == NULL){
		return FileSystemWrapper_Status_NullPointerError;
	}

	if (pFile->pFileSystem->state == FileSystemWrapper_State_NotInitialized){
		return FileSystemWrapper_Status_NotInitializedError;
	}

	FRESULT res = f_sync( (FIL*) &(pFile->sFile));
	return remapResult(res);
}

