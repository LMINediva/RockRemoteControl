#include "Struct_All.h"
#include "Tasks.h"
#include "OLED.h"
#include "Delay.h"

// volatile uint16_t Num_2ms, Num_10ms, Num_40ms, Num_250ms;

volatile uint16_t Battery_RC;

int main(void)
{	
	// OLED初始化
	OLED_Init();
	// 板级支持包中的硬件驱动初始化
	BSP_Init();
	
	/* 显示静态字符串 */
	OLED_ShowString(4, 1, "Battery:");
	
	while (1)
	{
		// 自转微调电位器
		OLED_ShowNum(1, 1, ADC_Value[0], 4);
		// 前后微调电位器
		OLED_ShowNum(1, 6, ADC_Value[1], 4);
		// 左右微调电位器
		OLED_ShowNum(1, 11, ADC_Value[2], 4);
		// 3.7V锂电池电压
		OLED_ShowNum(2, 1, ADC_Value[3], 4);
		// 左摇杆的Y轴（上下），油门（THROTTLE）
		OLED_ShowNum(2, 6, ADC_Value[4], 4);
		// 左摇杆的X轴（左右），偏航角（YAW）
		OLED_ShowNum(2, 11, ADC_Value[5], 4);
		// 右摇杆的Y轴（上下），俯仰角（PITCH）
		OLED_ShowNum(3, 1, ADC_Value[6], 4);
		// 右摇杆的X轴（左右），翻滚角（ROLL）
		OLED_ShowNum(3, 6, ADC_Value[7], 4);
		// 内部参考电压（1.2V）
		OLED_ShowNum(3, 11, ADC_Value[8], 4);
		
		// 电池电压的100倍：(uint16_t)(2.0f * ADC_Value[3] / ADC_Value[8] * 1.2f * 100)
		Battery_RC = (uint16_t)(2.0f * ADC_Value[3] / ADC_Value[8] * 1.2f * 100);
		OLED_ShowNum(4, 9, Battery_RC, 4);
		
		// 延时100ms，手动增加一些转换的间隔时间
		Delay_ms(100);
		
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
