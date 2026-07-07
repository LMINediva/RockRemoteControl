#ifndef __NRF24L01_H
#define __NRF24L01_H
#include "stm32f10x.h"                  // Device header
#include "NRF24L01_Define.h"

#define CE_Port 	GPIOC
#define CE_Pin		GPIO_Pin_14
#define IRQ_Port	GPIOC
#define IRQ_Pin		GPIO_Pin_15

extern uint8_t NRF24L01_TxAddress[];
extern uint8_t NRF24L01_TxPacket[];

extern uint8_t NRF24L01_RxAddress[];
extern uint8_t NRF24L01_RxPacket[];

extern uint8_t NRF24L01_SendFlag;
extern uint8_t NRF24L01_ReceiveFlag;

/* 指令实现 */
void NRF24L01_WriteReg(uint8_t RegAddress, uint8_t Data);
uint8_t NRF24L01_ReadReg(uint8_t RegAddress);
void NRF24L01_WriteRegs(uint8_t RegAddress, uint8_t *DataArray, uint8_t Count);
void NRF24L01_ReadRegs(uint8_t RegAddress, uint8_t *DataArray, uint8_t Count);
void NRF24L01_WriteTxPayload(uint8_t *DataArray, uint8_t Count);
void NRF24L01_ReadRxPayload(uint8_t *DataArray, uint8_t Count);
void NRF24L01_FlushTx(void);
void NRF24L01_FlushRx(void);

/* 功能函数 */
void NRF24L01_PowerDown(void);
void NRF24L01_StandbyI(void);
void NRF24L01_RxMode(void);
void NRF24L01_TxMode(void);
void NRF24L01_Init(void);
void NRF24L01_Send(void);
void NRF24L01_UpdateRxAddress(void);

#endif
