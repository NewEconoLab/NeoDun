#ifndef MAIN_DEFINE_H
#define MAIN_DEFINE_H

#include "stm32f4xx_hal.h"
#include <stdint.h>

#define HID_QUEUE_DEPTH					50

#define FLASH_ADDRESS_SIGN_DATA	0x0801F000 //签名结果地址
#define	FLASH_ADDRESS_SCENE		  0x0801FE00 //现场数据地址
#define FLASH_ADDRESS_PACK		  0x08010000 //扇区4
#define FLASH_DATA_SECTOR				FLASH_SECTOR_4

#define FLASH_ADDRESS_NEODUN	  0x08020000 //扇区5+扇区6
#define FLASH_ADDRESS_APP1		  0x08060000 //扇区7
#define FLASH_SECTOR_APP1				FLASH_SECTOR_7
#define FLASH_ADDRESS_APP2		  0x08080000 //扇区8
#define FLASH_SECTOR_APP2				FLASH_SECTOR_8
#define FLASH_ADDRESS_APP3		  0x080A0000 //扇区9
#define FLASH_SECTOR_APP3				FLASH_SECTOR_9
#define FLASH_ADDRESS_APP4		  0x080C0000 //扇区10
#define FLASH_SECTOR_APP4				FLASH_SECTOR_10
#define FLASH_ADDRESS_APP5		  0x080E0000 //扇区11
#define FLASH_SECTOR_APP5				FLASH_SECTOR_11

#define COIN_TYPE_OFFSET				0x1FFFC
#define COIN_VERSION_OFFSET			0x1FFFE

//程序版本号，高4位主版本号，低4位次版本号
#define	VERSION_NEODUN				0x0100
#define VERSION_NEO						0x0100
#define	VERSION_NEODUN_STR		"V1.0"
#define VERSION_NEO_STR				"V1.0"
//宏开关
//#define printf_debug
#define HID_Delay
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

//错误码
#define ERR_DATA_HASH					0x0100
#define ERR_NO_SPACE_INSTALL	0x0101
#define ERR_COIN_RUNNING			0x0102
#define ERR_COIN_TYPE					0x0103
#define ERR_VERIFY_FAIED			0x0104
#define ERR_KEY_MAX_COUNT			0x0201
#define	ERR_EXIST_KEY					0x0202
#define	ERR_SAME_ADD_NAME			0x0203
#define ERR_UNKNOW_COIN				0x0204
#define ERR_KEY_FORMAT				0x0205
#define ERR_UNKONW_ADD				0x0206
#define ERR_UNKNOW_KEY				0x0301
#define ERR_MULTI_SIGN				0x0302
#define ERR_DATA_PACK					0x0303
#define ERR_USER_REFUSE				0x0401
#define ERR_TIME_OUT					0x0501

//币种
#define COIN_NEO							"NEO"
#define COIN_BTC							"BTC"
#define COIN_ETH							"ETH"

//语言
enum
{
		Chinese	= 0,
		English = 1,
};

//返回值
enum
{
		NEO_SUCCESS			= 0,
		NEO_USER_REFUSE = 1,
		NEO_TIME_OUT		= 2,
};

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
				uint8_t usb_state_pre:1;						//USB前时刻的状态
		}flag;
		uint8_t data;
}SET_FLAG;

//密码标识
typedef union
{
		struct
		{
				uint8_t poweron:1;				//开机密码验证
				uint8_t sign_data:1;			//交易签名验证标志位
				uint8_t add_address:1;		//增加地址验证标志位
				uint8_t del_address:1;		//删除地址验证标志位

		}flag;
		uint8_t data;
}PASSPORT_FLAG;

//时间控制
typedef struct
{
		uint8_t ready;								//是否需要计时
		uint8_t time;									//计时时间
		uint8_t timeout;							//是否超时
}TIME_CONTROL;

//系统标识
typedef struct
{
		uint8_t	new_wallet; 			//1表示新钱包，0表示旧钱包
		uint8_t	update;						//1表示需要升级，0表示不需要升级
		uint8_t	language;					//1表示英文，0表示中文
		uint8_t count;						//地址数量
		uint8_t	sn[12];						//单片机唯一SN码
		uint8_t pin[8];
}SYSTEM_NEODUN;

//HID数据解析结构体
typedef struct
{
		uint32_t	dataLen;			
		uint16_t 	packIndex;
		uint16_t  packCount;
		uint16_t 	notifySerial;		
		uint16_t	reqSerial;
		uint8_t	  Pack_ID;
		uint8_t 	hashRecord[32];	
}DATA_HID_RECORD;

//为地址显示开辟内存
typedef struct
{
		uint8_t	hide;							//隐藏属性	
		uint8_t	len_name;					//地址名称长度
		char 		name[7];					//地址名称
		uint8_t	content[25];			//Base58前的地址数据
		uint8_t	address[40];			//Base58后的字符地址数据
}ADDRESS;

//系统插件信息,缺省值为0xFFFF,
typedef struct
{
		uint16_t count;						
		uint16_t coin1;						//类型
		uint16_t version1;				//版本
		uint16_t coin2;
		uint16_t version2;
		uint16_t coin3;
		uint16_t version3;
		uint16_t coin4;
		uint16_t version4;
		uint16_t coin5;
		uint16_t version5;
}COIN;

typedef struct
{
		uint8_t randoma[32];
		uint8_t pubkeyA[64];
		uint8_t hashA[2];
		uint8_t pubkeyB[64];		
		uint8_t keyM[32];
}SECURE_PIPE;

typedef struct
{
	uint8_t data[64];
	uint8_t len;
}HID_RECV_DATA;

enum TimerFunction
{
		OLED_INPUT_TIME = 0,
		KEY_TIME 				= 1,
};

//HID数据
extern volatile uint32_t hid_index_read;
extern volatile uint32_t hid_index_write;
extern HID_RECV_DATA hid_recv_data[HID_QUEUE_DEPTH];

extern volatile 	uint32_t 	system_base_time;
extern uint8_t					SysFlagType;
extern KEY_FLAG 				Key_Flag;
extern SET_FLAG 				Set_Flag;
extern SYSTEM_NEODUN 		Neo_System;
extern PASSPORT_FLAG 		Passport_Flag;
extern DATA_HID_RECORD	HidData;
extern ADDRESS					showaddress[5];
extern COIN							coinrecord;
extern SECURE_PIPE			secure_pipe;
extern volatile 	uint32_t 	moter_delay;
extern volatile 	uint8_t 	task_1s_flag;

//声明ASCII库
extern uint8_t ASC8X16[];
extern uint8_t HZ12X12_S[];
//声明图片
extern uint8_t gImage_emptypin[72];
extern uint8_t gImage_fullpin[72];
extern uint8_t gImage_Set[512];
extern uint8_t gImage_triangle_up[72];
extern uint8_t gImage_triangle_down[72];
extern uint8_t gImage_Address[512];
extern uint8_t gImage_Address_hide[512];
extern uint8_t gImage_arrow_left[128];
extern uint8_t gImage_arrow_right[128];
extern uint8_t gImage_Usb[288];
extern uint8_t gImage_delete[64];

void Sys_Data_Init(void);

#endif

