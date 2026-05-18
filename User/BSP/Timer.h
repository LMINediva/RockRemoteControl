#ifndef __TIMER_H
#define __TIMER_H
#include "stm32f10x.h"                  // Device header

extern uint32_t Timer3_Count;
extern uint16_t Timer3_Frequency;
extern volatile uint8_t Count_2ms, Count_10ms, Count_40ms, Count_250ms;

void Timer3_Init(uint16_t Handler_Frequency);

#endif
