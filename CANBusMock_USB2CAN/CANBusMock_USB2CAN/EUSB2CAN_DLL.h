#pragma once

#ifdef EUSB2CAN_DLL_EXPORTS
#define EUSB2CAN_DLL_API __declspec(dllexport)
#else
#define EUSB2CAN_DLL_API __declspec(dllimport)
#endif

#include <windows.h>
#include <stdio.h>

/* Return status */
// Common
#define EUSB2CAN_STAT_OK					0x00
#define EUSB2CAN_STAT_ERROR					0x01
#define EUSB2CAN_STAT_NOT_SUPP_FW_VER		0x02
#define EUSB2CAN_STAT_NOT_INITIALIZED		0x03
#define EUSB2CAN_STAT_DLL_LOAD_ERROR		0x04

// Initialize
#define EUSB2CAN_STAT_WRONG_CS				0x10
#define EUSB2CAN_STAT_DEV_NOT_FOUND			0x11
#define EUSB2CAN_STAT_CREATE_FILE_ERR		0x12
#define EUSB2CAN_STAT_SET_COMM_ERR			0x13
#define EUSB2CAN_STAT_GET_STATE_ERR			0x14
#define EUSB2CAN_STAT_SET_STATE_ERR			0x15
#define EUSB2CAN_STAT_SET_TIMEOUTS_ERR		0x16
#define EUSB2CAN_STAT_SERIAL_PORT_ERR		0x1F

// Communication
#define EUSB2CAN_STAT_RX_EMPTY				0x20
#define EUSB2CAN_STAT_XFER_ERR				0x21

// GetVersion & StartBootloader
#define EUSB2CAN_STAT_GET_VER_ERR			0xE0
#define EUSB2CAN_STAT_START_BL_ERR			0xE8

// Update Soft
#define EUSB2CAN_STAT_UPD_FILE_OPEN_ERR		0xF0
#define EUSB2CAN_STAT_UPD_WRONG_FILE_ERR	0xF1
#define EUSB2CAN_STAT_UPD_FILE_READ_ERR		0xF2
#define EUSB2CAN_STAT_UPD_NO_ACK_ERR		0xF3

// Init CAN Speed
#define EUSB2CAN_CS_1M		14
#define EUSB2CAN_CS_500K	10
#define EUSB2CAN_CS_250K	7
#define EUSB2CAN_CS_125K	4

// Running Soft
#define EUSB2CAN_RUNNING_SOFT_BL ((BYTE)'B')
#define EUSB2CAN_RUNNING_SOFT_FW ((BYTE)'F')

// Parameters
#define EUSB2CAN_PARAM_SILENT		1
#define EUSB2CAN_PARAM_ERR_AND_TS	2

// Communication errors
#define EUSB2CAN_XFER_ERR_WARNING	(0x1 << 0)
#define EUSB2CAN_XFER_ERR_PASSIVE	(0x1 << 1)
#define EUSB2CAN_XFER_ERR_BUS_OFF	(0x1 << 2)
#define EUSB2CAN_XFER_ERR_CODE_MASK (0x7 << 3)

#define EUSB2CAN_ERR_CODE_NONE		0
#define EUSB2CAN_ERR_CODE_STUFF		1
#define EUSB2CAN_ERR_CODE_FORM		2
#define EUSB2CAN_ERR_CODE_ACK		3
#define EUSB2CAN_ERR_CODE_BIT_REC	4
#define EUSB2CAN_ERR_CODE_BIT_DOM	5
#define EUSB2CAN_ERR_CODE_CRC		6

typedef DWORD	EUSB2CAN_Status;
typedef DWORD	EUSB2CAN_CanSpeed;
typedef DWORD	EUSB2CAN_Timestamp;
typedef BYTE	EUSB2CAN_XferErr;
typedef BYTE	EUSB2CAN_Param;

struct EUSB2CAN_Version {
	BYTE major;
	BYTE minor;
	BYTE revision;
};

struct EUSB2CAN_CanMsg {
	DWORD id;
	BYTE ext_id;
	BYTE rtr;
	BYTE dlc;
	BYTE data[8];
};

typedef void(*EUSB2CAN_progressProc)(DWORD percent, void* arg);

#ifdef __cplusplus
extern "C"
{
#endif

	EUSB2CAN_DLL_API EUSB2CAN_Status EUSB2CAN_initialize(EUSB2CAN_CanSpeed speed);
	EUSB2CAN_DLL_API EUSB2CAN_Status EUSB2CAN_uninitialize(void);

	EUSB2CAN_DLL_API EUSB2CAN_Status EUSB2CAN_createLogFile(const wchar_t *path);

	EUSB2CAN_DLL_API EUSB2CAN_Status EUSB2CAN_getVersion(/*out*/BYTE *running_soft, /*out*/EUSB2CAN_Version *bl_ver, /*out*/EUSB2CAN_Version *fw_ver);

	EUSB2CAN_DLL_API EUSB2CAN_Status EUSB2CAN_setSpeed(EUSB2CAN_CanSpeed speed);
	EUSB2CAN_DLL_API EUSB2CAN_Status EUSB2CAN_setParameter(EUSB2CAN_Param param, DWORD value);
	EUSB2CAN_DLL_API EUSB2CAN_Status EUSB2CAN_getParameter(EUSB2CAN_Param param, /*out*/DWORD *value);

	EUSB2CAN_DLL_API EUSB2CAN_Status EUSB2CAN_flush(void);

	EUSB2CAN_DLL_API EUSB2CAN_Status EUSB2CAN_read(/*out*/EUSB2CAN_CanMsg *msg, /*out*/EUSB2CAN_Timestamp *timestamp, /*out*/EUSB2CAN_XferErr *errors);
	EUSB2CAN_DLL_API EUSB2CAN_Status EUSB2CAN_write(EUSB2CAN_CanMsg *msg);
	
	EUSB2CAN_DLL_API EUSB2CAN_Status EUSB2CAN_waitWriteFinish(DWORD timeout);

	EUSB2CAN_DLL_API EUSB2CAN_Status EUSB2CAN_startBootloader(void);
	EUSB2CAN_DLL_API EUSB2CAN_Status EUSB2CAN_updateSoftware(const WCHAR *filepath, EUSB2CAN_progressProc proc, void* arg);


#ifdef __cplusplus
}
#endif