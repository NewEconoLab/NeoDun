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






#endif





