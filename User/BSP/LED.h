#ifndef __LED_H
#define __LED_H
#include "stm32f10x.h"                  // Device header

#define LED_Port	GPIOB
#define LED_Pin		GPIO_Pin_11

void LED_Init(void);
void LED_ON(void);
void LED_OFF(void);
void LED_ON_OFF(void);
void LED_ON_Frequency(uint8_t Light_Frequency);
void LED_Show(void);

#endif
