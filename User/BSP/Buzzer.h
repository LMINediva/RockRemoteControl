#ifndef __BUZZER_H
#define __BUZZER_H

#define Buzzer_Port		GPIOB
#define Buzzer_Pin		GPIO_Pin_10

void Buzzer_Init(void);
void Buzzer_ON(void);
void Buzzer_OFF(void);
void Buzzer_Ring(void);

#endif
