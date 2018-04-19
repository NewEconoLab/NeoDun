
#ifndef __HW_CONFIG_H
#define __HW_CONFIG_H

#include "stm32f4xx_hal.h"
#include "bitBand.h"

#define  __ON     1
#define  __OFF    0

/*&&&&&&&&&&&&&&&&&&&        LED引脚定义       &&&&&&&&&&&&&&&&&&&&&&&&*/
//开发板上LED灯用作USB状态灯
#define Set_Usb_Led(OnOff)      {PBout(0) = (1^OnOff);}//1为ON，0为OFF
#define Toggle_Usb_Led()        {PBout(0) ^=1;}
#define Set_Pass_Led(OnOff)     {PBout(7) = (1^OnOff);}//1为ON，0为OFF
#define Set_Fail_Led(OnOff)     {PBout(6) = (1^OnOff);}//1为ON，0为OFF

/*&&&&&&&&&&&&&&&&&&&        BUTTON引脚定义       &&&&&&&&&&&&&&&&&&&&&&&*/
#define  STATUS_BUTTON1         PBin(5)

/*&&&&&&&&&&&&&&&&&&&      芯片电源控制(通过场效应管)     &&&&&&&&&&&&&&&&&&&&&&&*/
#define  Set_Chip_Power(OnOff)  {PBout(14) = (1^OnOff);}//1为ON，0为OFF ----不建议对芯片电源进行高速开关操作


void ATSHA204_Init(void);
	
#endif  /*__HW_CONFIG_H*/

