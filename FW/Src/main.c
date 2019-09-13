/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "can.h"
#include "dma.h"
#include "fatfs.h"
#include "gfxsimulator.h"
#include "rtc.h"
#include "sdmmc.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "string.h"
#include "ff.h"

#include "user/file_writing_buffer.h"
#include "user/led_driver.h"
#include "user/ms_timer_driver.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define INITIALIZATION_LED_ON	950
#define INITIALIZATION_LED_OFF	50

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

FileSystemWrapper_TypeDef		fileSystem;
FileWritingBuffer_TypeDef		writingBuffer;
MSTimerDriver_TypeDef			msTimerDriver;

LedDriver_TypeDef				ledDebug1Driver;
LedDriver_TypeDef				ledDebug2Driver;

LedDriver_Pin_TypeDef ledDebug1Pin = my_LED_DEBUG1_Pin;
LedDriver_Pin_TypeDef ledDebug2Pin = my_LED_DEBUG2_Pin;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_NVIC_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void HAL_SYSTICK_Callback(void){
	if (LedDriver_1msElapsedCallbackHandler(&ledDebug1Driver) != LedDriver_Status_OK){
		Error_Handler();
	}
	if (LedDriver_1msElapsedCallbackHandler(&ledDebug2Driver) != LedDriver_Status_OK){
		Error_Handler();
	}
	/*if (ActionScheduler_1msElapsedCallbackHandler(&actionScheduler) != ActionScheduler_Status_OK){
		Error_Handler();
	}*/
}


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

#ifdef  USE_FULL_ASSERT
	GPSDriver_test();
#endif

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_SDMMC1_SD_Init();
  MX_FATFS_Init();

  /* Initialize interrupts */
  MX_NVIC_Init();
  /* USER CODE BEGIN 2 */

  if (LedDriver_init(&ledDebug1Driver, (LedDriver_Port_TypeDef*)my_LED_DEBUG1_GPIO_Port, &ledDebug1Pin) != LedDriver_Status_OK){
	  Error_Handler();
  }
  if (LedDriver_init(&ledDebug2Driver, (LedDriver_Port_TypeDef*)my_LED_DEBUG2_GPIO_Port, &ledDebug2Pin) != LedDriver_Status_OK){
	  Error_Handler();
  }

  if (FileSystemWrapper_init(&fileSystem) != FileSystemWrapper_Status_OK){
	  Error_Handler();
  }

  FileSystemWrapper_File_TypeDef file1;

  uint32_t result[50];
  uint16_t rIt = 0;

  uint16_t btw[] = {1000, 100, 1000};
  uint16_t loops[] = {1000, 1000, 100};

  if (MSTimerDriver_init(&msTimerDriver) != MSTimerDriver_Status_OK){
	  Error_Handler();
  }

  if (MSTimerDriver_startCounting(&msTimerDriver) != MSTimerDriver_Status_OK){
	  Error_Handler();
  }

  for (int it = 0; it < 3; it++){
  	  for (int retry = 0; retry < 2; retry++){

		  if (FileSystemWrapper_open(&fileSystem, &file1, "file1.out") != FileSystemWrapper_Status_OK){
			  Error_Handler();
		  }

		  FileWritingBuffer_TypeDef fwb;

		  if (FileWritingBuffer_init(&fwb, &file1) != FileWritingBuffer_Status_OK){
			  Error_Handler();
		  }

		  uint32_t startTime, endTime, diffTime;

		  uint32_t i=0;
		  uint8_t data[100000];

		  uint16_t bytesToWrite = btw[it];
		  for (int i=0; i<bytesToWrite; i++){
			  data[i] = i;
		  }

		  LedDriver_OnLed(&ledDebug1Driver);

		  if (MSTimerDriver_getMSTime(&msTimerDriver, &startTime) != MSTimerDriver_Status_OK){
			  Error_Handler();
		  }

		  uint8_t off = 1;
		  for (int i=0; i<loops[it]; i++){
			  if (FileWritingBuffer_writeString(&fwb, data, bytesToWrite) != FileWritingBuffer_Status_OK){
				  Error_Handler();
			  }
			  /*
			  uint32_t bytesWritten;
			  FileSystemWrapper_Status_TypeDef status = FileSystemWrapper_writeBinaryData(&file1, data, bytesToWrite, &bytesWritten);
			  if (status != FileSystemWrapper_Status_OK){
				  Error_Handler();
			  }
			  if (bytesWritten != bytesToWrite){
				  Error_Handler();
			  }*/
			  if (off){
				  off = 0;
				  LedDriver_OffLed(&ledDebug2Driver);
			  } else {
				  off = 1;
				  LedDriver_OnLed(&ledDebug2Driver);
			  }
		  }

		  if (MSTimerDriver_getMSTime(&msTimerDriver, &endTime) != MSTimerDriver_Status_OK){
			  Error_Handler();
		  }

		  LedDriver_OnLed(&ledDebug2Driver);

		  result[rIt++] = endTime - startTime;

		  if (FileWritingBuffer_deInit(&fwb) != FileWritingBuffer_Status_OK){
			  Error_Handler();
		  }

		  if (FileSystemWrapper_close(&file1) != FileSystemWrapper_Status_OK){
			  Error_Handler();
		  }

  	  }

  }


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
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
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Configure LSE Drive Capability 
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);
  /** Configure the main internal regulator output voltage 
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Activate the Over-Drive mode 
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC|RCC_PERIPHCLK_USART1
                              |RCC_PERIPHCLK_SDMMC1|RCC_PERIPHCLK_CLK48;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  PeriphClkInitStruct.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInitStruct.Clk48ClockSelection = RCC_CLK48SOURCE_PLL;
  PeriphClkInitStruct.Sdmmc1ClockSelection = RCC_SDMMC1CLKSOURCE_CLK48;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief NVIC Configuration.
  * @retval None
  */
static void MX_NVIC_Init(void)
{
  /* RCC_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(RCC_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(RCC_IRQn);
  /* CAN1_TX_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(CAN1_TX_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(CAN1_TX_IRQn);
  /* CAN1_RX0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(CAN1_RX0_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);
  /* CAN1_RX1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(CAN1_RX1_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(CAN1_RX1_IRQn);
  /* CAN1_SCE_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(CAN1_SCE_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(CAN1_SCE_IRQn);
  /* USART1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(USART1_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(USART1_IRQn);
  /* SDMMC1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SDMMC1_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(SDMMC1_IRQn);
  /* DMA2_Stream7_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream7_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream7_IRQn);
}

/* USER CODE BEGIN 4 */

void Warning_Handler(char* description){

}

static uint16_t errorInCounter = 0;

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  if (errorInCounter++ > 0){
	  return;
  }
  while(1)
  {
	  HAL_GPIO_WritePin(my_LED_DEBUG2_GPIO_Port, my_LED_DEBUG2_Pin, GPIO_PIN_SET);
	  HAL_GPIO_WritePin(my_LED_DEBUG1_GPIO_Port, my_LED_DEBUG1_Pin, GPIO_PIN_SET);
	  HAL_Delay(50);
	  HAL_GPIO_WritePin(my_LED_DEBUG2_GPIO_Port, my_LED_DEBUG2_Pin, GPIO_PIN_RESET);
	  HAL_GPIO_WritePin(my_LED_DEBUG1_GPIO_Port, my_LED_DEBUG1_Pin, GPIO_PIN_RESET);
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
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
