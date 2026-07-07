#include "Struct_All.h"
#include "Tasks.h"

// volatile uint16_t Num_2ms, Num_10ms, Num_40ms, Num_250ms;

uint8_t KeyNum;

// 发送成功计次，发送失败计次
uint8_t SendSuccessCount, SendFailedCount;
// 接收成功计次，接收失败计次
uint8_t ReceiveSuccessCount, ReceiveFailedCount;

int main(void)
{	
	// 板级支持包中的硬件驱动初始化
	// BSP_Init();
	// 串口初始化，波特率：115200，8位数据，1位停止位，禁用奇偶校验
	Uart1_Init(115200);
	// NVIC初始化
	My_NVIC_Init();
	// NRF24L01初始化
	NRF24L01_Init();
	Key_Init();
	
	/* 初始化测试数据，此处值为任意设定，便于观察实验现象 */
	NRF24L01_TxPacket[0] = 0x00;
	NRF24L01_TxPacket[1] = 0x01;
	NRF24L01_TxPacket[2] = 0x02;
	NRF24L01_TxPacket[3] = 0x03;
	
	// 显示格式为：T:发送成功计次-发送失败计次-发送标志位
	// 显示格式为：R:接收成功计次-接收失败计次-接收标志位
	
	while (1)
	{	
		// 读取按键，获取键码
		KeyNum = Key_GetNum();
		
		// 按键按下
		if (KeyNum == 1)
		{
			/* 变换测试数据，便于观察实验现象 */
			// 实际项目中，可以将待发送的数据赋值给NRF24L01_TxPacket数组
			NRF24L01_TxPacket[0]++;
			NRF24L01_TxPacket[1]++;
			NRF24L01_TxPacket[2]++;
			NRF24L01_TxPacket[3]++;
			
			/* 调用NRF24L01_Send函数，发送数据，
			同时置发送标志位，方便用户了解发送状态 */
			// 发送标志位与发送状态的对应关系，可以转到此函数定义上方查看
			NRF24L01_Send();
			Delay_ms(10);
			// 判断发送标志位
			if (NRF24L01_SendFlag == 1)
			{
				// 发送标志位为1，表示发送成功
				// 发送成功计次变量自增
				SendSuccessCount++;
			}
			else
			{
				// 发送标志位不为1，即2/3/4，表示发送不成功
				// 发送失败计次变量自增
				SendFailedCount++;
			}
			
			Serial_SendString("\r\n");
			Serial_SendString("T:");
			// 显示发送成功次数
			Serial_SendNumber(SendSuccessCount, 3);
			Serial_SendString("-");
			// 显示发送失败次数
			Serial_SendNumber(SendFailedCount, 3);
			Serial_SendString("-");
			// 显示最近一次的发送标志位
			Serial_SendNumber(NRF24L01_SendFlag, 1);
			
			/* 显示发送数据 */
			Serial_SendString("\r\n");
			Serial_SendHexAsText(NRF24L01_TxPacket[0]);
			Serial_SendString(" ");
			Serial_SendHexAsText(NRF24L01_TxPacket[1]);
			Serial_SendString(" ");
			Serial_SendHexAsText(NRF24L01_TxPacket[2]);
			Serial_SendString(" ");
			Serial_SendHexAsText(NRF24L01_TxPacket[3]);
			
			/* TX字符串闪烁一次，表明发送了一次数据 */
			Serial_SendString("\r\nTX");
		}
		
		/* 主循环内循环执行NRF24L01_Receive函数，接收数据，
		同时返回接收标志位，方便用户了解接收状态*/
		// 接收标志位与接收状态的对应关系，可以转到此函数定义上方查看
		// 判断接收标志位
		if (NRF24L01_ReceiveFlag)
		{
			// 接收标志位不为0，表示收到了一个数据包
			if (NRF24L01_ReceiveFlag == 1)
			{
				// 接收标志位为1，表示接收成功
				// 接收成功计次变量自增
				ReceiveSuccessCount++;
			}
			else
			{
				// 接收标志位不为0也不为1，即2/3，表示此次接收产生了错误，
				// 错误接收的数据不应该使用
				// 接收失败计次变量自增
				ReceiveFailedCount++;
			}
			
			Serial_SendString("\r\n");
			Serial_SendString("R:");
			// 显示接收成功次数
			Serial_SendNumber(ReceiveSuccessCount, 3);
			Serial_SendString("-");
			// 显示接收失败次数
			Serial_SendNumber(ReceiveFailedCount, 3);
			Serial_SendString("-");
			// 显示最近一次的接收标志位
			Serial_SendNumber(NRF24L01_ReceiveFlag, 1);
			
			/* 显示接收数据 */
			Serial_SendString("\r\n");
			Serial_SendHexAsText(NRF24L01_RxPacket[0]);
			Serial_SendString(" ");
			Serial_SendHexAsText(NRF24L01_RxPacket[1]);
			Serial_SendString(" ");
			Serial_SendHexAsText(NRF24L01_RxPacket[2]);
			Serial_SendString(" ");
			Serial_SendHexAsText(NRF24L01_RxPacket[3]);
			
			// RX字符串闪烁一次，表明接收到了一次数据
			Serial_SendString("\r\nRX");
			
			// 接收标志位置0，恢复初始值
			NRF24L01_ReceiveFlag = 0;
		}
		
		/**
		if (Count_2ms >= 1)
		{
			Num_2ms++;
			// 2秒递增1
			Count_2ms = 0;
			Task_25HZ();
		}
		if (Count_10ms >= 5)
		{
			Num_10ms++;
			// 10秒递增1
			Count_10ms = 0;
		}
		if (Count_40ms >= 20)
		{
			Num_40ms++;
			// 40秒递增1
			Count_40ms = 0;
		}
		if (Count_250ms >= 125)
		{
			Num_250ms++;
			// 250秒（4分钟）递增1
			Count_250ms = 0;
		}
		
		if (Num_2ms % 1000 == 0)
		{			
			OLED_ShowNum(1, 5, Num_2ms / 1000, 5);
		}
		if (Num_10ms % 100 == 0)
		{
			OLED_ShowNum(2, 6, Num_10ms / 100, 5);
		}
		if (Num_40ms % 100 == 0)
		{			
			OLED_ShowNum(3, 6, Num_40ms / 100, 5);
		}
		if (Num_250ms % 100 == 0)
		{			
			OLED_ShowNum(4, 6, Num_250ms / 100, 5);
		}
		**/
	}
}
