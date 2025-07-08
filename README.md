# ADS8688-STM32 驱动程序集合

一个完整的 ADS8688 八通道同步采样模数转换器的 STM32 驱动程序集合，支持多种STM32平台和应用场景。

> 本代码在 [obatsis/ADS8688-STM32](https://github.com/obatsis/ADS8688-STM32) 的基础上进行修改和扩展，增加了更多功能。  
> 感谢原作者的贡献。

## 📋 项目概述

ADS8688 是一款 16 位、8 通道、同步采样模数转换器，具有 500kSPS 的采样率。本项目提供了两个完整的示例程序，分别针对不同的STM32平台和应用需求。

### 🎯 主要特性

- **高精度采样**：16 位分辨率，500kSPS 采样率
- **8 通道同步采样**：支持多通道同步数据采集
- **定时器采样控制**：基于定时器中断的精确采样率控制
- **灵活的通道选择**：支持选择性通道采集，提高采样率
- **多种电压范围**：支持单极性和双极性电压范围
- **SPI 接口**：标准 SPI 通信协议
- **完整的驱动库**：包含初始化、配置、数据采集等完整功能
- **多平台支持**：支持STM32F103和STM32H743系列

### 📁 项目结构

```
ADS8688-STM32/
├── ADS8688-STM32F103C8/          # STM32F103C8 基础驱动示例
│   ├── Core/                     # 核心源代码
│   ├── Drivers/                  # STM32 HAL 驱动
│   ├── README.md                 # F103专用说明
│   └── ...
├── ADS8688-STM32H743VI/          # STM32H743VI 高性能示例
│   ├── Core/                     # 核心源代码
│   ├── Drivers/                  # STM32 HAL 驱动
│   ├── README.md                 # H743专用说明
│   ├── TIM2_SamplingRate_Table.md # 定时器采样率配置表
│   └── ...
├── docs/                         # 共享文档
│   ├── ADS8688_SPI_Interface_Guide.md
│   └── ADS8688_Channel_Selection_Guide.md
└── README.md                     # 本文件
```

## 🚀 示例程序选择

### STM32F103C8 基础版 (`ADS8688-STM32F103C8/`)

**适用场景**：
- 学习ADS8688基本使用方法
- 资源有限的应用
- 简单的数据采集需求
- 成本敏感的项目

**主要特性**：
- 基于STM32F103C8T6（Blue Pill）
- 72MHz ARM Cortex-M3内核
- 基础SPI通信驱动
- 简单的轮询采样模式
- 完整的API接口
- 详细的使用示例

### STM32H743VI 高性能版 (`ADS8688-STM32H743VI/`)

**适用场景**：
- 精确的定时器采样控制采样率

**主要特性**：
- 基于STM32H743VI
- 480MHz ARM Cortex-M7内核
- 精确的定时器中断采样
- 高精度采样率控制
- 优化的数据缓冲机制
- 详细的采样率配置表

## 🛠️ 硬件要求

### 共同硬件要求

| 组件 | 规格 | 说明 |
|------|------|------|
| **ADC 芯片** | ADS8688 | 16位，8通道，500kSPS |
| **通信接口** | SPI | 标准 SPI 协议 |
| **数字电源** | +3.3V | 数字部分供电 |
| **模拟电源** | +5V | 模拟部分供电 |
| **参考电压** | 可配置 | REFSEL 引脚接高电平：由 VREF 引脚输入<br>REFSEL 引脚接低电平：内部 4.095V |

### 引脚连接表

| 功能 | ADS8688 引脚 | STM32F103C8 | STM32H743VI |
|------|-------------|-------------|-------------|
| SPI 时钟 | SCLK | PA5 (SPI1_SCK) | PA5 (SPI1_SCK) |
| SPI 数据输出 | SDO | PA6 (SPI1_MISO) | PA6 (SPI1_MISO) |
| SPI 数据输入 | SDI | PA7 (SPI1_MOSI) | PA7 (SPI1_MOSI) |
| 片选信号 | CS | 用户定义 | 用户定义 |
| 复位信号 | RST | 用户定义 | 用户定义 |

## 🔧 电压范围配置

支持以下电压范围：

| 范围宏 | 电压范围 | 说明 |
|--------|----------|------|
| `ADS8688_RANGE_BIPOLAR_2_5_VREF` | ±2.5 × VREF | 双极性 ±10.24V |
| `ADS8688_RANGE_BIPOLAR_1_25_VREF` | ±1.25 × VREF | 双极性 ±5.12V |
| `ADS8688_RANGE_BIPOLAR_0_625_VREF` | ±0.625 × VREF | 双极性 ±2.56V |
| `ADS8688_RANGE_UNIPOLAR_2_5_VREF` | 0 to 2.5 × VREF | 单极性 0-10.24V |
| `ADS8688_RANGE_UNIPOLAR_1_25_VREF` | 0 to 1.25 × VREF | 单极性 0-5.12V |

## 📚 核心API参考

### 初始化函数

```c
uint8_t ADS8688_Init(ADS8688_HandleTypeDef *hADS8688, 
                     SPI_HandleTypeDef *hspi, 
                     GPIO_TypeDef *cs_port, 
                     uint16_t cs_pin);
```

### 配置函数

```c
// 设置所有通道范围
HAL_StatusTypeDef ADS8688_SetChannelRanges(ADS8688_HandleTypeDef *hADS8688, 
                                           uint8_t *channel_ranges);

// 设置活动通道
HAL_StatusTypeDef ADS8688_SetActiveChannels(ADS8688_HandleTypeDef *hADS8688, 
                                           uint8_t channel_mask);
```

### 数据采集函数

```c
// 读取所有通道数据
HAL_StatusTypeDef ADS8688_ReadAllChannelsRaw(ADS8688_HandleTypeDef *hADS8688, 
                                            uint16_t *data);

// 电压转换
float ADS8688_ConvertToVoltage(uint16_t raw_value, 
                               uint8_t range, 
                               float vref);
```

## 🚀 快速开始

1. **选择合适的示例程序**：
   - 初学者或资源有限：选择 `ADS8688-STM32F103C8`
   - 高性能需求：选择 `ADS8688-STM32H743VI`

2. **硬件连接**：
   - 按照引脚连接表连接硬件
   - 确保电源和参考电压正确连接

3. **软件配置**：
   - 进入对应的示例目录
   - 按照各示例的README进行配置
   - 编译和下载程序

4. **验证功能**：
   - 连接信号源到ADS8688输入
   - 观察采样数据输出
   - 验证电压转换准确性

## 📊 性能对比

| 特性 | STM32F103C8 | STM32H743VI |
|------|-------------|-------------|
| CPU频率 | 72MHz | 480MHz |
| 内存 | 20KB RAM, 64KB Flash | 1MB RAM, 2MB Flash |
| 采样控制 | 轮询模式 | 定时器中断 |
| 最大采样率 | - | ~200kSPS |
| 数据缓冲 | 基础 | 高级缓冲机制 |
| 采样精度 | 标准 | 高精度定时器控制 |
| 应用场景 | 学习、简单应用 | 工业级、高性能 |

## 📖 详细文档

- [STM32F103C8 基础驱动说明](ADS8688-STM32F103C8/README.md)
- [STM32H743VI 高性能驱动说明](ADS8688-STM32H743VI/README.md)
- [定时器采样率配置表](ADS8688-STM32H743VI/TIM2_SamplingRate_Table.md)
- [SPI接口使用指南](docs/ADS8688_SPI_Interface_Guide.md)
- [通道选择指南](docs/ADS8688_Channel_Selection_Guide.md)

## 🐛 故障排除

### 常见问题

1. **SPI通信失败**
   - 检查硬件连接
   - 确认SPI配置正确
   - 检查片选信号时序

2. **电压转换不准确**
   - 确认参考电压值正确
   - 检查通道范围设置

3. **采样率问题**
   - 检查定时器配置（H743版本）
   - 确认系统时钟配置
   - 检查中断优先级

### 调试建议

1. 从基础版本开始验证硬件连接
2. 使用示波器检查SPI信号
3. 逐步测试各个功能模块
4. 参考各示例的详细故障排除说明

## 🤝 贡献

欢迎提交 Issue 和 Pull Request 来改进本项目。

## 📄 许可证

本项目采用 MIT 许可证，详见 [LICENSE](LICENSE) 文件。

## 📞 联系

如有问题或建议，请提交 Issue 或联系项目维护者。

---

**注意**：
- 本项目基于 STM32 HAL 库开发，请确保您的开发环境已正确配置
- 两个示例程序可以独立使用，也可以作为移植参考
- 建议先从STM32F103C8版本开始学习，再根据需要升级到STM32H743VI版本
