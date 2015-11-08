#include "Wireless_handle.h"

Wireless wireless;


void wireless_setup(void)
{
	USART1_Config();
	USART1_DMA_Config();
// 	USART1_DMA_NVIC_Configuration();
	Hw_setup();
}

void Hw_setup(void)
{
// 	M0M1_GPIO_Init();
	AUX_GPIO_Init();
}

void M0M1_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	/* M0(不可悬空) PA6 推挽输出 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);    
	/* M1(不可悬空) PA7 推挽输出 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	/* 拉低 */
	GPIO_ResetBits(GPIOA , GPIO_Pin_6 | GPIO_Pin_7);
}

void AUX_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
// 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	/* AUX PA8 浮空输入 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);    
}


u8 wireless_send(u8 *pmes, u16 length)
{
	u16 i = 0;
	if(length > WL_MAX_SIZE)
		return TOO_LONG;
	
	if(!AUX)
		return WL_BUSY;
	
	for(i=0; i<length; i++)
	{
		wireless.wlbuff[i] = *(pmes+i);
	}

	if(DMA_GetCurrDataCounter(DMA1_Channel4) != 0)
		return DMA_ING;
	
	if(!AUX)
		return WL_BUSY;
	
	USART1_DMA_Start(length);
	return 0;
}

