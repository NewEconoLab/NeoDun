#include "main_define.h"
#include <string.h>

BIN_FILE_INFO Bin_File;
SIGN_KEY_FLAG Key_Flag;
BOOT_SYS_FLAG BootFlag;
volatile uint8_t touch_motor_flag = 1;           //1表示正在进行测试
//HID接收缓冲设置
uint8_t HID_RX_BUF[RECV_BIN_FILE_LEN] __attribute__ ((at(0X20001000)));//接收缓冲,最大USART_REC_LEN个字节,起始地址为0X20001000.


void DATA_Init(void)
{
		memset(&BootFlag,0,sizeof(BOOT_SYS_FLAG));
		memset(&Key_Flag,0,sizeof(SIGN_KEY_FLAG));//清空按键标志位，开启按键有效
		memset(&Bin_File,0,sizeof(BIN_FILE_INFO));
}


