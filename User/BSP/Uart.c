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
 * 函数：串口以HEX的形式输出U8（无符号8位整数）型数据
 * 参数：data 要发送的一个U8型数据，范围：0~255
 * 返回值：无
 */
void PrintHexU8(uint8_t data)
{
	// 将字节数据写入数据寄存器，写入后USART自动生成时序波形
	USART_SendData(USART1, data);
	
	// 等待发送完成
	// 下次写入数据寄存器会自动清除发送完成标志位，故此循环后，无需清除标志位
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}

/**
 * 函数：串口以HEX的形式输出S16（有符号16位整数）型数据
 * 参数：num 要发送的一个S16型数据，范围：-32768 ~ 32767
 * 返回值：无
 */
void PrintHexS16(int16_t num)
{
	// 先发送高8位，再发送低8位
	PrintHexU8((uint8_t)((num & 0xFF00) >> 8));
	PrintHexU8((uint8_t)(num & 0x00FF));
}

/**
 * 函数：串口以字符的形式输出S8（有符号8位整数）型数据
 * 参数：num 要发送的一个以字符的形式的S8型数据，范围：-128 ~ 127
 * 返回值：无
 */
void PrintS8(int8_t num)
{
	// 百位、十位和个位
	uint8_t hundred, ten, one;
	if (num < 0)
	{
		PrintHexU8('-');
		num = -num;
	}
	else
	{
		PrintHexU8(' ');
	}
	hundred = num / 100;
	ten = num % 100 / 10;
	one = num % 10;
	PrintHexU8('0' + hundred);
	PrintHexU8('0' + ten);
	PrintHexU8('0' + one);
}

/**
 * 函数：串口以字符的形式输出U8（无符号8位整数）型数据
 * 参数：num 要发送的一个以字符的形式的U8型数据，范围：0~255
 * 返回值：无
 */
void PrintU8(uint8_t num)
{
	// 百位、十位和个位
	uint8_t hundred, ten, one;
	hundred = num / 100;
	ten = num % 100 / 10;
	one = num % 10;
	PrintHexU8('0' + hundred);
	PrintHexU8('0' + ten);
	PrintHexU8('0' + one);
}

/**
 * 函数：串口以字符的形式输出S16（有符号16位整数）型数据
 * 参数：num 要发送的一个以字符的形式的S16型数据，范围：-32768 ~ 32767
 * 返回值：无
 */
void PrintS16(int16_t num)
{
	uint8_t w5, w4, w3, w2, w1;
	if (num < 0)
	{
		PrintHexU8('-');
		num = -num;
	}
	else
	{
		PrintHexU8(' ');
	}
	w5 = num % 100000 / 10000;
	w4 = num % 10000 / 1000;
	w3 = num % 1000 / 100;
	w2 = num % 100 / 10;
	w1 = num % 10;
	PrintHexU8('0' + w5);
	PrintHexU8('0' + w4);
	PrintHexU8('0' + w3);
	PrintHexU8('0' + w2);
	PrintHexU8('0' + w1);
}

/**
 * 函数：串口以字符的形式输出U16型数据
 * 参数：num 要发送的一个以字符的形式的U16型数据，范围：0 ~ 65535
 * 返回值：无
 */
void PrintU16(uint16_t num)
{
	uint8_t w5, w4, w3, w2, w1;
	w5 = num % 100000 / 10000;
	w4 = num % 10000 / 1000;
	w3 = num % 1000 / 100;
	w2 = num % 100 / 10;
	w1 = num % 10;
	PrintHexU8(' ');
	PrintHexU8('0' + w5);
	PrintHexU8('0' + w4);
	PrintHexU8('0' + w3);
	PrintHexU8('0' + w2);
	PrintHexU8('0' + w1);
}

/**
 * 函数：串口输出字符串
 * 参数：s 要发送的字符串
 * 返回值：无
 */
void PrintString(char *s)
{
	uint8_t i;
	// 遍历字符数组（字符串），遇到字符串结束标志位后停止
	for (i = 0; s[i] != '\0'; i++)
	{
		// 依次调用PrintHexU8发送每个字节数据
		PrintHexU8(s[i]);
	}
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
