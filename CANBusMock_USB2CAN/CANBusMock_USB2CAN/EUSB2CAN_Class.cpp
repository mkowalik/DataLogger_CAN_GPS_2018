#include <windows.h>
#include "EUSB2CAN_Class.h"
#include <iostream>

EUSB2CAN_Class::EUSB2CAN_Class() :
    lib("C:\\Users\\kowal\\Desktop\\AGHRacingRepos\\GitHub_DataLogger_CAN_GPS\\CANBusMock_USB2CAN\\CANBusMock_USB2CAN\\EUSB2CAN.dll"),
    init_fp{nullptr},
    uninit_fp{nullptr},
    createLogFile_fp{nullptr},
    getVersion_fp{nullptr},
    setSpeed_fp{nullptr},
    setParameter_fp{nullptr},
    getParameter_fp{nullptr},
    flush_fp{nullptr},
    read_fp{nullptr},
    write_fp{nullptr},
    waitWriteFinish_fp{nullptr},
    startBootloader_fp{nullptr},
    updateSoftware_fp{nullptr},
    was_loaded{false}
{

    if(!was_loaded) {
        if (!lib.load()){
            std::cout << "Couldn't load DLL lib. Error code: " << GetLastError() << std::endl;
            return;
        }
	}

    init_fp = (Init_fp_type)lib.resolve("EUSB2CAN_initialize");
    uninit_fp = (Uninit_fp_type)lib.resolve("EUSB2CAN_uninitialize");
	
    createLogFile_fp = (CreateLogFile_fp_type)lib.resolve("EUSB2CAN_createLogFile");
	
    getVersion_fp = (GetVersion_fp_type)lib.resolve("EUSB2CAN_getVersion");
	
    setSpeed_fp = (SetSpeed_fp_type)lib.resolve("EUSB2CAN_setSpeed");
    setParameter_fp = (SetParameter_fp_type)lib.resolve("EUSB2CAN_setParameter");
    getParameter_fp = (GetParameter_fp_type)lib.resolve("EUSB2CAN_getParameter");
	
    flush_fp = (Flush_fp_type)lib.resolve("EUSB2CAN_flush");
	
    read_fp = (Read_fp_type)lib.resolve("EUSB2CAN_read");
    write_fp = (Write_fp_type)lib.resolve("EUSB2CAN_write");
    waitWriteFinish_fp = (WaitWriteFinish_fp_type)lib.resolve("EUSB2CAN_waitWriteFinish");

    startBootloader_fp = (StartBootloader_fp_type)lib.resolve("EUSB2CAN_startBootloader");
    updateSoftware_fp = (UpdateSoftware_fp_type)lib.resolve("EUSB2CAN_updateSoftware");

	was_loaded = init_fp && uninit_fp && createLogFile_fp && getVersion_fp && setSpeed_fp && setParameter_fp && getParameter_fp && flush_fp && read_fp && write_fp && waitWriteFinish_fp && startBootloader_fp && updateSoftware_fp;
}

EUSB2CAN_Class::~EUSB2CAN_Class()
{
    uninitialize();
    lib.unload();
}

EUSB2CAN_Status EUSB2CAN_Class::initialize(EUSB2CAN_CanSpeed speed)
{
	if(!was_loaded)
		return EUSB2CAN_STAT_DLL_LOAD_ERROR;

	return (EUSB2CAN_Status)init_fp(speed);
}

EUSB2CAN_Status EUSB2CAN_Class::uninitialize(void)
{
    if(!was_loaded)
		return EUSB2CAN_STAT_DLL_LOAD_ERROR;

    return (EUSB2CAN_Status)uninit_fp();
}

EUSB2CAN_Status EUSB2CAN_Class::createLogFile(const wchar_t *path)
{
	if (!was_loaded)
		return EUSB2CAN_STAT_DLL_LOAD_ERROR;

	return (EUSB2CAN_Status)createLogFile_fp(path);
}

EUSB2CAN_Status EUSB2CAN_Class::getVersion(/*out*/BYTE *running_soft, /*out*/EUSB2CAN_Version *bl_ver, /*out*/EUSB2CAN_Version *fw_ver)
{
	if (!was_loaded) {
		if (running_soft != nullptr) memset(running_soft, 0, sizeof(BYTE));
		if (bl_ver != nullptr) memset(bl_ver, 0, sizeof(EUSB2CAN_Version));
		if (fw_ver != nullptr) memset(fw_ver, 0, sizeof(EUSB2CAN_Version));

		return EUSB2CAN_STAT_DLL_LOAD_ERROR;
	}

	return (EUSB2CAN_Status)getVersion_fp(/*out*/running_soft, /*out*/bl_ver, /*out*/fw_ver);
}

EUSB2CAN_Status EUSB2CAN_Class::setSpeed(EUSB2CAN_CanSpeed speed)
{
	if (!was_loaded)
		return EUSB2CAN_STAT_DLL_LOAD_ERROR;

	return (EUSB2CAN_Status)setSpeed_fp(speed);
}

EUSB2CAN_Status EUSB2CAN_Class::setParameter(EUSB2CAN_Param param, DWORD value)
{
	if (!was_loaded)
		return EUSB2CAN_STAT_DLL_LOAD_ERROR;

	return (EUSB2CAN_Status)setParameter_fp(param, value);
}

EUSB2CAN_Status EUSB2CAN_Class::getParameter(EUSB2CAN_Param param, /*out*/DWORD *value)
{
	if (!was_loaded) {
		if (value != nullptr) memset(value, 0, sizeof(DWORD));

		return EUSB2CAN_STAT_DLL_LOAD_ERROR;
	}

	return (EUSB2CAN_Status)getParameter_fp(param, /*out*/value);
}

EUSB2CAN_Status EUSB2CAN_Class::flush(void)
{
	if (!was_loaded)
		return EUSB2CAN_STAT_DLL_LOAD_ERROR;

	return (EUSB2CAN_Status)flush_fp();
}

EUSB2CAN_Status EUSB2CAN_Class::read(/*out*/EUSB2CAN_CanMsg *msg, /*out*/EUSB2CAN_Timestamp *timestamp, /*out*/EUSB2CAN_XferErr *errors)
{
	if (!was_loaded){
		if (msg != nullptr) memset(msg, 0, sizeof(EUSB2CAN_CanMsg));
		if (timestamp != nullptr) memset(timestamp, 0, sizeof(EUSB2CAN_Timestamp));
		if (errors != nullptr) memset(errors, 0, sizeof(EUSB2CAN_XferErr));

		return EUSB2CAN_STAT_DLL_LOAD_ERROR;
	}

	return (EUSB2CAN_Status)read_fp(/*out*/msg, /*out*/timestamp, /*out*/errors);
}

EUSB2CAN_Status EUSB2CAN_Class::write(EUSB2CAN_CanMsg *msg)
{
	if (!was_loaded)
		return EUSB2CAN_STAT_DLL_LOAD_ERROR;

	return (EUSB2CAN_Status)write_fp(msg);
}

EUSB2CAN_Status EUSB2CAN_Class::waitWriteFinish(DWORD timeout)
{
	if (!was_loaded)
		return EUSB2CAN_STAT_DLL_LOAD_ERROR;

	return (EUSB2CAN_Status)waitWriteFinish_fp(timeout);
}


EUSB2CAN_Status EUSB2CAN_Class::startBootloader(void)
{
	if (!was_loaded)
		return EUSB2CAN_STAT_DLL_LOAD_ERROR;

	return (EUSB2CAN_Status)startBootloader_fp();
}


EUSB2CAN_Status EUSB2CAN_Class::updateSoftware(const WCHAR *filepath, EUSB2CAN_progressProc proc, void* arg)
{
	if (!was_loaded)
		return EUSB2CAN_STAT_DLL_LOAD_ERROR;

	return (EUSB2CAN_Status)updateSoftware_fp(filepath, proc, arg);
}
