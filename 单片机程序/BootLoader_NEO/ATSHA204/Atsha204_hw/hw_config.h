
#ifndef __HW_CONFIG_H
#define __HW_CONFIG_H

#include "stm32f4xx_hal.h"
#include "bitBand.h"

#define  __ON     1
#define  __OFF    0

/*&&&&&&&&&&&&&&&&&&&        LED���Ŷ���       &&&&&&&&&&&&&&&&&&&&&&&&*/
//��������LED������USB״̬��
#define Set_Usb_Led(OnOff)      {PBout(0) = (1^OnOff);}//1ΪON��0ΪOFF
#define Toggle_Usb_Led()        {PBout(0) ^=1;}
#define Set_Pass_Led(OnOff)     {PBout(7) = (1^OnOff);}//1ΪON��0ΪOFF
#define Set_Fail_Led(OnOff)     {PBout(6) = (1^OnOff);}//1ΪON��0ΪOFF

/*&&&&&&&&&&&&&&&&&&&        BUTTON���Ŷ���       &&&&&&&&&&&&&&&&&&&&&&&*/
#define  STATUS_BUTTON1         PBin(5)

/*&&&&&&&&&&&&&&&&&&&      оƬ��Դ����(ͨ����ЧӦ��)     &&&&&&&&&&&&&&&&&&&&&&&*/
#define  Set_Chip_Power(OnOff)  {PBout(14) = (1^OnOff);}//1ΪON��0ΪOFF ----�������оƬ��Դ���и��ٿ��ز���


void ATSHA204_Init(void);
	
#endif  /*__HW_CONFIG_H*/

