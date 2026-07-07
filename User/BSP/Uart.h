#ifndef __UART_H
#define __UART_H
#include "stm32f10x.h"                  // Device header

void Uart1_Init(uint32_t baud);
void Serial_SendByte(uint8_t Byte);
void Serial_SendArray(uint8_t *Array, uint16_t Length);
void Serial_SendString(char *String);
void Serial_SendNumber(uint32_t Number, uint8_t Length);
void Serial_SendHexAsText(uint8_t Data);
uint8_t Serial_GetRxFlag(void);
uint8_t Serial_GetRxData(void);

#endif
