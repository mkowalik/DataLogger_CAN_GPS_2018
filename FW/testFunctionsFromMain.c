
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"

void testConfig(){

	  ConfigChannel_TypeDef channel0;
	  channel0.divider = 1;
	  channel0.multiplier = 1;
	  channel0.offset = 0;
	  channel0.valueType = UNSIGNED_16BIT;

	  ConfigChannel_TypeDef channel1 = channel0;
	  ConfigChannel_TypeDef channel2 = channel0;
	  ConfigChannel_TypeDef channel3 = channel0;

	  ConfigFrame_TypeDef configFrame;
	  configFrame.ID = 0x600;
	  configFrame.DLC = 8;

	  configFrame.channels[0] = channel0;
	  configFrame.channels[1] = channel1;
	  configFrame.channels[2] = channel2;
	  configFrame.channels[3] = channel3;

	  Config_TypeDef config;
	  config.version = CONFIG_FILE_USED_VERSION;
	  config.subversion = CONFIG_FILE_USED_SUBVERSION;
	  config.num_of_frames = 1;
	  config.frames[0] = configFrame;

	  //CANReceiver_init(&CANReceiver, &config);

	  CANData_TypeDef logedFrames[1000];
	  uint32_t i= 0;

	  while (1)
	  {
		  CANData_TypeDef tmp;
		  if (CANReceiver_pullLastFrame(&CANReceiver, &tmp) == CANReceiver_Status_OK){
			  logedFrames[(i++)%1000] = tmp;
			  HAL_GPIO_WritePin(my_LED_DEBUG2_GPIO_Port, my_LED_DEBUG2_Pin, GPIO_PIN_RESET);
		  } else {
			  HAL_GPIO_TogglePin(my_LED_DEBUG1_GPIO_Port, my_LED_DEBUG1_Pin);
		  }
	  }
}
void SDTestFunction(){

	  FRESULT res = f_mount(&SDFatFS, "", 1);

	  FRESULT open_res = f_open(&SDFile, "/test240.txt", FA_READ | FA_WRITE | FA_OPEN_ALWAYS);

	  char text[] = "aaaaaaaaaaaaaaaaaaahello uSD World";
	  uint32_t length = strlen(text);

	  unsigned int write_res;// = f_puts(text, &SDFile);
	  f_write(&SDFile, text, length, &write_res);

	  FRESULT sycc_res = f_sync(&SDFile);
	  FRESULT close_res = f_close(&SDFile);

	  HAL_SD_CardInfoTypeDef info;
	  memset(&info, 0, sizeof(HAL_SD_CardInfoTypeDef));
	  HAL_SD_GetCardInfo(&hsd1, &info);

	  HAL_SD_CardCIDTypeDef cid;
	  memset(&cid, 0, sizeof(HAL_SD_CardCIDTypeDef));
	  HAL_SD_GetCardCID(&hsd1, &cid);

}

void RTCTestFunction(){

	  RTCDriver_TypeDef	rtcDriver;
	  RTCDriver_Status_TypeDef stat = RTCDriver_init(&rtcDriver, &hrtc);

	  uint8_t prevSec = 0;
	  while (1)
	  {

		  DateTime_TypeDef date;
		  RTCDriver_getDateAndTime(&rtcDriver, &date);
		  if (date.second != prevSec){
			  prevSec = date.second;
			  HAL_GPIO_TogglePin(my_LED_DEBUG1_GPIO_Port, my_LED_DEBUG1_Pin);
		  }
		  HAL_GPIO_TogglePin(my_LED_DEBUG2_GPIO_Port, my_LED_DEBUG2_Pin);
		  HAL_Delay(1000);

	  }
}

void ConfigTestFunction(){

	  FileSystemWrapper_Status_TypeDef status;
	  FileSystemWrapper_TypeDef fileSystem = (FileSystemWrapper_TypeDef){0};
	  status = FileSystemWrapper_init(&fileSystem);

	  FileSystemWrapper_File_TypeDef file;
	  status = FileSystemWrapper_open(&fileSystem, &file, "logger.aghconf");

	  FileReadingBuffer_Status_TypeDef status2;
	  FileReadingBuffer_TypeDef readingBuffer = (FileReadingBuffer_TypeDef){0};
	  status2 = FileReadingBuffer_init(&readingBuffer, &file);

	  uint16_t ver, subver, frames_no, id;
	  uint8_t dlc;
	  status2 = FileReadingBuffer_readUInt16(&readingBuffer, &ver);
	  status2 = FileReadingBuffer_readUInt16(&readingBuffer, &subver);
	  status2 = FileReadingBuffer_readUInt16(&readingBuffer, &frames_no);
	  status2 = FileReadingBuffer_readUInt16(&readingBuffer, &id);
	  status2 = FileReadingBuffer_readUInt8(&readingBuffer, &dlc);

}

void FileReadWriteTest(){

	//MUST BE GLOBAL!!!
	FileSystemWrapper_TypeDef fileSystem;
	FileSystemWrapper_File_TypeDef	file2;
	FileWritingBuffer_TypeDef	writeBuffer;
	//MUST BE GLOBAL!!! or zero'ed properly

	  FileSystemWrapper_Status_TypeDef status;
	  status = FileSystemWrapper_init(&fileSystem);

	  ConfigDataManager_TypeDef	configManager = (ConfigDataManager_TypeDef){0};
	  ConfigDataManager_init(&configManager, &fileSystem);

	  Config_TypeDef* pConfig;
	  ConfigDataManager_getConfigPointer(&configManager, &pConfig);

	  status = FileSystemWrapper_open(&fileSystem, &file2, "/testtest2.out");

	  FileWritingBuffer_init(&writeBuffer, &file2);

	  for (uint16_t i=0; i<300; i++){
		  FileWritingBuffer_writeUInt16(&writeBuffer, i);
	  }
	  FileWritingBuffer_flush(&writeBuffer);
	  FileSystemWrapper_close(&file2);

}

void AllTogetherTest(){

	  RTCDriver_Status_TypeDef statusRTC			= RTCDriver_init(&rtcDriver, &hrtc);
	  FileSystemWrapper_Status_TypeDef statusFS		= FileSystemWrapper_init(&fileSystem);
	  ConfigDataManager_Status_TypeDef statusCDM	= ConfigDataManager_init(&configDataManager, &fileSystem);
//	  DataSaver_Status_TypeDef statusDS				= DataSaver_init(&dataSaver, &configDataManager, &fileSystem);

	  DateTime_TypeDef dateTime;
	  statusRTC = RTCDriver_getDateAndTime(&rtcDriver, &dateTime);

	  CANData_TypeDef data;
	  data.ID = 2;
	  data.DLC = 3;
	  data.msTime = 115;
	  data.Data[0] = 15;
	  data.Data[1] = 30;
	  data.Data[2] = 60;


//	  statusDS = DataSaver_startLogging(&dataSaver, dateTime);
//	  statusDS = DataSaver_writeData(&dataSaver, &data);
//	  statusDS = DataSaver_stopLogging(&dataSaver);
}

#pragma GCC diagnostic pop
#pragma GCC diagnostic pop
#pragma GCC diagnostic pop