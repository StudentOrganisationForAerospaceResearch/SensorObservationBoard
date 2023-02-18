/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
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
#include "stm32f4xx_hal.h"

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

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define RADIO_UART_TX_Pin GPIO_PIN_2
#define RADIO_UART_TX_GPIO_Port GPIOA
#define RADIO_UART_RX_Pin GPIO_PIN_3
#define RADIO_UART_RX_GPIO_Port GPIOA
#define Clk_pin_Pin GPIO_PIN_9
#define Clk_pin_GPIO_Port GPIOD
#define Data_pin_Pin GPIO_PIN_10
#define Data_pin_GPIO_Port GPIOD
#define LED_1_Pin GPIO_PIN_12
#define LED_1_GPIO_Port GPIOD
#define LED_2_Pin GPIO_PIN_14
#define LED_2_GPIO_Port GPIOD
#define LED_3_Pin GPIO_PIN_15
#define LED_3_GPIO_Port GPIOD
#define DEBUG_UART_TX_Pin GPIO_PIN_6
#define DEBUG_UART_TX_GPIO_Port GPIOC
#define DEBUG_UART_RX_Pin GPIO_PIN_7
#define DEBUG_UART_RX_GPIO_Port GPIOC
#define GPS_UART_TX_Pin GPIO_PIN_9
#define GPS_UART_TX_GPIO_Port GPIOA
#define GPS_UART_RX_Pin GPIO_PIN_10
#define GPS_UART_RX_GPIO_Port GPIOA
#define MEM_WP_Pin GPIO_PIN_6
#define MEM_WP_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
