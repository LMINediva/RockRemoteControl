#include "Uart.h"

// 定义串口接收的数据变量
uint8_t Serial_RxData;
// 定义串口接收的标志位变量
uint8_t Serial_RxFlag;

/**
 * 函数：串口初始化
 * 参数：baud 波特率
 * 返回值：无
 */
void Uart1_Init(uint32_t baud)
{
	/* 开启时钟 */
	// 开启USART1和GPIOA的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	/* GPIO初始化 */
	// 将PA9引脚初始化为复用推挽输出
	GPIO_InitTypeDef GPIO_InitStructure;
	// 推挽输出模式
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	// PA9：TX引脚
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	//输出速度为50MHz
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	// GPIO初始化
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	// 将PA10引脚初始化为上拉输入
	// 上拉输入模式
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	// PA10：RX引脚
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	//输出速度为50MHz
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	// GPIO初始化
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/* USART初始化 */
	// 定义结构体变量
	USART_InitTypeDef USART_InitStructure;
	// 波特率
	USART_InitStructure.USART_BaudRate = baud;
	// 硬件流控制，不需要
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	// 模式，发送模式和接收模式均选择
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	// 奇偶校验，不需要
	USART_InitStructure.USART_Parity = USART_Parity_No;
	// 停止位，选择1位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	// 字长，选择8位
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	// 将结构体变量交给USART_Init，配置USART1
	USART_Init(USART1, &USART_InitStructure);
	
	/* 中断输出配置 */
	// 开启串口接收数据的中断
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	
	/* USART使能 */
	// 使能USART1，串口开始运行
	USART_Cmd(USART1, ENABLE);
}

/**
 * 函数：串口发送一个字节
 * 参数：Byte 要发送的一个字节
 * 返回值：无
 */
void Serial_SendByte(uint8_t Byte)
{
	// 将字节数据写入数据寄存器，写入后USART自动生成时序波形
	USART_SendData(USART1, Byte);
	
	// 等待发送完成
	// 下次写入数据寄存器会自动清除发送完成标志位，故此循环后，无需清除标志位
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}

/**
 * 函数：串口发送一个数组
 * 参数：Array 要发送数组的首地址
 * 参数：Length 要发送数组的长度
 * 返回值：无
 */
void Serial_SendArray(uint8_t *Array, uint16_t Length)
{
	uint16_t i;
	// 遍历数组
	for (i = 0; i < Length; i++)
	{
		// 依次调用Serial_SendByte发送每个字节数据
		Serial_SendByte(Array[i]);
	}
}

/**
 * 函数：串口发送一个字符串
 * 参数：String 要发送字符串的首地址
 * 返回值：无
 */
void Serial_SendString(char *String)
{
	uint8_t i;
	// 遍历字符数组（字符串），遇到字符串结束标志位后停止
	for (i = 0; String[i] != '\0'; i++)
	{
		// 依次调用Serial_SendByte发送每个字节数据
		Serial_SendByte(String[i]);
	}
}

/**
 * 函数：次方函数（内部使用）
 * 返回值：返回值等于X的Y次方
 */
static uint32_t Serial_Pow(uint32_t X, uint32_t Y)
{
	// 设置结果初值为1
	uint32_t Result = 1;
	// 执行Y次
	while (Y--)
	{
		// 将X累乘到结果
		Result *= X;
	}
	return Result;
}

/**
 * 函数：串口发送数字
 * 参数：Number 要发送的数字，范围：0~4294967295
 * 参数：Length 要发送数字的长度，范围：0~10
 * 返回值：无
 */
void Serial_SendNumber(uint32_t Number, uint8_t Length)
{
	uint8_t i;
	// 根据数字长度遍历数字的每一位
	for (i = 0; i < Length; i++)
	{
		// 依次调用Serial_SendByte发送每位数字
		Serial_SendByte(Number / Serial_Pow(10, Length - i - 1) % 10 + '0');
	}
}

/**
 * 函数：单个字节转两个 ASCII 字符 (例如 0xAB -> "AB")，并通过串口发送
 * 参数：Data 要发送的十六进制数
 * 返回值：无
 */
void Serial_SendHexAsText(uint8_t Data) {
    char hexStr[3];
    const char hexChars[] = "0123456789ABCDEF";
	// 高 4 位
    hexStr[0] = hexChars[(Data >> 4) & 0x0F];
	// 低 4 位
    hexStr[1] = hexChars[Data & 0x0F];
    hexStr[2] = '\0';
	Serial_SendString(hexStr);
}

/**
 * 函数：获取串口接收标志位
 * 参数：无
 * 返回值：串口接收标志位，范围：0~1，接收到数据后，标志位置1，
 * 读取后标志位自动清零
 */
uint8_t Serial_GetRxFlag(void)
{
	// 如果标志位为1
	if (Serial_RxFlag == 1)
	{
		Serial_RxFlag = 0;
		// 则返回1，并自动清零标志位
		return 1;
	}
	// 如果标志位为0，则返回0
	return 0;
}

/**
 * 函数：获取串口接收的数据
 * 参数：无
 * 返回值：接收的数据，范围：0~255
 */
uint8_t Serial_GetRxData(void)
{
	// 返回接收的数据变量
	return Serial_RxData;
}

/**
 * 函数：USART1中断函数
 * 参数：无
 * 返回值：无
 * 注意事项：此函数为中断函数，无需调用，中断触发后自动执行
 * 函数名为预留的指定名称，可以从启动文件复制
 * 请确保函数名正确，不能有任何差异，否则中断函数将不能进入
 */
void USART1_IRQHandler(void)
{
	// 判断是否是USART1的接收事件触发的中断，即RXNE置1
	if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
	{
		// 读取数据寄存器，存放在接收的数据变量中
		Serial_RxData = USART_ReceiveData(USART1);
		// 置接收标志位变量为1
		Serial_RxFlag = 1;
		
		// 清除USART1的RXNE标志位
		// 读取数据寄存器会自动清除此标志位
		// 如果已经读取了数据寄存器，也可以不执行此代码
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	}
}
