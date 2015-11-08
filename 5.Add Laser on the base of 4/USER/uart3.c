#include "uart3.h"

DMA_InitTypeDef DMA_InitStructure;

void USART3_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	/* config USART3 clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	
	/* USART3 GPIO config */
	/* Configure USART3 Tx (PB.10) as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);    
	/* Configure USART3 Rx (PB.11) as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	/* USART3 mode config */
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART3, &USART_InitStructure);
// 	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	
	USART_Cmd(USART3, ENABLE);
}


void USART3_NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure; 
	/* Configure the NVIC Preemption Priority Bits */  
	/* ��ռ���ȼ� - 2bit, �����ȼ� - 2bit */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	/* Enable the USARTy Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;	 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;	/* ��ռ���ȼ�ȡֵ��Χ��0~3 */
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;			/* �����ȼ�ȡֵ��Χ��0~3 */
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

extern uint8_t LaserBuff[MAX_RECV_SIZE];
void USART3_DMA_Config(void)
{
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);											//ʹ��DMAʱ��
		DMA_InitStructure.DMA_PeripheralBaseAddr = USART3_DR_BASE;	 						//USART3 DR�Ĵ�����ַ(�������ַ)  
		DMA_InitStructure.DMA_MemoryBaseAddr = (u8)LaserBuff;										//�ڴ����ַ		
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;											//������Ϊ���ݵ���Դ
// 		DMA_InitStructure.DMA_BufferSize = MAX_RECV_SIZE;												//DMA�����С
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; 				//�����ַ�Ĵ�������
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;									//�ڴ��ַ�Ĵ�������
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;	//�������ݿ��Ϊ8bit
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;	 				//�ڴ����ݿ��Ϊ8bit
		DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;	 												//ѭ������ģʽ
		DMA_InitStructure.DMA_Priority = DMA_Priority_High; 										//DMAͨ��xӵ�и����ȼ�
		DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;														//��ֹ�ڴ浽�ڴ�Ĵ���	   
		DMA_Init(DMA1_Channel3, &DMA_InitStructure); 	   												//USART3_Rx - Channel3; USART3_Tx - Channel2
// 		DMA_Cmd (DMA1_Channel3, ENABLE);																					//DMA1 Channel5����
		DMA_ITConfig(DMA1_Channel3, DMA_IT_TC,ENABLE);  													//����DMA������ɺ�����ж�
}

void USART3_DMA_Start(u16 size, u16 cmd_id)
{
	
	DMA_Cmd (DMA1_Channel3, DISABLE);																					//�ȹر�ͨ��
	DMA_InitStructure.DMA_BufferSize = size;																	//ͨ����������������
	DMA_Init(DMA1_Channel3, &DMA_InitStructure); 
	DMA_Cmd (DMA1_Channel3, ENABLE);																					//DMA1 Channel5����
	//����DMA����Ӧ�������跢��һ�������źŵ�DMA������
	USART_DMACmd(USART3, USART_DMAReq_Rx, ENABLE);														//����һ������USART3���յ�����DMA
}

