/*
 * file_reading_buffer.h
 *
 *  Created on: 17.07.2018
 *      Author: Michal Kowalik
 */

#ifndef USER_FILE_READING_BUFFER_H_
#define USER_FILE_READING_BUFFER_H_

#include "user/file_system_wrapper.h"

#define	FILE_READING_BUFFER_SIZE	16

typedef enum {
	FileReadingBuffer_State_UnInitialized = 0,
	FileReadingBuffer_State_Initialized
} FileReadingBuffer_State_TypeDef;


typedef enum {
	FileReadingBuffer_Status_OK = 0,
	FileReadingBuffer_Status_UnInitializedError,
	FileReadingBuffer_Status_EndOfFile,
	FileReadingBuffer_Status_Error
} FileReadingBuffer_Status_TypeDef;

typedef struct {
	FileSystemWrapper_File_TypeDef*	pFile;
	FileReadingBuffer_State_TypeDef	state;
	uint8_t							buffer[FILE_READING_BUFFER_SIZE];
	uint32_t						iterator;
	uint32_t						bytesLeft;
} FileReadingBuffer_TypeDef;

FileReadingBuffer_Status_TypeDef FileReadingBuffer_init(FileReadingBuffer_TypeDef* pSelf, FileSystemWrapper_File_TypeDef* pFile);
FileReadingBuffer_Status_TypeDef FileReadingBuffer_readUInt8(FileReadingBuffer_TypeDef* pSelf, uint8_t* pReturnValue);
FileReadingBuffer_Status_TypeDef FileReadingBuffer_readUInt16(FileReadingBuffer_TypeDef* pSelf, uint16_t* pReturnValue);
FileReadingBuffer_Status_TypeDef FileReadingBuffer_readChar(FileReadingBuffer_TypeDef* pSelf, char* pReturnValue);
FileReadingBuffer_Status_TypeDef FileReadingBuffer_readString(FileReadingBuffer_TypeDef* pSelf, char* pReturnString, uint16_t length);
FileReadingBuffer_Status_TypeDef FileReadingBuffer_skipBytes(FileReadingBuffer_TypeDef* pSelf, uint16_t length);





#endif /* USER_FILE_READING_BUFFER_H_ */
