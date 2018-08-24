#include "main_define.h"
#include <string.h>

BIN_FILE_INFO Bin_File;
SIGN_KEY_FLAG Key_Flag;
BOOT_SYS_FLAG BootFlag;
volatile uint8_t touch_motor_flag = 1;           //1表示正在进行测试
uint8_t HID_RX_BUF[RECV_BIN_FILE_LEN] __attribute__ ((at(0X20001000)));//接收缓冲,最大USART_REC_LEN个字节,起始地址为0X20001000.

//HID数据
volatile uint32_t hid_index_read = 0;
volatile uint32_t hid_index_write = 0;
HID_RECV_DATA hid_recv_data[HID_QUEUE_DEPTH];

void DATA_Init(void)
{
		//清空初始化
		BootFlag.data = 0;
		Key_Flag.data = 0;
		memset(&Bin_File,0,sizeof(BIN_FILE_INFO));
}


