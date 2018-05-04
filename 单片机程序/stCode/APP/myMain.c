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
** 1、STM32内部FLASH分配
** 2、修改Base58解码出现数组中出现0项的bug，修改Ripemd160算法的bug
** 3、安全性考虑
			查询地址					不需要密码
			增加地址					可设置是否重复		保存
			删除地址					可设置是否重复		保存
			获取安全信息			可设置是否重复		保存
			交易签名					重复						不保存
			保存安全设置			重复						不保存
			重置密码					重复						不保存
** 4、修复底层OLED显示，参数传递出错的BUG，导致换行显示出问题
** 5、重构代码
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

void my_main(void)
{
		//数据初始化
		Sys_Data_Init();
	
		//开机更新系统标识、设置标识
		if(Update_PowerOn_SYSFLAG(&System_Flag)==0)			
		{
				Fill_RAM(0x00);
				Asc8_16(60,24,"ATSHA204 ERROR!!!");
				HAL_Delay(DISPLAY_ERROR_TIME);
				return;
		}
		Update_PowerOn_SetFlag(&Set_Flag,&System_Flag);
	
		//开机振动下马达 200ms
		Motor_touch(200);
		
		//新、旧钱包开机处理
NEWWALLET:
		if(System_Flag.new_wallet)
		{
				Key_Control(1);
				Fill_RAM(0x00);
				Show_HZ12_12(84,16,0,1);//欢迎
				Asc8_16(116,16,"NEODUN");
				Display_Triangle(0);	
				while(Key_Flag.flag.middle == 0);
				Key_Flag.flag.middle = 0;
				Display_SetCode();
				System_Flag.new_wallet = 0;
				Update_Flag_ATSHA(&Set_Flag,&System_Flag);
				Key_Control(1);
				{
						Fill_RAM(0x00);
						Show_HZ12_12(80,8,4,5);//密码
						Show_HZ12_12(112,8,2,3);//设置
						Show_HZ12_12(144,8,26,27);//完成
						Show_HZ12_12(16,28,12,12);//请
						Show_HZ12_12(32,28,28,29);//牢记
						Show_HZ12_12(64,28,4,5);//密码
						Show_HZ12_12(96,28,30,32);//丢失将
						Show_HZ12_12(144,28,24,24);//导
						Show_HZ12_12(160,28,33,33);//致
						Show_HZ12_12(176,28,17,18);//钱包
						Show_HZ12_12(208,28,34,34);//重
						Show_HZ12_12(224,28,3,3);//置
						Display_Triangle(0);
				}
				while(Key_Flag.flag.middle == 0);
				Key_Flag.flag.middle = 0;
				{
						Fill_RAM(0x00); 
						Show_HZ12_12(88,16,10,11);//空的
						Asc8_16(120,16,"NEODUN");
				}
				Display_Triangle(0);
				while(Key_Flag.flag.middle == 0);
				Key_Flag.flag.middle = 0;
				{
						Fill_RAM(0x00);
						Show_HZ12_12(40,8,12,16);//请使用支持
						Asc8_16(120,8,"NEODUN");
						Show_HZ12_12(168,8,11,11);//的
						Show_HZ12_12(184,8,17,18);//钱包
						Show_HZ12_12(40,28,19,21);//创建新
						Show_HZ12_12(88,28,4,4);//密
						Show_HZ12_12(104,28,22,25);//钥并导入
						Asc8_16(168,28,"NEODUN");
				}
				Display_Triangle(0);
				while(Key_Flag.flag.middle == 0);
				Key_Flag.flag.middle = 0;
				Key_Control(0);
		}
		else
		{
				if(Display_VerifyCode_PowerOn())//密码验证5次出错
						goto NEWWALLET;
		}
		
		System_Flag.count =1;
		//此处，已进行过密码验证，置此位为1
		Passport_Flag.flag.poweron = 1;
		//显示主页面
		Display_MainPage();
		
		while (1)
		{
				if(hid_flag == 1)						 		 			//HID数据包解析
				{
						Hid_Data_Analysis(hid_data,len_hid);
						memset(hid_data,0,64);
						hid_flag = 0;
				}
				Display_MainPage_judge();							//显示更新
				if(task_1s_flag)											//1s任务
				{
						task_1s_flag = 0;
						if(Scan_USB())	//扫描USB
								Set_Flag.flag.usb_offline = 0;
																							//电池电量较低做个提醒
				}
				//考虑USB连上后关掉其它功能
		}
}


