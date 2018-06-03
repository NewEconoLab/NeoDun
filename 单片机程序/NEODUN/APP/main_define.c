#include "main_define.h"
#include <string.h>

volatile 	uint32_t 	system_base_time = 0;
uint8_t						  SysFlagType = 0;//0��ʾ�������̣�1��ʾ�������������ص�����
KEY_FLAG 						Key_Flag;
SET_FLAG 						Set_Flag;
SYSTEM_NEODUN 			Neo_System;
PASSPORT_FLAG 			Passport_Flag;
DATA_HID_RECORD			HidData;
ADDRESS							showaddress[5];
COIN								coinrecord;
uint8_t		cur_address = 0;//��ʾ�û���ǰ���ڻ��߽�Ҫ�����ĵ�ַ����
volatile 	uint32_t 	moter_delay = 0;
volatile 	uint8_t 	task_1s_flag = 0;

volatile int hid_flag = 0;
uint8_t hid_data[64];
int len_hid = 0;

void Sys_Data_Init(void)
{
		Key_Flag.data 			= 0;										//���������־λ
		Set_Flag.data 			= 0xff;									//������ñ�־λ
		Passport_Flag.data 	= 0;										//��������ʶ
		memset(&Neo_System,0,sizeof(Neo_System));	//���ϵͳ��ʶ
		memset(&showaddress,0,sizeof(showaddress)); //������ٵ�ַ�ڴ�ռ�
		showaddress[0].name[6] = '\0';
		showaddress[1].name[6] = '\0';
		showaddress[2].name[6] = '\0';
		showaddress[3].name[6] = '\0';
		showaddress[4].name[6] = '\0';
		memset(&coinrecord,0,sizeof(COIN));					//��������Ϣ��¼
}


