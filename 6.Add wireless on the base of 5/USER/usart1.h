#ifndef __USART1_H
#define	__USART1_H

#include "stm32f10x.h"
#include <stdio.h>
#include "Wireless_handle.h"

#define USART1_DR_BASE		0x40013804

void USART1_Config(void);
void USART1_NVIC_Configuration(void);
int fputc(int ch, FILE *f);
void USART_SendData_By_ZZB(USART_TypeDef* USARTx, u16 Data);
void USART_SendString_By_ZZB(USART_TypeDef* USARTx, u8* String);
void USART1_DMA_Config(void);
void USART1_DMA_Start(u16 size);
void USART1_DMA_NVIC_Configuration(void);

#endif /* __USART1_H */
