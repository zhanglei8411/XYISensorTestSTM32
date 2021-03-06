#ifndef __USART3_H
#define	__USART3_H

#include "stm32f10x.h"
#include <stdio.h>
#include "M100_handle.h"
#include "Laser_handle.h"

#define USART3_DR_BASE		0x40004804

void USART3_Config(void);
void USART3_NVIC_Configuration(void);
void USART3_DMA_Config(void);
void USART3_DMA_Start(u16 size);
void USART3_DMA_NVIC_Configuration(void);

#endif /* __USART3_H */

