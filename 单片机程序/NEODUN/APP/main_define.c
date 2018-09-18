#include "main_define.h"

volatile 	uint32_t 	system_base_time = 0;
uint8_t						  SysFlagType = 0;//0��ʾ�������̣�1��ʾ�������������ص�����
KEY_FLAG 						Key_Flag;
SET_FLAG 						Set_Flag;
SYSTEM_NEODUN 			Neo_System;
PASSPORT_FLAG 			Passport_Flag;
TIME_CONTROL				Time_control;
DATA_HID_RECORD			HidData;
ADDRESS							showaddress[5];
COIN								coinrecord;
SECURE_PIPE					secure_pipe;
uint8_t		cur_address = 0;//��ʾ�û���ǰ���ڻ��߽�Ҫ�����ĵ�ַ����
volatile 	uint32_t 	moter_delay = 0;
volatile 	uint8_t 	task_1s_flag = 0;

//HID����
volatile uint32_t hid_index_read = 0;
volatile uint32_t hid_index_write = 0;
HID_RECV_DATA hid_recv_data[HID_QUEUE_DEPTH];

void Sys_Data_Init(void)
{
		Key_Flag.data 			= 0;										//���������־λ
		Set_Flag.data 			= 0xff;									//������ñ�־λ
		Passport_Flag.data 	= 0;										//��������ʶ
		memset(&Neo_System,0,sizeof(Neo_System));		//���ϵͳ��ʶ
		memset(&showaddress,0,sizeof(showaddress)); //������ٵ�ַ�ڴ�ռ�
		memset(&coinrecord,0xFFFF,sizeof(COIN));		//��������Ϣ��¼
		coinrecord.count = 0;
}


