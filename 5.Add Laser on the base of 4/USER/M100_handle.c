#include "M100_handle.h"

		
const u8 hex_table[23] = {0,1,2,3,4,5,6,7,8,9,
													0,0,0,0,0,0,0,
													10,11,12,13,14,15};

sdk_std_msg_t std_broadcast_data;


void stream_store_data(Stream* p_filter, unsigned char in_data)
{
	if (p_filter->recv_index < MAX_RECV_SIZE)
	{
		p_filter->comm_recv_buf[p_filter->recv_index] = in_data;
		p_filter->recv_index++;
	}
	else
	{
		// Error, buffer overflow! Just clear and continue.
		memset(p_filter->comm_recv_buf, 0, p_filter->recv_index);
		p_filter->recv_index = 0;
	}
}

void check_stream_state(Stream* p_filter)
{
	M100Header* p_head = (M100Header*)(p_filter->comm_recv_buf);
	if (p_filter->recv_index < sizeof(M100Header))
	{
		// Continue receive data, nothing to do

	}
	else if (p_filter->recv_index == sizeof(M100Header))
	{
		// recv a full-head
		stream_verify_head(p_filter);
	}
	else if (p_filter->recv_index == p_head->length)
	{
		stream_verify_data(p_filter);
	}
}

void stream_verify_head(Stream* p_filter)
{
	M100Header* p_head = (M100Header*)(p_filter->comm_recv_buf);

	if ((p_head->sof == 0xAA) &&
		(p_head->version == 0) &&
		(p_head->length <= MAX_RECV_SIZE) &&
		(p_head->reversed0 == 0) &&
		(p_head->reversed1 == 0)
		)
	{
		// check if this head is a ack or simple package
		if (p_head->length == sizeof(M100Header))
		{
			call_data_app(p_filter);
		}
	}
	else
	{
		stream_shift_data(p_filter);
	}
}

void stream_verify_data(Stream* p_filter)
{
	call_data_app(p_filter);
}

void stream_shift_data(Stream* p_filter)
{
	if (p_filter->recv_index)
	{
		p_filter->recv_index--;
		if (p_filter->recv_index)
		{
			memmove(p_filter->comm_recv_buf, p_filter->comm_recv_buf + 1, p_filter->recv_index);
		}
	}
}

void call_data_app(Stream* p_filter)
{
	// pass current data to handler
	M100Header *p_head = (M100Header*)p_filter->comm_recv_buf;

	DJI_Pro_Parse_Broadcast_Data(p_head);

	stream_prepare(p_filter, p_head);
}


static void DJI_Pro_Parse_Broadcast_Data(M100Header *header)
{
    unsigned char *pdata = (unsigned char *)&(header->head_crc);
    unsigned short *msg_enable_flag;
    unsigned short data_len = 2;
    pdata += 2;	//定位到status bits起始地址
    msg_enable_flag = (unsigned short *)pdata;
	
		PARSE_STD_MSG( *msg_enable_flag, ENABLE_MSG_TIME	,std_broadcast_data.time_stamp      , pdata, data_len);
    PARSE_STD_MSG( *msg_enable_flag, ENABLE_MSG_Q		,std_broadcast_data.q				, pdata, data_len);
    PARSE_STD_MSG( *msg_enable_flag, ENABLE_MSG_A		,std_broadcast_data.a				, pdata, data_len);
    PARSE_STD_MSG( *msg_enable_flag, ENABLE_MSG_V		,std_broadcast_data.v				, pdata, data_len);
    PARSE_STD_MSG( *msg_enable_flag, ENABLE_MSG_W		,std_broadcast_data.w				, pdata, data_len);
    PARSE_STD_MSG( *msg_enable_flag, ENABLE_MSG_POS     ,std_broadcast_data.pos				, pdata, data_len);
    PARSE_STD_MSG( *msg_enable_flag, ENABLE_MSG_MAG     ,std_broadcast_data.mag				, pdata, data_len);
    PARSE_STD_MSG( *msg_enable_flag, ENABLE_MSG_RC		,std_broadcast_data.rc				, pdata, data_len);
    PARSE_STD_MSG( *msg_enable_flag, ENABLE_MSG_GIMBAL	,std_broadcast_data.gimbal			, pdata, data_len);
    PARSE_STD_MSG( *msg_enable_flag, ENABLE_MSG_STATUS	,std_broadcast_data.status			, pdata, data_len);
    PARSE_STD_MSG( *msg_enable_flag, ENABLE_MSG_BATTERY ,std_broadcast_data.battery_remaining_capacity	, pdata, data_len);
    PARSE_STD_MSG( *msg_enable_flag, ENABLE_MSG_DEVICE	,std_broadcast_data.ctrl_info			, pdata, data_len);
	
		Print_Param();

}

void stream_prepare(Stream* p_filter, M100Header* p_head)
{
	unsigned int bytes_to_move = sizeof(M100Header) - 1;
	unsigned int index_of_move = p_filter->recv_index - bytes_to_move;

	memmove(p_filter->comm_recv_buf, p_filter->comm_recv_buf + index_of_move, bytes_to_move);
	memset(p_filter->comm_recv_buf + bytes_to_move, 0, index_of_move);
	p_filter->recv_index = bytes_to_move;
}

int Hex_To_Dec(u8 *psrc, u8 *pdst)
{
	int i = 0,j = 0;
	u8 high = 0, low = 0;

	if(sizeof(psrc)/2 > sizeof(pdst) )
	{
		return -1;
	}
	
	while(*(psrc+i) != '\0')
	{
		high = hex_table[*(psrc+i++) - 48];
		low = hex_table[*(psrc+i++) - 48];
		*(pdst+j++) = (u8)(uint16_t)(high<<4 | low);
	}
	//*(pdst+j) = '\0';
	return 0;
}

extern Stream m100_stream;
void Print_Param(void)	//(protocol_frame_without_data fr)
{
	printf("\r\n");
	printf("-----------   < Parameter list >  ----------\r\n\r\n");
	printf("\r\n");
	printf(" time stamp is %d\n", std_broadcast_data.time_stamp);
	printf(" q0: %f, q1: %f, q2: %f, q3: %f\n", std_broadcast_data.q.q0, 
																							std_broadcast_data.q.q1,
																							std_broadcast_data.q.q2,
																							std_broadcast_data.q.q3);
	
	printf(" agx: %f, agy: %f, agz: %f\n", std_broadcast_data.a.x, 
																				 std_broadcast_data.a.y,
																				 std_broadcast_data.a.z);
	
	printf(" vgx: %f, vgy: %f, vgz: %f\n", std_broadcast_data.v.x, 
																				 std_broadcast_data.v.y,
																				 std_broadcast_data.v.z);
	printf(" wx: %f, wy: %f, wz: %f\n", std_broadcast_data.w.x,
																			std_broadcast_data.w.y,
																			std_broadcast_data.w.z);
	
	printf(" longti: %lf, lati: %lf, alti: %lf, height: %lf\n", std_broadcast_data.pos.longti,
																														std_broadcast_data.pos.lati, 
																														std_broadcast_data.pos.alti,
																														std_broadcast_data.pos.height);
																														
	printf(" mx: %d, my: %d, mz: %d\n", std_broadcast_data.mag.x,
																			std_broadcast_data.mag.y, 
																			std_broadcast_data.mag.z);

	printf(" roll: %d, pitch: %d, yaw: %d, throttle: %d, mode: %d, gear: %d\n", std_broadcast_data.rc.roll,
																																							std_broadcast_data.rc.pitch, 
																																							std_broadcast_data.rc.yaw, 
																																							std_broadcast_data.rc.throttle, 
																																							std_broadcast_data.rc.mode, 
																																							std_broadcast_data.rc.gear);
																																							
	printf(" roll: %f, pitch: %f, yaw: %f\n", std_broadcast_data.gimbal.x, 
																						std_broadcast_data.gimbal.y, 
																						std_broadcast_data.gimbal.z);
																						
	printf(" flight_status: %d\n", std_broadcast_data.status);
	
	printf(" remaining batter power: %d\n", std_broadcast_data.battery_remaining_capacity);
	printf(" control_device: %d\n", std_broadcast_data.ctrl_info.cur_ctrl_dev_in_navi_mode);
	
	printf("--------------------------------------------\r\n");
	
}

