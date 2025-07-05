#include "ads8688.h"

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
    HAL_Delay(1); //

    // 发送无操作命令使ADS进入空闲模式
    status += ADS8688_SendCommand(hADS8688, ADS8688_CMD_NO_OP, reg_data);
    HAL_Delay(1);

    // 启用所有输入的自动扫描（数据手册第54页）
    // 如果只需要部分输入，请确保关闭未使用的通道
    reg_data[0] = ADS8688_AUTO_SEQ_ALL_EN; // 启用所有8个通道
    status += ADS8688_WriteRegister(hADS8688, ADS8688_REG_AUTO_SEQ_EN, reg_data);
    HAL_Delay(1);

    // 设置所需功能，如设备ID（多设备时）、报警使能/禁用及输出格式
    reg_data[0] = 0x03; // 此处选择id=0，报警禁用，SDO格式=3（数据手册第56页）
    status += ADS8688_WriteRegister(hADS8688, ADS8688_REG_FEATURE_SELECT, reg_data);
    HAL_Delay(1);

    // 设置所有通道输入范围（数据手册Table 15），初始化时默认设置为最大量程来保护芯片
    uint8_t channel_ranges[ADS8688_NUM_CHANNELS] = {
        ADS8688_RANGE_BIPOLAR_2_5_VREF, // 通道0: -2.5 × VREF to 2.5 × VREF
        ADS8688_RANGE_BIPOLAR_2_5_VREF, // 通道1: -2.5 × VREF to 2.5 × VREF
        ADS8688_RANGE_BIPOLAR_2_5_VREF, // 通道2: -2.5 × VREF to 2.5 × VREF
        ADS8688_RANGE_BIPOLAR_2_5_VREF, // 通道3: -2.5 × VREF to 2.5 × VREF
        ADS8688_RANGE_BIPOLAR_2_5_VREF, // 通道4: -2.5 × VREF to 2.5 × VREF
        ADS8688_RANGE_BIPOLAR_2_5_VREF, // 通道5: -2.5 × VREF to 2.5 × VREF
        ADS8688_RANGE_BIPOLAR_2_5_VREF, // 通道6: -2.5 × VREF to 2.5 × VREF
        ADS8688_RANGE_BIPOLAR_2_5_VREF  // 通道7: -2.5 × VREF to 2.5 × VREF
    };

    if (ADS8688_SetChannelRanges(hADS8688, channel_ranges) != HAL_OK)
    {
        status += 1; // 记录错误
    }

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
 * @note 循环读取所有通道的16位原始ADC数据
 */
HAL_StatusTypeDef ADS8688_ReadAllChannelsRaw(ADS8688_HandleTypeDef *hADS8688, uint16_t *data)
{
    HAL_StatusTypeDef ret = HAL_OK;
    uint8_t raw_data[2];

    for (int i = 0; i < ADS8688_NUM_CHANNELS; i++)
    {
        // 发送无操作命令并读取数据
        ret = ADS8688_SendCommand(hADS8688, ADS8688_CMD_NO_OP, raw_data);
        if (ret != HAL_OK)
        {
            break; // 如果发生错误则退出
        }

        // 将接收到的两个8位数据合并为一个16位数据
        // ADS8688输出16位数据，MSB在前，LSB在后，stm32是小端序
        data[i] = (uint16_t)(raw_data[1] << 8 | raw_data[0]);
    }

    return ret;
}

/**
 * @brief 设置所有通道的输入范围
 * @param hADS8688 ADS8688句柄
 * @param channel_ranges 8个通道的范围配置数组
 * @retval HAL状态
 * @note channel_ranges数组按通道0-7顺序排列，每个元素为对应通道的范围值
 */
HAL_StatusTypeDef ADS8688_SetChannelRanges(ADS8688_HandleTypeDef *hADS8688, uint8_t *channel_ranges)
{
    HAL_StatusTypeDef ret = HAL_OK;
    uint8_t reg_data[2];

    // 设置各通道范围寄存器的基地址
    uint8_t range_reg_base = ADS8688_REG_CH0_RANGE;

    for (int i = 0; i < ADS8688_NUM_CHANNELS; i++)
    {
        reg_data[0] = channel_ranges[i];
        ret = ADS8688_WriteRegister(hADS8688, range_reg_base + i, reg_data);
        if (ret != HAL_OK)
        {
            break; // 如果发生错误则退出
        }
        HAL_Delay(1); // 短暂延时确保寄存器写入完成
    }

    // 进入自动扫描状态，开始自动采集
    ADS8688_SendCommand(hADS8688, ADS8688_CMD_AUTO_RST, reg_data);
    return ret;
}

/**
 * @brief 设置单个通道的输入范围
 * @param hADS8688 ADS8688句柄
 * @param channel 通道号 (0-7)
 * @param range 范围值 (使用ADS8688_RANGE_xxx定义)
 * @retval HAL状态
 */
HAL_StatusTypeDef ADS8688_SetSingleChannelRange(ADS8688_HandleTypeDef *hADS8688, uint8_t channel, uint8_t range)
{
    if (channel >= ADS8688_NUM_CHANNELS)
    {
        return HAL_ERROR; // 通道号超出范围
    }

    uint8_t reg_data[2];
    reg_data[0] = range;

    return ADS8688_WriteRegister(hADS8688, ADS8688_REG_CH0_RANGE + channel, reg_data);
    
    // 进入自动扫描状态，开始自动采集
    ADS8688_SendCommand(hADS8688, ADS8688_CMD_AUTO_RST, reg_data);
}

/**
 * @brief 将ADC原始数据转换为电压值
 * @param raw_value ADC原始数据 (0-65535)
 * @param range 通道范围设置 (ADS8688_RANGE_xxx)
 * @param vref 参考电压值 (V)
 * @retval 转换后的电压值 (V)
 */
float ADS8688_ConvertToVoltage(uint16_t raw_value, uint8_t range, float vref)
{
    float voltage = 0.0;

    switch (range)
    {
    case ADS8688_RANGE_BIPOLAR_2_5_VREF: // ±2.5 × VREF
        voltage = ((float)raw_value - 32768.0) / 32768.0 * (2.5 * vref);
        break;

    case ADS8688_RANGE_BIPOLAR_1_25_VREF: // ±1.25 × VREF
        voltage = ((float)raw_value - 32768.0) / 32768.0 * (1.25 * vref);
        break;

    case ADS8688_RANGE_BIPOLAR_0_625_VREF: // ±0.625 × VREF
        voltage = ((float)raw_value - 32768.0) / 32768.0 * (0.625 * vref);
        break;

    case ADS8688_RANGE_UNIPOLAR_2_5_VREF: // 0 to 2.5 × VREF
        voltage = (float)raw_value / 65535.0 * (2.5 * vref);
        break;

    case ADS8688_RANGE_UNIPOLAR_1_25_VREF: // 0 to 1.25 × VREF
        voltage = (float)raw_value / 65535.0 * (1.25 * vref);
        break;

    default:
        voltage = 0.0;
        break;
    }

    return voltage;
}
