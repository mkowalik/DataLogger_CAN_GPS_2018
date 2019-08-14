/*
 * file_system_wrapper.h
 *
 *  Created on: 27.01.2018
 *      Author: Michal Kowalik
 */

#ifndef FILE_SYSTEM_WRAPPER_H_
#define FILE_SYSTEM_WRAPPER_H_

#include "ff.h"
#include "stdint.h"

#define	FILESYSTEM_MOUNT_IMMEDIATELY	1

typedef enum {
	FileSystemWrapper_Status_OK = 0,				/* (0) Succeeded */
	FileSystemWrapper_Status_DISK_ERR,				/* (1) A hard error occurred in the low level disk I/O layer */
	FileSystemWrapper_Status_INT_ERR,				/* (2) Assertion failed */
	FileSystemWrapper_Status_NOT_READY,				/* (3) The physical drive cannot work */
	FileSystemWrapper_Status_NO_FILE,				/* (4) Could not find the file */
	FileSystemWrapper_Status_NO_PATH,				/* (5) Could not find the path */
	FileSystemWrapper_Status_INVALID_NAME,			/* (6) The path name format is invalid */
	FileSystemWrapper_Status_DENIED,				/* (7) Access denied due to prohibited access or directory full */
	FileSystemWrapper_Status_EXIST,					/* (8) Access denied due to prohibited access */
	FileSystemWrapper_Status_INVALID_OBJECT,		/* (9) The file/directory object is invalid */
	FileSystemWrapper_Status_WRITE_PROTECTED,		/* (10) The physical drive is write protected */
	FileSystemWrapper_Status_INVALID_DRIVE,			/* (11) The logical drive number is invalid */
	FileSystemWrapper_Status_NOT_ENABLED,			/* (12) The volume has no work area */
	FileSystemWrapper_Status_NO_FILESYSTEM,			/* (13) There is no valid FAT volume */
	FileSystemWrapper_Status_MKFS_ABORTED,			/* (14) The f_mkfs() aborted due to any parameter error */
	FileSystemWrapper_Status_TIMEOUT,				/* (15) Could not get a grant to access the volume within defined period */
	FileSystemWrapper_Status_LOCKED,				/* (16) The operation is rejected according to the file sharing policy */
	FileSystemWrapper_Status_NOT_ENOUGH_CORE,		/* (17) LFN working buffer could not be allocated */
	FileSystemWrapper_Status_TOO_MANY_OPEN_FILES,	/* (18) Number of open files > _FS_SHARE */
	FileSystemWrapper_Status_INVALID_PARAMETER,		/* (19) Given parameter is invalid */
	FileSystemWrapper_Status_NotInitializedError,
	FileSystemWrapper_Status_Error
} FileSystemWrapper_Status_TypeDef;

typedef struct {
	volatile FATFS		sFatFS;
	volatile uint8_t	bInitialized;
} FileSystemWrapper_TypeDef;

typedef struct {
	volatile FileSystemWrapper_TypeDef* volatile	pFileSystem;
	volatile FIL									sFile;
} FileSystemWrapper_File_TypeDef;

FileSystemWrapper_Status_TypeDef FileSystemWrapper_init(FileSystemWrapper_TypeDef* pSelf);
FileSystemWrapper_Status_TypeDef FileSystemWrapper_open(FileSystemWrapper_TypeDef* pSelf, FileSystemWrapper_File_TypeDef* pFile, char* pFilename);
FileSystemWrapper_Status_TypeDef FileSystemWrapper_putString(FileSystemWrapper_File_TypeDef* pFile, const char* pBuffer);
FileSystemWrapper_Status_TypeDef FileSystemWrapper_close(FileSystemWrapper_File_TypeDef* pFile);
FileSystemWrapper_Status_TypeDef FileSystemWrapper_writeBinaryData(FileSystemWrapper_File_TypeDef* pFile, const void* pBuffer, uint32_t uiBytesToWrite, uint32_t* pBytesWritten);
FileSystemWrapper_Status_TypeDef FileSystemWrapper_readData(FileSystemWrapper_File_TypeDef* pFile, void* pBuffer, uint32_t uiBytesToRead, uint32_t* pBytesRead);

#endif /* FILE_SYSTEM_WRAPPER_H_ */
