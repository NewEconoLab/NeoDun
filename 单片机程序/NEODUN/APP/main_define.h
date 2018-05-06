#ifndef MAIN_DEFINE_H
#define MAIN_DEFINE_H

#include "stm32f4xx_hal.h"
#include <stdint.h>

//程序版本号，高4位主版本号，低4位次版本号
#define	VERSION								"V1.0"
#define VERSION_NEO						"V1.0"
//宏开关
#define printf_debug
//#define HID_Delay
//宏定义
#define ADDR_XiaoYi 					0x0101
#define ADDR_SIZE   					40
#define SLOT_FLAG							14
#define DISPLAY_ERROR_TIME		2000
#define INPUT_TIME_DIV				100
#define SYSTEM_BASE_TIME			10
#define MOTOR_TIME  					80
#define KEY_VALID_TIME				3

#define HID_MAX_DATA_LEN			64*1024
#define DATA_PACK_SIZE				50
#define HID_SEND_DELAY				15

//按键标识
typedef union
{
		struct
		{
				volatile	uint8_t key_state:1;	//防误按的标志位
				volatile	uint8_t left:1;				//左按键标识
				volatile	uint8_t middle:1;			//中间按键标识
				volatile	uint8_t right:1;			//右边按键标识
		}flag;
		uint8_t data;
}KEY_FLAG;

//设置标识    现只用到新增、删除和备份地址    未用标识默认为0，其它标识默认值为1
typedef union
{
		struct
		{
				uint8_t usb_offline:1;							//0表示连上USB，1表示没连上USB
				uint8_t auto_show:1;								//连接钱包后自动弹出驱动界面
				uint8_t auto_update:1;							//开机时自动检查更新
				uint8_t add_address:1;							//新增地址
				uint8_t del_address:1;							//删除地址
				uint8_t backup_address:1;				  	//备份地址
				uint8_t backup_address_encrypt:1; 	//备份钱包时进行加密标识
		}flag;
		uint8_t data;
}SET_FLAG;

//密码标识
typedef union
{
		struct
		{
				uint8_t poweron:1;				//开机密码验证标志位
				uint8_t sign_data:1;			//交易签名验证标志位
				uint8_t set_system:1;			//设置密码验证标志位
				uint8_t add_address:1;		//增加地址验证标志位
				uint8_t del_address:1;		//删除地址验证标志位
				uint8_t backup_address:1;	//备份地址验证标志位
		}flag;
		uint8_t data;
}PASSPORT_FLAG;

//系统标识
typedef struct
{
		uint8_t	new_wallet; 			//1表示新钱包，0表示旧钱包
		uint8_t	update;						//1表示需要升级，0表示不需要升级
		uint8_t	language;					//1表示英文，0表示中文
		uint8_t count;						//地址数量
		uint8_t	sn[12];						//单片机唯一SN码
}SYSTEM_FLAG;

//HID数据解析结构体
typedef struct
{
		uint32_t	dataLen;
		uint16_t 	packIndex;
		uint16_t  packCount;
		uint16_t 	notifySerial;
		uint16_t	reqSerial;
		uint8_t 	hashRecord[32];
}DATA_HID_RECORD;

//为地址显示开辟内存
typedef struct
{
		uint8_t name[6];					//地址名称
		uint8_t	len_name;					//地址名称长度
		uint8_t	hide;							//隐藏属性
		uint8_t	content[25];			//Base58前的地址数据
		uint8_t	address[40];			//Base58后的字符地址数据
}ADDRESS;

enum TimerFunction
{
		OLED_INPUT_TIME = 0,
		KEY_TIME 				= 1,
};

extern KEY_FLAG 				Key_Flag;
extern SET_FLAG 				Set_Flag;
extern SYSTEM_FLAG 			System_Flag;
extern PASSPORT_FLAG 		Passport_Flag;
extern DATA_HID_RECORD	HidData;
extern ADDRESS					showaddress[5];
extern volatile 	uint32_t 	moter_delay;
extern volatile 	uint8_t 	task_1s_flag;

extern volatile int hid_flag;
extern uint8_t hid_data[64];
extern int len_hid;


void Sys_Data_Init(void);

#endif

