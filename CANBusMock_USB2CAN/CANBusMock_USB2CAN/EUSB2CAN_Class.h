#pragma once

#include <QLibrary>

#include "EUSB2CAN_DLL.h"

// Function pointers
typedef void(*EUSB2CAN_progressProc)(DWORD percent, void* arg);

typedef EUSB2CAN_Status (*Init_fp_type)(EUSB2CAN_CanSpeed speed);
typedef EUSB2CAN_Status (*Uninit_fp_type)(void);

typedef EUSB2CAN_Status (*CreateLogFile_fp_type)(const wchar_t *path);

typedef EUSB2CAN_Status (*GetVersion_fp_type)(/*out*/BYTE *running_soft, /*out*/EUSB2CAN_Version *bl_ver, /*out*/EUSB2CAN_Version *fw_ver);

typedef EUSB2CAN_Status (*SetSpeed_fp_type)(EUSB2CAN_CanSpeed speed);
typedef EUSB2CAN_Status (*SetParameter_fp_type)(EUSB2CAN_Param param, DWORD value);
typedef EUSB2CAN_Status (*GetParameter_fp_type)(EUSB2CAN_Param param, /*out*/DWORD *value);

typedef EUSB2CAN_Status (*Flush_fp_type)(void);

typedef EUSB2CAN_Status (*Read_fp_type)(/*out*/EUSB2CAN_CanMsg *msg, /*out*/EUSB2CAN_Timestamp *timestamp, /*out*/EUSB2CAN_XferErr *errors);
typedef EUSB2CAN_Status (*Write_fp_type)(EUSB2CAN_CanMsg *msg);

typedef EUSB2CAN_Status (*WaitWriteFinish_fp_type)(DWORD timeout);

typedef EUSB2CAN_Status (*StartBootloader_fp_type)(void);
typedef EUSB2CAN_Status (*UpdateSoftware_fp_type)(const WCHAR *filepath, EUSB2CAN_progressProc proc, void* arg);

class EUSB2CAN_Class
{
    private:
        //DLL
        QLibrary lib;

		//Function pointers
		Init_fp_type init_fp;
		Uninit_fp_type uninit_fp;

		CreateLogFile_fp_type createLogFile_fp; 

		GetVersion_fp_type getVersion_fp;
		
		SetSpeed_fp_type setSpeed_fp;
		SetParameter_fp_type setParameter_fp;
		GetParameter_fp_type getParameter_fp;

		Flush_fp_type flush_fp;
		
		Read_fp_type read_fp;
		Write_fp_type write_fp;
		
		WaitWriteFinish_fp_type waitWriteFinish_fp;

		StartBootloader_fp_type startBootloader_fp;
		UpdateSoftware_fp_type updateSoftware_fp;

        bool was_loaded;

    public:       
		// Class constructor
		EUSB2CAN_Class();
		// Class destructor
		~EUSB2CAN_Class();

		EUSB2CAN_Status initialize(EUSB2CAN_CanSpeed baud_rate);
		EUSB2CAN_Status uninitialize(void);

		EUSB2CAN_Status createLogFile(const wchar_t *path);

		EUSB2CAN_Status getVersion(/*out*/BYTE *running_soft, /*out*/EUSB2CAN_Version *bl_ver, /*out*/EUSB2CAN_Version *fw_ver);
		
		EUSB2CAN_Status setSpeed(EUSB2CAN_CanSpeed baud_rate);
		EUSB2CAN_Status setParameter(EUSB2CAN_Param param, DWORD value);
		EUSB2CAN_Status getParameter(EUSB2CAN_Param param, /*out*/DWORD *value);
		
		EUSB2CAN_Status flush(void);

		EUSB2CAN_Status read(/*out*/EUSB2CAN_CanMsg *msg, /*out*/EUSB2CAN_Timestamp *timestamp, /*out*/EUSB2CAN_XferErr *errors);
		EUSB2CAN_Status write(EUSB2CAN_CanMsg *msg);
		
		EUSB2CAN_Status waitWriteFinish(DWORD timeout);

		EUSB2CAN_Status startBootloader(void);
		EUSB2CAN_Status updateSoftware(const WCHAR *filepath, EUSB2CAN_progressProc proc, void* arg);
};
