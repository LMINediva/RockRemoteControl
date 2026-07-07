#ifndef __MYSPI_H
#define __MYSPI_H
#include "stm32f10x.h"                  // Device header

#define CSN_Port 	GPIOC
#define CSN_Pin		GPIO_Pin_13
#define SCK_Port	GPIOB
#define SCK_Pin		GPIO_Pin_13
#define MOSI_Port	GPIOB
#define MOSI_Pin	GPIO_Pin_15
#define MISO_Port	GPIOB
#define MISO_Pin	GPIO_Pin_14

void MySPI_Init(void);
void MySPI_Start(void);
void MySPI_Stop(void);
uint8_t MySPI_SwapByte(uint8_t ByteSend);

#endif
