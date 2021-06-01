/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
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
#include "stm32f1xx_hal.h"

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

// System Defines
//---------------
#define PROG_NAME "DMX_CTRL"
#define VERSION_MAJOR 1
#define VERSION_MINOR 1



//Feature Defines
//---------------


//Enable USB Support. There are numerous issues with USB support (specifcally that it was working at some point but now it is not...
//No idea why, could be a framework issue. At the moment this is a global define to remove it completely...)
//Also saves 16% of RAM...
//#define SUPPORT_USB
//#define ENABLE_WATCHDOG

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED_Pin GPIO_PIN_13
#define LED_GPIO_Port GPIOC
#define MODE_0_Pin GPIO_PIN_14
#define MODE_0_GPIO_Port GPIOC
#define MODE_1_Pin GPIO_PIN_15
#define MODE_1_GPIO_Port GPIOC
#define ANALOG_0_Pin GPIO_PIN_0
#define ANALOG_0_GPIO_Port GPIOA
#define CH1_R_Pin GPIO_PIN_1
#define CH1_R_GPIO_Port GPIOA
#define CH1_G_Pin GPIO_PIN_2
#define CH1_G_GPIO_Port GPIOA
#define CH1_B_Pin GPIO_PIN_3
#define CH1_B_GPIO_Port GPIOA
#define ANALOG_1_Pin GPIO_PIN_4
#define ANALOG_1_GPIO_Port GPIOA
#define BUTTON_Pin GPIO_PIN_5
#define BUTTON_GPIO_Port GPIOA
#define CH2_R_Pin GPIO_PIN_6
#define CH2_R_GPIO_Port GPIOA
#define CH2_G_Pin GPIO_PIN_7
#define CH2_G_GPIO_Port GPIOA
#define CH2_B_Pin GPIO_PIN_0
#define CH2_B_GPIO_Port GPIOB
#define CH2_W_Pin GPIO_PIN_1
#define CH2_W_GPIO_Port GPIOB
#define RS232_TX_Pin GPIO_PIN_10
#define RS232_TX_GPIO_Port GPIOB
#define RS232_RX_Pin GPIO_PIN_11
#define RS232_RX_GPIO_Port GPIOB
#define ADDR_5_Pin GPIO_PIN_12
#define ADDR_5_GPIO_Port GPIOB
#define ADDR_6_Pin GPIO_PIN_13
#define ADDR_6_GPIO_Port GPIOB
#define ADDR_7_Pin GPIO_PIN_14
#define ADDR_7_GPIO_Port GPIOB
#define WS8212_CH2_Pin GPIO_PIN_15
#define WS8212_CH2_GPIO_Port GPIOB
#define CH3_R_Pin GPIO_PIN_8
#define CH3_R_GPIO_Port GPIOA
#define CH3_G_Pin GPIO_PIN_9
#define CH3_G_GPIO_Port GPIOA
#define CH3_B_Pin GPIO_PIN_10
#define CH3_B_GPIO_Port GPIOA
#define ADDR_0_Pin GPIO_PIN_15
#define ADDR_0_GPIO_Port GPIOA
#define ADDR_1_Pin GPIO_PIN_3
#define ADDR_1_GPIO_Port GPIOB
#define ADDR_2_Pin GPIO_PIN_4
#define ADDR_2_GPIO_Port GPIOB
#define WS8212_CH1_Pin GPIO_PIN_5
#define WS8212_CH1_GPIO_Port GPIOB
#define RS485_RXTX_Pin GPIO_PIN_6
#define RS485_RXTX_GPIO_Port GPIOB
#define RS485_DIR_Pin GPIO_PIN_7
#define RS485_DIR_GPIO_Port GPIOB
#define ADDR_3_Pin GPIO_PIN_8
#define ADDR_3_GPIO_Port GPIOB
#define ADDR_4_Pin GPIO_PIN_9
#define ADDR_4_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
