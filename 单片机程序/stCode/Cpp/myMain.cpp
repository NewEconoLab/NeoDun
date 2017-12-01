#include "stm32f4xx_hal.h"
#include "Hal.h"
#include "KeyScan.h"
#include "Commands.h"
#include "DisplayMem.h"
#include "ReceiveAnalysis.h"
#include "Utils.h"
#include "string.h"
#include "main_define.h"

using namespace view;
extern "C" void my_main();
extern "C" void oled281_test(void );

DisplayMem &Disp = DisplayMem::getInstance();


extern bool hid_flag;
extern u8 hid_data[64];
extern int len_hid;
extern SIGN_KEY_FLAG Key_Flag;


//static void OnKeyUp(int type);
//static void OnKeDown(int type);



//KeyScan keyUp  ('A',0,OnKeyUp);
//KeyScan keyDown('A',0,OnKeDown);

//char *STR_OK =       "       OK       "   ;
//char *STR_CANCLE =   "     CANCLE     "   ;


//static void UsbRx(u8 *data ,u8 len)
//{
//	Disp.drawString(0,16,"PC is requesting,Please choose ");
//	Disp.drawString(0,48,   STR_OK ,true);
//	Disp.drawString(128,48,STR_CANCLE ,false);
//}


//static void OnKeyUp(int type)
//{
//
//}
//static void OnKeDown(int type)
//{
//		static bool isOK = true;
//		if(type == KeyScan::EVENT_SHORT_CLICK)
//		{
//				isOK = !isOK;
//				if(isOK)
//				{
//						Disp.drawString(0,48,  STR_OK ,true);
//						Disp.drawString(128,48,STR_CANCLE ,false);
//				}
//				else
//				{
//						Disp.drawString(0,48, STR_OK,false);
//						Disp.drawString(128,48,STR_CANCLE,true);
//				}
//				Commands command;
//				command.SendToPc();
//		}
//		else
//		{
//				if(isOK)
//						Hal::getInstance().SendUSBData((u8*)"111",16);
//				else
//						Hal::getInstance().SendUSBData((u8*)"000",16);
//		}
//}

void my_main() 
{
		Hal::getInstance().HardwareInit();	//初始化所有硬件	
		memset(&Key_Flag,0,sizeof(Key_Flag));
	
		printf("hardware init ok\r\n");
	
//	while(true)
//	Hal::getInstance().SendUSBData((u8*)"123",500);
	
    Disp.drawString(26,20,"Welcom Use NeoDun",FONT_12X24);
	
		while (1) 
		{
//				if(HAL_GetTick() % 10 == 0 )
//				{
//						keyDown.Scan();
//				}
				if(hid_flag == true)
				{
						ReceiveAnalysis::GetInstance().PackDataFromPcCallback(hid_data,len_hid);
						memset(hid_data,0,64);
						hid_flag = false;
				}
				ReceiveAnalysis::GetInstance().Poll();
		}
}

