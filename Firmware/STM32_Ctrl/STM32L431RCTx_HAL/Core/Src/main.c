/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "uart.h"
#include "oledlib.h"
#include "hlw8032.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
extern DEVS_TypeDef oleds;
int32_t i = 0;
uint8_t data[2000] = {0};
volatile float fps1 = 0, fps2 = 0, fps3 = 0;

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

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_TIM6_Init();
  MX_ADC1_Init();
  MX_SPI2_Init();
  MX_USART1_UART_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */
  TIMER_Confi();
  UART_Init();
  OLED_Confi();

  int8_t oled_mode = 0;
  /* 显示while(1)循环次数 */
  OLED_Printf(0 + 0 * 8, 0 + 0 * 8, "L431RCT6%8d", i);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

    /* 获取8032数据 */
    if(HLW8032_Ctrl()) {
      HLW8032_Limit();

      /* 串口输出 */
      if(delayms_Timer_paral(500)) {
        printf("V=%7.3lfV  I=%7.3lfA  P=%7.3lfW  PFC=%6.4lf  W=%7.3lfkWh\r\n",
              pfcres.voltage / 1000.0, pfcres.currentIntensity / 1000.0, 
              pfcres.power / 1000.0, pfcres.powerFactorer / 1000.0, 
              pfcres.electricQuantity / 1000.0);
      }
      
      /*  更新显存 */
      if(delayus_Timer_paral(100)) {
        OLED_clearBuffer();
        switch (oled_mode) {
        case 0:
          SetFontSize(2);
          OLED_Printf(0 + 0 * 8, 0 + 0 * 8, "V=%7.3lfV", pfcres.voltage / 1000.0);
          OLED_Printf(0 + 2 * 8, 0 + 0 * 8, "I=%7.3lfA", pfcres.currentIntensity / 1000.0);
          OLED_Printf(0 + 4 * 8, 0 + 0 * 8, "P=%7.3lfW", pfcres.power / 1000.0);
          OLED_Printf(0 + 6 * 8, 0 + 0 * 8, "PFC=%5.2lf%%", pfcres.powerFactorer / 1000.0);
          SetFontSize(0);
          break;
        case 1:
          /* 显示while(1)循环次数 */
          OLED_Printf(0 + 0 * 8, 0 + 0 * 8, "L431RBT6%8d", i);
          SetFontSize(1);
          OLED_Printf(0 + 3 * 8, 0 + 0 * 8, "V=%7.3lfV I=%7.3lfA", 
                      pfcres.voltage / 1000.0, pfcres.currentIntensity / 1000.0);
          OLED_Printf(0 + 4 * 8, 0 + 0 * 8, "P=%7.3lfW PFC=%5.2lf%%", 
                      pfcres.power / 1000.0, pfcres.powerFactorer / 1000.0);
          OLED_Printf(0 + 5 * 8, 0 + 0 * 8, "W=%7.3lfkWh", 
                      pfcres.electricQuantity / 1000.0);
          SetFontSize(0);
          break;
        case 2:
          /* 显示while(1)循环次数 */
          OLED_Printf(0 + 0 * 8, 0 + 0 * 8, "L431RBT6%8d", i);
          SetFontSize(0);
          OLED_Printf(0 + 2 * 8, 0 + 0 * 8, "V=%4.1lfV  I=%4.1lfA", 
                      pfcres.voltage / 1000.0, pfcres.currentIntensity / 1000.0);
          OLED_Printf(0 + 4 * 8, 0 + 0 * 8, "P=%4.1lfW  PFC=%2.0lf%%", 
                      pfcres.power / 1000.0, pfcres.powerFactorer / 1000.0);
          OLED_Printf(0 + 6 * 8, 0 + 0 * 8, "W=%4.1lfkWh", 
                      pfcres.electricQuantity / 1000.0);
          break;
        
        default:
          break;
        }
      }
    }
    i = i==100000 ? 0 : i+1;

    /* oled显示 */
    DEV_setActStream(&oleds, 0);
    if(DEV_getActState() == idle) {
      TIMER_tick();
      // OLED_Error();
      OLED_updateScreen();
      fps2 = TIMER_query();
      DEV_setActState(500);
    }

    /* 串口回显 */
    if(UART1_ScanString((char *)data, sizeof(data))) {
        UART1_PrintString((char *)data);
    }
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

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

