#ifndef ADS8688_H
#define ADS8688_H

// 根据你的MCU包含正确的文件
#include "main.h"

// 数据手册链接
// https://www.ti.com/lit/ds/symlink/ads8668.pdf?ts=1620233637463&ref_url=https%253A%252F%252Fwww.ti.com%252Fproduct%252FADS8668

// ----------------------- ADS命令和寄存器 ------------------------ //
// 命令
#define CONT 0x00     // 继续在之前的模式下运行
#define STBY 0x82     // 设备进入待机模式
#define PWDN 0x83     // 设备掉电
#define RST 0x85      // 程序寄存器重置为默认值
#define AUTO_RST 0xa0 // 重置后启用自动模式
#define MAN_0 0xc0    // 通道0手动输入选择
#define MAN_1 0xc4    // 通道1手动输入选择
#define MAN_2 0xc8    // 通道2手动输入选择
#define MAN_3 0xcc    // 通道3手动输入选择
#define MAN_4 0xd0    // 通道4手动输入选择
#define MAN_5 0xd4    // 通道5手动输入选择
#define MAN_6 0xd8    // 通道6手动输入选择
#define MAN_7 0xdc    // 通道7手动输入选择

// 寄存器地址
#define AUTO_SEQ_EN 0x01     // 自动扫描序列控制
#define CHN_PWRDN 0x02       // 通道掉电
#define FEATURE_SELECT 0x03  // 功能选择
#define CHN_0_RANGE 0x05     // 通道0输入范围
#define CHN_1_RANGE 0x06     // 通道1输入范围
#define CHN_2_RANGE 0x07     // 通道2输入范围
#define CHN_3_RANGE 0x08     // 通道3输入范围
#define CHN_4_RANGE 0x09     // 通道4输入范围
#define CHN_5_RANGE 0x0a     // 通道5输入范围
#define CHN_6_RANGE 0x0b     // 通道6输入范围
#define CHN_7_RANGE 0x0c     // 通道7输入范围
#define ALRM_OVRVW_TRIP 0x10 // 报警概览跳闸标志
#define ALRM_0_3_TRIP 0x11   // 报警通道0-3跳闸标志
#define ALRM_0_3_ACT 0x12    // 报警通道0-3活动标志
#define ALRM_4_7_TRIP 0x13   // 报警通道4-7跳闸标志
#define ALRM_4_7_ACT 0x14    // 报警通道4-7活动标志
/*
    这里放置你想要使用的其他寄存器...
*/
#define CMD_RD_BCK 0x3f // 命令读回（只读）

// --------------------------------------------------------------------------- //

#define CHNS_NUM_READ 8 // 想要获取原始数据的通道数量（你还需要调整AUTO_SEQ_EN寄存器值以匹配要读取的通道数量）

typedef struct
{

    /* SPI */
    SPI_HandleTypeDef *spiHandle;
    GPIO_TypeDef *csPinBank;
    uint16_t csPin;

} ADS8688;

/*
 *
 * 初始化
 *
 */
uint8_t ADS8688_Init(ADS8688 *ads, SPI_HandleTypeDef *spiHandle, GPIO_TypeDef *csPinBank, uint16_t csPin);

/*
 *
 * 寄存器和命令底层函数
 *
 */
HAL_StatusTypeDef ADS_Prog_Read(ADS8688 *ads, uint8_t addr, uint8_t *data);
HAL_StatusTypeDef ADS_Prog_Write(ADS8688 *ads, uint8_t addr, uint8_t *data);
HAL_StatusTypeDef ADS_Cmd_Write(ADS8688 *ads, uint8_t cmd, uint8_t *data);

/*
 *
 * 高级函数
 *
 */

HAL_StatusTypeDef ADS_Read_All_Raw(ADS8688 *ads, uint16_t *data);

#endif