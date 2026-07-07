#include "MySPI.h"

/**
 * 函数：SPI写CSN引脚电平
 * 参数：BitValue 协议层传入的当前需要写入CSN的电平，范围：0~1
 * 返回值：无
 * 注意事项：此函数需要用户实现内容，
 * 当BitValue为0时，需要置CSN为低电平，
 * 当BitValue为1时，需要置CSN为高电平
 */
static void MySPI_W_CSN(uint8_t BitValue)
{
	// 根据BitValue，设置CSN引脚电平
	GPIO_WriteBit(CSN_Port, CSN_Pin, (BitAction)BitValue);
}

/**
 * 函数：SPI初始化
 * 参数：无
 * 返回值：无
 * 注意事项：此函数需要用户实现内容，
 * 实现CSN、SCK、MOSI和MISO引脚的初始化
 */
void MySPI_Init(void)
{
	/* 开启时钟 */
	// 开启IO端口B和C时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);
	// 开启SPI2的时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
	
	/* GPIO初始化 */
	// 将CSN引脚初始化为推挽输出模式
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = CSN_Pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(CSN_Port, &GPIO_InitStructure);
	
	// 将SCK引脚初始化为复用推挽输出模式
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = SCK_Pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SCK_Port, &GPIO_InitStructure);
	
	// 将MOSI引脚初始化为复用推挽输出模式
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = MOSI_Pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(MOSI_Port, &GPIO_InitStructure);
	
	// 将MISO引脚初始化为上拉输入模式
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = MISO_Pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(MISO_Port, &GPIO_InitStructure);
	
	/* SPI初始化 */
	// 定义结构体变量
	SPI_InitTypeDef SPI_InitStructure;
	// 模式，选择为SPI主模式
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	// 方向，选择2线全双工
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	// 数据宽度，选择为8位
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	// 先行位，选择高位先行
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	// 波特率分频，选择8分频，为36MHz / 4 = 9MHz
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
	// SPI极性，选择低极性，为SPI模式0
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	// SPI相位，选择第一个时钟边沿采样，即为0，极性和相位决定选择SPI模式0
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	// NSS，选择由软件控制
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	// CRC多项式，暂时用不到，给默认值7
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	// 将结构体变量交给SPI_Init，配置SPI2
	SPI_Init(SPI2, &SPI_InitStructure);
	
	/* SPI使能 */
	// 使能SPI2，开始运行
	SPI_Cmd(SPI2, ENABLE);
	
	/* 设置默认电平 */
	// CSN默认高电平，即不选中从机
	MySPI_W_CSN(1);
}

/**
 * 函数：SPI起始
 * 参数：无
 * 返回值：无
 */
void MySPI_Start(void)
{
	// 拉低CSN，开始时序
	MySPI_W_CSN(0);
}

/**
 * 函数：SPI终止
 * 参数：无
 * 返回值：无
 */
void MySPI_Stop(void)
{
	// 拉高CSN，终止时序
	MySPI_W_CSN(1);
}

/**
 * 函数：SPI交换传输一个字节，使用SPI模式0
 * 参数：ByteSend 要发送的一个字节
 * 返回值：接收的一个字节
 */
uint8_t MySPI_SwapByte(uint8_t ByteSend)
{
	// 等待发送数据寄存器为空
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) != SET);
	// 写入数据到发送数据寄存器，开始产生时序
	SPI_I2S_SendData(SPI2, ByteSend);
	// 等待接收数据寄存器非空
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) != SET);
	// 读取接收到的数据并返回
	return SPI_I2S_ReceiveData(SPI2);
}
