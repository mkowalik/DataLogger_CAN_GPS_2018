/*
 * file_writing_buffer.h
 *
 *  Created on: 17.07.2018
 *      Author: Michal Kowalik
 */

#ifndef USER_FILE_WRITING_BUFFER_H_
#define USER_FILE_WRITING_BUFFER_H_

#include "stdint.h"
#include "user/file_system_wrapper.h"
#include "user/fixed_point.h"

#define FILE_WRITING_BUFFER_SIZE				256

#define	FILE_WRITING_BUFFER_FLUSH_TIMEOUT_MS	1000

typedef enum {
	FileWritingBuffer_State_UnInitialized = 0,
	FileWritingBuffer_State_Initialized,
	FileWritingBuffer_State_Flushing
} FileWritingBuffer_State_TypeDef;

typedef enum {
	FileWritingBuffer_Status_OK = 0,
	FileWritingBuffer_Status_UnInitializedError,
	FileWritingBuffer_Status_FlushingError,
	FileWritingBuffer_Status_FileSystemError,
	FileWritingBuffer_Status_NullPointerError,
	FileWritingBuffer_Status_Error
} FileWritingBuffer_Status_TypeDef;

typedef struct {
	FileSystemWrapper_File_TypeDef*	pFile;
	FileWritingBuffer_State_TypeDef	state;
	uint8_t							buffer[FILE_WRITING_BUFFER_SIZE];
	uint32_t						bytesBuffered;
	uint32_t						bytesLeft;
} FileWritingBuffer_TypeDef;

FileWritingBuffer_Status_TypeDef FileWritingBuffer_init(FileWritingBuffer_TypeDef* pSelf, FileSystemWrapper_File_TypeDef* pFile);
FileWritingBuffer_Status_TypeDef FileWritingBuffer_deInit(FileWritingBuffer_TypeDef* pSelf);
FileWritingBuffer_Status_TypeDef FileWritingBuffer_resetBuffer(FileWritingBuffer_TypeDef* pSelf);
FileWritingBuffer_Status_TypeDef FileWritingBuffer_writeToFileSystem(FileWritingBuffer_TypeDef* pSelf);

FileWritingBuffer_Status_TypeDef FileWritingBuffer_flush(FileWritingBuffer_TypeDef* pSelf);

FileWritingBuffer_Status_TypeDef FileWritingBuffer_writeUInt8(FileWritingBuffer_TypeDef* pSelf, uint8_t value);
FileWritingBuffer_Status_TypeDef FileWritingBuffer_writeUInt16(FileWritingBuffer_TypeDef* pSelf, uint16_t value);
FileWritingBuffer_Status_TypeDef FileWritingBuffer_writeUInt32(FileWritingBuffer_TypeDef* pSelf, uint32_t value);
FileWritingBuffer_Status_TypeDef FileWritingBuffer_writeUInt64(FileWritingBuffer_TypeDef* pSelf, uint64_t value);

FileWritingBuffer_Status_TypeDef FileWritingBuffer_writeInt8(FileWritingBuffer_TypeDef* pSelf, int8_t value);

FileWritingBuffer_Status_TypeDef FileWritingBuffer_writeChar(FileWritingBuffer_TypeDef* pSelf, char value);
FileWritingBuffer_Status_TypeDef FileWritingBuffer_writeString(FileWritingBuffer_TypeDef* pSelf, const char* pStringToWrite, uint16_t stringLength);
FileWritingBuffer_Status_TypeDef FileWritingBuffer_writeFixedPoint32(FileWritingBuffer_TypeDef* pSelf, FixedPoint value);

#endif /* USER_FILE_WRITING_BUFFER_H_ */
