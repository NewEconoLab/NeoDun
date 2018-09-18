#include "main_define.h"

volatile 	uint32_t 	system_base_time = 0;
uint8_t						  SysFlagType = 0;//0表示正常流程，1表示从其它币种跳回的流程
KEY_FLAG 						Key_Flag;
SET_FLAG 						Set_Flag;
SYSTEM_NEODUN 			Neo_System;
PASSPORT_FLAG 			Passport_Flag;
TIME_CONTROL				Time_control;
DATA_HID_RECORD			HidData;
ADDRESS							showaddress[5];
COIN								coinrecord;
SECURE_PIPE					secure_pipe;
uint8_t		cur_address = 0;//表示用户当前正在或者将要操作的地址索引
volatile 	uint32_t 	moter_delay = 0;
volatile 	uint8_t 	task_1s_flag = 0;

//HID数据
volatile uint32_t hid_index_read = 0;
volatile uint32_t hid_index_write = 0;
HID_RECV_DATA hid_recv_data[HID_QUEUE_DEPTH];

void Sys_Data_Init(void)
{
		Key_Flag.data 			= 0;										//清除按键标志位
		Set_Flag.data 			= 0xff;									//清除设置标志位
		Passport_Flag.data 	= 0;										//清除密码标识
		memset(&Neo_System,0,sizeof(Neo_System));		//清除系统标识
		memset(&showaddress,0,sizeof(showaddress)); //清除开辟地址内存空间
		memset(&coinrecord,0xFFFF,sizeof(COIN));		//清除插件信息记录
		coinrecord.count = 0;
}


