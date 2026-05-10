#include "Struct_All.h"

// 定义用于接收按键键码的变量
uint8_t KeyNum;

int main(void)
{	
	// LED初始化
	LED_Init();
	// 按键初始化
	Key_Init();
	
	while (1)
	{
		// 获取按键键码
		KeyNum = Key_GetNum();
		// 左上角按键按下
		if (KeyNum == 2)
		{
			// LED翻转
			LED_Turn();
		}
	}
}
