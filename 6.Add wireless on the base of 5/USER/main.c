/******************** (C) COPYRIGHT 2012 WildFire Team **************************
 * 介绍		：参照DJI SDK中的解析方法监听与解析USART2数据，由USART1输出打印信息
 * 时间		：2015-11-6
 * 作者		：zhouzibo
**********************************************************************************/
#include "stm32f10x.h"
#include "usart1.h"
#include "usart2.h"
#include "uart3.h"
#include "string.h"
#include "M100_handle.h"
#include "Laser_handle.h"
#include "SysTick.h"
#include "Wireless_handle.h"

#define LED1(x)	x ? GPIO_SetBits(GPIOA,GPIO_Pin_4): GPIO_ResetBits(GPIOA,GPIO_Pin_4)
#define LED2(x)	x ? GPIO_SetBits(GPIOA,GPIO_Pin_5): GPIO_ResetBits(GPIOA,GPIO_Pin_5)
#define LED3(x)	x ? GPIO_SetBits(GPIOA,GPIO_Pin_6): GPIO_ResetBits(GPIOA,GPIO_Pin_6)
#define LED4(x)	x ? GPIO_SetBits(GPIOA,GPIO_Pin_7): GPIO_ResetBits(GPIOA,GPIO_Pin_7)


void Delay_ms(u16 nms)
{     
   u16 i=0;   
   while(nms--) 
   { 
      i=12000;  
      while(i--) ;     
   } 
} 

void GPIO_Configuration(void);


Stream m100_stream = {0};
extern Stream serial1_stream;
extern sdk_std_msg_t std_broadcast_data;
extern u8 laser_irq_flag;
extern Laser laser;
extern Wireless wireless;
int main(void)
{
	u16 length = 0;
	float a;	/* 4Bytes */
	double b;	/* 8Bytes */
	u8 buf[64];
	int i;
	char test[20] = {0};//"Hello world, this is a message sent from wireless serial port\n";
	char *ptest = test;

	/* 配置SysTick 为1ms中断一次 */
	SysTick_Init();	
	
	m100_setup();
	laser_setup();
	wireless_setup();
	
#if 0
	printf("time_stamp: %d\n", sizeof(int));
	printf("api_quaternion_data_t: %d\n", sizeof(api_quaternion_data_t));
	printf("api_common_data_t: %d\n", sizeof(api_common_data_t));
	printf("api_vel_data_t: %d\n", sizeof(api_vel_data_t));
	printf("api_pos_data_t: %d\n", sizeof(api_pos_data_t));
	printf("api_mag_data_t: %d\n", sizeof(api_mag_data_t));	
	printf("api_rc_data_t: %d\n", sizeof(api_rc_data_t));
	printf("api_ctrl_info_data_t: %d\n", sizeof(api_ctrl_info_data_t));
#endif

	while(1)
	{
		if(serial1_stream.recv_index > 0)
		{
			length = (serial1_stream.recv_index-serial1_stream.offset) > sizeof(buf) ? sizeof(buf) : (serial1_stream.recv_index-serial1_stream.offset);
			memcpy(buf, serial1_stream.comm_recv_buf + serial1_stream.offset, length);
			serial1_stream.offset += length;
			for(i=0; i<length; i++)
			{
					stream_store_data(&m100_stream, buf[i]);
					check_stream_state(&m100_stream);
			}
		}
		
		laser_single_measure();
		while(DMA_GetCurrDataCounter(DMA1_Channel3) != 0 || laser_irq_flag == 0);
		/* DMA_GetCurrDataCounter(DMA1_Channel3) == 0 and laser_irq_flag == 1 */
		if(laser.result == _SUCCESS)
		{
			/* ... */
// 			printf("%f\n", laser.distance);
			sprintf(test, "%f\n", laser.distance);
			wireless_send((u8 *)ptest, strlen((const char *)ptest));
		}
		
// 		wireless_send((u8 *)ptest, strlen((const char *)ptest));
// 		Systick_Delay(10);
	}
	// add your code here ^_^。
}
/******************* (C) COPYRIGHT 2012 WildFire Team *****END OF FILE************/


void GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
  
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA |RCC_APB2Periph_GPIOE | RCC_APB2Periph_GPIOF, ENABLE); 						 
	/**
	*  LED1 -> PA4 , LED2 -> PA5 , LED3 -> PA6 , LED4 -> PA7
	*/					 
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Key */
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOF, &GPIO_InitStructure);

}

