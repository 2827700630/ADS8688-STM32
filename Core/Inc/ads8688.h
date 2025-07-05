#ifndef ADS8688_H
#define ADS8688_H

// 根据你的MCU包含正确的文件
#include "main.h"

// 数据手册链接
// https://www.ti.com/document-viewer/ads8688/datasheet

// ----------------------- ADS8688命令和寄存器 ------------------------ //
// 命令（参考数据手册官方命名）
#define ADS8688_CMD_NO_OP 0x00     // Continued Operation - 继续在之前的模式下运行
#define ADS8688_CMD_STANDBY 0x82   // Standby Mode - 设备进入待机模式
#define ADS8688_CMD_POWERDOWN 0x83 // Power-Down Mode - 设备掉电
#define ADS8688_CMD_RESET 0x85     // Reset Registers to Default - 程序寄存器重置为默认值
#define ADS8688_CMD_AUTO_RST 0xA0  // Auto Channel Enable with Reset - 重置后启用自动模式
#define ADS8688_CMD_MAN_CH0 0xC0   // Manual Channel 0 Select - 通道0手动输入选择
#define ADS8688_CMD_MAN_CH1 0xC4   // Manual Channel 1 Select - 通道1手动输入选择
#define ADS8688_CMD_MAN_CH2 0xC8   // Manual Channel 2 Select - 通道2手动输入选择
#define ADS8688_CMD_MAN_CH3 0xCC   // Manual Channel 3 Select - 通道3手动输入选择
#define ADS8688_CMD_MAN_CH4 0xD0   // Manual Channel 4 Select - 通道4手动输入选择
#define ADS8688_CMD_MAN_CH5 0xD4   // Manual Channel 5 Select - 通道5手动输入选择
#define ADS8688_CMD_MAN_CH6 0xD8   // Manual Channel 6 Select - 通道6手动输入选择
#define ADS8688_CMD_MAN_CH7 0xDC   // Manual Channel 7 Select - 通道7手动输入选择

// 寄存器地址（参考数据手册官方命名）
#define ADS8688_REG_AUTO_SEQ_EN 0x01    // Auto-Scan Sequence Enable Register - 自动扫描序列控制
#define ADS8688_REG_CH_PWRDN 0x02       // Channel Power Down Register - 通道掉电
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
/*
    这里放置你想要使用的其他寄存器...
*/
#define ADS8688_REG_CMD_READBACK 0x3F // Command Read-Back (Read Only) - 命令读回（只读）

// 通道数量
#define ADS8688_NUM_CHANNELS 8 // 想要获取原始数据的通道数量（你还需要调整AUTO_SEQ_EN寄存器值以匹配要读取的通道数量）

// 输入范围定义
// 0x05 -> 输入范围设置为0到2.5倍VREF（对于VREF=4.096V，这意味着0-10.24V范围）
// 0x06 -> 输入范围设置为0到1.25倍VREF（对于VREF=4.096V，这意味着0-5.12V范围）
#define ADS8688_RANGE_0_TO_10_24V 0x05 // 0 to 2.5 x VREF
#define ADS8688_RANGE_0_TO_5_12V 0x06  // 0 to 1.25 x VREF

typedef struct
{
    SPI_HandleTypeDef *hspi; // SPI句柄
    GPIO_TypeDef *cs_port;   // 片选端口
    uint16_t cs_pin;         // 片选引脚
} ADS8688_HandleTypeDef;

// 初始化函数
uint8_t ADS8688_Init(ADS8688_HandleTypeDef *hads, SPI_HandleTypeDef *hspi, GPIO_TypeDef *cs_port, uint16_t cs_pin);

// 寄存器和命令底层函数
HAL_StatusTypeDef ADS8688_ReadRegister(ADS8688_HandleTypeDef *hads, uint8_t reg, uint8_t *data);
HAL_StatusTypeDef ADS8688_WriteRegister(ADS8688_HandleTypeDef *hads, uint8_t reg, uint8_t *data);
HAL_StatusTypeDef ADS8688_SendCommand(ADS8688_HandleTypeDef *hads, uint8_t cmd, uint8_t *data);

// 高级应用函数
HAL_StatusTypeDef ADS8688_ReadAllChannelsRaw(ADS8688_HandleTypeDef *hads, uint16_t *data);

#endif