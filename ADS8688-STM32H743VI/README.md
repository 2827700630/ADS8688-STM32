# ADS8688-STM32H743VI 高性能驱动

基于 STM32H743VI 微控制器的 ADS8688 高性能数据采集系统，支持精确的定时器采样控制和大容量数据缓冲。

## 🎯 项目特点

本示例程序专门针对**高性能数据采集**应用设计，相比STM32F103C8基础版本，具有以下显著优势：

- **高性能处理器**：480MHz ARM Cortex-M7内核，1MB RAM，2MB Flash
- **精确定时器采样**：基于TIM2的高精度采样率控制
- **大容量数据缓冲**：支持长时间连续采样和数据存储
- **优化的数据处理**：高效的数据管理和传输机制
- **工业级稳定性**：适用于要求苛刻的工业应用

## 🚀 快速开始

### 硬件准备

使用与[根目录README](../README.md)相同的硬件连接配置。

### 软件配置

1. **开发环境**：
   - STM32CubeIDE 或 platformIO
   - STM32CubeMX（用于配置生成）
   - STM32H7 HAL库

2. **关键配置项**：
   - **系统时钟**：480MHz
   - **TIM2配置**：用于精确采样控制
   - **SPI1配置**：与ADS8688通信
   - **USART3配置**：调试和数据输出


## ⚙️ 定时器采样控制

### 核心特性

本版本的最大特点是**精确的定时器采样控制**，通过TIM2定时器中断实现固定采样率的数据采集。

**采样率计算公式**：
```
采样率 = TIM2时钟频率 / ((Prescaler+1) × (Period+1))
```

**STM32H743VI配置**：
- TIM2时钟：240MHz（APB1×2）
- 采样率 = 240,000,000 / ((PSC+1) × (ARR+1))

### 常用采样率配置

| 采样率 | Prescaler (PSC) | Counter Period (ARR) | 说明 |
|--------|-----------------|---------------------|------|
| 100 kHz | 479 | 4 | 超高速单通道采集 |
| 50 kHz | 479 | 9 | **推荐配置** |
| 25 kHz | 479 | 19 | 中高速多通道 |
| 10 kHz | 479 | 49 | 标准音频采集 |
| 1 kHz | 479 | 499 | 控制系统采集 |

详细配置表请参考：[TIM2采样率配置表](TIM2_SamplingRate_Table.md)

### 代码示例

```c
// 启动定时器采样
HAL_TIM_Base_Start_IT(&htim2);
ADS8688_SetSamplingEnabled(true);

// 在主循环中处理采样数据
while(1) {
    if(sampling_flag) {
        sampling_flag = 0;
        
        // 读取ADS8688数据
        ADS8688_ReadAllChannelsRaw(&ads, ads_data);
        
        // 数据处理...
    }
}

// 定时器中断回调
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if(htim->Instance == TIM2) {
        sampling_flag = 1;
        sample_count++;
    }
}
```

## 📊 数据缓冲机制

### 历史数据存储

```c
// 历史数据缓冲区（512个采样点 × 8通道）
#define HISTORY_BUFFER_SIZE 512
uint16_t history_data[8][HISTORY_BUFFER_SIZE];

// 获取历史数据
uint16_t value = ADS8688_GetHistoryData(channel, index);
```

### 数据输出格式

通过USART3输出CSV格式的采样数据：

```
=== ADS8688 History Data ===
Index,CH0,CH1,CH2,CH3,CH4,CH5,CH6,CH7
0,2048,2049,2047,2050,2048,2049,2047,2048
1,2049,2048,2048,2049,2047,2048,2048,2049
...
=== End of History Data ===
```

## 🔧 高级配置

### 多通道采样优化

```c
// 选择性通道采样，提高单通道采样率
ADS8688_SetActiveChannels(&ads, ADS8688_CH0_CH1);  // 只采集通道0和1

// 读取选定通道数据
uint16_t selected_data[2];
ADS8688_ReadActiveChannelsRaw(&ads, selected_data, ADS8688_CH0_CH1);
```

### 采样率验证

使用ADS8688采集已知频率的正弦波，使用串口助手接收一个完整的采样周期数据包。数据示例：[采样数据_在通道0输入1kHz正弦波.txt](./采样数据_在通道0输入1kHz正弦波.txt) 
打开[FFT分析器](./FFT_Analyzer.html)进行频谱分析。这个工具使用FFT算法对采样数据进行频谱分析，反向推导采样率。

## 📈 性能指标

### 实际性能测试

| 配置  | 实际采样率 | 
|------|------------|
| 单通道 | ~100kSPS | 
| 双通道 | ~60kSPS/通道 |
| 八通道 | ~12kSPS/通道 |


## 🛠️ 开发工具

### FFT分析器

项目包含一个独立的HTML工具 `FFT_Analyzer.html`，用于：

- 手动输入采样数据进行FFT分析
- 验证采样率准确性
- 频谱分析和信号质量评估

**使用方法**：
1. 将采样数据导出为CSV格式
2. 打开 `FFT_Analyzer.html`
3. 粘贴数据，选择通道，设置信号频率
4. 点击"频谱分析"查看结果

### CubeMX配置

**TIM2配置步骤**：
1. 选择 Timers → TIM2
2. 设置 Clock Source: Internal Clock
3. 配置 Prescaler 和 Period（参考采样率表）
4. 启用 TIM2 global interrupt
5. 生成代码

**SPI1配置**：
- Mode: Full-Duplex Master
- Data Size: 16 Bits
- Clock Polarity (CPOL): Low
- Clock Phase (CPHA): 1 Edge

## 🐛 故障排除

### H743特有问题

1. **时钟配置问题**
   - 确认SYSCLK配置为480MHz
   - 检查APB1和APB2时钟配置
   - 验证TIM2时钟源

2. **定时器中断问题**
   - 检查NVIC中断优先级配置
   - 确认TIM2中断已启用
   - 验证中断服务函数实现

3. **高速采样问题**
   - 监控CPU占用率
   - 检查内存使用情况
   - 优化数据处理时间

### 调试技巧

```c
// 采样率统计
static uint32_t last_tick = 0;
static uint32_t last_count = 0;

if(HAL_GetTick() - last_tick >= 1000) {
    uint32_t rate = sample_count - last_count;
    printf("实际采样率: %lu Hz\n", rate);
    last_count = sample_count;
    last_tick = HAL_GetTick();
}
```

## 📚 相关文档

- [定时器采样率配置表](TIM2_SamplingRate_Table.md)
- [项目根目录说明](../README.md)
- [STM32F103C8基础版本](../ADS8688-STM32F103C8/README.md)

## 🔄 从F103版本迁移

如果您已经熟悉STM32F103C8版本，迁移到H743版本主要需要注意：

1. **时钟配置差异**：H743的时钟树更复杂
2. **定时器功能**：H743版本增加了定时器采样控制
3. **性能提升**：可以支持更高的采样率和更多功能
4. **内存管理**：更大的内存空间允许更复杂的数据处理

---

**注意**：本版本适用于对性能有较高要求的应用场景。如果是初次使用ADS8688，建议先从[STM32F103C8基础版本](../ADS8688-STM32F103C8/README.md)开始学习。
