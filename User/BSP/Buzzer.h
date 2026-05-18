#ifndef __BUZZER_H
#define __BUZZER_H
#include "stm32f10x.h"                  // Device header

#define Buzzer_Port		GPIOB
#define Buzzer_Pin		GPIO_Pin_10

void Buzzer_Init(void);
void Buzzer_ON(void);
void Buzzer_OFF(void);
void Buzzer_ON_Frequency(uint8_t Buzzer_Frequency);
void Buzzer_Ring(void);

#endif
