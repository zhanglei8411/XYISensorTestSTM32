#ifndef __LASER_HANDLE_H__
#define __LASER_HANDLE_H__

#include "stm32f10x.h"
#include "uart3.h"
#include "usart1.h"
#include "SysTick.h"


#define LASER_MAX_SIZE 32

#define DEFAULT_ADDRESS 0x80
#define DEFAULT_RANGE 0x05		/* 5m */
#define DEFAULT_FREQ 0x14			/* 20Hz */
#define DEFAULT_RESOLUTION 0x01

#define RESOLUTION_1MM	0x01
#define RESOLUTION_01MM	0x02

#define _SUCCESS 	0
#define _FAILURE 	1
#define _ERROT		2

#define CLEAR										0x0000
#define SINGLE_MEASURE 					0x0001
#define CIRCULAR_MEASURE			 	0x0002
#define SET_RANGE 		 					0x0004
#define SET_FREQ			 					0x0008
#define SET_RESOLUTION 					0x0010


typedef struct
{
	u8 address;
	u8 range;				/* Range: 05, 10, 30, 50, 80m(Dec) */
	u8 freq;				/* Freq: 05 10 20Hz - 连续测量频率 */
	u8 resolution;	/* Resolution: 1(1mm) 2(0.1mm) */
	float distance;
	u8 result;
	u16 cmd_id;
	u8 expect;
	u8 laserbuff[LASER_MAX_SIZE];
}Laser;

void laser_setup(void);
void laser_set_initial_value(void);
void laser_single_measure(void);
void after_single_measure(void);
static float mt_atof(u8 *p, u8 length);
void laser_set_range(u8 range);
void after_set_range(void);
void laser_set_freq(u8 freq);
void after_set_freq(void);
void laser_set_resolution(u8 resolution);
void after_set_resolution(void);

#endif
