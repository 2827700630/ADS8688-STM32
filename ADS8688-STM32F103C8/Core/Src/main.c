/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "spi.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ads8688.h"
#include <stdio.h>
#include <string.h>
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

// ADS变量
ADS8688_HandleTypeDef ADS8688; // 这个设计对一个STM32接入多个ADS8688友好
uint16_t ads_data[8] = {0};
float voltage[8] = {0};

// ADS8688参考电压定义（根据实际硬件电路设置）
#define ADS8688_VREF 4.096f // 参考电压为4.096V（内部参考电压典型值）

// 存储当前通道范围设置，用于正确的电压转换
uint8_t current_channel_ranges[ADS8688_NUM_CHANNELS];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */

  HAL_GPIO_WritePin(ADS8688_RST_GPIO_Port, ADS8688_RST_Pin, GPIO_PIN_SET); // 初始化ADS8688复位引脚

  ADS8688_Init(&ADS8688, &hspi1, ADS8688_CS_GPIO_Port, ADS8688_CS_Pin); // 初始化ADS8688

  // 设置通道输入范围（在初始化之后重新配置）
  uint8_t channel_ranges[ADS8688_NUM_CHANNELS] = {
      ADS8688_RANGE_BIPOLAR_2_5_VREF, // 通道0: ±2.5 × VREF (±10.24V)
      ADS8688_RANGE_BIPOLAR_2_5_VREF, // 通道1: ±2.5 × VREF (±10.24V)
      ADS8688_RANGE_BIPOLAR_2_5_VREF, // 通道2: ±2.5 × VREF (±10.24V)
      ADS8688_RANGE_BIPOLAR_2_5_VREF, // 通道3: ±2.5 × VREF (±10.24V)
      ADS8688_RANGE_BIPOLAR_2_5_VREF, // 通道4: ±2.5 × VREF (±10.24V)
      ADS8688_RANGE_BIPOLAR_2_5_VREF, // 通道5: ±2.5 × VREF (±10.24V)
      ADS8688_RANGE_BIPOLAR_2_5_VREF, // 通道6: ±2.5 × VREF (±10.24V)
      ADS8688_RANGE_BIPOLAR_2_5_VREF  // 通道7: ±2.5 × VREF (±10.24V)
  };
  // 应用通道范围配置
  ADS8688_SetChannelRanges(&ADS8688, channel_ranges);

  // 保存当前通道范围设置以便后续电压转换使用
  for (int i = 0; i < ADS8688_NUM_CHANNELS; i++)
  {
    current_channel_ranges[i] = channel_ranges[i];
  }

  // 使用下面的函数设置采集通道数，并且要修改ADS8688_NUM_CHANNELS
  ADS8688_SetActiveChannels(&ADS8688, ADS8688_AUTO_SEQ_ALL_EN); // 采集所有通道

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

    ADS8688_ReadAllChannelsRaw(&ADS8688, ads_data);

    // 使用专用的电压转换函数将原始数据转换为实际电压
    for (int i = 0; i < ADS8688_NUM_CHANNELS; i++)
    {
      voltage[i] = ADS8688_ConvertToVoltage(ads_data[i], current_channel_ranges[i], ADS8688_VREF);
    }

    // 可以在这里添加其他非时间关键的任务
    // 例如：处理串口通信、LED指示等

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

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
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

#ifdef USE_FULL_ASSERT
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
