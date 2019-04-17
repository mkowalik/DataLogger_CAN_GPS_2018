#pragma once

#ifdef EUSB2CAN_DLL_EXPORTS
#define EUSB2CAN_DLL_API __declspec(dllexport)
#else
#define EUSB2CAN_DLL_API __declspec(dllimport)
#endif

#include <windows.h>

// Common Status
#define EUSB2CAN_STAT_OK					0x00
#define EUSB2CAN_STAT_ERROR					0x01

// Initialize Status
#define EUSB2CAN_STAT_WRONG_BR				0x10
#define EUSB2CAN_STAT_DEV_NOT_FOUND			0x11
#define EUSB2CAN_STAT_CREATE_FILE_ERR		0x12
#define EUSB2CAN_STAT_SET_COMM_ERR			0x13
#define EUSB2CAN_STAT_GET_STATE_ERR			0x14
#define EUSB2CAN_STAT_SET_STATE_ERR			0x15
#define EUSB2CAN_STAT_SET_TIMEOUTS_ERR		0x16
#define EUSB2CAN_STAT_SERIAL_PORT_ERR		0x1F

// GetVersion & StartBootloader Status
#define EUSB2CAN_STAT_GET_VER_ERR			0xE0
#define EUSB2CAN_STAT_START_BL_ERR			0xE8

// Update Soft Status
#define EUSB2CAN_STAT_UPD_FILE_OPEN_ERR		0xF0
#define EUSB2CAN_STAT_UPD_WRONG_FILE_ERR	0xF1
#define EUSB2CAN_STAT_UPD_FILE_READ_ERR		0xF2
#define EUSB2CAN_STAT_UPD_NO_ACK_ERR		0xF3

// Init CAN Baud Rate
#define EUSB2CAN_BR_1M	14

// Running Soft
#define EUSB2CAN_RUNNING_SOFT_BL ((BYTE)'B')
#define EUSB2CAN_RUNNING_SOFT_FW ((BYTE)'F')

typedef DWORD	EUSB2CAN_Status;
typedef DWORD	EUSB2CAN_BaudRate;

struct EUSB2CAN_HdlcFrame {
	BYTE address = 0;
	BYTE control = 0;
	BYTE data[20];

	BYTE data_length = 0;
};

struct EUSB2CAN_CanMsg {
	DWORD id;
	BYTE ext_id;
	BYTE rtr;
	BYTE dlc;
	BYTE data[8];
};

struct EUSB2CAN_Timestamp {
	DWORD microseconds;
	DWORD miliseconds;
	DWORD miliseconds_overflow;
};

struct EUSB2CAN_Version {
	BYTE major;
	BYTE minor;
	BYTE revision;
};

typedef void(*EUSB2CAN_progressProc)(DWORD percent, void* arg);

#ifdef __cplusplus
extern "C"
{
#endif

	EUSB2CAN_DLL_API EUSB2CAN_Status EUSB2CAN_initialize(EUSB2CAN_BaudRate baud_rate);
	EUSB2CAN_DLL_API EUSB2CAN_Status EUSB2CAN_uninitialize(void);

	EUSB2CAN_DLL_API EUSB2CAN_Status EUSB2CAN_flush(void);

	EUSB2CAN_DLL_API EUSB2CAN_Status EUSB2CAN_canRead(/*out*/EUSB2CAN_CanMsg *msg, /*out*/EUSB2CAN_Timestamp *timestamp);
	EUSB2CAN_DLL_API EUSB2CAN_Status EUSB2CAN_canWrite(EUSB2CAN_CanMsg *msg);

	EUSB2CAN_DLL_API EUSB2CAN_Status EUSB2CAN_canSyncWrite();

	EUSB2CAN_DLL_API EUSB2CAN_Status EUSB2CAN_getVersion(/*out*/BYTE *runing_soft, /*out*/EUSB2CAN_Version *bl_ver, /*out*/EUSB2CAN_Version *fw_ver);

	EUSB2CAN_DLL_API EUSB2CAN_Status EUSB2CAN_startBootloader();
	EUSB2CAN_DLL_API EUSB2CAN_Status EUSB2CAN_updateSoftware(const WCHAR *filepath, EUSB2CAN_progressProc proc, void* arg);


#ifdef __cplusplus
}
#endif