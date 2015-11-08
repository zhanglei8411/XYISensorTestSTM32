/******************** (C) COPYRIGHT 2012 WildFire Team ***************************
 * 文件名  ：usart1.c
 * 描述    ：将printf函数重定向到USART1。这样就可以用printf函数将单片机的数据
 *           打印到PC上的超级终端或串口调试助手。         
 * 实验平台：野火STM32开发板
 * 硬件连接：------------------------
 *          | PA9  - USART1(Tx)      |
 *          | PA10 - USART1(Rx)      |
 *           ------------------------
 * 库版本  ：ST3.5.0
 * 作者    ：wildfire team 
 * 论坛    ：http://www.amobbs.com/forum-1008-1.html
 * 淘宝    ：http://firestm32.taobao.com
**********************************************************************************/	
#include "usart1.h"
#include "misc.h"

DMA_InitTypeDef USART1_DMA_InitStructure;
/*
 * 函数名：USART1_Config
 * 描述  ：USART1 GPIO 配置,工作模式配置
 * 输入  ：无
 * 输出  : 无
 * 调用  ：外部调用
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
 * 函数名：fputc
 * 描述  ：重定向c库函数printf到USART1
 * 输入  ：无
 * 输出  ：无
 * 调用  ：由printf调用
 */
int fputc(int ch, FILE *f)
{
	/* 将Printf内容发往串口 */
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
		while(!(USARTx->SR & 0x00000080));//bit7 TXE:发送数据寄存器为空标记
		USARTx->DR = (u16)*String & 0x01ff;
 		while(!(USARTx->SR & 0x00000040));//bit6 TC:发送完成标记
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
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);															//使能DMA时钟
		USART1_DMA_InitStructure.DMA_PeripheralBaseAddr = USART1_DR_BASE;	 							//USART1 DR寄存器地址(外设基地址)  
		USART1_DMA_InitStructure.DMA_MemoryBaseAddr = (u32)wireless.wlbuff;							//内存基地址		
		USART1_DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;												//外设作为数据的目的地
// 		USART1_DMA_InitStructure.DMA_BufferSize = 0;												//DMA缓存大小
		USART1_DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; 				//外设地址寄存器不变
		USART1_DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;									//内存地址寄存器递增
		USART1_DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;	//外设数据宽度为8bit
		USART1_DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;	 				//内存数据宽度为8bit
		USART1_DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;	 													//普通模式，不循环
		USART1_DMA_InitStructure.DMA_Priority = DMA_Priority_High; 											//DMA通道x拥有高优先级
		USART1_DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;															//禁止内存到内存的传输	   
		DMA_Init(DMA1_Channel4, &USART1_DMA_InitStructure); 	   												//USART1_Tx - DMA1_Channel4
// 		DMA_ITConfig(DMA1_Channel4, DMA_IT_TC,ENABLE);  																//配置DMA发送完成后产生中断
}

void USART1_DMA_Start(u16 size)
{
	DMA_Cmd (DMA1_Channel4, DISABLE);																					//先关闭通道
	USART1_DMA_InitStructure.DMA_BufferSize = size;														//设置数据传输数量，若通道开启此项不可配置
	DMA_Init(DMA1_Channel4, &USART1_DMA_InitStructure); 
	DMA_Cmd (DMA1_Channel4, ENABLE);																					//DMA1 Channel4开启
	//启动DMA处理应当由外设发送一个请求信号到DMA控制器
	USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);														//发送一个处理USART1发送的请求到DMA
}

void USART1_DMA_NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure; 
	/* Configure the NVIC Preemption Priority Bits */  
	/* 先占优先级 - 2bit, 从优先级 - 2bit */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	/* Enable the USARTy Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_IRQn;	 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;			/* 先占优先级取值范围：0~3 */
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;						/* 从优先级取值范围：0~3 */
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}



/******************* (C) COPYRIGHT 2012 WildFire Team *****END OF FILE************/


