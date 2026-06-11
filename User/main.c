#include "Struct_All.h"
#include "Tasks.h"
#include "OLED.h"
#include "Delay.h"
#include "Key.h"

// volatile uint16_t Num_2ms, Num_10ms, Num_40ms, Num_250ms;

// 定义用于接收按键键码的变量
uint8_t KeyNum;
// 按下次数记录
uint8_t PressNum = 0;
// 定义用于接收串口数据的变量
uint8_t RxData;

int main(void)
{	
	// OLED初始化
	OLED_Init();
	// 板级支持包中的硬件驱动初始化
	BSP_Init();
	// 按键初始化
	Key_Init();
	
	/* 显示静态字符串 */
	OLED_ShowString(1, 1, "Rx:");
	OLED_ShowString(2, 1, "Tx:");
	OLED_ShowString(3, 1, "Press:");
	
	while (1)
	{
		// 获取按键键码
		KeyNum = Key_GetNum();
		
		// 串口接收
		// 检查串口接收数据的标志位
		if (Serial_GetRxFlag() == 1)
		{
			// 获取串口接收的数据
			RxData = Serial_GetRxData();
			// 显示串口接收的数据
			OLED_ShowHexNum(1, 4, RxData, 2);
		}
		
		// 串口发送
		if (KeyNum == 1)
		{
			PressNum++;
			// 显示按键次数
			OLED_ShowNum(3, 7, PressNum, 2);
			// 根据按下次数的不同，测试自定义串口函数
			switch (PressNum)
			{
				case 1:
					// 串口以HEX的形式输出U8（无符号8位整数）型数据
					PrintHexU8(0x11);
					// 显示串口发送的数据
					OLED_ShowHexNum(2, 4, 0x11, 2);
				break;
				case 2:
					// 串口以HEX的形式输出S16（有符号16位整数）型数据
					PrintHexS16(0x2222);
					// 显示串口发送的数据
					OLED_ShowHexNum(2, 4, 0x2222, 4);
				break;
				case 3:
					// 串口以字符的形式输出S8（有符号8位整数）型数据
					PrintS8(100);
					// 显示串口发送的数据
					OLED_ShowNum(2, 4, 100, 3);
				break;
				case 4:
					// 串口以字符的形式输出U8（无符号8位整数）型数据
					PrintU8(200);
					// 显示串口发送的数据
					OLED_ShowNum(2, 4, 200, 3);
				break;
				case 5:
					// 串口以字符的形式输出S16（有符号16位整数）型数据
					PrintS16(30000);
					// 显示串口发送的数据
					OLED_ShowNum(2, 4, 30000, 5);
				break;
				case 6:
					// 串口以字符的形式输出U16型数据
					PrintU16(60000);
					// 显示串口发送的数据
					OLED_ShowNum(2, 4, 60000, 5);
				break;
				case 7:
					// 串口输出字符串
					PrintString("aaa");
					// 显示串口发送的数据
					OLED_ShowString(2, 4, "aaa");
				break;
				default:
					break;
			}
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
