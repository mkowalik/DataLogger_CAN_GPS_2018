/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

#define	UBLOX_M8_GPS	0
#define	SIM28_GPS		1

#define	USED_GPS		UBLOX_8M_GPS

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
void Warning_Handler(char* description);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define my_LED_DEBUG2_Pin GPIO_PIN_0
#define my_LED_DEBUG2_GPIO_Port GPIOB
#define my_LED_DEBUG1_Pin GPIO_PIN_1
#define my_LED_DEBUG1_GPIO_Port GPIOB
#define my_GPS_UART_TX_Pin GPIO_PIN_14
#define my_GPS_UART_TX_GPIO_Port GPIOB
#define my_GPS_UART_RX_Pin GPIO_PIN_15
#define my_GPS_UART_RX_GPIO_Port GPIOB
#define my_GPS_RESET_N_Pin GPIO_PIN_9
#define my_GPS_RESET_N_GPIO_Port GPIOD
#define my_SDMMC_IRQ_Pin GPIO_PIN_0
#define my_SDMMC_IRQ_GPIO_Port GPIOD
#define my_SDMMC_CS_CD_Pin GPIO_PIN_1
#define my_SDMMC_CS_CD_GPIO_Port GPIOD
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
