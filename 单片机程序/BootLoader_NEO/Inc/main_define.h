#ifndef MAIN_DEFINE_H
#define MAIN_DEFINE_H

#include <stdint.h>


#define Debug_Print

#define RECV_BIN_FILE_LEN		100*1024
#define Page_Per_Size				50
#define MAX_Page_Index			RECV_BIN_FILE_LEN/Page_Per_Size

typedef struct
{
		volatile int Key_Control_Flag;	//���󰴵ı�־λ
		volatile int Key_left_Flag;			//�󰴼���ʶ
		volatile int Key_center_Flag;	  //�м䰴����ʶ
		volatile int Key_right_Flag;		//�ұ߰�����ʶ
		volatile int Key_double_Flag;		//����˫��������ʶ
}SIGN_KEY_FLAG;


typedef struct
{
		unsigned char	new_wallet; 			//1��ʾ��Ǯ����0��ʾ��Ǯ�� 
		unsigned char	update;						//1��ʾ��Ҫ������0��ʾ����Ҫ����
		unsigned char	language;					//1��ʾӢ�ģ�0��ʾ����
}BOOT_FLAG;


typedef struct 
{
		unsigned int size;
		unsigned int packIndex;
		unsigned char hash_tran[32];
		unsigned short int reqSerial;
		unsigned short int notifySerial;
		
		unsigned char signature[64];
		unsigned char hash_actual[32];
}BIN_FILE_INFO;


#endif





