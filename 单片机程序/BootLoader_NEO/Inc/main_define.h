#ifndef MAIN_DEFINE_H
#define MAIN_DEFINE_H

#include <stdint.h>


#define Debug_Print


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



#endif





