/**
 * @file    main.c 通道选择功能示例
 * @brief   展示如何在现有main.c中使用通道选择功能
 * @note    将此代码添加到您的main.c中替换现有的初始化和循环代码
 */

// 在 USER CODE BEGIN PV 部分添加以下变量
/* USER CODE BEGIN PV */
// ADS变量
ADS8688_HandleTypeDef ads;
uint16_t ads_data[8];
float voltage[8] = {0};

// 通道选择相关变量
uint8_t selected_channels = ADS8688_CH0_CH1;  // 选择通道0和1
uint16_t selected_data[2];  // 对应2个通道的数据
float selected_voltage[2];  // 对应2个通道的电压
uint8_t active_channel_count = 2;  // 活动通道数量

// ADS8688参考电压定义
#define ADS8688_VREF 4.096f

// 存储当前通道范围设置
uint8_t current_channel_ranges[ADS8688_NUM_CHANNELS];
/* USER CODE END PV */

// 在 USER CODE BEGIN 2 部分添加以下初始化代码
/* USER CODE BEGIN 2 */
// 初始化ADS8688复位引脚
HAL_GPIO_WritePin(ADS8688_RST_GPIO_Port, ADS8688_RST_Pin, GPIO_PIN_SET);

// 初始化ADS8688
ADS8688_Init(&ads, &hspi1, ADS8688_CS_GPIO_Port, ADS8688_CS_Pin);

// 设置通道输入范围
uint8_t channel_ranges[ADS8688_NUM_CHANNELS] = {
    ADS8688_RANGE_UNIPOLAR_1_25_VREF, // 通道0: 0 to 1.25 × VREF (0-5.12V)
    ADS8688_RANGE_UNIPOLAR_1_25_VREF, // 通道1: 0 to 1.25 × VREF (0-5.12V)
    ADS8688_RANGE_BIPOLAR_2_5_VREF,   // 通道2: ±2.5 × VREF (±10.24V)
    ADS8688_RANGE_BIPOLAR_2_5_VREF,   // 通道3: ±2.5 × VREF (±10.24V)
    ADS8688_RANGE_BIPOLAR_2_5_VREF,   // 通道4: ±2.5 × VREF (±10.24V)
    ADS8688_RANGE_BIPOLAR_2_5_VREF,   // 通道5: ±2.5 × VREF (±10.24V)
    ADS8688_RANGE_BIPOLAR_2_5_VREF,   // 通道6: ±2.5 × VREF (±10.24V)
    ADS8688_RANGE_BIPOLAR_2_5_VREF    // 通道7: ±2.5 × VREF (±10.24V)
};

// 应用通道范围配置
ADS8688_SetChannelRanges(&ads, channel_ranges);

// 保存当前通道范围设置
for(int i = 0; i < ADS8688_NUM_CHANNELS; i++) {
    current_channel_ranges[i] = channel_ranges[i];
}

// 设置活动通道（只采集通道0和1，提高采样率）
ADS8688_SetActiveChannels(&ads, selected_channels);

// 获取活动通道数量
active_channel_count = ADS8688_GetActiveChannelCount(selected_channels);
/* USER CODE END 2 */

// 在主循环中添加以下代码
/* USER CODE BEGIN WHILE */
while (1)
{
    // 方法1：读取选定的通道（高效）
    ADS8688_ReadActiveChannelsRaw(&ads, selected_data, selected_channels);
    
    // 转换为电压（注意：数据顺序为CH0, CH1）
    selected_voltage[0] = ADS8688_ConvertToVoltage(selected_data[0], current_channel_ranges[0], ADS8688_VREF);
    selected_voltage[1] = ADS8688_ConvertToVoltage(selected_data[1], current_channel_ranges[1], ADS8688_VREF);
    
    // 方法2：如果需要，也可以读取所有通道（兼容性）
    // ADS8688_ReadAllChannelsRaw(&ads, ads_data);
    // for (int i = 0; i < ADS8688_NUM_CHANNELS; i++)
    // {
    //     voltage[i] = ADS8688_ConvertToVoltage(ads_data[i], current_channel_ranges[i], ADS8688_VREF);
    // }
    
    // 在这里处理数据...
    // 例如：通过串口输出电压值、存储到缓冲区、进行信号处理等
    
    /* USER CODE END WHILE */
    
    /* USER CODE BEGIN 3 */
}
/* USER CODE END 3 */

// 如果需要动态切换通道配置，可以添加以下函数
/* USER CODE BEGIN 4 */

/**
 * @brief 动态切换到单通道高速模式
 */
void SwitchToSingleChannelMode(void)
{
    selected_channels = ADS8688_CH0_ONLY;
    ADS8688_SetActiveChannels(&ads, selected_channels);
    active_channel_count = ADS8688_GetActiveChannelCount(selected_channels);
}

/**
 * @brief 动态切换到双通道模式
 */
void SwitchToDoubleChannelMode(void)
{
    selected_channels = ADS8688_CH0_CH1;
    ADS8688_SetActiveChannels(&ads, selected_channels);
    active_channel_count = ADS8688_GetActiveChannelCount(selected_channels);
}

/**
 * @brief 动态切换到四通道模式
 */
void SwitchToQuadChannelMode(void)
{
    selected_channels = ADS8688_CH0_TO_CH3;
    ADS8688_SetActiveChannels(&ads, selected_channels);
    active_channel_count = ADS8688_GetActiveChannelCount(selected_channels);
}

/**
 * @brief 动态切换到全通道模式
 */
void SwitchToAllChannelMode(void)
{
    selected_channels = ADS8688_AUTO_SEQ_ALL_EN;
    ADS8688_SetActiveChannels(&ads, selected_channels);
    active_channel_count = ADS8688_GetActiveChannelCount(selected_channels);
}

/* USER CODE END 4 */
