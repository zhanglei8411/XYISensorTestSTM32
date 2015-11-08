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
	/* 先占优先级 - 2bit, 从优先级 - 2bit */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	/* Enable the USARTy Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;	 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;	/* 先占优先级取值范围：0~3 */
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;			/* 从优先级取值范围：0~3 */
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

extern uint8_t LaserBuff[MAX_RECV_SIZE];
void USART3_DMA_Config(void)
{
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);											//使能DMA时钟
		DMA_InitStructure.DMA_PeripheralBaseAddr = USART3_DR_BASE;	 						//USART3 DR寄存器地址(外设基地址)  
		DMA_InitStructure.DMA_MemoryBaseAddr = (u8)LaserBuff;										//内存基地址		
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;											//外设作为数据的来源
// 		DMA_InitStructure.DMA_BufferSize = MAX_RECV_SIZE;												//DMA缓存大小
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; 				//外设地址寄存器不变
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;									//内存地址寄存器递增
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;	//外设数据宽度为8bit
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;	 				//内存数据宽度为8bit
		DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;	 												//循环缓冲模式
		DMA_InitStructure.DMA_Priority = DMA_Priority_High; 										//DMA通道x拥有高优先级
		DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;														//禁止内存到内存的传输	   
		DMA_Init(DMA1_Channel3, &DMA_InitStructure); 	   												//USART3_Rx - Channel3; USART3_Tx - Channel2
// 		DMA_Cmd (DMA1_Channel3, ENABLE);																					//DMA1 Channel5开启
		DMA_ITConfig(DMA1_Channel3, DMA_IT_TC,ENABLE);  													//配置DMA发送完成后产生中断
}

void USART3_DMA_Start(u16 size, u16 cmd_id)
{
	
	DMA_Cmd (DMA1_Channel3, DISABLE);																					//先关闭通道
	DMA_InitStructure.DMA_BufferSize = size;																	//通道开启后此项不可配置
	DMA_Init(DMA1_Channel3, &DMA_InitStructure); 
	DMA_Cmd (DMA1_Channel3, ENABLE);																					//DMA1 Channel5开启
	//启动DMA处理应当由外设发送一个请求信号到DMA控制器
	USART_DMACmd(USART3, USART_DMAReq_Rx, ENABLE);														//发送一个处理USART3接收的请求到DMA
}

