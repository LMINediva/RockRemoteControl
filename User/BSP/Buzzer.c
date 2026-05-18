#include "stm32f10x.h"                  // Device header
#include "Buzzer.h"
#include "Delay.h"
#include "Timer.h"

/**
 * 函数：蜂鸣器初始化
 * 参数：无
 * 返回值：无
 */
void Buzzer_Init(void)
 {
	 // 开启IO端口B时钟
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	 
	 // GPIO初始化
	 GPIO_InitTypeDef GPIO_InitStructure;
	 // 推挽输出模式（Push Pull Output）
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	 // Buzzer：蜂鸣器
	 GPIO_InitStructure.GPIO_Pin = Buzzer_Pin;
	 // 输出速度为50MHz
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	 // GPIO初始化
	 GPIO_Init(Buzzer_Port, &GPIO_InitStructure);
	 
	 // 设置蜂鸣器控制引脚为低电平，即蜂鸣器默认不响
	 GPIO_ResetBits(Buzzer_Port, Buzzer_Pin);
 }
 
/**
 * 函数：蜂鸣器开启
 * 参数：无
 * 返回值：无
 */
void Buzzer_ON(void)
 {
	 // 设置蜂鸣器控制引脚为高电平
	 GPIO_SetBits(Buzzer_Port, Buzzer_Pin);
 }

/**
 * 函数：蜂鸣器关闭
 * 参数：无
 * 返回值：无
 */
void Buzzer_OFF(void)
 {
	 // 设置蜂鸣器控制引脚为低电平
	 GPIO_ResetBits(Buzzer_Port, Buzzer_Pin);
 }
 
 /**
 * 函数：蜂鸣器按照某个频率闪烁（需要循环扫描该函数）
 * 参数：Buzzer_Frequency 闪烁频率（HZ）
 * 返回值：无
 */
void Buzzer_ON_Frequency(uint8_t Buzzer_Frequency)
{
	uint16_t time_count;
	static uint8_t Buzzer_State;
	static uint32_t Last_Time, Now_Time;
	
	Now_Time = Timer3_Count;
	if (Buzzer_State)
	{
		time_count = (uint16_t)(Timer3_Frequency / Buzzer_Frequency / 2);
		if (Now_Time - Last_Time >= time_count)
		{
			Buzzer_OFF();
			Buzzer_State = 0;
			Last_Time = Timer3_Count;
		}
	}
	else
	{
		time_count = (uint16_t)(Timer3_Frequency / Buzzer_Frequency / 2);
		if (Now_Time - Last_Time >= time_count)
		{
			Buzzer_ON();
			Buzzer_State = 1;
			Last_Time = Timer3_Count;
		}
	}
}
 
 /**
 * 函数：蜂鸣器响
 * 参数：无
 * 返回值：无
 */
void Buzzer_Ring(void)
 {
	 /**
	 // 开启蜂鸣器
	 Buzzer_ON();
	 // 延时100ms
	 Delay_ms(100);
	 // 关闭蜂鸣器
	 Buzzer_OFF();
	 // 延时100ms
	 Delay_ms(100);
	 
	 // 开启蜂鸣器
	 Buzzer_ON();
	 // 延时100ms
	 Delay_ms(100);
	 // 关闭蜂鸣器
	 Buzzer_OFF();
	 // 延时700ms
	 Delay_ms(700);
	 **/
	 
	 // 蜂鸣器以1HZ的频率响
	 Buzzer_ON_Frequency(1);
 }
