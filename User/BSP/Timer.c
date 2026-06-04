#include "Timer.h"
#include "Tasks.h"

// 记录Timer3的中断次数
uint32_t Timer3_Count = 0;
// 记录Timer3的中断频率
uint16_t Timer3_Frequency;
// 计数2ms、10ms、40ms和250ms
volatile uint8_t Count_2ms, Count_10ms, Count_40ms, Count_250ms;

/**
 * 函数：定时器3中断初始化
 * 参数：Handler_Frequency 设定频率
 * 返回值：无
 */
void Timer3_Init(uint16_t Handler_Frequency)
{
	// 开启TIM3的时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
	// 选择TIM3为内部时钟，若不调用此函数，TIM默认也为内部时钟
	TIM_InternalClockConfig(TIM3);
	
	// Timer3的中断频率参数赋值
	Timer3_Frequency = Handler_Frequency;
	
	// 重置TIM3为默认值
	TIM_DeInit(TIM3);
	
	// 时基单元初始化
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	// 时钟分频，选择不分频，此参数用于配置滤波器时钟，不影响时基单元功能
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	// 计数器模式，选择向上计数
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	// 计数周期，即ARR的值
	TIM_TimeBaseInitStructure.TIM_Period = 1000 * 1000 / Handler_Frequency;
	// 预分频器，即PSC的值
	TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;
	// 重复计数器，高级定时器才会用到
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	// 配置TIM3的时基单元，1MHz频率下，计2000个数，为2毫秒时间
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);
	
	// 中断输出配置，清除定时器更新标志位
	// TIM_TimeBaseInit函数末尾，手动产生了更新事件
	// 若不清除此标志位，则开启中断后，会立刻进入一次中断
	// 如果不介意此问题，则不清除此标志位也可
	TIM_ClearFlag(TIM3, TIM_FLAG_Update);
	
	// 开启TIM3的更新中断
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	
	// 使能TIM3，定时器开始运行
	TIM_Cmd(TIM3, ENABLE);
}

/**
 * 函数：定时器3中断函数，可以复制到使用它的地方
 * 参数：无
 * 返回值：无
 */
void TIM3_IRQHandler(void)
{
	// 如果更新中断标志位 == SET
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) == SET)
	{
		// 板级支持包中的硬件驱动初始化未完成，则返回
		if (BSP_Init_OK == 0) {
			// 清除更新中断标志位
			TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
			return;
		}
		Timer3_Count++;
		Count_2ms++;
		Count_10ms++;
		Count_40ms++;
		Count_250ms++;
		// 清除更新中断标志位
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	}
}
