#ifndef MAIN_DEFINE_H
#define MAIN_DEFINE_H

#include <stdint.h>


#define Debug_Print


typedef struct
{
		volatile int Key_Control_Flag;	//防误按的标志位
		volatile int Key_left_Flag;			//左按键标识
		volatile int Key_center_Flag;	  //中间按键标识
		volatile int Key_right_Flag;		//右边按键标识
		volatile int Key_double_Flag;		//左右双击按键标识
}SIGN_KEY_FLAG;


typedef struct
{
		unsigned char	new_wallet; 			//1表示新钱包，0表示旧钱包 
		unsigned char	update;						//1表示需要升级，0表示不需要升级
		unsigned char	language;					//1表示英文，0表示中文
}BOOT_FLAG;



#endif





