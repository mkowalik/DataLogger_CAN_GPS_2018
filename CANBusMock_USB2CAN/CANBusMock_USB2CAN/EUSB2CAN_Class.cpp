//#include "StdAfx.h"
#include "windows.h"
#include "EUSB2CAN_Class.h"
#include <iostream>

EUSB2CAN_Class::EUSB2CAN_Class()
{
	was_loaded = false;

	clearPointers();

	if(!was_loaded) {
        dll_handle = LoadLibrary(L"EUSB2CAN.dll");

        if (dll_handle == NULL) {
            std::cout << "Couldn't load DLL lib. Error code: " << GetLastError() << std::endl;
            return;
        }
	}

	init_fp = (Init_fp_type)GetProcAddress(dll_handle, "EUSB2CAN_initialize");
	uninit_fp = (Void_fp_type)GetProcAddress(dll_handle, "EUSB2CAN_uninitialize");
	flush_fp = (Void_fp_type)GetProcAddress(dll_handle, "EUSB2CAN_flush");
	canRead_fp = (CanRead_fp_type)GetProcAddress(dll_handle, "EUSB2CAN_canRead");
	canWrite_fp = (CanWrite_fp_type)GetProcAddress(dll_handle, "EUSB2CAN_canWrite");
	canSyncWrite_fp = (Void_fp_type)GetProcAddress(dll_handle, "EUSB2CAN_canSyncWrite");

	getVersion_fp = (GetVersion_fp_type)GetProcAddress(dll_handle, "EUSB2CAN_getVersion");
	startBootloader_fp = (StartBootloader_fp_type)GetProcAddress(dll_handle, "EUSB2CAN_startBootloader");
	updateSoftware_fp = (UpdateSoftware_fp_type)GetProcAddress(dll_handle, "EUSB2CAN_updateSoftware");

	was_loaded = init_fp && uninit_fp && flush_fp && canRead_fp && canWrite_fp && getVersion_fp && startBootloader_fp && updateSoftware_fp;

    if (was_loaded)
        std::cout << "DLL lib loaded properly." << std::endl;
    else
        std::cout << "Couldn't load DLL lib. Error code: " << GetLastError() << std::endl;

}

EUSB2CAN_Class::~EUSB2CAN_Class()
{
	uninitialize();

	if (dll_handle != NULL) {
		FreeLibrary(dll_handle);
		dll_handle = NULL;
	}
	
	clearPointers();

	was_loaded = false;
}


void EUSB2CAN_Class::clearPointers()
{
	init_fp = nullptr;
	uninit_fp = nullptr;
	flush_fp = nullptr;
	canRead_fp = nullptr;
	canWrite_fp = nullptr;
	canSyncWrite_fp = nullptr;

	getVersion_fp = nullptr;
	startBootloader_fp = nullptr;
	updateSoftware_fp = nullptr;
}


EUSB2CAN_Status EUSB2CAN_Class::initialize(EUSB2CAN_BaudRate baud_rate)
{

    if(!was_loaded)
        return EUSB2CAN_STAT_ERROR;

    return (EUSB2CAN_Status)init_fp(baud_rate);
}

EUSB2CAN_Status EUSB2CAN_Class::uninitialize(void)
{
	if(!was_loaded)
		return EUSB2CAN_STAT_ERROR;

	return (EUSB2CAN_Status)uninit_fp();
}

EUSB2CAN_Status EUSB2CAN_Class::flush(void)
{
	if (!was_loaded)
		return EUSB2CAN_STAT_ERROR;

	return (EUSB2CAN_Status)flush_fp();
}

EUSB2CAN_Status EUSB2CAN_Class::canRead(/*out*/EUSB2CAN_CanMsg *msg, /*out*/EUSB2CAN_Timestamp *timestamp)
{
	if (!was_loaded)
		return EUSB2CAN_STAT_ERROR;

	return (EUSB2CAN_Status)canRead_fp(/*out*/msg, /*out*/timestamp);
}

EUSB2CAN_Status EUSB2CAN_Class::canWrite(EUSB2CAN_CanMsg *msg)
{
	if (!was_loaded)
		return EUSB2CAN_STAT_ERROR;

	return (EUSB2CAN_Status)canWrite_fp(msg);
}

EUSB2CAN_Status EUSB2CAN_Class::canSyncWrite()
{
	if (!was_loaded)
		return EUSB2CAN_STAT_ERROR;

	return (EUSB2CAN_Status)canSyncWrite_fp();
}

EUSB2CAN_Status EUSB2CAN_Class::getVersion(/*out*/BYTE *runing_soft, /*out*/EUSB2CAN_Version *bl_ver, /*out*/EUSB2CAN_Version *fw_ver)
{
	if (!was_loaded)
		return EUSB2CAN_STAT_ERROR;

	return (EUSB2CAN_Status)getVersion_fp(/*out*/runing_soft, /*out*/bl_ver, /*out*/fw_ver);
}

EUSB2CAN_Status EUSB2CAN_Class::startBootloader()
{
	if (!was_loaded)
		return EUSB2CAN_STAT_ERROR;

	return (EUSB2CAN_Status)startBootloader_fp();
}


EUSB2CAN_Status EUSB2CAN_Class::updateSoftware(const WCHAR *filepath, EUSB2CAN_progressProc proc, void* arg)
{
	if (!was_loaded)
		return EUSB2CAN_STAT_ERROR;

	return (EUSB2CAN_Status)updateSoftware_fp(filepath, proc, arg);
}
