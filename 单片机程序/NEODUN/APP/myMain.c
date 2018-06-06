/*------------------------------------------------File Info---------------------------------------------
** File Name: myMain.cpp
** Last modified Date:  2017-04-20
** Last Version: 
** Descriptions: 
**------------------------------------------------------------------------------------------------------
** Created By: 		 hkh
** Created date:   2017-11-14
** Version:
** Descriptions: BootLoader NEODUN
**
** 1��STM32�ڲ�FLASH����
** 2���޸�Base58������������г���0���bug���޸�Ripemd160�㷨��bug
** 3����ȫ�Կ���
			��ѯ��ַ					����Ҫ����
			���ӵ�ַ					�������Ƿ��ظ�		����
			ɾ����ַ					�������Ƿ��ظ�		����
			��ȡ��ȫ��Ϣ			�������Ƿ��ظ�		����
			����ǩ��					�ظ�						������
			���氲ȫ����			�ظ�						������
			��������					�ظ�						������
** 4���޸��ײ�OLED��ʾ���������ݳ����BUG�����»�����ʾ������
** 5���ع�����
** 6���޸�FALSH����BUG,���������ת��������FLASH��ʶ
********************************************************************************************************/
#include "stm32f4xx_hal.h"
#include "myMain.h"
#include "app_command.h"
#include "string.h"
#include "main_define.h"
#include "Algorithm.h"
#include "app_interface.h"
#include "OledMenu.h"
#include "aw9136.h"
#include "OLED281/oled281.h"
#include "app_hal.h"
#include "stmflash.h"

void my_main(void)
{
		//���ݳ�ʼ��
		Sys_Data_Init();

		//��������ϵͳ��ʶ�����ñ�ʶ����ַ�������Ϣ
		if(Update_PowerOn_SYSFLAG(&Neo_System)==0)			
		{
				Fill_RAM(0x00);
				Asc8_16(60,24,"ATSHA204 ERROR!!!");
				HAL_Delay(DISPLAY_ERROR_TIME);
				return;
		}
		Update_PowerOn_SetFlag(&Set_Flag,&Neo_System);
		
		//���ٿ�������Ӧ���ڲ������APP
		if(SysFlagType == 0)
		{
				//����������� 200ms
				Motor_touch(200);
				
				//�¡���Ǯ����������
NEWWALLET:
				if(Neo_System.new_wallet)
				{
						Key_Control(1);
						Fill_RAM(0x00);
						Show_HZ12_12(84,16,0,1);//��ӭ
						Asc8_16(116,16,"NEODUN");
						Display_Triangle(0);	
						while(Key_Flag.flag.middle == 0);
						Key_Flag.flag.middle = 0;
						Display_SetCode();
						Neo_System.new_wallet = 0;
						Update_Flag_ATSHA(&Set_Flag,&Neo_System);
						Key_Control(1);
						{
								Fill_RAM(0x00);
								Show_HZ12_12(80,8,4,5);//����
								Show_HZ12_12(112,8,2,3);//����
								Show_HZ12_12(144,8,26,27);//���
								Show_HZ12_12(16,28,12,12);//��
								Show_HZ12_12(32,28,28,29);//�μ�
								Show_HZ12_12(64,28,4,5);//����
								Show_HZ12_12(96,28,30,32);//��ʧ��
								Show_HZ12_12(144,28,24,24);//��
								Show_HZ12_12(160,28,33,33);//��
								Show_HZ12_12(178,28,2,2);//��
								Show_HZ12_12(192,28,43,43);//��
								Show_HZ12_12(208,28,34,34);//��
								Show_HZ12_12(224,28,3,3);//��
								Display_Triangle(0);
						}
						while(Key_Flag.flag.middle == 0);
						Key_Flag.flag.middle = 0;
						{
								Fill_RAM(0x00); 
								Show_HZ12_12(88,16,10,11);//�յ�
								Asc8_16(120,16,"NEODUN");
						}
						Display_Triangle(0);
						while(Key_Flag.flag.middle == 0);
						Key_Flag.flag.middle = 0;
						{
								Fill_RAM(0x00);
								Show_HZ12_12(40,8,12,16);//��ʹ��֧��
								Asc8_16(120,8,"NEODUN");
								Show_HZ12_12(168,8,11,11);//��
								Show_HZ12_12(184,8,17,18);//Ǯ��
								Show_HZ12_12(40,28,19,21);//������
								Show_HZ12_12(88,28,4,4);//��
								Show_HZ12_12(104,28,22,25);//Կ������
								Asc8_16(168,28,"NEODUN");
						}
						Display_Triangle(0);
						while(Key_Flag.flag.middle == 0);
						Key_Flag.flag.middle = 0;
						Key_Control(0);
				}
				else
				{
						if(Display_VerifyCode_PowerOn())//������֤5�γ���
								goto NEWWALLET;
				}

				//�˴����ѽ��й�������֤���ô�λΪ1
				Passport_Flag.flag.poweron = 1;
				//��ʾ��ҳ��
				Display_MainPage();
		}
		else
		{
				SysFlagType = 0;
				Deal_Sign_Data_Restart();
				Passport_Flag.flag.poweron = 1;
		}

		while (1)
		{
				if(hid_flag == 1)						 		 						//�յ�HID���ݰ�
				{
						Hid_Data_Analysis(hid_data,len_hid);		//HID���ݰ�����
						memset(hid_data,0,64);
						hid_flag = 0;
				}
				if(task_1s_flag)														//1s����
				{
						task_1s_flag = 0;
						if(Scan_USB())													//ɨ��USB
						{
								if(Set_Flag.flag.usb_state_pre)			//��ʾ�ӶϿ�����USB��������USB
										Display_Usb();
								Set_Flag.flag.usb_offline 	= 0;		//USB����
								Set_Flag.flag.usb_state_pre = 0;
						}
						else
						{
								if(Set_Flag.flag.usb_state_pre == 0)//��ʾ������USB�����Ͽ�USB
								{
										Fill_RAM(0x00);
										Display_MainPage();
								}
								Set_Flag.flag.usb_offline 	= 1;		//USB�Ͽ�
								Set_Flag.flag.usb_state_pre = 1;
						}
						//��ص����ϵ���������
						
				}
				if(Set_Flag.flag.usb_offline)								//USB�Ͽ�����ϵͳ��ʾ����
				{
						Display_MainPage_judge();								//��ʾ����
				}
		}
}


