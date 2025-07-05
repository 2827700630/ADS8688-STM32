#include "ADS8688.h"

/*
 * 初始化
 */
uint8_t ADS8688_Init(ADS8688 *ads, SPI_HandleTypeDef *spiHandle, GPIO_TypeDef *csPinBank, uint16_t csPin)
{
    /* 在结构体中存储接口参数 */
    ads->spiHandle = spiHandle;
    ads->csPinBank = csPinBank;
    ads->csPin = csPin;

    uint8_t ads_data[2] = {0};
    uint8_t state = 0;
    // 重置所有寄存器为默认值
    state += ADS_Cmd_Write(ads, RST, ads_data);
    HAL_Delay(1); // 复位后需要短暂延时
    // 向ADS发送无操作消息以进入空闲模式
    state += ADS_Cmd_Write(ads, CONT, ads_data);
    HAL_Delay(1);
    // 为所有输入启用自动传输（数据手册第54页）或者根据需要启用
    // 如果只想启用部分输入，请确保关闭未使用的输入
    ads_data[0] = 0xff;
    state += ADS_Prog_Write(ads, AUTO_SEQ_EN, ads_data);
    HAL_Delay(1);
    // 设置所需的功能，如设备ID（如果使用多个设备）、报警启用/禁用和输出格式
    ads_data[0] = 0x03; // 这里我选择了id = 0，报警 = 禁用，SDO_格式 = 3（数据手册第56页）
    state += ADS_Prog_Write(ads, FEATURE_SELECT, ads_data);
    HAL_Delay(1);
    // 设置所有通道范围（第57页）
    // 0x05 -> 输入范围设置为0到2.5倍VREF（对于VREF=4.096V，这意味着0-10.24伏范围）
    // 0x06 -> 输入范围设置为0到1.25倍VREF（对于VREF=4.096V，这意味着0-5.12伏范围）
    ads_data[0] = 0x06;
    state += ADS_Prog_Write(ads, CHN_0_RANGE, ads_data);
    HAL_Delay(1);
    ads_data[0] = 0x06;
    state += ADS_Prog_Write(ads, CHN_1_RANGE, ads_data);
    HAL_Delay(1);
    ads_data[0] = 0x05;
    state += ADS_Prog_Write(ads, CHN_2_RANGE, ads_data);
    HAL_Delay(1);
    ads_data[0] = 0x05;
    state += ADS_Prog_Write(ads, CHN_3_RANGE, ads_data);
    HAL_Delay(1);
    ads_data[0] = 0x05;
    state += ADS_Prog_Write(ads, CHN_4_RANGE, ads_data);
    HAL_Delay(1);
    ads_data[0] = 0x05;
    state += ADS_Prog_Write(ads, CHN_5_RANGE, ads_data);
    HAL_Delay(1);
    ads_data[0] = 0x06;
    state += ADS_Prog_Write(ads, CHN_6_RANGE, ads_data);
    HAL_Delay(1);
    ads_data[0] = 0x06;
    state += ADS_Prog_Write(ads, CHN_7_RANGE, ads_data);
    HAL_Delay(1);
    // 通过进入适当的状态启动自动传输
    state += ADS_Cmd_Write(ads, AUTO_RST, ads_data);

    return state;
}

// 读取后，data包含来自指定寄存器的数据
HAL_StatusTypeDef ADS_Prog_Read(ADS8688 *ads, uint8_t addr, uint8_t *data)
{
    HAL_StatusTypeDef ret;
    uint8_t txbuf[2] = {0x00, (addr << 1 & 0xfe)}; // [15:9]->地址, [8]->0, [7:0]->无关紧要（0x00）（stm32使用小端序所以需要反转）
    uint8_t rxbuf[4];

    HAL_GPIO_WritePin(ads->csPinBank, ads->csPin, GPIO_PIN_RESET);
    ret = HAL_SPI_TransmitReceive(ads->spiHandle, txbuf, rxbuf, 2, 10);
    HAL_GPIO_WritePin(ads->csPinBank, ads->csPin, GPIO_PIN_SET);

    data[0] = rxbuf[2];
    data[1] = rxbuf[3];
    return ret;
}

// 写入后，data应该包含写入指定寄存器的数据（字节）（检查等式以进行评估）
HAL_StatusTypeDef ADS_Prog_Write(ADS8688 *ads, uint8_t addr, uint8_t *data)
{
    HAL_StatusTypeDef ret;
    uint8_t txbuf[2] = {data[0], (addr << 1 | 0x01)}; // [15:9]->地址[6:0], [8]->1, [7:0]->数据[7:0]（stm32使用小端序所以需要反转）
    uint8_t rxbuf[4];

    HAL_GPIO_WritePin(ads->csPinBank, ads->csPin, GPIO_PIN_RESET);
    ret = HAL_SPI_TransmitReceive(ads->spiHandle, txbuf, rxbuf, 2, 10);
    HAL_GPIO_WritePin(ads->csPinBank, ads->csPin, GPIO_PIN_SET);

    data[0] = rxbuf[3];
    data[1] = 0x00;
    return ret;
}

HAL_StatusTypeDef ADS_Cmd_Write(ADS8688 *ads, uint8_t cmd, uint8_t *data)
{
    HAL_StatusTypeDef ret;
    uint8_t txbuf[2] = {0x00, cmd}; // [15:8]为命令, [7:0]为0x00
    uint8_t rxbuf[4];               // 恢复为4字节缓冲区

    HAL_GPIO_WritePin(ads->csPinBank, ads->csPin, GPIO_PIN_RESET);
    // 注意：Size参数为2，表示传输2个16位的数据，总共4个字节
    ret = HAL_SPI_TransmitReceive(ads->spiHandle, txbuf, rxbuf, 2, 10);
    HAL_GPIO_WritePin(ads->csPinBank, ads->csPin, GPIO_PIN_SET);

    // 有效数据在接收缓冲区的后两个字节
    data[0] = rxbuf[2];
    data[1] = rxbuf[3];
    return ret;
}

HAL_StatusTypeDef ADS_Read_All_Raw(ADS8688 *ads, uint16_t *data)
{
    HAL_StatusTypeDef ret = HAL_OK;
    uint8_t ads_raw[2];

    for (int i = 0; i < CHNS_NUM_READ; i++)
    {
        // 恢复调用ADS_Cmd_Write来发送NO_OP命令并读取数据
        ret = ADS_Cmd_Write(ads, CONT, ads_raw);
        if (ret != HAL_OK)
        {
            break; // 如果发生错误则退出
        }
        // 将接收到的2个8位数据合并为1个16位数据，并修正字节序
        data[i] = (uint16_t)(ads_raw[1] << 8 | ads_raw[0]);
    }
    return ret;
}
