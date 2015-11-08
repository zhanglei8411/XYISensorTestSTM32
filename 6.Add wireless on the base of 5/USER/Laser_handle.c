#include "Laser_handle.h"

const float atof_table[8] = {100, 10, 1, 0, 0.1, 0.01, 0.001, 0.0001};

Laser laser;

void laser_setup(void)
{
	USART3_Config();
// 	USART3_NVIC_Configuration();
	USART3_DMA_Config();
	USART3_DMA_NVIC_Configuration();	//使能DMA IRQn通道
	laser_set_initial_value();
}

void laser_set_initial_value(void)
{
	laser.address = DEFAULT_ADDRESS;
	laser.range = DEFAULT_RANGE;
	laser.freq = DEFAULT_FREQ;
	laser.resolution = DEFAULT_RESOLUTION;
	laser.distance = 0;
	laser.result = _FAILURE;
	laser.cmd_id = CLEAR;
	laser.expect = 0;
}

static u8 cal_cs(u8 *head, u8 length)
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
	cs_sum8 = 0xFF - cs_sum8;		//取反
	cs_sum8++;									//加1
	
	return cs_sum8;
}

void laser_single_measure(void)
{
	u8 cmd[4];
	u8 i = 0, size_t = 0;
	
	cmd[0] = laser.address;
	cmd[1] = 0x06;
	cmd[2] = 0x02;
	cmd[3] = cal_cs(cmd, 3);
	
	for(i=0; i<4; i++)
	{
		USART_SendData_By_ZZB(USART3, cmd[i]);
	}
	
	/* 1mm: ADDR 06 82 3X 3X 3X 2E 3X 3X 3X CS - 11bytes
   * 0.1mm: ADDR 06 82 3X 3X 3X 2E 3X 3X 3X 3X CS - 12bytes
   */
	if(laser.resolution == RESOLUTION_1MM)
	{
		size_t = 11;
	}
	else
	{
		size_t = 12;
	}
	USART3_DMA_Start(size_t);		
	laser.cmd_id = SINGLE_MEASURE;
}

void after_single_measure(void)
{
	u8 len;
	if(laser.laserbuff[0] != laser.address)
	{
		laser.result = _FAILURE;
		return;
	}
	if(laser.laserbuff[3] == 'E')
	{
		laser.result = _FAILURE;
		return;
	}
	
	if(laser.resolution == RESOLUTION_1MM)
	{
		len = 7;
	}
	else
	{
		len = 8;
	}
	
	laser.distance = mt_atof(&(laser.laserbuff[3]), len);
	laser.result = _SUCCESS;
	laser.cmd_id = CLEAR;
	
}

static float mt_atof(u8 *p, u8 length)
{
	u8 i;
	float ret;
	
	if(length > 8)
		return 0;
	
	ret = 0;
	for(i=0; i<3; i++)
	{
		ret += (*(p+i) - 0x30) * atof_table[i];
	}
	for(i=4; i<length; i++)
	{
		ret += (*(p+i) - 0x30) * atof_table[i];
	}
		
	return ret;
}

void laser_set_range(u8 range)
{
	u8 cmd[5];
	u8 i = 0;
	
	cmd[0] = 0xFA;
	cmd[1] = 0x04;
	cmd[2] = 0x09;
	cmd[3] = range;
	cmd[4] = cal_cs(cmd, 4);
	
	laser.expect = range;		//用期望保存要设置的值
	
	for(i=0; i<5; i++)
	{
		USART_SendData(USART3, cmd[i]);
	}

	USART3_DMA_Start(4);		
	laser.cmd_id = SET_RANGE;
}

void after_set_range(void)
{
	if(laser.laserbuff[0] != 0xFA)
	{
		laser.result = _FAILURE;
		return;
	}
	if(laser.laserbuff[1] != 0x04)
	{
		laser.result = _FAILURE;
		return;
	}
	
	laser.range = laser.expect;
	laser.result = _SUCCESS;
	laser.cmd_id = CLEAR;
	
}

void laser_set_freq(u8 freq)
{
	u8 cmd[5];
	u8 i = 0;
	
	cmd[0] = 0xFA;
	cmd[1] = 0x04;
	cmd[2] = 0x0A;
	cmd[3] = freq;
	cmd[4] = cal_cs(cmd, 4);
	
	laser.expect = freq;		//用期望保存要设置的值
	
	for(i=0; i<5; i++)
	{
		USART_SendData(USART3, cmd[i]);
	}

	USART3_DMA_Start(4);		
	laser.cmd_id = SET_FREQ;
}

void after_set_freq(void)
{
	if(laser.laserbuff[0] != 0xFA)
	{
		laser.result = _FAILURE;
		return;
	}
	if(laser.laserbuff[1] != 0x04)
	{
		laser.result = _FAILURE;
		return;
	}
	
	laser.freq = laser.expect;
	laser.result = _SUCCESS;
	laser.cmd_id = CLEAR;
}

void laser_set_resolution(u8 resolution)
{
	u8 cmd[5];
	u8 i = 0;
	
	cmd[0] = 0xFA;
	cmd[1] = 0x04;
	cmd[2] = 0x0C;
	cmd[3] = resolution;
	cmd[4] = cal_cs(cmd, 4);
	
	laser.expect = resolution;		//用期望保存要设置的值
	
	for(i=0; i<5; i++)
	{
		USART_SendData(USART3, cmd[i]);
	}

	USART3_DMA_Start(4);		
	laser.cmd_id = SET_RESOLUTION;
}

void after_set_resolution(void)
{
	if(laser.laserbuff[0] != 0xFA)
	{
		laser.result = _FAILURE;
		return;
	}
	if(laser.laserbuff[1] != 0x04)
	{
		laser.result = _FAILURE;
		return;
	}
	
	laser.resolution = laser.expect;
	laser.result = _SUCCESS;
	laser.cmd_id = CLEAR;
}
