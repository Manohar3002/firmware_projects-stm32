/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "stm32g0xx_hal.h"

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

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define PWR_CTRL_Pin GPIO_PIN_14
#define PWR_CTRL_GPIO_Port GPIOC
#define TOUCH_RD_Pin GPIO_PIN_15
#define TOUCH_RD_GPIO_Port GPIOC
#define led_G_Pin GPIO_PIN_0
#define led_G_GPIO_Port GPIOA
#define led_B_Pin GPIO_PIN_1
#define led_B_GPIO_Port GPIOA
#define TXD_Pin GPIO_PIN_2
#define TXD_GPIO_Port GPIOA
#define RXD_Pin GPIO_PIN_3
#define RXD_GPIO_Port GPIOA
#define VSENS_Pin GPIO_PIN_4
#define VSENS_GPIO_Port GPIOA
#define BT_RST_Pin GPIO_PIN_5
#define BT_RST_GPIO_Port GPIOA
#define LDR_Pin GPIO_PIN_6
#define LDR_GPIO_Port GPIOA
#define BTM_LED_Pin GPIO_PIN_8
#define BTM_LED_GPIO_Port GPIOA
#define BTM_NIR_Pin GPIO_PIN_6
#define BTM_NIR_GPIO_Port GPIOC
#define POWER_CTRL_REG_Pin GPIO_PIN_15
#define POWER_CTRL_REG_GPIO_Port GPIOA
#define BAT_FULL_Pin GPIO_PIN_4
#define BAT_FULL_GPIO_Port GPIOB
#define BAT_CHG_Pin GPIO_PIN_5
#define BAT_CHG_GPIO_Port GPIOB
#define led_R_Pin GPIO_PIN_8
#define led_R_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
