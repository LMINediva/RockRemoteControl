#ifndef __KEY_H
#define __KEY_H
#include "stm32f10x.h"                  // Device header

#define Key_Port	GPIOB
#define Key1_Pin	GPIO_Pin_0
#define Key2_Pin	GPIO_Pin_1

void Key_Init(void);
uint8_t Key_GetNum(void);

#endif
