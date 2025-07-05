#ifndef ADS8688_H
#define ADS8688_H

// 根据你的MCU包含正确的文件
#include "main.h"

// 数据手册链接
// https://www.ti.com/lit/ds/symlink/ads8668.pdf?ts=1620233637463&ref_url=https%253A%252F%252Fwww.ti.com%252Fproduct%252FADS8668

// ----------------------- ADS命令和寄存器 ------------------------ //
// 命令（参考数据手册官方命名）
#define ADS8688_CMD_CONT        0x00  // Continue Operation (NO_OP)
#define ADS8688_CMD_STANDBY     0x82  // Standby Mode
#define ADS8688_CMD_POWERDOWN   0x83  // Power-Down Mode
#define ADS8688_CMD_RESET       0x85  // Reset Registers to Default
#define ADS8688_CMD_AUTO_RST    0xA0  // Auto Channel Enable with Reset
#define ADS8688_CMD_MAN_CH0     0xC0  // Manual Channel 0 Select
#define ADS8688_CMD_MAN_CH1     0xC4  // Manual Channel 1 Select
#define ADS8688_CMD_MAN_CH2     0xC8  // Manual Channel 2 Select
#define ADS8688_CMD_MAN_CH3     0xCC  // Manual Channel 3 Select
#define ADS8688_CMD_MAN_CH4     0xD0  // Manual Channel 4 Select
#define ADS8688_CMD_MAN_CH5     0xD4  // Manual Channel 5 Select
#define ADS8688_CMD_MAN_CH6     0xD8  // Manual Channel 6 Select
#define ADS8688_CMD_MAN_CH7     0xDC  // Manual Channel 7 Select

// 寄存器地址（参考数据手册官方命名）
#define ADS8688_REG_AUTO_SEQ_EN      0x01 // Auto-Scan Sequence Enable Register
#define ADS8688_REG_CH_PWRDN         0x02 // Channel Power Down Register
#define ADS8688_REG_FEATURE_SELECT   0x03 // Device Feature Selection Register
#define ADS8688_REG_CH0_RANGE        0x05 // Channel 0 Range Select
#define ADS8688_REG_CH1_RANGE        0x06 // Channel 1 Range Select
#define ADS8688_REG_CH2_RANGE        0x07 // Channel 2 Range Select
#define ADS8688_REG_CH3_RANGE        0x08 // Channel 3 Range Select
#define ADS8688_REG_CH4_RANGE        0x09 // Channel 4 Range Select
#define ADS8688_REG_CH5_RANGE        0x0A // Channel 5 Range Select
#define ADS8688_REG_CH6_RANGE        0x0B // Channel 6 Range Select
#define ADS8688_REG_CH7_RANGE        0x0C // Channel 7 Range Select
#define ADS8688_REG_ALARM_OVERVIEW   0x10 // Alarm Overview Trip Flag
#define ADS8688_REG_ALARM_0_3_TRIP   0x11 // Alarm Channel 0-3 Trip Flag
#define ADS8688_REG_ALARM_0_3_ACT    0x12 // Alarm Channel 0-3 Active Flag
#define ADS8688_REG_ALARM_4_7_TRIP   0x13 // Alarm Channel 4-7 Trip Flag
#define ADS8688_REG_ALARM_4_7_ACT    0x14 // Alarm Channel 4-7 Active Flag
#define ADS8688_REG_CMD_READBACK     0x3F // Command Read-Back (Read Only)

// --------------------------------------------------------------------------- //

// 通道数量
#define ADS8688_NUM_CHANNELS 8 // 采集通道数

typedef struct
{
    SPI_HandleTypeDef *hspi;   // SPI句柄
    GPIO_TypeDef *cs_port;     // 片选端口
    uint16_t cs_pin;           // 片选引脚
} ADS8688_HandleTypeDef;

// 初始化
uint8_t ADS8688_Init(ADS8688_HandleTypeDef *hads, SPI_HandleTypeDef *hspi, GPIO_TypeDef *cs_port, uint16_t cs_pin);

// 底层寄存器和命令操作
HAL_StatusTypeDef ADS8688_ReadRegister(ADS8688_HandleTypeDef *hads, uint8_t reg, uint8_t *data);
HAL_StatusTypeDef ADS8688_WriteRegister(ADS8688_HandleTypeDef *hads, uint8_t reg, uint8_t *data);
HAL_StatusTypeDef ADS8688_SendCommand(ADS8688_HandleTypeDef *hads, uint8_t cmd, uint8_t *data);

// 采集全部通道原始数据
HAL_StatusTypeDef ADS8688_ReadAllChannelsRaw(ADS8688_HandleTypeDef *hads, uint16_t *data);

#endif