/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define PA2_BLE_RX_Pin GPIO_PIN_2
#define PA2_BLE_RX_GPIO_Port GPIOA
#define PA3_BLE_TX_Pin GPIO_PIN_3
#define PA3_BLE_TX_GPIO_Port GPIOA
#define PA4_STATE_Pin GPIO_PIN_4
#define PA4_STATE_GPIO_Port GPIOA
#define PA6_TRIG_Pin GPIO_PIN_6
#define PA6_TRIG_GPIO_Port GPIOA
#define PA7_LS_ADC_Pin GPIO_PIN_7
#define PA7_LS_ADC_GPIO_Port GPIOA
#define PB0_RD_Pin GPIO_PIN_0
#define PB0_RD_GPIO_Port GPIOB
#define PB1_INF_Pin GPIO_PIN_1
#define PB1_INF_GPIO_Port GPIOB
#define PB10_INFC_Pin GPIO_PIN_10
#define PB10_INFC_GPIO_Port GPIOB
#define PB11_ECHO_Pin GPIO_PIN_11
#define PB11_ECHO_GPIO_Port GPIOB
#define PB9_MF_Pin GPIO_PIN_9
#define PB9_MF_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
