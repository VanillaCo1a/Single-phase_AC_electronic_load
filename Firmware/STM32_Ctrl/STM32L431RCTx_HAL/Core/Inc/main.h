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
#include "stm32l4xx_hal.h"

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
#define BOARD_ADC_KEY_Pin GPIO_PIN_0
#define BOARD_ADC_KEY_GPIO_Port GPIOA
#define HLW8032_USART3_TX_Pin GPIO_PIN_4
#define HLW8032_USART3_TX_GPIO_Port GPIOC
#define HLW8032_USART3_RX_Pin GPIO_PIN_5
#define HLW8032_USART3_RX_GPIO_Port GPIOC
#define BOARD_OLED_DC_Pin GPIO_PIN_0
#define BOARD_OLED_DC_GPIO_Port GPIOB
#define BOARD_OLED_CS_Pin GPIO_PIN_1
#define BOARD_OLED_CS_GPIO_Port GPIOB
#define BOARD_ZK_CS_Pin GPIO_PIN_2
#define BOARD_ZK_CS_GPIO_Port GPIOB
#define BOARD_OLED_SPI2_SCK_Pin GPIO_PIN_13
#define BOARD_OLED_SPI2_SCK_GPIO_Port GPIOB
#define BOARD_OLED_SPI2_SDI_Pin GPIO_PIN_14
#define BOARD_OLED_SPI2_SDI_GPIO_Port GPIOB
#define BOARD_OLED_SPI2_SDO_Pin GPIO_PIN_15
#define BOARD_OLED_SPI2_SDO_GPIO_Port GPIOB
#define INVERTER_TIM1_CH1_Pin GPIO_PIN_8
#define INVERTER_TIM1_CH1_GPIO_Port GPIOA
#define BOARD_DEBUG_USART1_TX_Pin GPIO_PIN_9
#define BOARD_DEBUG_USART1_TX_GPIO_Port GPIOA
#define BOARD_DEBUG_USART1_RX_Pin GPIO_PIN_10
#define BOARD_DEBUG_USART1_RX_GPIO_Port GPIOA
#define INVERTER_TIM1_CH4_Pin GPIO_PIN_11
#define INVERTER_TIM1_CH4_GPIO_Port GPIOA
#define SPI3_SCK_Pin GPIO_PIN_10
#define SPI3_SCK_GPIO_Port GPIOC
#define SPI3_SDA_Pin GPIO_PIN_11
#define SPI3_SDA_GPIO_Port GPIOC
#define SPI3_SDO_Pin GPIO_PIN_12
#define SPI3_SDO_GPIO_Port GPIOC
#define DIGITAL_POTENTIOMETER_I2C1_SCL_Pin GPIO_PIN_8
#define DIGITAL_POTENTIOMETER_I2C1_SCL_GPIO_Port GPIOB
#define DIGITAL_POTENTIOMETER_I2C1_SDA_Pin GPIO_PIN_9
#define DIGITAL_POTENTIOMETER_I2C1_SDA_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
