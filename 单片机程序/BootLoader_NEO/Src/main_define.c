#include "main_define.h"
#include <string.h>

BIN_FILE_INFO Bin_File;
SIGN_KEY_FLAG Key_Flag;
BOOT_SYS_FLAG BootFlag;
volatile uint8_t touch_motor_flag = 1;           //1��ʾ���ڽ��в���
//HID���ջ�������
uint8_t HID_RX_BUF[RECV_BIN_FILE_LEN] __attribute__ ((at(0X20001000)));//���ջ���,���USART_REC_LEN���ֽ�,��ʼ��ַΪ0X20001000.


void DATA_Init(void)
{
		memset(&BootFlag,0,sizeof(BOOT_SYS_FLAG));
		memset(&Key_Flag,0,sizeof(SIGN_KEY_FLAG));//��հ�����־λ������������Ч
		memset(&Bin_File,0,sizeof(BIN_FILE_INFO));
}


