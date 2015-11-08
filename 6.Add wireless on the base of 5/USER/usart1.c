/******************** (C) COPYRIGHT 2012 WildFire Team ***************************
 * �ļ���  ��usart1.c
 * ����    ����printf�����ض���USART1�������Ϳ�����printf��������Ƭ��������
 *           ��ӡ��PC�ϵĳ����ն˻򴮿ڵ������֡�         
 * ʵ��ƽ̨��Ұ��STM32������
 * Ӳ�����ӣ�------------------------
 *          | PA9  - USART1(Tx)      |
 *          | PA10 - USART1(Rx)      |
 *           ------------------------
 * ��汾  ��ST3.5.0
 * ����    ��wildfire team 
 * ��̳    ��http://www.amobbs.com/forum-1008-1.html
 * �Ա�    ��http://firestm32.taobao.com
**********************************************************************************/	
#include "usart1.h"
#include "misc.h"

DMA_InitTypeDef USART1_DMA_InitStructure;
/*
 * ��������USART1_Config
 * ����  ��USART1 GPIO ����,����ģʽ����
 * ����  ����
 * ���  : ��
 * ����  ���ⲿ����
 */
void USART1_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	/* config USART1 clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);
	
	/* USART1 GPIO config */
	/* Configure USART1 Tx (PA.09) as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);    
	/* Configure USART1 Rx (PA.10) as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/* USART1 mode config */
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	
	USART_Cmd(USART1, ENABLE);
}


void USART1_NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure; 
	/* Configure the NVIC Preemption Priority Bits */  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	
	/* Enable the USARTy Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;	 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}



/*
 * ��������fputc
 * ����  ���ض���c�⺯��printf��USART1
 * ����  ����
 * ���  ����
 * ����  ����printf����
 */
int fputc(int ch, FILE *f)
{
	/* ��Printf���ݷ������� */
	USART_SendData(USART1, (unsigned char) ch);
	while (!(USART1->SR & USART_FLAG_TXE));
	
	return (ch);
}


void USART_SendData_By_ZZB(USART_TypeDef* USARTx, u16 Data)
{
	/* Check the parameters */
	assert_param(IS_USART_ALL_PERIPH(USARTx));
	assert_param(IS_USART_DATA(Data)); 
    
	/* Transmit Data */
	while(!(USARTx->SR & 0x00000080));
	USARTx->DR = (Data & (u16)0x01FF);
	while(!(USARTx->SR & 0x00000040));
}

void USART_SendString_By_ZZB(USART_TypeDef* USARTx, u8* String)
{	
	assert_param(IS_USART_ALL_PERIPH(USARTx));
	
	while(*String != '\0')
	{
		while(!(USARTx->SR & 0x00000080));//bit7 TXE:�������ݼĴ���Ϊ�ձ��
		USARTx->DR = (u16)*String & 0x01ff;
 		while(!(USARTx->SR & 0x00000040));//bit6 TC:������ɱ��
		String++;
	}
}

void USART_SendString_With_Length(USART_TypeDef* USARTx, u8* String, u16 Length)
{
	int i;
	USART_GetFlagStatus(USART1, USART_FLAG_TC);
	for(i=0; i<Length; i++)	
	{
		 USARTx->DR = (u16)*String & (uint16_t)0x01FF;	 
		 while(USART_GetFlagStatus(USART1, USART_FLAG_TC) != SET);
	}
}

extern Wireless wireless;
void USART1_DMA_Config(void)
{
		/* USART1 - DMA1
	   * USART1_Rx - DMA1_Channel5
		 * USART1_Tx - DMA1_Channel4
		 */
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);															//ʹ��DMAʱ��
		USART1_DMA_InitStructure.DMA_PeripheralBaseAddr = USART1_DR_BASE;	 							//USART1 DR�Ĵ�����ַ(�������ַ)  
		USART1_DMA_InitStructure.DMA_MemoryBaseAddr = (u32)wireless.wlbuff;							//�ڴ����ַ		
		USART1_DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;												//������Ϊ���ݵ�Ŀ�ĵ�
// 		USART1_DMA_InitStructure.DMA_BufferSize = 0;												//DMA�����С
		USART1_DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; 				//�����ַ�Ĵ�������
		USART1_DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;									//�ڴ��ַ�Ĵ�������
		USART1_DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;	//�������ݿ��Ϊ8bit
		USART1_DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;	 				//�ڴ����ݿ��Ϊ8bit
		USART1_DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;	 													//��ͨģʽ����ѭ��
		USART1_DMA_InitStructure.DMA_Priority = DMA_Priority_High; 											//DMAͨ��xӵ�и����ȼ�
		USART1_DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;															//��ֹ�ڴ浽�ڴ�Ĵ���	   
		DMA_Init(DMA1_Channel4, &USART1_DMA_InitStructure); 	   												//USART1_Tx - DMA1_Channel4
// 		DMA_ITConfig(DMA1_Channel4, DMA_IT_TC,ENABLE);  																//����DMA������ɺ�����ж�
}

void USART1_DMA_Start(u16 size)
{
	DMA_Cmd (DMA1_Channel4, DISABLE);																					//�ȹر�ͨ��
	USART1_DMA_InitStructure.DMA_BufferSize = size;														//�������ݴ�����������ͨ���������������
	DMA_Init(DMA1_Channel4, &USART1_DMA_InitStructure); 
	DMA_Cmd (DMA1_Channel4, ENABLE);																					//DMA1 Channel4����
	//����DMA����Ӧ�������跢��һ�������źŵ�DMA������
	USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);														//����һ������USART1���͵�����DMA
}

void USART1_DMA_NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure; 
	/* Configure the NVIC Preemption Priority Bits */  
	/* ��ռ���ȼ� - 2bit, �����ȼ� - 2bit */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	/* Enable the USARTy Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_IRQn;	 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;			/* ��ռ���ȼ�ȡֵ��Χ��0~3 */
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;						/* �����ȼ�ȡֵ��Χ��0~3 */
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}



/******************* (C) COPYRIGHT 2012 WildFire Team *****END OF FILE************/


