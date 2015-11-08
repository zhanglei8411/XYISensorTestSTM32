#ifndef __WIRELESS_HANDLE_H__
#define __WIRELESS_HANDLE_H__

#include "stm32f10x.h"
#include "usart1.h"
#include "SysTick.h"

#define AUX GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_8)

#define WL_MAX_SIZE 1024

#define WL_BUSY			1
#define DMA_ING			2
#define TOO_LONG		3

typedef struct
{
	u8 wlbuff[WL_MAX_SIZE];
}Wireless;


void wireless_setup(void);
void Hw_setup(void);
void M0M1_GPIO_Init(void);
void AUX_GPIO_Init(void);
u8 wireless_send(u8 *pmes, u16 length);

#endif

