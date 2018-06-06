#ifndef MAIN_DEFINE_H
#define MAIN_DEFINE_H

#include <stdint.h>
#include "stm32f4xx_hal.h"


//#define Debug_Print
#define RECV_BIN_FILE_LEN		60*1024
#define Page_Per_Size				50
#define MAX_Page_Index			RECV_BIN_FILE_LEN/Page_Per_Size
#define PACK_INDEX_SIZE			((RECV_BIN_FILE_LEN-1)/50 + 1)
#define MOTOR_TIME  				80
#define SLOT_FLAG						14
#define SLOT_SECRET					15

typedef union
{
		struct
		{
				volatile uint8_t key_state:1;	//���󰴵ı�־λ
				volatile uint8_t left:1;			//�󰴼���ʶ
				volatile uint8_t middle:1;	  //�м䰴����ʶ
				volatile uint8_t right:1;			//�ұ߰�����ʶ
		}flag;
		uint8_t data;
}SIGN_KEY_FLAG;


typedef union
{
		struct
		{		
				uint8_t	new_wallet:1; 			//1��ʾ��Ǯ����0��ʾ��Ǯ��
				uint8_t	update:1;						//1��ʾ��Ҫ������0��ʾ����Ҫ����
				uint8_t	language:1;					//1��ʾӢ�ģ�0��ʾ����
				uint8_t update_flag_failed:1;
		}flag;
		uint8_t data;
}BOOT_SYS_FLAG;


typedef struct
{
		uint32_t 			size;
		uint32_t 			packIndex;
		uint8_t 			hash_tran[32];
		uint16_t 			reqSerial;
		uint16_t 			notifySerial;		
		uint8_t 			signature[64];
		uint8_t 			Len_sign;
}BIN_FILE_INFO;


enum
{
		PAGE_TOO_BIG_ERROR		 	= 1,
		PAGE_SERIALID_ERROR 		= 2,
		PAGE_HASH_ERROR 				= 3,
		PAGE_SIGNATURE_ERROR 		= 4,
		PAGE_RECV_ERROR 				= 5,
		PAGE_INDEX_ID_ERROR 		= 6
};

extern uint8_t gImage_wait[128];


extern BIN_FILE_INFO Bin_File;
extern SIGN_KEY_FLAG Key_Flag;
extern BOOT_SYS_FLAG BootFlag;
extern uint8_t HID_RX_BUF[RECV_BIN_FILE_LEN] __attribute__ ((at(0X20001000)));
extern volatile uint8_t touch_motor_flag;    //1��ʾ���������񶯣�0��ʾ�ر���

void DATA_Init(void);

#endif





