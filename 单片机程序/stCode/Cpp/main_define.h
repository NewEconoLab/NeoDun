#ifndef MAIN_DEFINE_H
#define MAIN_DEFINE_H

#include <stdint.h>

#define printf_debug
#define HID_Delay

#define ADDR_XiaoYi 0x0101
#define ADDR_SIZE   40

typedef struct
{
		int Sign_Key_Flag;				//���󰴵ı�־λ
		int Sign_Key_left_Flag;		//�󰴼���ʶ
		int Sign_Key_center_Flag;	//�м䰴����ʶ
		int Sign_Key_right_Flag;	//�ұ߰�����ʶ
}SIGN_KEY_FLAG;


//���ñ�ʶ    ��ֻ�õ�������ɾ���ͱ��ݵ�ַ    δ�ñ�ʶĬ��Ϊ0��������ʶĬ��ֵΪ1
typedef struct
{
		uint32_t New_Device_Flag;						  //���豸��ʶ
		uint32_t Auto_Show_Flag;							//����Ǯ�����Զ�������������
		uint32_t Auto_Update_Flag;						//����ʱ�Զ�������
		uint32_t Add_Address_Flag;						//������ַ
		uint32_t Del_Address_Flag;						//ɾ����ַ
		uint32_t Backup_Address_Flag;				  //���ݵ�ַ
		uint32_t Backup_Address_Encrypt_Flag; //����Ǯ��ʱ���м��ܱ�ʶ
}SIGN_SET_FLAG;



#endif

