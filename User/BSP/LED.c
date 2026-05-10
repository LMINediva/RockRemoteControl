#include "Struct_All.h"

/**
 * 函数：LED初始化
 * 参数：无
 * 返回值：无
 */
void LED_Init(void)
{
	// 开启IO端口B时钟和AFIO
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	// 推挽输出模式
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	// LED：电源旁的指示灯
	GPIO_InitStructure.GPIO_Pin = LED_Pin;
	// 输出速度为50MHz
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	// GPIO初始化
	GPIO_Init(LED_Port, &GPIO_InitStructure);
}

/**
 * 函数：LED开启
 * 参数：无
 * 返回值：无
 */
void LED_ON(void)
{
	// 设置LED_Pin引脚为低电平
	GPIO_ResetBits(LED_Port, LED_Pin);
}

/**
 * 函数：LED关闭
 * 参数：无
 * 返回值：无
 */
void LED_OFF(void)
{
	// 设置LED_Pin引脚为高电平
	GPIO_SetBits(LED_Port, LED_Pin);
}

/**
* 函数：LED开机闪烁
 * 参数：无
 * 返回值：无
 */
void LED_ON_OFF(void)
{
	uint8_t i;
	for (i = 0; i < 3; i++)
	{
		// 开启LED
		LED_ON();
		// 延时100ms
		Delay_ms(100);
		// 关闭LED
		LED_OFF();
		// 延时100ms
		Delay_ms(100);
	}
	// 关闭LED
	LED_OFF();
	// 延时100ms
	Delay_ms(100);
}
