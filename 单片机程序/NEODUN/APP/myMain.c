/*------------------------------------------------File Info---------------------------------------------
** File Name: myMain.cpp
** Created By: hkh 
** Version:
** Descriptions: 
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
#include "app_utils.h"

static void new_wallet_proc(void)
{
		Key_Control(1);
		{
				Fill_RAM(0x00);
				#ifdef Chinese	
						Show_HZ12_12(84,16,0,1);//��ӭ
						Asc8_16(116,16,"NEODUN");
				#endif
				#ifdef English									
						Show_AscII_Picture(60,16,welcomeBitmapDot,sizeof(welcomeBitmapDot));//64
						Show_AscII_Picture(128,16,toBitmapDot,sizeof(toBitmapDot));//16
						Show_AscII_Picture(148,16,NEODUNBitmapDot,sizeof(NEODUNBitmapDot));//48
				#endif
				Display_Triangle(0);	
		}
		while(Key_Flag.flag.middle == 0);
		Key_Flag.flag.middle = 0;
		Display_SetCode(0);
		Neo_System.new_wallet = 0;
		Update_Flag_ATSHA(&Set_Flag,&Neo_System);
		Key_Control(1);
		{
				Fill_RAM(0x00);
				#ifdef Chinese
						Show_HZ12_12(80,8,4,5);//����
						Show_HZ12_12(112,8,2,3);//����
						Show_HZ12_12(144,8,26,27);//���
			
						Show_HZ12_12(12,28,12,12);//��
						Show_HZ12_12(28,28,28,29);//�μ�
						Show_HZ12_12(60,28,4,5);//����
						Asc8_16(92,28,",");
						Show_HZ12_12(100,28,30,32);//��ʧ��
						Show_HZ12_12(148,28,24,24);//��
						Show_HZ12_12(164,28,33,33);//��
						Show_HZ12_12(180,28,2,2);//��
						Show_HZ12_12(196,28,43,43);//��
						Show_HZ12_12(212,28,34,34);//��
						Show_HZ12_12(228,28,3,3);//��
				#endif
				#ifdef English
						Show_AscII_Picture(52,16,YourBitmapDot,sizeof(YourBitmapDot));//32
						Show_AscII_Picture(88,16,PINBitmapDot,sizeof(PINBitmapDot));//24
						Show_AscII_Picture(116,16,hasBitmapDot,sizeof(hasBitmapDot));//24
						Show_AscII_Picture(144,16,beenBitmapDot,sizeof(beenBitmapDot));//32
						Show_AscII_Picture(180,16,set_BitmapDot,sizeof(set_BitmapDot));//24
				#endif
				Display_Triangle(0);
		}
		while(Key_Flag.flag.middle == 0);
		Key_Flag.flag.middle = 0;
		{
				Fill_RAM(0x00);
				#ifdef Chinese
						Show_HZ12_12(88,16,10,11);//�յ�
						Asc8_16(120,16,"NEODUN");
				#endif
				#ifdef English
						Show_AscII_Picture(72,8,NEODUNBitmapDot,sizeof(NEODUNBitmapDot));//48
						Show_AscII_Picture(124,8,doesBitmapDot,sizeof(doesBitmapDot));//32
						Show_AscII_Picture(160,8,notBitmapDot,sizeof(notBitmapDot));//24
			
						Show_AscII_Picture(72,28,containBitmapDot,sizeof(containBitmapDot));//48
						Show_AscII_Picture(124,28,anyBitmapDot,sizeof(anyBitmapDot));//24
						Show_AscII_Picture(152,28,keysBitmapDot,sizeof(keysBitmapDot));//32
				#endif
		}
		Display_Triangle(0);
		while(Key_Flag.flag.middle == 0);
		Key_Flag.flag.middle = 0;
		{
				Fill_RAM(0x00);
				#ifdef Chinese
						Show_HZ12_12(40,8,12,16);//��ʹ��֧��
						Asc8_16(120,8,"NEODUN");
						Show_HZ12_12(168,8,11,11);//��
						Show_HZ12_12(184,8,17,18);//Ǯ��
						Show_HZ12_12(40,28,19,21);//������
						Show_HZ12_12(88,28,4,4);//��
						Show_HZ12_12(104,28,22,25);//Կ������
						Asc8_16(168,28,"NEODUN");
				#endif
				#ifdef English
						Show_AscII_Picture(64,8,PleaseBitmapDot,sizeof(PleaseBitmapDot));//48
						Show_AscII_Picture(116,8,importBitmapDot,sizeof(importBitmapDot));//40
						Show_AscII_Picture(160,8,keysBitmapDot,sizeof(keysBitmapDot));//32
			
						Show_AscII_Picture(52,28,usingBitmapDot,sizeof(usingBitmapDot));//40
						Show_AscII_Picture(96,28,NEODUNBitmapDot,sizeof(NEODUNBitmapDot));//48
						Show_AscII_Picture(148,28,ManagerBitmapDot,sizeof(ManagerBitmapDot));//56
				#endif
		}
		Display_Triangle(0);
		while(Key_Flag.flag.middle == 0);
		Key_Flag.flag.middle = 0;
		Key_Control(0);
}

void my_main(void)
{
		//1 ���ݳ�ʼ��
		Sys_Data_Init();
	
		//2 ��������ϵͳ��ʶ�����ñ�ʶ����ַ�������Ϣ������
		if(Update_PowerOn_SYSFLAG(&Neo_System)==0)			
		{
				Fill_RAM(0x00);
				Asc8_16(60,24,"ATSHA204 ERROR!!!");
				HAL_Delay(DISPLAY_ERROR_TIME);
				return;
		}
		Update_PowerOn_SetFlag(&Set_Flag,&Neo_System);
			
		//3 ���ٿ�������Ӧ���ڲ������APP
		if(SysFlagType == 0)
		{
				//����������� 200ms
				Motor_touch(200);
				
				//�¡���Ǯ����������
NEWWALLET:
				if(Neo_System.new_wallet)
				{
						new_wallet_proc();
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
				USB_Init();
		}
		else
		{
				Deal_Sign_Data_Restart();
				Passport_Flag.flag.poweron = 1;
		}

		//4 �ϵ���USB
		if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_9))
				Set_Flag.flag.usb_offline = 0;

		while (1)
		{
				//1 HID���ݰ�����
				if(hid_index_read != hid_index_write)				//�յ�HID���ݰ�
				{
						Hid_Data_Analysis(hid_recv_data[hid_index_read].data,hid_recv_data[hid_index_read].len);
						memset(hid_recv_data[hid_index_read].data,0,64);
						hid_index_read++;
						if(hid_index_read == hid_index_write)
						{
							hid_index_write = 0;
							hid_index_read = 0;
						}
				}
				
				//2 task 1s
				if(task_1s_flag)														//1s����
				{
						task_1s_flag = 0;
						if(Set_Flag.flag.usb_offline == 0)
						{
								if(Set_Flag.flag.usb_state_pre)			//��ʾ�ӶϿ�����USB��������USB
								{
										if(SysFlagType != 0xFF)
												Display_Usb();
								}
								Set_Flag.flag.usb_state_pre = 0;
						}
						else
						{
								if(Set_Flag.flag.usb_state_pre == 0)//��ʾ������USB�����Ͽ�USB
								{
										Fill_RAM(0x00);
										Display_MainPage();
								}
								Set_Flag.flag.usb_state_pre = 1;
						}
						//��ص����ϵ���������  TBD
				}
				
				//3 ���水��ˢ�´���
				if((Set_Flag.flag.usb_offline == 0)&&(Key_Flag.flag.middle == 1))//����USB�������м��Ϊ�����ʾ
				{
						Key_Flag.flag.middle = 0;
						Display_Usb();
				}

				//4 ���洦��
				if(Set_Flag.flag.usb_offline)								//USB�Ͽ�����ϵͳ��ʾ����
				{
						Display_MainPage_proc();								//��ʾ����
				}
		}
}


