#include "Struct_All.h"

/**
 * 函数：按键初始化
 * 参数：无
 * 返回值：无
 */
void Key_Init(void)
{
	// 开启GPIOB的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	// GPIO初始化
	GPIO_InitTypeDef GPIO_InitStructure;
	// 上拉输入IPU（In Pull Up）
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	// Key1：左上角按键；Key2：右上角按键
	GPIO_InitStructure.GPIO_Pin = Key1_Pin | Key2_Pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(Key_Port, &GPIO_InitStructure);
}

/**
 * 函数：按键获取键码
 * 参数：无
 * 返回值：按下按键的键码值，范围：0~2，返回0代表没有按键按下
 */
uint8_t Key_GetNum(void)
{
	// 定义变量，默认键码值为0
	uint8_t KeyNum = 0;
	
	// 读取Key1_Pin（左上角按键）输入寄存器的状态，如果为0，则代表按键1按下
	if (GPIO_ReadInputDataBit(Key_Port, Key1_Pin) == 0) 
	{
		// 延时消抖
		Delay_ms(20);
		// 等待按键松手
		while (GPIO_ReadInputDataBit(Key_Port, Key1_Pin) == 0);
		// 延时消抖
		Delay_ms(20);
		// 置键码为1
		KeyNum = 1;
	}
	
	// 读取Key2_Pin（右上角按键）输入寄存器的状态，如果为0，则代表按键1按下
	if (GPIO_ReadInputDataBit(Key_Port, Key2_Pin) == 0) 
	{
		// 延时消抖
		Delay_ms(20);
		// 等待按键松手
		while (GPIO_ReadInputDataBit(Key_Port, Key2_Pin) == 0);
		// 延时消抖
		Delay_ms(20);
		// 置键码为2
		KeyNum = 2;
	}
	
	return KeyNum;
}
