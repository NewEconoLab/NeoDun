#include "main_define.h"
#include <string.h>

BIN_FILE_INFO Bin_File;
SIGN_KEY_FLAG Key_Flag;
BOOT_SYS_FLAG BootFlag;
volatile uint8_t touch_motor_flag = 1;           //1��ʾ���ڽ��в���
uint8_t HID_RX_BUF[RECV_BIN_FILE_LEN] __attribute__ ((at(0X20001000)));//���ջ���,���USART_REC_LEN���ֽ�,��ʼ��ַΪ0X20001000.

//HID����
volatile uint32_t hid_index_read = 0;
volatile uint32_t hid_index_write = 0;
HID_RECV_DATA hid_recv_data[HID_QUEUE_DEPTH];

void DATA_Init(void)
{
		//��ճ�ʼ��
		BootFlag.data = 0;
		Key_Flag.data = 0;
		memset(&Bin_File,0,sizeof(BIN_FILE_INFO));
}


