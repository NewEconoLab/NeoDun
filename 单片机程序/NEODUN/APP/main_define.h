#ifndef MAIN_DEFINE_H
#define MAIN_DEFINE_H

#include "stm32f4xx_hal.h"
#include <stdint.h>

#define FLASH_ADDRESS_SIGN_DATA	0x0801F000 //ǩ�������ַ
#define	FLASH_ADDRESS_SCENE		  0x0801FE00 //�ֳ����ݵ�ַ
#define FLASH_ADDRESS_PACK		  0x08010000 //����4

////Ϊ�˵���
//#define FLASH_ADDRESS_SIGN_DATA	0x0808F000 //ǩ�������ַ
//#define	FLASH_ADDRESS_SCENE		  0x0808FE00 //�ֳ����ݵ�ַ
//#define FLASH_ADDRESS_PACK		  0x08080000 //����4

#define FLASH_ADDRESS_NEODUN	  0x08020000 //����5+����6
#define FLASH_ADDRESS_APP1		  0x08060000 //����7
#define FLASH_ADDRESS_APP2		  0x08080000 //����8
#define FLASH_ADDRESS_APP3		  0x080a0000 //����9
#define FLASH_ADDRESS_APP4		  0x080c0000 //����10
#define FLASH_ADDRESS_APP5		  0x080e0000 //����11



//����汾�ţ���4λ���汾�ţ���4λ�ΰ汾��
#define	VERSION_NEODUN				0x0100
#define VERSION_NEO						0x0100
#define	VERSION_NEODUN_STR		"V1.0"
#define VERSION_NEO_STR				"V1.0"
//�꿪��
#define printf_debug
//#define HID_Delay
//�궨��
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

//������ʶ
typedef union
{
		struct
		{
				volatile	uint8_t key_state:1;	//���󰴵ı�־λ
				volatile	uint8_t left:1;				//�󰴼���ʶ
				volatile	uint8_t middle:1;			//�м䰴����ʶ
				volatile	uint8_t right:1;			//�ұ߰�����ʶ
		}flag;
		uint8_t data;
}KEY_FLAG;

//���ñ�ʶ    ��ֻ�õ�������ɾ���ͱ��ݵ�ַ    δ�ñ�ʶĬ��Ϊ0��������ʶĬ��ֵΪ1
typedef union
{
		struct
		{
				uint8_t usb_offline:1;							//0��ʾ����USB��1��ʾû����USB
				uint8_t auto_show:1;								//����Ǯ�����Զ�������������
				uint8_t auto_update:1;							//����ʱ�Զ�������
				uint8_t add_address:1;							//������ַ
				uint8_t del_address:1;							//ɾ����ַ
				uint8_t backup_address:1;				  	//���ݵ�ַ
				uint8_t backup_address_encrypt:1; 	//����Ǯ��ʱ���м��ܱ�ʶ
				uint8_t usb_state_pre:1;						//USBǰʱ�̵�״̬
		}flag;
		uint8_t data;
}SET_FLAG;

//�����ʶ
typedef union
{
		struct
		{
				uint8_t poweron:1;				//����������֤��־λ
				uint8_t sign_data:1;			//����ǩ����֤��־λ
				uint8_t set_system:1;			//����������֤��־λ
				uint8_t add_address:1;		//���ӵ�ַ��֤��־λ
				uint8_t del_address:1;		//ɾ����ַ��֤��־λ
				uint8_t backup_address:1;	//���ݵ�ַ��֤��־λ
		}flag;
		uint8_t data;
}PASSPORT_FLAG;

//ϵͳ��ʶ
typedef struct
{
		uint8_t	new_wallet; 			//1��ʾ��Ǯ����0��ʾ��Ǯ��
		uint8_t	update;						//1��ʾ��Ҫ������0��ʾ����Ҫ����
		uint8_t	language;					//1��ʾӢ�ģ�0��ʾ����
		uint8_t count;						//��ַ����
		uint8_t	sn[12];						//��Ƭ��ΨһSN��
		uint8_t pin[8];
}SYSTEM_NEODUN;

//HID���ݽ����ṹ��
typedef struct
{
		uint32_t	dataLen;
		uint16_t 	packIndex;
		uint16_t  packCount;
		uint16_t 	notifySerial;
		uint16_t	reqSerial;
		uint8_t 	hashRecord[32];
}DATA_HID_RECORD;

//Ϊ��ַ��ʾ�����ڴ�
typedef struct
{
		uint8_t	hide;							//��������	
		uint8_t	len_name;					//��ַ���Ƴ���
		char 		name[7];					//��ַ����
		uint8_t	content[25];			//Base58ǰ�ĵ�ַ����
		uint8_t	address[40];			//Base58����ַ���ַ����
}ADDRESS;

//ϵͳ�����Ϣ,ȱʡֵΪ0
typedef struct
{
		uint8_t  count;
		uint16_t coin1;
		uint16_t version1;
		uint16_t coin2;
		uint16_t version2;
		uint16_t coin3;
		uint16_t version3;
		uint16_t coin4;
		uint16_t version4;
		uint16_t coin5;
		uint16_t version5;
}COIN;

enum TimerFunction
{
		OLED_INPUT_TIME = 0,
		KEY_TIME 				= 1,
};

extern volatile 	uint32_t 	system_base_time;
extern uint8_t					SysFlagType;
extern KEY_FLAG 				Key_Flag;
extern SET_FLAG 				Set_Flag;
extern SYSTEM_NEODUN 		Neo_System;
extern PASSPORT_FLAG 		Passport_Flag;
extern DATA_HID_RECORD	HidData;
extern ADDRESS					showaddress[5];
extern COIN							coinrecord;
extern volatile 	uint32_t 	moter_delay;
extern volatile 	uint8_t 	task_1s_flag;

extern volatile int hid_flag;
extern uint8_t hid_data[64];
extern int len_hid;

//����ASCII��
extern uint8_t ASC8X16[];
extern uint8_t HZ12X12_S[];
//����ͼƬ
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


void Sys_Data_Init(void);

#endif

