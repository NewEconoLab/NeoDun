/*------------------------------------------------File Info---------------------------------------------
** File Name: myMain.cpp
** Last modified Date:  2017-11-14
** Last Version: 
** Descriptions: 
**------------------------------------------------------------------------------------------------------
** Created By: 		 hkh
** Created date:   2017-11-14
** Version: 
** Descriptions: BootLoader NEODUN
**	
** 1������STM32�����ڲ�FLASH���ٶȺ�����FLASH��������
				����0-2��  48KB     BootLoader����
				����3��		 16KB		 �������롢���ñ�ʶ��������������������Ҫ������д������һ��С�Ŀռ�
				����4-5��  192KB		 APP���� 
				����6-11�� 768KB   ˽Կ��ַ��
** 2���ڲ�FLASHһ�������Ĳ���ʱ�䣺16K/64K/128K �Ĳ���ʱ��ֱ�Ϊ 230/490/875ms
** 3������HID�豸�ղ�����λ����Ϣ�����߶���λ����Ϣ�������������޸���HID���ձ�־λ���޸���λ�����ͼ��
** 4���޸�Base58������������г���0���bug���޸�Ripemd160�㷨��bug
** 5������ID������Ҫ������λ������һ��ID���·���˽Կ���ڶ���ID�����ݰ���ID����λ���ȶ��ģ�ÿ���һ�����������ݽ��������ID��������飬
ÿ��ID��Ӧ��50�ֽڵ�����
** 6��HID�������ͳ���������ڴ��ţ�����HID��ʱ�ȴ�״̬
** 7����ȫ�Կ���
			��ѯ��ַ					����Ҫ����
			���ӵ�ַ					�������Ƿ��ظ�		����
			ɾ����ַ					�������Ƿ��ظ�		����
			��ȡ��ȫ��Ϣ			�������Ƿ��ظ�		����
			����ǩ��					�ظ�						������
			���氲ȫ����			�ظ�						������
			��������					�ظ�						������
** 8�������̣��޸�CUSTOM_HID_EPIN_SIZE��CUSTOM_HID_EPOUT_SIZE��ֵ
** 9���޸��ײ�OLED��ʾ���������ݳ����BUG�����»�����ʾ������
********************************************************************************************************/
#include "stm32f4xx_hal.h"
#include "Hal.h"
#include "Commands.h"
#include "DisplayMem.h"
#include "ReceiveAnalysis.h"
#include "Utils.h"
#include "string.h"
#include "main_define.h"
#include "stmflash.h"
#include "Algorithm.h"

using namespace view;
extern "C" void my_main();

DisplayMem &Disp = DisplayMem::getInstance();

extern volatile int hid_flag;
extern u8 hid_data[64];
extern int len_hid;
extern SIGN_KEY_FLAG Key_Flag;
SIGN_SET_FLAG Set_Flag;

volatile int passport_flag_poweron = 0;	//����������֤��־λ
volatile int passport_flag_sign = 0;		//����ǩ����֤��־λ
volatile int passport_flag_set = 0;			//����������֤��־λ
volatile int passport_flag_add = 0;			//���ӵ�ַ��֤��־λ
volatile int passport_flag_del = 0;			//ɾ����ַ��֤��־λ
volatile int passport_flag_bek = 0;			//���ݵ�ַ��֤��־λ
int passport_num[9] = {1,2,3,4,5,6,7,8,9};//������룬��������

void my_main() 
{
		Hal::getInstance().HardwareInit();	 //��ʼ������Ӳ��	
		memset(&Key_Flag,0,sizeof(Key_Flag));//���������־λ
		memset(&Set_Flag,1,sizeof(Set_Flag));//������ñ�־λ

		Hal::getInstance().Start_Motor(200); //����������� 200ms
		Update_PowerOn_Count();							 //�����������ֵ
		Update_PowerOn_SetFlag(&Set_Flag);	 //�����������ñ�ʶ

		Disp.drawString(0,0,"PassPort:");
		Disp.GetPassportArray(passport_num); //������ʾ���9��������
	
	
//		Disp.SetCode();
//		HAL_Delay(3000);
//		Disp.VerifyCode();
	
	
//		//��������Ϊ����		
//		Disp.drawString(92,20,"NeoDun",view::FONT_12X24);
//		passport_flag_poweron = 1;

		while (1) 
		{
				if(hid_flag == 1)						 //HID���ݰ�����
				{
						ReceiveAnalysis::GetInstance().PackDataFromPcCallback(hid_data,len_hid);
						memset(hid_data,0,64);
						hid_flag = 0;
				}
				//��ص����ϵ��������ѣ�
				
								
				
				ReceiveAnalysis::GetInstance().Poll();
		}
}

