#ifndef ADS8688_H
#define ADS8688_H

#include "main.h"

// 数据手册链接
// https://www.ti.com.cn/product/cn/ADS8688

// ----------------------- ADS8688命令和寄存器 ------------------------ //
// 命令（参考数据手册Table 6. Command Register Map命名）
#define ADS8688_CMD_NO_OP 0x00         // Continued Operation - 继续在之前的模式下运行
#define ADS8688_CMD_STANDBY 0x82       // Standby Mode - 设备进入待机模式
#define ADS8688_CMD_POWERDOWN 0x83     // Power-Down Mode - 设备掉电
#define ADS8688_CMD_RESET 0x85         // Reset Registers to Default - 程序寄存器重置为默认值
#define ADS8688_CMD_AUTO_RST 0xA0      // Auto Channel Enable with Reset - 重置后启用自动模式
#define ADS8688_CMD_MAN_CH0 0xC0       // Manual Channel 0 Select - 通道0手动输入选择
#define ADS8688_CMD_MAN_CH1 0xC4       // Manual Channel 1 Select - 通道1手动输入选择
#define ADS8688_CMD_MAN_CH2 0xC8       // Manual Channel 2 Select - 通道2手动输入选择
#define ADS8688_CMD_MAN_CH3 0xCC       // Manual Channel 3 Select - 通道3手动输入选择
#define ADS8688_CMD_MAN_CH4 0xD0       // Manual Channel 4 Select - 通道4手动输入选择
#define ADS8688_CMD_MAN_CH5 0xD4       // Manual Channel 5 Select - 通道5手动输入选择
#define ADS8688_CMD_MAN_CH6 0xD8       // Manual Channel 6 Select - 通道6手动输入选择
#define ADS8688_CMD_MAN_CH7 0xDC       // Manual Channel 7 Select - 通道7手动输入选择
#define ADS8688_CMD_AUX_SELECTION 0xE0 // Auxiliary Channel Selection - 辅助通道选择

// 寄存器地址（参考数据手册Table 9. Program Register Map命名）
#define ADS8688_REG_AUTO_SEQ_EN 0x01    // Auto-Scan Sequence Enable Register - 自动扫描序列控制
#define ADS8688_REG_CH_POWERDOWN 0x02   // Channel Power Down Register - 通道掉电
#define ADS8688_REG_FEATURE_SELECT 0x03 // Device Feature Selection Register - 功能选择
#define ADS8688_REG_CH0_RANGE 0x05      // Channel 0 Range Select - 通道0输入范围
#define ADS8688_REG_CH1_RANGE 0x06      // Channel 1 Range Select - 通道1输入范围
#define ADS8688_REG_CH2_RANGE 0x07      // Channel 2 Range Select - 通道2输入范围
#define ADS8688_REG_CH3_RANGE 0x08      // Channel 3 Range Select - 通道3输入范围
#define ADS8688_REG_CH4_RANGE 0x09      // Channel 4 Range Select - 通道4输入范围
#define ADS8688_REG_CH5_RANGE 0x0A      // Channel 5 Range Select - 通道5输入范围
#define ADS8688_REG_CH6_RANGE 0x0B      // Channel 6 Range Select - 通道6输入范围
#define ADS8688_REG_CH7_RANGE 0x0C      // Channel 7 Range Select - 通道7输入范围
#define ADS8688_REG_ALARM_OVERVIEW 0x10 // Alarm Overview Trip Flag - 报警概览跳闸标志
#define ADS8688_REG_ALARM_0_3_TRIP 0x11 // Alarm Channel 0-3 Trip Flag - 报警通道0-3跳闸标志
#define ADS8688_REG_ALARM_0_3_ACT 0x12  // Alarm Channel 0-3 Active Flag - 报警通道0-3活动标志
#define ADS8688_REG_ALARM_4_7_TRIP 0x13 // Alarm Channel 4-7 Trip Flag - 报警通道4-7跳闸标志
#define ADS8688_REG_ALARM_4_7_ACT 0x14  // Alarm Channel 4-7 Active Flag - 报警通道4-7活动标志

// 通道0阈值寄存器
#define ADS8688_REG_CH0_HYSTERESIS 0x15  // Ch 0 Hysteresis - 通道0迟滞
#define ADS8688_REG_CH0_HIGH_TH_MSB 0x16 // Ch 0 High Threshold MSB - 通道0高阈值高字节
#define ADS8688_REG_CH0_HIGH_TH_LSB 0x17 // Ch 0 High Threshold LSB - 通道0高阈值低字节
#define ADS8688_REG_CH0_LOW_TH_MSB 0x18  // Ch 0 Low Threshold MSB - 通道0低阈值高字节
#define ADS8688_REG_CH0_LOW_TH_LSB 0x19  // Ch 0 Low Threshold LSB - 通道0低阈值低字节

// 通道1阈值寄存器
#define ADS8688_REG_CH1_HYSTERESIS 0x1A  // Ch 1 Hysteresis - 通道1迟滞
#define ADS8688_REG_CH1_HIGH_TH_MSB 0x1B // Ch 1 High Threshold MSB - 通道1高阈值高字节
#define ADS8688_REG_CH1_HIGH_TH_LSB 0x1C // Ch 1 High Threshold LSB - 通道1高阈值低字节
#define ADS8688_REG_CH1_LOW_TH_MSB 0x1D  // Ch 1 Low Threshold MSB - 通道1低阈值高字节
#define ADS8688_REG_CH1_LOW_TH_LSB 0x1E  // Ch 1 Low Threshold LSB - 通道1低阈值低字节

// 通道2阈值寄存器
#define ADS8688_REG_CH2_HYSTERESIS 0x1F  // Ch 2 Hysteresis - 通道2迟滞
#define ADS8688_REG_CH2_HIGH_TH_MSB 0x20 // Ch 2 High Threshold MSB - 通道2高阈值高字节
#define ADS8688_REG_CH2_HIGH_TH_LSB 0x21 // Ch 2 High Threshold LSB - 通道2高阈值低字节
#define ADS8688_REG_CH2_LOW_TH_MSB 0x22  // Ch 2 Low Threshold MSB - 通道2低阈值高字节
#define ADS8688_REG_CH2_LOW_TH_LSB 0x23  // Ch 2 Low Threshold LSB - 通道2低阈值低字节

// 通道3阈值寄存器
#define ADS8688_REG_CH3_HYSTERESIS 0x24  // Ch 3 Hysteresis - 通道3迟滞
#define ADS8688_REG_CH3_HIGH_TH_MSB 0x25 // Ch 3 High Threshold MSB - 通道3高阈值高字节
#define ADS8688_REG_CH3_HIGH_TH_LSB 0x26 // Ch 3 High Threshold LSB - 通道3高阈值低字节
#define ADS8688_REG_CH3_LOW_TH_MSB 0x27  // Ch 3 Low Threshold MSB - 通道3低阈值高字节
#define ADS8688_REG_CH3_LOW_TH_LSB 0x28  // Ch 3 Low Threshold LSB - 通道3低阈值低字节

// 通道4阈值寄存器
#define ADS8688_REG_CH4_HYSTERESIS 0x29  // Ch 4 Hysteresis - 通道4迟滞
#define ADS8688_REG_CH4_HIGH_TH_MSB 0x2A // Ch 4 High Threshold MSB - 通道4高阈值高字节
#define ADS8688_REG_CH4_HIGH_TH_LSB 0x2B // Ch 4 High Threshold LSB - 通道4高阈值低字节
#define ADS8688_REG_CH4_LOW_TH_MSB 0x2C  // Ch 4 Low Threshold MSB - 通道4低阈值高字节
#define ADS8688_REG_CH4_LOW_TH_LSB 0x2D  // Ch 4 Low Threshold LSB - 通道4低阈值低字节

// 通道5阈值寄存器
#define ADS8688_REG_CH5_HYSTERESIS 0x2E  // Ch 5 Hysteresis - 通道5迟滞
#define ADS8688_REG_CH5_HIGH_TH_MSB 0x2F // Ch 5 High Threshold MSB - 通道5高阈值高字节
#define ADS8688_REG_CH5_HIGH_TH_LSB 0x30 // Ch 5 High Threshold LSB - 通道5高阈值低字节
#define ADS8688_REG_CH5_LOW_TH_MSB 0x31  // Ch 5 Low Threshold MSB - 通道5低阈值高字节
#define ADS8688_REG_CH5_LOW_TH_LSB 0x32  // Ch 5 Low Threshold LSB - 通道5低阈值低字节

// 通道6阈值寄存器
#define ADS8688_REG_CH6_HYSTERESIS 0x33  // Ch 6 Hysteresis - 通道6迟滞
#define ADS8688_REG_CH6_HIGH_TH_MSB 0x34 // Ch 6 High Threshold MSB - 通道6高阈值高字节
#define ADS8688_REG_CH6_HIGH_TH_LSB 0x35 // Ch 6 High Threshold LSB - 通道6高阈值低字节
#define ADS8688_REG_CH6_LOW_TH_MSB 0x36  // Ch 6 Low Threshold MSB - 通道6低阈值高字节
#define ADS8688_REG_CH6_LOW_TH_LSB 0x37  // Ch 6 Low Threshold LSB - 通道6低阈值低字节

// 通道7阈值寄存器
#define ADS8688_REG_CH7_HYSTERESIS 0x38  // Ch 7 Hysteresis - 通道7迟滞
#define ADS8688_REG_CH7_HIGH_TH_MSB 0x39 // Ch 7 High Threshold MSB - 通道7高阈值高字节
#define ADS8688_REG_CH7_HIGH_TH_LSB 0x3A // Ch 7 High Threshold LSB - 通道7高阈值低字节
#define ADS8688_REG_CH7_LOW_TH_MSB 0x3B  // Ch 7 Low Threshold MSB - 通道7低阈值高字节
#define ADS8688_REG_CH7_LOW_TH_LSB 0x3C  // Ch 7 Low Threshold LSB - 通道7低阈值低字节

// 命令读回寄存器
#define ADS8688_REG_CMD_READBACK 0x3F // Command Read-Back (Read Only) - 命令读回（只读）

// 通道数量
#define ADS8688_NUM_CHANNELS 8 // 想要获取原始数据的通道数量（你还需要调整AUTO_SEQ_EN寄存器值以匹配要读取的通道数量）

// 输入范围定义（根据数据手册Table 15. Channel n Input Range Registers Field Descriptions）
// 实际电压范围 = 倍数 × VREF
#define ADS8688_RANGE_BIPOLAR_2_5_VREF 0x00   // ±2.5 × VREF (双极性范围)(±10.24V for VREF=4.096V)
#define ADS8688_RANGE_BIPOLAR_1_25_VREF 0x01  // ±1.25 × VREF (双极性范围)(±5.12V for VREF=4.096V)
#define ADS8688_RANGE_BIPOLAR_0_625_VREF 0x02 // ±0.625 × VREF (双极性范围)(±2.56V for VREF=4.096V)
#define ADS8688_RANGE_UNIPOLAR_2_5_VREF 0x05  // 0 to 2.5 × VREF (单极性范围)(0-10.24V for VREF=4.096V)
#define ADS8688_RANGE_UNIPOLAR_1_25_VREF 0x06 // 0 to 1.25 × VREF (单极性范围)(0-5.12V for VREF=4.096V)

// Feature Select Register位定义
#define ADS8688_FEATURE_DEV_ID_MASK 0x30     // Device ID位掩码 [5:4]
#define ADS8688_FEATURE_DEV_ID_SHIFT 4       // Device ID位偏移
#define ADS8688_FEATURE_ALARM_EN 0x08        // 报警使能位 [3]
#define ADS8688_FEATURE_SDO_FORMAT_MASK 0x07 // SDO格式位掩码 [2:0]

// 自动扫描序列使能寄存器位定义
#define ADS8688_AUTO_SEQ_CH0_EN 0x01 // 通道0使能
#define ADS8688_AUTO_SEQ_CH1_EN 0x02 // 通道1使能
#define ADS8688_AUTO_SEQ_CH2_EN 0x04 // 通道2使能
#define ADS8688_AUTO_SEQ_CH3_EN 0x08 // 通道3使能
#define ADS8688_AUTO_SEQ_CH4_EN 0x10 // 通道4使能
#define ADS8688_AUTO_SEQ_CH5_EN 0x20 // 通道5使能
#define ADS8688_AUTO_SEQ_CH6_EN 0x40 // 通道6使能
#define ADS8688_AUTO_SEQ_CH7_EN 0x80 // 通道7使能
#define ADS8688_AUTO_SEQ_ALL_EN 0xFF // 所有通道使能

typedef struct
{
    SPI_HandleTypeDef *hspi; // SPI句柄（SPI通道）
    GPIO_TypeDef *cs_port;   // 片选端口
    uint16_t cs_pin;         // 片选引脚
} ADS8688_HandleTypeDef;

// 初始化函数
uint8_t ADS8688_Init(ADS8688_HandleTypeDef *hads, SPI_HandleTypeDef *hspi, GPIO_TypeDef *cs_port, uint16_t cs_pin);

// 寄存器和命令底层函数
HAL_StatusTypeDef ADS8688_ReadRegister(ADS8688_HandleTypeDef *hads, uint8_t reg, uint8_t *data);
HAL_StatusTypeDef ADS8688_WriteRegister(ADS8688_HandleTypeDef *hads, uint8_t reg, uint8_t *data);
HAL_StatusTypeDef ADS8688_SendCommand(ADS8688_HandleTypeDef *hads, uint8_t cmd, uint8_t *data);

// 配置函数
HAL_StatusTypeDef ADS8688_SetChannelRanges(ADS8688_HandleTypeDef *hads, uint8_t *channel_ranges);
HAL_StatusTypeDef ADS8688_SetSingleChannelRange(ADS8688_HandleTypeDef *hads, uint8_t channel, uint8_t range);

// 高级应用函数
HAL_StatusTypeDef ADS8688_ReadAllChannelsRaw(ADS8688_HandleTypeDef *hads, uint16_t *data);
float ADS8688_ConvertToVoltage(uint16_t raw_value, uint8_t range, float vref);

#endif