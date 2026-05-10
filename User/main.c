#include "Struct_All.h"

int main(void)
{	
	// 蜂鸣器初始化
	Buzzer_Init();
	
	while (1)
	{
		// 蜂鸣器响
		Buzzer_Ring();
	}
}
