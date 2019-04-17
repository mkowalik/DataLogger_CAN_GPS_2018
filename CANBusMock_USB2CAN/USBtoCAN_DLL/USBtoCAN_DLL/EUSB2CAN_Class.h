#pragma once

#include "EUSB2CAN_DLL.h"

// Function pointers
typedef void(*EUSB2CAN_progressProc)(DWORD percent, void* arg);

typedef EUSB2CAN_Status (*Init_fp_type)(EUSB2CAN_BaudRate);
typedef EUSB2CAN_Status (*Void_fp_type)();
typedef EUSB2CAN_Status (*CanRead_fp_type)(EUSB2CAN_CanMsg *msg, EUSB2CAN_Timestamp *timestamp);
typedef EUSB2CAN_Status (*CanWrite_fp_type)(EUSB2CAN_CanMsg *msg);

typedef EUSB2CAN_Status(*GetVersion_fp_type)(/*out*/BYTE *runing_soft, /*out*/EUSB2CAN_Version *bl_ver, /*out*/EUSB2CAN_Version *fw_ver);
typedef EUSB2CAN_Status(*StartBootloader_fp_type)();
typedef EUSB2CAN_Status(*UpdateSoftware_fp_type)(const WCHAR *filepath, EUSB2CAN_progressProc proc, void* arg);

class EUSB2CAN_Class
{
    private:
		//DLL
		HINSTANCE dll_handle;

		//Function pointers
		Init_fp_type init_fp;
		Void_fp_type uninit_fp;
		Void_fp_type flush_fp;
		CanRead_fp_type canRead_fp;
		CanWrite_fp_type canWrite_fp;
		Void_fp_type canSyncWrite_fp;

		GetVersion_fp_type getVersion_fp;
		StartBootloader_fp_type startBootloader_fp;
		UpdateSoftware_fp_type updateSoftware_fp;

		bool was_loaded;

		void clearPointers();

    public:       
		// Class constructor
		EUSB2CAN_Class();
		// Class destructor
		~EUSB2CAN_Class();

		EUSB2CAN_Status initialize(EUSB2CAN_BaudRate baud_rate);
		EUSB2CAN_Status uninitialize(void);

		EUSB2CAN_Status flush(void);

		EUSB2CAN_Status canRead(/*out*/EUSB2CAN_CanMsg *msg, /*out*/EUSB2CAN_Timestamp *timestamp);
		EUSB2CAN_Status canWrite(EUSB2CAN_CanMsg *msg);
		
		EUSB2CAN_Status canSyncWrite();

		EUSB2CAN_Status getVersion(/*out*/BYTE *runing_soft, /*out*/EUSB2CAN_Version *bl_ver, /*out*/EUSB2CAN_Version *fw_ver);
		EUSB2CAN_Status startBootloader();
		EUSB2CAN_Status updateSoftware(const WCHAR *filepath, EUSB2CAN_progressProc proc, void* arg);
};
