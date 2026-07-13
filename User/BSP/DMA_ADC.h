#ifndef __DMA_ADC_H
#define __DMA_ADC_H
#include "stm32f10x.h"                  // Device header

#define M 9

extern uint16_t ADC_Value[M];

void ADC1_Init(void);
uint16_t Get_Filtered_ADC(uint16_t Value);

#endif
