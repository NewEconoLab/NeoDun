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
** 1、由于STM32擦除内部FLASH的速度很慢，FLASH分配如下
				扇区0-2：  48KB     BootLoader程序
				扇区3：		 16KB		 开机密码、设置标识、计数总数，改扇区需要经常擦写，分配一个小的空间
				扇区4-5：  192KB		 APP程序 
				扇区6-11： 768KB   私钥地址对
** 2、内部FLASH一个扇区的擦除时间：16K/64K/128K 的擦除时间分别为 230/490/875ms
** 3、出现HID设备收不到上位机信息，或者对上位机信息不处理的情况，修改了HID接收标志位，修改上位机发送间隔
** 4、修改Base58解码出现数组中出现0项的bug，修改Ripemd160算法的bug
** 5、数据ID分配需要传给上位机，第一个ID是下发的私钥，第二个ID是数据包，ID是下位机先定的，每完成一次完整的数据交互，清除ID并清空数组，
每个ID对应着50字节的数据
** 6、HID连续发送出错，问题出在串号，发送HID延时等待状态
** 7、安全性考虑
			查询地址					不需要密码
			增加地址					可设置是否重复		保存
			删除地址					可设置是否重复		保存
			获取安全信息			可设置是否重复		保存
			交易签名					重复						不保存
			保存安全设置			重复						不保存
			重置密码					重复						不保存
** 8、整理工程，修改CUSTOM_HID_EPIN_SIZE和CUSTOM_HID_EPOUT_SIZE的值
** 9、修复底层OLED显示，参数传递出错的BUG，导致换行显示出问题
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

volatile int passport_flag_poweron = 0;	//开机密码验证标志位
volatile int passport_flag_sign = 0;		//交易签名验证标志位
volatile int passport_flag_set = 0;			//设置密码验证标志位
volatile int passport_flag_add = 0;			//增加地址验证标志位
volatile int passport_flag_del = 0;			//删除地址验证标志位
volatile int passport_flag_bek = 0;			//备份地址验证标志位
int passport_num[9] = {1,2,3,4,5,6,7,8,9};//随机密码，生成数组

void my_main() 
{
		Hal::getInstance().HardwareInit();	 //初始化所有硬件	
		memset(&Key_Flag,0,sizeof(Key_Flag));//清除按键标志位
		memset(&Set_Flag,1,sizeof(Set_Flag));//清除设置标志位

		Hal::getInstance().Start_Motor(200); //开机振动下马达 200ms
		Update_PowerOn_Count();							 //开机检验计数值
		Update_PowerOn_SetFlag(&Set_Flag);	 //开机更新设置标识

		Disp.drawString(0,0,"PassPort:");
		Disp.GetPassportArray(passport_num); //开机显示随机9宫格密码
	
	
//		Disp.SetCode();
//		HAL_Delay(3000);
//		Disp.VerifyCode();
	
	
//		//这两句作为调试		
//		Disp.drawString(92,20,"NeoDun",view::FONT_12X24);
//		passport_flag_poweron = 1;

		while (1) 
		{
				if(hid_flag == 1)						 //HID数据包解析
				{
						ReceiveAnalysis::GetInstance().PackDataFromPcCallback(hid_data,len_hid);
						memset(hid_data,0,64);
						hid_flag = 0;
				}
				//电池电量较低做个提醒：
				
								
				
				ReceiveAnalysis::GetInstance().Poll();
		}
}

