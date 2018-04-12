#ifndef MAIN_DEFINE_H
#define MAIN_DEFINE_H

#include <stdint.h>


#define Debug_Print

#define RECV_BIN_FILE_LEN		100*1024
#define Page_Per_Size				50
#define MAX_Page_Index			RECV_BIN_FILE_LEN/Page_Per_Size

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


typedef struct 
{
		unsigned int size;
		unsigned int packIndex;
		unsigned char hash_tran[32];
		unsigned short int reqSerial;
		unsigned short int notifySerial;
		
		unsigned char signature[64];
		unsigned char hash_actual[32];
}BIN_FILE_INFO;


#endif





