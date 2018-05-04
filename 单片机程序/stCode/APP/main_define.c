#include "main_define.h"
#include <string.h>

KEY_FLAG 						Key_Flag;
SET_FLAG 						Set_Flag;
SYSTEM_FLAG 				System_Flag;
PASSPORT_FLAG 			Passport_Flag;
DATA_HID_RECORD			HidData;
ADDRESS							showaddress[5];
uint8_t		cur_address = 0;//表示用户当前正在或者将要操作的地址索引
volatile 	uint32_t 	moter_delay = 0;
volatile 	uint8_t 	task_1s_flag = 0;

volatile int hid_flag = 0;
uint8_t hid_data[64];
int len_hid = 0;

void Sys_Data_Init(void)
{
		Key_Flag.data 			= 0;										//清除按键标志位
		Set_Flag.data 			= 0xff;									//清除设置标志位		
		Passport_Flag.data 	= 0;										//清除密码标识
		memset(&System_Flag,0,sizeof(System_Flag));	//清除系统标识
		memset(&showaddress,0,sizeof(showaddress)); //清除开辟地址内存空间
}


