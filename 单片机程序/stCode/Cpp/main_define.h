#ifndef MAIN_DEFINE_H
#define MAIN_DEFINE_H

#include <stdint.h>

#define printf_debug
#define HID_Delay

#define ADDR_XiaoYi 0x0101
#define ADDR_SIZE   40

typedef struct
{
		int Sign_Key_Flag;				//防误按的标志位
		int Sign_Key_left_Flag;		//左按键标识
		int Sign_Key_center_Flag;	//中间按键标识
		int Sign_Key_right_Flag;	//右边按键标识
}SIGN_KEY_FLAG;


//设置标识    现只用到新增、删除和备份地址    未用标识默认为0，其它标识默认值为1
typedef struct
{
		uint32_t New_Device_Flag;						  //新设备标识
		uint32_t Auto_Show_Flag;							//连接钱包后自动弹出驱动界面
		uint32_t Auto_Update_Flag;						//开机时自动检查更新
		uint32_t Add_Address_Flag;						//新增地址
		uint32_t Del_Address_Flag;						//删除地址
		uint32_t Backup_Address_Flag;				  //备份地址
		uint32_t Backup_Address_Encrypt_Flag; //备份钱包时进行加密标识
}SIGN_SET_FLAG;



#endif

