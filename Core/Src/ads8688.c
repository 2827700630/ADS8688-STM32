#include "ADS8688.h"

/*
 * 初始化
 */
uint8_t ADS8688_Init(ADS8688_HandleTypeDef *hads, SPI_HandleTypeDef *hspi, GPIO_TypeDef *cs_port, uint16_t cs_pin)
{
    // 存储接口参数
    hads->hspi = hspi;
    hads->cs_port = cs_port;
    hads->cs_pin = cs_pin;

    uint8_t reg_data[2] = {0};
    uint8_t status = 0;
    // 复位所有寄存器为默认值
    status += ADS8688_SendCommand(hads, ADS8688_CMD_RESET, reg_data);
    HAL_Delay(1);
    // 进入空闲模式
    status += ADS8688_SendCommand(hads, ADS8688_CMD_CONT, reg_data);
    HAL_Delay(1);
    // 启用所有输入的自动扫描
    reg_data[0] = 0xFF;
    status += ADS8688_WriteRegister(hads, ADS8688_REG_AUTO_SEQ_EN, reg_data);
    HAL_Delay(1);
    // 设置功能寄存器（如设备ID、报警、输出格式）
    reg_data[0] = 0x03;
    status += ADS8688_WriteRegister(hads, ADS8688_REG_FEATURE_SELECT, reg_data);
    HAL_Delay(1);
    // 设置各通道输入范围
    reg_data[0] = 0x06;
    status += ADS8688_WriteRegister(hads, ADS8688_REG_CH0_RANGE, reg_data);
    HAL_Delay(1);
    reg_data[0] = 0x06;
    status += ADS8688_WriteRegister(hads, ADS8688_REG_CH1_RANGE, reg_data);
    HAL_Delay(1);
    reg_data[0] = 0x05;
    status += ADS8688_WriteRegister(hads, ADS8688_REG_CH2_RANGE, reg_data);
    HAL_Delay(1);
    reg_data[0] = 0x05;
    status += ADS8688_WriteRegister(hads, ADS8688_REG_CH3_RANGE, reg_data);
    HAL_Delay(1);
    reg_data[0] = 0x05;
    status += ADS8688_WriteRegister(hads, ADS8688_REG_CH4_RANGE, reg_data);
    HAL_Delay(1);
    reg_data[0] = 0x05;
    status += ADS8688_WriteRegister(hads, ADS8688_REG_CH5_RANGE, reg_data);
    HAL_Delay(1);
    reg_data[0] = 0x06;
    status += ADS8688_WriteRegister(hads, ADS8688_REG_CH6_RANGE, reg_data);
    HAL_Delay(1);
    reg_data[0] = 0x06;
    status += ADS8688_WriteRegister(hads, ADS8688_REG_CH7_RANGE, reg_data);
    HAL_Delay(1);
    // 启动自动扫描
    status += ADS8688_SendCommand(hads, ADS8688_CMD_AUTO_RST, reg_data);

    return status;
}

// 读取寄存器
HAL_StatusTypeDef ADS8688_ReadRegister(ADS8688_HandleTypeDef *hads, uint8_t reg, uint8_t *data)
{
    HAL_StatusTypeDef ret;
    uint8_t txbuf[2] = {0x00, (reg << 1 & 0xFE)};
    uint8_t rxbuf[4];
    HAL_GPIO_WritePin(hads->cs_port, hads->cs_pin, GPIO_PIN_RESET);
    ret = HAL_SPI_TransmitReceive(hads->hspi, txbuf, rxbuf, 2, 10);
    HAL_GPIO_WritePin(hads->cs_port, hads->cs_pin, GPIO_PIN_SET);
    data[0] = rxbuf[2];
    data[1] = rxbuf[3];
    return ret;
}

// 写寄存器
HAL_StatusTypeDef ADS8688_WriteRegister(ADS8688_HandleTypeDef *hads, uint8_t reg, uint8_t *data)
{
    HAL_StatusTypeDef ret;
    uint8_t txbuf[2] = {data[0], (reg << 1 | 0x01)};
    uint8_t rxbuf[4];
    HAL_GPIO_WritePin(hads->cs_port, hads->cs_pin, GPIO_PIN_RESET);
    ret = HAL_SPI_TransmitReceive(hads->hspi, txbuf, rxbuf, 2, 10);
    HAL_GPIO_WritePin(hads->cs_port, hads->cs_pin, GPIO_PIN_SET);
    data[0] = rxbuf[3];
    data[1] = 0x00;
    return ret;
}

// 发送命令
HAL_StatusTypeDef ADS8688_SendCommand(ADS8688_HandleTypeDef *hads, uint8_t cmd, uint8_t *data)
{
    HAL_StatusTypeDef ret;
    uint8_t txbuf[2] = {0x00, cmd};
    uint8_t rxbuf[4];
    HAL_GPIO_WritePin(hads->cs_port, hads->cs_pin, GPIO_PIN_RESET);
    ret = HAL_SPI_TransmitReceive(hads->hspi, txbuf, rxbuf, 2, 10);
    HAL_GPIO_WritePin(hads->cs_port, hads->cs_pin, GPIO_PIN_SET);
    data[0] = rxbuf[2];
    data[1] = rxbuf[3];
    return ret;
}

// 读取所有通道原始数据
HAL_StatusTypeDef ADS8688_ReadAllChannelsRaw(ADS8688_HandleTypeDef *hads, uint16_t *data)
{
    HAL_StatusTypeDef ret = HAL_OK;
    uint8_t raw[2];
    for (int i = 0; i < ADS8688_NUM_CHANNELS; i++)
    {
        ret = ADS8688_SendCommand(hads, ADS8688_CMD_CONT, raw);
        if (ret != HAL_OK)
        {
            break;
        }
        data[i] = (uint16_t)(raw[1] << 8 | raw[0]);
    }
    return ret;
}
