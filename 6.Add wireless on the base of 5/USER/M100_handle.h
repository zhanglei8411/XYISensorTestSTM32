#ifndef __M100_HANDLE_H__
#define __M100_HANDLE_H__

#include "stm32f10x.h"
#include <stdio.h>
#include <string.h>
#include "SysTick.h"
#include "usart2.h"


#define PARSE_STD_MSG(_flag, _enable, _data, _buf, _datalen)\
    if((_flag & _enable))\
    {\
        memcpy((unsigned char *)&(_data),(unsigned char *)(_buf)+(_datalen), sizeof(_data));\
        _datalen += sizeof(_data);\
    }
		
#define MAX_RECV_SIZE (1024)


#define ENABLE_MSG_TIME			0x0001
#define ENABLE_MSG_Q			0x0002
#define ENABLE_MSG_A			0x0004
#define ENABLE_MSG_V			0x0008
#define ENABLE_MSG_W			0x0010
#define ENABLE_MSG_POS			0x0020
#define ENABLE_MSG_MAG			0x0040
#define ENABLE_MSG_RC			0x0080
#define ENABLE_MSG_GIMBAL		0x0100
#define ENABLE_MSG_STATUS		0x0200
#define ENABLE_MSG_BATTERY		0x0400
#define ENABLE_MSG_DEVICE		0x0800

typedef struct 
{
	volatile uint16_t recv_index;
	uint16_t offset;
	u8 comm_recv_buf[MAX_RECV_SIZE];
}Stream;

typedef struct
{
	unsigned int sof : 8; // 1byte

	unsigned int length : 10;
	unsigned int version : 6; // 2byte
	unsigned int session_id : 5;
	unsigned int is_ack : 1;
	unsigned int reversed0 : 2; // always 0

	unsigned int padding : 5;
	unsigned int enc_type : 3;
	unsigned int reversed1 : 24;

	unsigned int sequence_number : 16;
	unsigned int head_crc;
// 	unsigned int magic;
}M100Header;


#pragma  pack(1)	//使变量以1字节对齐, 如果不加此对齐指令，api_vel_data_t的大小为16而不是13


typedef struct
{
    float q0;
    float q1;
    float q2;
    float q3;
}api_quaternion_data_t;

typedef struct
{
    float x;
    float y;
    float z;
}api_common_data_t;


typedef struct
{
    float x;
    float y;
    float z;
    unsigned char health_flag         :1;
    unsigned char feedback_sensor_id  :4;
    unsigned char reserve             :3;
}api_vel_data_t;

typedef struct
{
		double longti;
    double lati;
    float alti;
    float height;
    unsigned char health_flag;
}api_pos_data_t;

typedef struct
{
    signed short roll;
    signed short pitch;
    signed short yaw;
    signed short throttle;
    signed short mode;
    signed short gear;
}api_rc_data_t;

typedef struct
{
    signed short x;
    signed short y;
    signed short z;
}api_mag_data_t;

typedef struct
{
    unsigned char cur_ctrl_dev_in_navi_mode   :3;/*0->rc  1->app  2->serial*/
    unsigned char serial_req_status           :1;/*1->opensd  0->close*/
    unsigned char reserved                    :4;
}api_ctrl_info_data_t;

typedef struct
{
    volatile unsigned int time_stamp;
    volatile api_quaternion_data_t q;
    volatile api_common_data_t a;
    volatile api_vel_data_t v;
    volatile api_common_data_t w;
    volatile api_pos_data_t pos;
    volatile api_mag_data_t mag;
    volatile api_rc_data_t rc;
    volatile api_common_data_t gimbal;
    volatile unsigned char status;
    volatile unsigned char battery_remaining_capacity;
    volatile api_ctrl_info_data_t ctrl_info;
    volatile uint8_t obtained_control;
		volatile uint8_t activation;
}sdk_std_msg_t;

#pragma  pack()		//取消自定义字节对齐方式

void stream_store_data(Stream* p_filter, unsigned char in_data);
void check_stream_state(Stream* p_filter);
void stream_verify_head(Stream* p_filter);
void stream_verify_data(Stream* p_filter);
void stream_shift_data(Stream* p_filter);
void call_data_app(Stream* p_filter);
static void DJI_Pro_Parse_Broadcast_Data(M100Header *header);
void stream_prepare(Stream* p_filter, M100Header* p_head);
int Hex_To_Dec(u8 *psrc, u8 *pdst);
void Print_Param(void);
void m100_setup(void);


#endif
