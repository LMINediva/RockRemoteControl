#ifndef __UART_H
#define __UART_H
#include "stm32f10x.h"                  // Device header

void Uart1_Init(uint32_t baud);
void PrintHexU8(uint8_t data);
void PrintHexS16(int16_t num);
void PrintS8(int8_t num);
void PrintU8(uint8_t num);
void PrintS16(int16_t num);
void PrintU16(uint16_t num);
void PrintString(char *s);
uint8_t Serial_GetRxFlag(void);
uint8_t Serial_GetRxData(void);

#endif
