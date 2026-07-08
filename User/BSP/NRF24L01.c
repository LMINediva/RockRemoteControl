#include "NRF24L01.h"
#include "MySPI.h"

/* 发送部分 */
// 发送地址，固定5字节
uint8_t NRF24L01_TxAddress[5] = {0x11, 0x22, 0x33, 0x44, 0x55};
// 发送数据包宽度，范围：1~32字节
#define NRF24L01_TX_PACKET_WIDTH		4
// 发送数据包
uint8_t NRF24L01_TxPacket[NRF24L01_TX_PACKET_WIDTH];

/* 接收部分 */
// 接收通道0地址，固定5字节
uint8_t NRF24L01_RxAddress[5] = {0x11, 0x22, 0x33, 0x44, 0x55};
// 接收通道0数据包宽度，范围：1~32字节
#define NRF24L01_RX_PACKET_WIDTH		4
// 接收数据包
uint8_t NRF24L01_RxPacket[NRF24L01_RX_PACKET_WIDTH];

// 定义NRF24L01的模式变量，1：发送模式，2：接收模式
uint8_t NRF24L01_Mode;
// 定义NRF24L01发送数据包的标志位变量
uint8_t NRF24L01_SendFlag;
// 定义NRF24L01接收数据包的标志位变量
uint8_t NRF24L01_ReceiveFlag;

/* 引脚配置 */

/**
 * 函数：NRF24L01写CE高低电平
 * 参数：BitValue 要写入CE的电平，范围：0/1
 * 返回值：无
 * 说明：当上层函数需要写CE时，此函数会被调用，
 * 用户需要根据参数传入的值，将CE置高电平或者低电平，
 * 当参数传入0时，置CE为低电平，当参数传入1时，置CE为高电平
 */
static void NRF24L01_W_CE(uint8_t BitValue)
{
	// 根据BitValue的值，将CE置高电平或者低电平
	GPIO_WriteBit(CE_Port, CE_Pin, (BitAction)BitValue);
}

/**
 * 函数：NRF24L01引脚初始化
 * 参数：无
 * 返回值：无
 * 说明：当上层函数需要初始化时，此函数会被调用，
 * 用户需要将CSN、CE、MOSI、SCK引脚初始化为推挽输出模式，MISO引脚初始化为上拉输入模式
 */
static void NRF24L01_GPIO_Init(void)
{
	// 先初始化底层的SPI
	MySPI_Init();
	// 开启AFIO的时钟，外部中断必须开启AFIO的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	// 将CE引脚初始化为推挽输出模式
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = CE_Pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(CE_Port, &GPIO_InitStructure);
	
	// 将IRQ引脚初始化为上拉输入模式
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = IRQ_Pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(IRQ_Port, &GPIO_InitStructure);
	
	/* AFIO选中中断引脚 */
	// 将外部中断的15号线映射到GPIOC，即选择PC15为外部中断引脚
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource15);
	
	/* EXTI初始化 */
	// 定义结构体变量
	EXTI_InitTypeDef EXTI_InitStructure;
	// 选择配置外部中断的15号线
	EXTI_InitStructure.EXTI_Line = EXTI_Line15;
	// 指定外部中断线使能
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	// 指定外部中断线为中断模式
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	// 指定外部中断线为下降沿触发
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	// 将结构体变量交给EXTI_Init，配置EXTI外设
	EXTI_Init(&EXTI_InitStructure);
	
	/* 置引脚初始化后的默认电平 */
	// CE默认为0，退出收发模式
	NRF24L01_W_CE(0);
}

/* 指令实现 */

/**
 * 函数：NRF24L01写入寄存器（一个字节）
 * 参数：RegAddress 指定寄存器地址，范围：0x00~0x1F
 * 参数：Data 要写入的一个字节数据，范围：0x00~0xFF
 * 返回值：无
 */
void NRF24L01_WriteReg(uint8_t RegAddress, uint8_t Data)
{
	// SPI起始
	MySPI_Start();
	// 交换发送一个字节，通信开始的第一个字节为指令码，写寄存器（低5位为寄存器地址）
	MySPI_SwapByte(NRF24L01_W_REGISTER | RegAddress);
	// 发送写寄存器指令后，开始交换发送，在指定地址下写入数据
	MySPI_SwapByte(Data);
	// SPI终止
	MySPI_Stop();
}

/**
 * 函数：NRF24L01读取寄存器（一个字节）
 * 参数：RegAddress 指定寄存器地址，范围：0x00~0x1F
 * 返回值：指定寄存器的数据，范围：0x00~0xFF
 */
uint8_t NRF24L01_ReadReg(uint8_t RegAddress)
{
	uint8_t Data;
	// SPI起始
	MySPI_Start();
	// 交换发送一个字节，通信开始的第一个字节为指令码，读寄存器（低5位为寄存器地址）
	MySPI_SwapByte(NRF24L01_R_REGISTER | RegAddress);
	// 发送读寄存器指令后，开始交换接收，得到指定地址的数据
	Data = MySPI_SwapByte(NRF24L01_NOP);
	// SPI终止
	MySPI_Stop();
	// 返回读到的一个字节数据
	return Data;
}

/**
 * 函数：NRF24L01写入寄存器（多个字节）
 * 参数：RegAddress 指定寄存器地址，范围：0x00~0x1F
 * 参数：DataArray 要写入的数据数组，输入参数
 * 参数：Count 指定写入的数量，范围：0~5
 * 返回值：无
 */
void NRF24L01_WriteRegs(uint8_t RegAddress, uint8_t *DataArray, uint8_t Count)
{
	uint8_t i;
	// SPI起始
	MySPI_Start();
	// 交换发送一个字节，通信开始的第一个字节为指令码，写寄存器（低5位为寄存器地址）
	MySPI_SwapByte(NRF24L01_W_REGISTER | RegAddress);
	// 发送写寄存器指令后，开始交换发送，循环发送多次，在指定地址下写入多个数据
	for (i = 0; i < Count; i++)
	{
		// 将输入参数DataArray的数据写入到指定地址中
		MySPI_SwapByte(DataArray[i]);
	}
	// SPI终止
	MySPI_Stop();
}

/**
 * 函数：NRF24L01读取寄存器（多个字节）
 * 参数：RegAddress 指定寄存器地址，范围：0x00~0x1F
 * 参数：DataArray 读取得到的数据数组，输出参数
 * 参数：Count 指定读取的数量，范围：0~5
 * 返回值：无
 */
void NRF24L01_ReadRegs(uint8_t RegAddress, uint8_t *DataArray, uint8_t Count)
{
	uint8_t i;
	// SPI起始
	MySPI_Start();
	// 交换发送一个字节，通信开始的第一个字节为指令码，读寄存器（低5位为寄存器地址）
	MySPI_SwapByte(NRF24L01_R_REGISTER | RegAddress);
	// 发送读寄存器指令后，开始交换接收，循环接收多次，得到指定地址下的多个数据
	for (i = 0; i < Count; i++)
	{
		// 将接收到的数据写入到输出参数DataArray中
		DataArray[i] = MySPI_SwapByte(NRF24L01_NOP);
	}
	// SPI终止
	MySPI_Stop();
}

/**
 * 函数：NRF24L01写入Tx有效载荷
 * 参数：DataArray 要写入的数据数组，输入参数
 * 参数：Count 指定写入的数量，范围：0~5
 * 返回值：无
 */
void NRF24L01_WriteTxPayload(uint8_t *DataArray, uint8_t Count)
{
	uint8_t i;
	// SPI起始
	MySPI_Start();
	// 交换发送一个字节，通信开始的第一个字节为指令码，写入Tx有效载荷
	MySPI_SwapByte(NRF24L01_W_TX_PAYLOAD);
	// 发送写入Tx有效载荷指令后，开始交换发送，循环发送多次，写入多个数据
	for (i = 0; i < Count; i++)
	{
		// 将输入参数DataArray的数据写入到Tx有效载荷中
		MySPI_SwapByte(DataArray[i]);
	}
	// SPI终止
	MySPI_Stop();
}

/**
 * 函数：NRF24L01读取Rx有效载荷
 * 参数：DataArray 读取得到的数据数组，输出参数
 * 参数：Count 指定读取的数量，范围：0~32
 * 返回值：无
 */
void NRF24L01_ReadRxPayload(uint8_t *DataArray, uint8_t Count)
{
	uint8_t i;
	// SPI起始
	MySPI_Start();
	// 交换发送一个字节，通信开始的第一个字节为指令码，读取Rx有效载荷
	MySPI_SwapByte(NRF24L01_R_RX_PAYLOAD);
	// 发送读取Rx有效载荷指令后，开始交换接收，循环接收多次，得到多个数据
	for (i = 0; i < Count; i++)
	{
		// 将读取的数据写入到输出参数DataArray中
		DataArray[i] = MySPI_SwapByte(NRF24L01_NOP);
	}
	// SPI终止
	MySPI_Stop();
}

/**
 * 函数：NRF24L01清空Tx FIFO的所有数据
 * 参数：无
 * 返回值：无
 */
void NRF24L01_FlushTx(void)
{
	// SPI起始
	MySPI_Start();
	// 交换发送一个字节，通信开始的第一个字节为指令码，清空Tx FIFO
	MySPI_SwapByte(NRF24L01_FLUSH_TX);
	// SPI终止
	MySPI_Stop();
}

/**
 * 函数：NRF24L01清空Rx FIFO的所有数据
 * 参数：无
 * 返回值：无
 */
void NRF24L01_FlushRx(void)
{
	// SPI起始
	MySPI_Start();
	// 交换发送一个字节，通信开始的第一个字节为指令码，清空Rx FIFO
	MySPI_SwapByte(NRF24L01_FLUSH_RX);
	// SPI终止
	MySPI_Stop();
}

/**
 * 函数：NRF24L01读取状态寄存器
 * 参数：无
 * 返回值：状态寄存器的值，范围：0x00~0xFF
 */
static uint8_t NRF24L01_ReadStatus(void)
{
	uint8_t Status;
	// SPI起始
	MySPI_Start();
	// 交换发送一个字节，通信开始的第一个字节为指令码，空指令
	// 第一个字节发送任意指令，都可以交换得到状态寄存器的值
	Status = MySPI_SwapByte(NRF24L01_NOP);
	// SPI终止
	MySPI_Stop();
	// 返回状态寄存器的值
	return Status;
}

/* 功能函数 */

/**
 * 函数：NRF24L01进入掉电模式（CE = 0，PWR_UP = 0）
 * 参数：无
 * 返回值：无
 */
void NRF24L01_PowerDown(void)
{
	uint8_t Config;
	// CE置0，退出收发模式
	NRF24L01_W_CE(0);
	// 读-改-写操作流程，单独修改配置寄存器的某些位而不影响其他位
	// 读取配置寄存器
	Config = NRF24L01_ReadReg(NRF24L01_CONFIG);
	// 配置寄存器全为1，出错，退出函数
	if (Config == 0xFF)
	{
		return;
	}
	// 配置寄存器位1（PWR_UP）置0
	Config &= ~0x02;
	// 写回配置寄存器
	NRF24L01_WriteReg(NRF24L01_CONFIG, Config);
}

/**
 * 函数：NRF24L01进入待机模式1（CE = 0，PWR_UP = 1）
 * 参数：无
 * 返回值：无
 */
void NRF24L01_StandbyI(void)
{
	uint8_t Config;
	// CE置0，退出收发模式
	NRF24L01_W_CE(0);
	// 读-改-写操作流程，单独修改配置寄存器的某些位而不影响其他位
	// 读取配置寄存器
	Config = NRF24L01_ReadReg(NRF24L01_CONFIG);
	// 配置寄存器全为1，出错，退出函数
	if (Config == 0xFF)
	{
		return;
	}
	// 配置寄存器位1（PWR_UP）置1
	Config |= 0x02;
	// 写回配置寄存器
	NRF24L01_WriteReg(NRF24L01_CONFIG, Config);
}

/**
 * 函数：NRF24L01进入接收模式（CE = 1，PWR_UP = 1，PRIM_RX = 1）
 * 参数：无
 * 返回值：无
 */
void NRF24L01_RxMode(void)
{
	uint8_t Config;
	// CE置0，退出收发模式
	NRF24L01_W_CE(0);
	// 读-改-写操作流程，单独修改配置寄存器的某些位而不影响其他位
	// 读取配置寄存器
	Config = NRF24L01_ReadReg(NRF24L01_CONFIG);
	// 配置寄存器全为1，出错，退出函数
	if (Config == 0xFF)
	{
		return;
	}
	// 配置寄存器位1（PWR_UP）和位0（PRIM_RX）都置1
	Config |= 0x03;
	// 写回配置寄存器
	NRF24L01_WriteReg(NRF24L01_CONFIG, Config);
	// CE置1，进入收发模式，因为PRIM_RX为1，所以进入接收模式
	NRF24L01_W_CE(1);
	// NRF24L01的模式变量置2（接收模式）
	NRF24L01_Mode = 2;
}

/**
 * 函数：NRF24L01进入发送模式（CE = 1，PWR_UP = 1，PRIM_RX = 0）
 * 参数：无
 * 返回值：无
 */
void NRF24L01_TxMode(void)
{
	uint8_t Config;
	// CE置0，退出收发模式
	NRF24L01_W_CE(0);
	// 读-改-写操作流程，单独修改配置寄存器的某些位而不影响其他位
	// 读取配置寄存器
	Config = NRF24L01_ReadReg(NRF24L01_CONFIG);
	// 配置寄存器全为1，出错，退出函数
	if (Config == 0xFF)
	{
		return;
	}
	// 配置寄存器位1（PWR_UP）置1
	Config |= 0x02;
	// 配置寄存器位0（PRIM_RX）置0
	Config &= ~0x01;
	// 写回配置寄存器
	NRF24L01_WriteReg(NRF24L01_CONFIG, Config);
	// CE置1，进入收发模式，因为PRIM_RX为0，所以进入发送模式
	NRF24L01_W_CE(1);
	// NRF24L01的模式变量置1（发送模式）
	NRF24L01_Mode = 1;
}

/**
 * 函数：NRF24L01初始化
 * 参数：无
 * 返回值：无
 * 说明：使用前，需要调用此初始化函数
 */
void NRF24L01_Init(void)
{
	// 先调用底层的端口初始化
	NRF24L01_GPIO_Init();
	
	// 初始化配置一系列寄存器，寄存器值的意义需参考手册中的寄存器描述
	// 以下配置通信双方必须保持一致，否则无法进行通信
	// 配置寄存器，不屏蔽中断，使能CRC，CRC为1字节，PWR_UP = 0，PRIM_RX = 0
	NRF24L01_WriteReg(NRF24L01_CONFIG, 0x08);
	// 使能自动应答，开启接收通道0~通道5的自动应答
	NRF24L01_WriteReg(NRF24L01_EN_AA, 0x3F);
	// 使能接收通道，只开启接收通道0
	NRF24L01_WriteReg(NRF24L01_EN_RXADDR, 0x01);
	// 设置地址宽度，地址宽度为5字节
	NRF24L01_WriteReg(NRF24L01_SETUP_AW, 0x03);
	// 设置自动重传，间隔250us，重传3次
	NRF24L01_WriteReg(NRF24L01_SETUP_RETR, 0x03);
	// 射频通道，频率为(2400 + 2)MHz = 2.402GHz
	NRF24L01_WriteReg(NRF24L01_RF_CH, 0x02);
	// 射频设置，通信速率为2Mbps，发射功率为0dBm
	NRF24L01_WriteReg(NRF24L01_RF_SETUP, 0x0E);
	
	// 接收通道0地址，设置为全局数组NRF24L01_RxAddress指定的地址，地址宽度固定为5字节
	NRF24L01_WriteRegs(NRF24L01_RX_ADDR_P0, NRF24L01_RxAddress, 5);
	// 接收通道0的数据包宽度，设置为宏定义NRF24L01_RX_PACKET_WIDTH指定的值
	NRF24L01_WriteReg(NRF24L01_RX_PW_P0, NRF24L01_RX_PACKET_WIDTH);
	
	// 清空Tx FIFO的所有数据
	NRF24L01_FlushTx();
	// 清空Rx FIFO的所有数据
	NRF24L01_FlushRx();
	// 给状态寄存器的位4（MAX_RT）、位5（TX_DS）和位6（RX_DR）写1，清标志位
	NRF24L01_WriteReg(NRF24L01_STATUS, 0x70);
	
	// 初始化配置完成，芯片默认进入接收模式
	NRF24L01_RxMode();
}

/**
 * 函数：NRF24L01发送数据包
 * 参数：无
 * 返回值：无
 * 说明：调用此函数前，直接修改全局数组NRF24L01_TxAddress和NRF24L01_TxPacket
 * 来设置发送的地址和数据
 */
void NRF24L01_Send(void)
{
	// 发送地址，设置为全局数组NRF24L01_TxAddress指定的地址，地址宽度固定为5字节
	NRF24L01_WriteRegs(NRF24L01_TX_ADDR, NRF24L01_TxAddress, 5);
	// 写发送有效载荷，写入全局数组NRF24L01_TxPacket指定的数据，
	// 数据宽度为NRF24L01_TX_PACKET_WIDTH
	NRF24L01_WriteTxPayload(NRF24L01_TxPacket, NRF24L01_TX_PACKET_WIDTH);
	// 接收通道0地址，此处必须也设置为发送地址，用于接收应答
	NRF24L01_WriteRegs(NRF24L01_RX_ADDR_P0, NRF24L01_TxAddress, 5);
	
	// 发送的地址和有效载荷写入完成，进入发送模式，开始发送数据
	NRF24L01_TxMode();
}

/**
 * 函数：NRF24L01更新接收地址
 * 参数：无
 * 返回值：无
 * 说明：如果想在运行时动态修改接收地址，则可先向全局数组NRF24L01_RxAddress写入修改的地址，
 *		 然后再调用此函数，使修改的接收地址生效
 */
void NRF24L01_UpdateRxAddress(void)
{
	// 接收通道0地址，设置为全局数组NRF24L01_RxAddress指定的地址，地址宽度固定为5字节
	NRF24L01_WriteRegs(NRF24L01_RX_ADDR_P0, NRF24L01_RxAddress, 5);
}

/**
 * 函数：NRF24L01中断
 * 参数：无
 * 返回值：无
 * 说明：NRF24L01_SendFlag发送标志位，方便用户了解发送状态
 *			1：发送成功，无错误
 *			2：达到了最大重发次数仍未收到应答，可能是收发双方配置不一致、
 *			接收方不存在、接收FIFO已满或者多个发送数据包碰撞
 *			3：状态寄存器的值不合法，可能是设备不存在、断路、短路或者引脚配置不正确
 *			4：发送超时，可能是设备未初始化、断路、短路或者引脚配置不正确
 *		 NRF24L01_ReceiveFlag接收标志位，方便用户了解接收状态
 *			0：未接收到数据包
 *			1：成功接收到一个数据包
 *			2：状态寄存器的值不合法，可能是设备不存在、断路、短路或者引脚配置不正确
 *			3：设备仍处于掉电模式，可能是设备未初始化、曾经断电过、断路、短路
 *			或者引脚配置配置不正确
 */
static void NRF24L01_IRQ(void)
{
	uint8_t Status;
	// 读取状态寄存器，保存至Status变量
	Status = NRF24L01_ReadStatus();
	
	// 判断NRF24L01当前的工作模式
	if (NRF24L01_Mode == 1)
	{
		/* 处于发送模式 */
		/* 根据状态寄存器的值，判断发送状态 */
		if ((Status & 0x30) == 0x30)
		{
			// 状态寄存器位4（MAX_RT）和位5（TX_DS）同时为1
			// 状态寄存器的值不合法，置标志位为3
			NRF24L01_SendFlag = 3;
			// 发送出错，重新初始化一次设备，这样有助于设备从错误中恢复正常
			NRF24L01_Init();
		}
		else if ((Status & 0x10) == 0x10)
		{
			// 状态寄存器位4（MAX_RT）为1
			// 达到了最大重发次数仍未收到应答，置标志位为2
			NRF24L01_SendFlag = 2;
			// 发送出错，重新初始化一次设备，这样有助于设备从错误中恢复正常
			NRF24L01_Init();
		}
		else if ((Status & 0x20) == 0x20)
		{
			// 状态寄存器位5（TX_DS）为1
			// 发送成功，无错误，置标志位为1
			NRF24L01_SendFlag = 1;
		}
		// 给状态寄存器的位4（MAX_RT）和位5（TX_DS）写1，清除标志位
		NRF24L01_WriteReg(NRF24L01_STATUS, 0x30);
		// 清空Tx FIFO的所有数据
		NRF24L01_FlushTx();
		// 发送完成后，恢复接收通道0原来的地址
		// 如果发送地址和接收通道0地址设置相同，则可不执行这一句
		NRF24L01_WriteRegs(NRF24L01_RX_ADDR_P0, NRF24L01_RxAddress, 5);
		//发送完成，芯片恢复为接收模式
		NRF24L01_RxMode();
	} else if (NRF24L01_Mode == 2)
	{
		/* 处于接收模式 */
		uint8_t Config;
		// 读取配置寄存器，保存至Config变量
		Config = NRF24L01_ReadReg(NRF24L01_CONFIG);
		/* 根据配置寄存器和状态寄存器的值，判断接收状态 */
		if ((Config & 0x02) == 0x00)
		{
			// 配置寄存器位1（PWR_UP）为0
			// 设备仍处于掉电模式，置标志位为3
			NRF24L01_ReceiveFlag = 3;
			// 接收出错，重新初始化一次设备，这样有助于设备从错误中恢复正常
			NRF24L01_Init();
		}
		else if ((Status & 0x30) == 0x30)
		{
			// 状态寄存器位4（MAX_RT）和位5（TX_DS）同时为1
			// 状态寄存器的值不合法，置标志位为2
			NRF24L01_ReceiveFlag = 2;
			// 接收出错，重新初始化一次设备，这样有助于设备从错误中恢复正常
			NRF24L01_Init();
		}
		else if ((Status & 0x40) == 0x40)
		{
			// 状态寄存器位6（RX_DR）为1
			// 接收到数据，置标志位为1
			NRF24L01_ReceiveFlag = 1;
			// 读接收有效载荷，存放在全局数组NRF24L01_RxPacket中，
			// 数据宽度为NRF24L01_RX_PACKET_WIDTH
			NRF24L01_ReadRxPayload(NRF24L01_RxPacket, NRF24L01_RX_PACKET_WIDTH);
			// 给状态寄存器的位6（RX_DR）写1，清标志位
			NRF24L01_WriteReg(NRF24L01_STATUS, 0x40);
			// 清空Rx FIFO的所有数据
			NRF24L01_FlushRx();
		}
		else
		{
			// 未收到数据，置标志位为0
			NRF24L01_ReceiveFlag = 0;
		}
	}
}

/**
 * 函数：EXTI15外部中断函数
 * 参数：无
 * 返回值：无
 * 注意事项：此函数为中断函数，无需调用，中断触发后自动执行
 * 函数名为预留的指定名称，可以从启动文件复制
 * 请确保函数名正确，不能有任何差异，否则中断函数将不能进入
 */
void EXTI15_10_IRQHandler(void)
{
	// 判断是否是外部中断15号线触发的中断
	if (EXTI_GetITStatus(EXTI_Line15) == SET)
	{
		NRF24L01_IRQ();
		// 清除外部中断15号线的中断标志位
		// 中断标志位必须清除
		// 否则中断将连续不断地触发，导致主程序卡死
		EXTI_ClearITPendingBit(EXTI_Line15);
	}
}
