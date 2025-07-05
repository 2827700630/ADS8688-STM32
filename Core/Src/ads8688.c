#include "ADS8688.h"

/**
 * @brief ADS8688 初始化函数
 * @param hADS8688 ADS8688句柄
 * @param hspi SPI句柄
 * @param cs_port 片选端口
 * @param cs_pin 片选引脚
 * @retval 错误状态累计值
 */
uint8_t ADS8688_Init(ADS8688_HandleTypeDef *hADS8688, SPI_HandleTypeDef *hspi, GPIO_TypeDef *cs_port, uint16_t cs_pin)
{
    // 将接口参数存储到结构体中
    hADS8688->hspi = hspi;
    hADS8688->cs_port = cs_port;
    hADS8688->cs_pin = cs_pin;

    uint8_t reg_data[2] = {0};
    uint8_t status = 0;
    
    // 重置所有寄存器为默认值
    status += ADS8688_SendCommand(hADS8688, ADS8688_CMD_RESET, reg_data);
    HAL_Delay(100);  // 复位后需要较长延时
    
    // 发送无操作命令使ADS进入空闲模式
    status += ADS8688_SendCommand(hADS8688, ADS8688_CMD_NO_OP, reg_data);
    HAL_Delay(10);
    
    // 启用所有输入的自动扫描（数据手册第54页）
    // 如果只需要部分输入，请确保关闭未使用的通道
    reg_data[0] = 0xFF;  // 启用所有8个通道
    status += ADS8688_WriteRegister(hADS8688, ADS8688_REG_AUTO_SEQ_EN, reg_data);
    HAL_Delay(10);
    
    // 设置所需功能，如设备ID（多设备时）、报警使能/禁用及输出格式
    reg_data[0] = 0x03;  // 此处选择id=0，报警禁用，SDO格式=3（数据手册第56页）
    status += ADS8688_WriteRegister(hADS8688, ADS8688_REG_FEATURE_SELECT, reg_data);
    HAL_Delay(10);
    
    // 设置所有通道输入范围（数据手册第57页）
    // 0x05 -> 输入范围为0到2.5倍VREF（VREF=4.096V时为0-10.24V）
    // 0x06 -> 输入范围为0到1.25倍VREF（VREF=4.096V时为0-5.12V）
    reg_data[0] = ADS8688_RANGE_0_TO_5_12V;
    status += ADS8688_WriteRegister(hADS8688, ADS8688_REG_CH0_RANGE, reg_data);
    HAL_Delay(10);
    
    reg_data[0] = ADS8688_RANGE_0_TO_5_12V;
    status += ADS8688_WriteRegister(hADS8688, ADS8688_REG_CH1_RANGE, reg_data);
    HAL_Delay(10);
    
    reg_data[0] = ADS8688_RANGE_0_TO_10_24V;
    status += ADS8688_WriteRegister(hADS8688, ADS8688_REG_CH2_RANGE, reg_data);
    HAL_Delay(10);
    
    reg_data[0] = ADS8688_RANGE_0_TO_10_24V;
    status += ADS8688_WriteRegister(hADS8688, ADS8688_REG_CH3_RANGE, reg_data);
    HAL_Delay(10);
    
    reg_data[0] = ADS8688_RANGE_0_TO_10_24V;
    status += ADS8688_WriteRegister(hADS8688, ADS8688_REG_CH4_RANGE, reg_data);
    HAL_Delay(10);
    
    reg_data[0] = ADS8688_RANGE_0_TO_10_24V;
    status += ADS8688_WriteRegister(hADS8688, ADS8688_REG_CH5_RANGE, reg_data);
    HAL_Delay(10);
    
    reg_data[0] = ADS8688_RANGE_0_TO_5_12V;
    status += ADS8688_WriteRegister(hADS8688, ADS8688_REG_CH6_RANGE, reg_data);
    HAL_Delay(10);
    
    reg_data[0] = ADS8688_RANGE_0_TO_5_12V;
    status += ADS8688_WriteRegister(hADS8688, ADS8688_REG_CH7_RANGE, reg_data);
    HAL_Delay(10);
    
    // 进入自动扫描状态，开始自动采集
    status += ADS8688_SendCommand(hADS8688, ADS8688_CMD_AUTO_RST, reg_data);

    return status;
}

/**
 * @brief 读取寄存器函数
 * @param hADS8688 ADS8688句柄
 * @param reg 寄存器地址
 * @param data 存储读取数据的缓冲区
 * @retval HAL状态
 * @note 读取后，data包含指定寄存器的数据
 */
HAL_StatusTypeDef ADS8688_ReadRegister(ADS8688_HandleTypeDef *hADS8688, uint8_t reg, uint8_t *data)
{
    HAL_StatusTypeDef ret;
    // [15:9]->地址, [8]->0, [7:0]->无关（0x00）（STM32为小端序需反转）
    uint8_t txbuf[2] = {0x00, (reg << 1 & 0xFE)};
    uint8_t rxbuf[4];

    HAL_GPIO_WritePin(hADS8688->cs_port, hADS8688->cs_pin, GPIO_PIN_RESET);
    ret = HAL_SPI_TransmitReceive(hADS8688->hspi, txbuf, rxbuf, 2, 10);
    HAL_GPIO_WritePin(hADS8688->cs_port, hADS8688->cs_pin, GPIO_PIN_SET);

    data[0] = rxbuf[2];
    data[1] = rxbuf[3];
    return ret;
}

/**
 * @brief 写寄存器函数
 * @param hADS8688 ADS8688句柄
 * @param reg 寄存器地址
 * @param data 要写入的数据
 * @retval HAL状态
 * @note 写入后，data应包含写入指定寄存器的数据（字节）（可用于校验）
 */
HAL_StatusTypeDef ADS8688_WriteRegister(ADS8688_HandleTypeDef *hADS8688, uint8_t reg, uint8_t *data)
{
    HAL_StatusTypeDef ret;
    // [15:9]->地址[6:0], [8]->1, [7:0]->数据[7:0]（STM32为小端序需反转）
    uint8_t txbuf[2] = {data[0], (reg << 1 | 0x01)};
    uint8_t rxbuf[4];

    HAL_GPIO_WritePin(hADS8688->cs_port, hADS8688->cs_pin, GPIO_PIN_RESET);
    ret = HAL_SPI_TransmitReceive(hADS8688->hspi, txbuf, rxbuf, 2, 10);
    HAL_GPIO_WritePin(hADS8688->cs_port, hADS8688->cs_pin, GPIO_PIN_SET);

    data[0] = rxbuf[3];
    data[1] = 0x00;
    return ret;
}

/**
 * @brief 发送命令函数
 * @param hADS8688 ADS8688句柄
 * @param cmd 命令字节
 * @param data 存储返回数据的缓冲区
 * @retval HAL状态
 */
HAL_StatusTypeDef ADS8688_SendCommand(ADS8688_HandleTypeDef *hADS8688, uint8_t cmd, uint8_t *data)
{
    HAL_StatusTypeDef ret;
    // [15:8]为命令, [7:0]为0x00
    uint8_t txbuf[2] = {0x00, cmd};
    uint8_t rxbuf[4];

    HAL_GPIO_WritePin(hADS8688->cs_port, hADS8688->cs_pin, GPIO_PIN_RESET);
    ret = HAL_SPI_TransmitReceive(hADS8688->hspi, txbuf, rxbuf, 2, 10);
    HAL_GPIO_WritePin(hADS8688->cs_port, hADS8688->cs_pin, GPIO_PIN_SET);

    // 有效数据在接收缓冲区的后两个字节
    data[0] = rxbuf[2];
    data[1] = rxbuf[3];
    return ret;
}

/**
 * @brief 读取所有通道原始数据函数
 * @param hADS8688 ADS8688句柄
 * @param data 存储8个通道原始数据的数组
 * @retval HAL状态
 * @note 循环读取所有通道的16位原始ADC数据，并进行右移4位处理
 */
HAL_StatusTypeDef ADS8688_ReadAllChannelsRaw(ADS8688_HandleTypeDef *hADS8688, uint16_t *data)
{
    HAL_StatusTypeDef ret;
    uint8_t raw_data[2];
    
    for (int i = 0; i < ADS8688_NUM_CHANNELS; i++)
    {
        // 发送无操作命令并读取数据
        ret = ADS8688_SendCommand(hADS8688, ADS8688_CMD_NO_OP, raw_data);
        if (ret != HAL_OK)
        {
            break;  // 如果发生错误则退出
        }
        
        // 将接收到的两个8位数据合并为一个16位数据，并右移4位
        // ADS8688输出16位数据，但有效数据在高12位
        data[i] = (uint16_t)(raw_data[1] << 8 | raw_data[0]);
    }
    
    return ret;
}
