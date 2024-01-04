/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "stm32wlxx_hal.h"

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
#define I2C2_SDA_Pin GPIO_PIN_15
#define I2C2_SDA_GPIO_Port GPIOA
#define I2C2_SCL_Pin GPIO_PIN_15
#define I2C2_SCL_GPIO_Port GPIOB
#define usbPowerPresent_Pin GPIO_PIN_4
#define usbPowerPresent_GPIO_Port GPIOB
#define writeProtect_Pin GPIO_PIN_9
#define writeProtect_GPIO_Port GPIOB
#define displayDataCommand_Pin GPIO_PIN_14
#define displayDataCommand_GPIO_Port GPIOB
#define displayChipSelect_Pin GPIO_PIN_5
#define displayChipSelect_GPIO_Port GPIOB
#define displayReset_Pin GPIO_PIN_0
#define displayReset_GPIO_Port GPIOA
#define displayBusy_Pin GPIO_PIN_10
#define displayBusy_GPIO_Port GPIOB
#define rfControl1_Pin GPIO_PIN_4
#define rfControl1_GPIO_Port GPIOA
#define rfControl2_Pin GPIO_PIN_5
#define rfControl2_GPIO_Port GPIOA
#define loraTiming_Pin GPIO_PIN_9
#define loraTiming_GPIO_Port GPIOA

void   MX_SUBGHZ_Init(void);
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
