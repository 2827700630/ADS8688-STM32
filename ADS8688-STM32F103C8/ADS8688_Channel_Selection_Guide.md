/**
 * @file    ADS8688通道选择功能使用指南
 * @brief   详细说明如何使用ADS8688的通道选择功能
 * @author  GitHub Copilot
 * @date    2025-07-05
 */

# ADS8688通道选择功能使用指南

## 概述
ADS8688支持选择性通道采集，允许您只采集需要的通道，从而：
- 提高采样率（减少通道数量 = 更高的采样频率）
- 降低CPU负载和内存使用
- 节省功耗

## 1. 基本概念

### 通道掩码 (Channel Mask)
通道掩码是一个8位数值，每个位对应一个通道：
- Bit 0 = 通道0
- Bit 1 = 通道1
- ...
- Bit 7 = 通道7

### 预定义的通道选择宏
```c
#define ADS8688_CH0_ONLY    0x01    // 只采集通道0
#define ADS8688_CH1_ONLY    0x02    // 只采集通道1
#define ADS8688_CH0_CH1     0x03    // 采集通道0和通道1
#define ADS8688_CH0_TO_CH3  0x0F    // 采集通道0-3
#define ADS8688_CH4_TO_CH7  0xF0    // 采集通道4-7
#define ADS8688_AUTO_SEQ_ALL_EN 0xFF // 采集所有通道
```

## 2. 基本使用方法

### 方法1：使用预定义宏
```c
// 示例：只采集通道0和通道1
ADS8688_SetActiveChannels(&ads, ADS8688_CH0_CH1);

// 创建数据数组（大小 = 活动通道数量）
uint16_t active_data[2];  // 2个活动通道

// 读取数据
ADS8688_ReadActiveChannelsRaw(&ads, active_data, ADS8688_CH0_CH1);
```

### 方法2：使用自定义掩码
```c
// 示例：采集通道0、通道2和通道5
uint8_t custom_mask = ADS8688_AUTO_SEQ_CH0_EN | 
                      ADS8688_AUTO_SEQ_CH2_EN | 
                      ADS8688_AUTO_SEQ_CH5_EN;

ADS8688_SetActiveChannels(&ads, custom_mask);

// 创建数据数组
uint16_t active_data[3];  // 3个活动通道

// 读取数据
ADS8688_ReadActiveChannelsRaw(&ads, active_data, custom_mask);
```

## 3. 完整的使用示例

### 示例1：高速采集单通道
```c
// 在main.c中的USER CODE BEGIN 2部分
void setup_single_channel_high_speed(void)
{
    // 初始化ADS8688
    ADS8688_Init(&ads, &hspi1, ADS8688_CS_GPIO_Port, ADS8688_CS_Pin);
    
    // 设置通道0为单极性范围
    ADS8688_SetSingleChannelRange(&ads, 0, ADS8688_RANGE_UNIPOLAR_1_25_VREF);
    
    // 只启用通道0
    ADS8688_SetActiveChannels(&ads, ADS8688_CH0_ONLY);
    
    // 保存当前设置
    current_channel_ranges[0] = ADS8688_RANGE_UNIPOLAR_1_25_VREF;
}

// 在主循环中
void loop_single_channel_high_speed(void)
{
    uint16_t single_channel_data[1];
    float single_voltage;
    
    // 高速读取单通道数据
    ADS8688_ReadActiveChannelsRaw(&ads, single_channel_data, ADS8688_CH0_ONLY);
    
    // 转换为电压
    single_voltage = ADS8688_ConvertToVoltage(single_channel_data[0], 
                                             current_channel_ranges[0], 
                                             ADS8688_VREF);
    
    // 处理数据...
}
```

### 示例2：多通道选择性采集
```c
// 在main.c中的USER CODE BEGIN 2部分
void setup_selective_channels(void)
{
    // 初始化ADS8688
    ADS8688_Init(&ads, &hspi1, ADS8688_CS_GPIO_Port, ADS8688_CS_Pin);
    
    // 设置各通道范围
    ADS8688_SetSingleChannelRange(&ads, 0, ADS8688_RANGE_UNIPOLAR_1_25_VREF);
    ADS8688_SetSingleChannelRange(&ads, 1, ADS8688_RANGE_UNIPOLAR_1_25_VREF);
    ADS8688_SetSingleChannelRange(&ads, 4, ADS8688_RANGE_BIPOLAR_2_5_VREF);
    ADS8688_SetSingleChannelRange(&ads, 5, ADS8688_RANGE_BIPOLAR_2_5_VREF);
    
    // 选择通道0、1、4、5
    uint8_t selected_channels = ADS8688_AUTO_SEQ_CH0_EN | 
                               ADS8688_AUTO_SEQ_CH1_EN | 
                               ADS8688_AUTO_SEQ_CH4_EN | 
                               ADS8688_AUTO_SEQ_CH5_EN;
    
    ADS8688_SetActiveChannels(&ads, selected_channels);
    
    // 保存范围设置
    current_channel_ranges[0] = ADS8688_RANGE_UNIPOLAR_1_25_VREF;
    current_channel_ranges[1] = ADS8688_RANGE_UNIPOLAR_1_25_VREF;
    current_channel_ranges[4] = ADS8688_RANGE_BIPOLAR_2_5_VREF;
    current_channel_ranges[5] = ADS8688_RANGE_BIPOLAR_2_5_VREF;
}

// 在主循环中
void loop_selective_channels(void)
{
    uint16_t selective_data[4];  // 4个活动通道
    float selective_voltage[4];
    
    uint8_t selected_channels = ADS8688_AUTO_SEQ_CH0_EN | 
                               ADS8688_AUTO_SEQ_CH1_EN | 
                               ADS8688_AUTO_SEQ_CH4_EN | 
                               ADS8688_AUTO_SEQ_CH5_EN;
    
    // 读取选定通道的数据
    ADS8688_ReadActiveChannelsRaw(&ads, selective_data, selected_channels);
    
    // 转换为电压（注意：数据顺序为CH0, CH1, CH4, CH5）
    selective_voltage[0] = ADS8688_ConvertToVoltage(selective_data[0], current_channel_ranges[0], ADS8688_VREF); // CH0
    selective_voltage[1] = ADS8688_ConvertToVoltage(selective_data[1], current_channel_ranges[1], ADS8688_VREF); // CH1
    selective_voltage[2] = ADS8688_ConvertToVoltage(selective_data[2], current_channel_ranges[4], ADS8688_VREF); // CH4
    selective_voltage[3] = ADS8688_ConvertToVoltage(selective_data[3], current_channel_ranges[5], ADS8688_VREF); // CH5
    
    // 处理数据...
}
```

### 示例3：动态切换通道配置
```c
// 在main.c中的USER CODE BEGIN 2部分
void setup_dynamic_switching(void)
{
    // 初始化ADS8688
    ADS8688_Init(&ads, &hspi1, ADS8688_CS_GPIO_Port, ADS8688_CS_Pin);
    
    // 设置所有通道范围
    uint8_t channel_ranges[8] = {
        ADS8688_RANGE_UNIPOLAR_1_25_VREF, // CH0
        ADS8688_RANGE_UNIPOLAR_1_25_VREF, // CH1
        ADS8688_RANGE_BIPOLAR_1_25_VREF,  // CH2
        ADS8688_RANGE_BIPOLAR_1_25_VREF,  // CH3
        ADS8688_RANGE_BIPOLAR_2_5_VREF,   // CH4
        ADS8688_RANGE_BIPOLAR_2_5_VREF,   // CH5
        ADS8688_RANGE_BIPOLAR_2_5_VREF,   // CH6
        ADS8688_RANGE_BIPOLAR_2_5_VREF    // CH7
    };
    
    ADS8688_SetChannelRanges(&ads, channel_ranges);
    
    // 保存范围设置
    for(int i = 0; i < 8; i++) {
        current_channel_ranges[i] = channel_ranges[i];
    }
}

// 在主循环中
void loop_dynamic_switching(void)
{
    static uint32_t switch_counter = 0;
    static uint8_t current_mode = 0;
    
    switch_counter++;
    
    // 每1000次循环切换一次配置
    if(switch_counter >= 1000) {
        switch_counter = 0;
        current_mode = (current_mode + 1) % 3;
        
        switch(current_mode) {
            case 0: // 高速单通道模式
                ADS8688_SetActiveChannels(&ads, ADS8688_CH0_ONLY);
                break;
                
            case 1: // 前4通道模式
                ADS8688_SetActiveChannels(&ads, ADS8688_CH0_TO_CH3);
                break;
                
            case 2: // 全通道模式
                ADS8688_SetActiveChannels(&ads, ADS8688_AUTO_SEQ_ALL_EN);
                break;
        }
    }
    
    // 根据当前模式读取数据
    switch(current_mode) {
        case 0: {
            uint16_t single_data[1];
            ADS8688_ReadActiveChannelsRaw(&ads, single_data, ADS8688_CH0_ONLY);
            // 处理单通道数据...
            break;
        }
        
        case 1: {
            uint16_t quad_data[4];
            ADS8688_ReadActiveChannelsRaw(&ads, quad_data, ADS8688_CH0_TO_CH3);
            // 处理4通道数据...
            break;
        }
        
        case 2: {
            uint16_t all_data[8];
            ADS8688_ReadActiveChannelsRaw(&ads, all_data, ADS8688_AUTO_SEQ_ALL_EN);
            // 处理全通道数据...
            break;
        }
    }
}
```

## 4. 性能优化建议

### 采样率提升
- **单通道**：理论上可达到接近500kSPS
- **双通道**：约250kSPS每通道
- **四通道**：约125kSPS每通道
- **八通道**：约62.5kSPS每通道

### 内存优化
```c
// 根据最大可能的活动通道数量定义缓冲区
#define MAX_ACTIVE_CHANNELS 4
uint16_t dynamic_buffer[MAX_ACTIVE_CHANNELS];

// 使用函数获取实际的活动通道数量
uint8_t active_count = ADS8688_GetActiveChannelCount(channel_mask);
```

### CPU负载优化
```c
// 使用缓存避免重复计算
static uint8_t cached_channel_mask = 0;
static uint8_t cached_active_count = 0;

if(channel_mask != cached_channel_mask) {
    cached_channel_mask = channel_mask;
    cached_active_count = ADS8688_GetActiveChannelCount(channel_mask);
}
```

## 5. 注意事项

1. **数据数组大小**：确保数据数组大小等于活动通道数量
2. **通道顺序**：数据按通道号从小到大的顺序排列
3. **范围设置**：确保为活动通道设置正确的电压范围
4. **状态同步**：每次更改通道配置后，设备需要时间来同步状态

## 6. 故障排除

### 问题1：数据全为65535
**原因**：通道配置更改后设备状态未同步
**解决**：`ADS8688_SetActiveChannels`函数已内置NO_OP命令来解决此问题

### 问题2：数据顺序错误
**原因**：数据按通道号顺序排列，不按掩码位顺序
**解决**：正确理解数据顺序，或使用通道映射数组

### 问题3：采样率未提升
**原因**：SPI速度限制或CPU处理时间过长
**解决**：优化SPI配置，使用DMA，减少函数调用开销
