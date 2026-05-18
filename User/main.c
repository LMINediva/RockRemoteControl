#include "Struct_All.h"
#include "Tasks.h"
#include "OLED.h"

volatile uint16_t Num_2ms, Num_10ms, Num_40ms, Num_250ms;

int main(void)
{	
	// OLED初始化
	OLED_Init();
	// 板级支持包中的硬件驱动初始化
	BSP_Init();
	
	// 显示字符串
	OLED_ShowString(1, 1, "2ms:");
	OLED_ShowString(2, 1, "10ms:");
	OLED_ShowString(3, 1, "40ms:");
	OLED_ShowString(4, 1, "250ms:");
	
	while (1)
	{
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

	}
}
