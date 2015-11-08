#include "Laser_handle.h"

uint8_t LaserBuff[MAX_RECV_SIZE];

void laser_setup(void)
{
	USART3_Config();
	USART3_NVIC_Configuration();
	USART3_DMA_Config();
}


u8 cal_cs(u8 *head, u8 length)
{
	u8 *p = head;
	u8 i = 0;
	u16 cs_sum16 = 0;
	u8 cs_sum8 = 0;
	
	for(i=0; i<length; i++)
	{
		cs_sum16 += *(p+i);
	}
	cs_sum8 = (u8)(cs_sum16 & 0x00FF);
	cs_sum8 = 0xFF - cs_sum8;		//È¡·´
	cs_sum8++;									//¼Ó1
	
	return cs_sum8;
}

void single_measure(u8 address)
{
	u8 cmd[4];
	u8 i = 0;
	
	cmd[0] = address;
	cmd[1] = 0x06;
	cmd[2] = 0x02;
	cmd[3] = cal_cs(cmd, 3);
	
	for(i=0; i<4; i++)
	{
		USART_SendData(USART3, cmd[i]);
	}
	USART3_DMA_Start(11, SINGLE_MEASURE);		/* ADDR 06 82 3X 3X 3X 2E 3X 3X 3X CS - 11bytes */
}



// int set_range()
// {
// 	
// }

