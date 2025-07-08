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
#include "memorymap.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
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
// 外部变量声明
extern UART_HandleTypeDef huart1;

// SPI_HandleTypeDef hspi1;

// ADS变量
ADS8688_HandleTypeDef ads; // 这个设计对一个STM32接入多个ADS8688友好
uint16_t ads_data[8] = {0};
float voltage[8] = {0};

// ADS8688参考电压定义（根据实际硬件电路设置）
#define ADS8688_VREF 4.096f // 参考电压为4.096V（内部参考电压典型值）

// 存储当前通道范围设置，用于正确的电压转换
uint8_t current_channel_ranges[ADS8688_NUM_CHANNELS];

// 定时器中断采样相关变量
volatile uint8_t sampling_flag = 0;    // 采样标志位，在定时器中断中设置
volatile uint32_t sample_count = 0;    // 采样计数器
volatile uint8_t sampling_enabled = 1; // 采样使能标志，可用于暂停/恢复采样

// 历史记录相关变量
#define HISTORY_SIZE 512                                        // 历史记录缓冲区大小
float history_buffer[ADS8688_NUM_CHANNELS][HISTORY_SIZE] = {0}; // 历史数据缓冲区
volatile uint16_t history_index = 0;                            // 当前写入位置
volatile uint8_t history_full = 0;                              // 历史缓冲区是否已满标志
volatile uint8_t send_data_flag = 0;                            // 发送数据标志

/*
 * 历史记录功能说明：
 * 1. 使用循环缓冲区存储最近的HISTORY_SIZE个采样数据
 * 2. 支持按通道和时间索引查询历史数据
 * 3. 提供统计功能如获取最新N个数据
 * 4. 内存使用：8通道 × 512个数据点 × 4字节 = 16.384KB
 * 5. 缓冲区满时通过USART1发送全部数据
 */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MPU_Config(void);
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

  /* MPU Configuration--------------------------------------------------------*/
  MPU_Config();

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
  MX_USART1_UART_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
  HAL_GPIO_WritePin(ADS8688_RESET_GPIO_Port, ADS8688_RESET_Pin, GPIO_PIN_SET);

  // 初始化ADS8688
  ADS8688_Init(&ads, &hspi1, ADS8688_CS_GPIO_Port, ADS8688_CS_Pin);

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
  ADS8688_SetChannelRanges(&ads, channel_ranges);

  // 保存当前通道范围设置以便后续电压转换使用
  for (int i = 0; i < ADS8688_NUM_CHANNELS; i++)
  {
    current_channel_ranges[i] = channel_ranges[i];
  }

  ADS8688_SetActiveChannels(&ads, 0b00000001); // 只采集通道0，提高采样率

  // 发送启动信息
  char start_msg[] = "\r\n=== ADS8688 Data Acquisition System Started ===\r\n";
  HAL_UART_Transmit(&huart1, (uint8_t *)start_msg, strlen(start_msg), HAL_MAX_DELAY);

  // 启动定时器中断，开始定时采样
  HAL_TIM_Base_Start_IT(&htim2); // 启动TIM2中断
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    // 等待定时器中断设置采样标志
    if (sampling_flag)
    {
      sampling_flag = 0; // 清除采样标志

      // 执行采样操作
      ADS8688_ReadAllChannelsRaw(&ads, ads_data);

      // 使用专用的电压转换函数将原始数据转换为实际电压
      for (int i = 0; i < ADS8688_NUM_CHANNELS; i++)
      {
        voltage[i] = ADS8688_ConvertToVoltage(ads_data[i], current_channel_ranges[i], ADS8688_VREF);
      }

      // 将原始数据存储到历史记录缓冲区
      for (int i = 0; i < ADS8688_NUM_CHANNELS; i++)
      {
        history_buffer[i][history_index] = voltage[i];
      }

      // 更新历史记录索引（循环缓冲区）
      history_index++;
      if (history_index >= HISTORY_SIZE)
      {
        history_index = 0;
        history_full = 1;   // 标记缓冲区已满
        send_data_flag = 1; // 每次缓冲区满时都设置发送数据标志
      }

      // 增加采样计数
      sample_count++;

      // 可以在这里添加调试输出或其他处理
      // 例如：通过串口输出电压值等
      // 为了避免影响定时采样，建议使用缓冲区或状态机处理数据输出
    }

    // 检查是否需要发送历史数据
    if (send_data_flag)
    {
      send_data_flag = 0; // 清除发送标志

      // 暂停采样以避免发送过程中数据被覆盖
      uint8_t temp_sampling_enabled = sampling_enabled;
      sampling_enabled = 0;

      // 发送缓冲区数据
      ADS8688_SendHistoryData();

      // 恢复采样
      sampling_enabled = temp_sampling_enabled;
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

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 60;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 5;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/**
 * @brief 定时器中断回调函数
 * @param htim: 定时器句柄
 * @retval None
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM2) // 确保是TIM2中断
  {
    if (sampling_enabled) // 检查采样使能标志
    {
      sampling_flag = 1; // 设置采样标志，通知主循环执行采样
    }
  }
}

/**
 * @brief 控制采样的启动和停止
 * @param enable: 1-启动采样，0-停止采样
 * @retval None
 */
void ADS8688_SetSamplingEnabled(uint8_t enable)
{
  sampling_enabled = enable;
}

/**
 * @brief 通过USART1发送历史数据
 * @retval None
 */
void ADS8688_SendHistoryData(void)
{
  char buffer[200];

  // 发送开始标记
  sprintf(buffer, "\r\n=== ADS8688 History Data (Sample Count: %lu) ===\r\n", sample_count);
  HAL_UART_Transmit(&huart1, (uint8_t *)buffer, strlen(buffer), HAL_MAX_DELAY);

  // 发送表头
  sprintf(buffer, "Index,CH0(V),CH1(V),CH2(V),CH3(V),CH4(V),CH5(V),CH6(V),CH7(V)\r\n");
  HAL_UART_Transmit(&huart1, (uint8_t *)buffer, strlen(buffer), HAL_MAX_DELAY);

  // 发送数据
  for (uint16_t i = 0; i < HISTORY_SIZE; i++)
  {
    // 计算实际的数据索引（按时间顺序）
    uint16_t actual_index = (history_index + i) % HISTORY_SIZE;

    sprintf(buffer, "%d,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f\r\n",
            i,
            history_buffer[0][actual_index],
            history_buffer[1][actual_index],
            history_buffer[2][actual_index],
            history_buffer[3][actual_index],
            history_buffer[4][actual_index],
            history_buffer[5][actual_index],
            history_buffer[6][actual_index],
            history_buffer[7][actual_index]);

    HAL_UART_Transmit(&huart1, (uint8_t *)buffer, strlen(buffer), HAL_MAX_DELAY);
  }
  // 发送结束标记
  sprintf(buffer, "=== End of History Data ===\r\n\r\n");
  HAL_UART_Transmit(&huart1, (uint8_t *)buffer, strlen(buffer), HAL_MAX_DELAY);
  HAL_Delay(5);
  // 可选：发送完成后重置缓冲区状态，便于下一次数据收集
  // 注意：这里不清空数据，只是重置状态标志
  // history_full = 0;  // 取消注释此行可以重置缓冲区满标志
}

/* USER CODE END 4 */

 /* MPU Configuration */

void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct = {0};

  /* Disables the MPU */
  HAL_MPU_Disable();

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.BaseAddress = 0x0;
  MPU_InitStruct.Size = MPU_REGION_SIZE_4GB;
  MPU_InitStruct.SubRegionDisable = 0x87;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  /* Enables the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

}

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
