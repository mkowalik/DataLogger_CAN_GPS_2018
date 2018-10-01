
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2018 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f7xx_hal.h"
#include "can.h"
#include "fatfs.h"
#include "rtc.h"
#include "sdmmc.h"
#include "usart.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */

#include "user/config.h"
#include "user/can_receiver.h"
#include "user/action_scheduler.h"

#include "string.h"

#include "user/file_reading_buffer.h"
#include "user/file_writing_buffer.h"
#include "user/led_driver.h"

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

ConfigDataManager_TypeDef		configDataManager;
DataSaver_TypeDef				dataSaver;

CANReceiver_TypeDef				CANReceiver;
RTCDriver_TypeDef				rtcDriver;

ActionScheduler_TypeDef			actionScheduler;

FileSystemWrapper_TypeDef		fileSystem;

CANTransceiverDriver_TypeDef	canTransceiverDriver;
MSTimerDriver_TypeDef			msTimerDriver;

LedDriver_TypeDef				ledDebug1Driver;
LedDriver_TypeDef				ledDebug2Driver;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_NVIC_Init(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

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

void HAL_SYSTICK_Callback(void){
	if (LedDriver_1msElapsedCallbackHandler(&ledDebug1Driver) != LedDriver_Status_OK){
		Error_Handler();
	}
	/*if (ActionScheduler_1msElapsedCallbackHandler(&actionScheduler) != ActionScheduler_Status_OK){
		Error_Handler();
	}*/
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  *
  * @retval None
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SDMMC1_SD_Init();
  MX_USART1_UART_Init();
  MX_CAN1_Init();
  MX_RTC_Init();
  MX_FATFS_Init();

  /* Initialize interrupts */
  MX_NVIC_Init();
  /* USER CODE BEGIN 2 */

  LedDriver_Pin_TypeDef ledDebug1Pin = my_LED_DEBUG1_Pin;
  LedDriver_Pin_TypeDef ledDebug2Pin = my_LED_DEBUG2_Pin;

  if (LedDriver_init(&ledDebug1Driver, (LedDriver_Port_TypeDef*)my_LED_DEBUG1_GPIO_Port, &ledDebug1Pin) != LedDriver_Status_OK){
	  Error_Handler();
  }
  if (LedDriver_init(&ledDebug2Driver, (LedDriver_Port_TypeDef*)my_LED_DEBUG2_GPIO_Port, &ledDebug2Pin) != LedDriver_Status_OK){
	  Error_Handler();
  }
  if (RTCDriver_init(&rtcDriver, &hrtc) != RTCDriver_Status_OK){
	  Error_Handler();
  }
  if (CANTransceiverDriver_init(&canTransceiverDriver, &hcan1) != CANTransceiverDriver_Status_OK){
	  Error_Handler();
  }
  if (MSTimerDriver_init(&msTimerDriver) != MSTimerDriver_Status_OK){
	  Error_Handler();
  }

  if (FileSystemWrapper_init(&fileSystem) != FileSystemWrapper_Status_OK){
	  Error_Handler();
  }

  if (ConfigDataManager_init(&configDataManager, &fileSystem) != ConfigDataManager_Status_OK){
	  Error_Handler();
  }

  Config_TypeDef* pConfig;
  if (ConfigDataManager_getConfigPointer(&configDataManager, &pConfig) != ConfigDataManager_Status_OK){
	  Error_Handler();
  }

  if (DataSaver_init(&dataSaver, pConfig, &fileSystem) != DataSaver_Status_OK){
	  Error_Handler();
  }
  if (CANReceiver_init(&CANReceiver, pConfig, &canTransceiverDriver, &msTimerDriver) != CANReceiver_Status_OK){
	  Error_Handler();
  }


  if (ActionScheduler_init(&actionScheduler, &configDataManager, &dataSaver, &CANReceiver, &rtcDriver, &ledDebug1Driver) != ActionScheduler_Status_OK){
	  Error_Handler();
  }
  if (ActionScheduler_startScheduler(&actionScheduler) != ActionScheduler_Status_OK){
	  Error_Handler();
  }

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  return 1;

  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */

  }
  /* USER CODE END 3 */

}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;

    /**Configure the main internal regulator output voltage 
    */
  __HAL_RCC_PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 96;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Activate the Over-Drive mode 
    */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC|RCC_PERIPHCLK_USART1
                              |RCC_PERIPHCLK_SDMMC1|RCC_PERIPHCLK_CLK48;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  PeriphClkInitStruct.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInitStruct.Clk48ClockSelection = RCC_CLK48SOURCE_PLL;
  PeriphClkInitStruct.Sdmmc1ClockSelection = RCC_SDMMC1CLKSOURCE_CLK48;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/**
  * @brief NVIC Configuration.
  * @retval None
  */
static void MX_NVIC_Init(void)
{
  /* CAN1_RX0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(CAN1_RX0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);
  /* CAN1_RX1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(CAN1_RX1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(CAN1_RX1_IRQn);
  /* CAN1_TX_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(CAN1_TX_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(CAN1_TX_IRQn);
  /* CAN1_SCE_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(CAN1_SCE_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(CAN1_SCE_IRQn);
  /* SDMMC1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SDMMC1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(SDMMC1_IRQn);
  /* RCC_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(RCC_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(RCC_IRQn);
}

/* USER CODE BEGIN 4 */

static uint16_t errorInCounter = 0;

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  if (errorInCounter++ > 0){
	  return;
  }
  while(1)
  {
	  HAL_GPIO_WritePin(my_LED_DEBUG2_GPIO_Port, my_LED_DEBUG2_Pin, GPIO_PIN_SET);
	  HAL_Delay(50);
	  HAL_GPIO_WritePin(my_LED_DEBUG2_GPIO_Port, my_LED_DEBUG2_Pin, GPIO_PIN_RESET);
	  HAL_Delay(50);
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
