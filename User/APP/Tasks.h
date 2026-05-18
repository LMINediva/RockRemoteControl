#ifndef __TASKS_H
#define __TASKS_H
#include "stm32f10x.h"                  // Device header

extern uint8_t BSP_Init_OK;

void My_NVIC_Init(void);
void BSP_Init(void);
void Task_25HZ(void);

#endif
