#include "app_command.h"
#include "usbd_customhid.h"
#include "crypto.h"
#include "HASH/SHA256/sha256.h"
#include "stmflash.h"
#include "iap.h"
#include "app_ecdsa.h"
#include "oled.h"

//Global variable
extern uint8_t HID_RX_BUF[RECV_BIN_FILE_LEN] __attribute__ ((at(0X20003000)));
extern USBD_HandleTypeDef hUsbDeviceFS;
uint8_t Show_Wait_Pic = 0;
static const uint32_t  POLYNOMIAL = 0xEDB88320;
static uint8_t have_table = 0;
static uint32_t table[256];
static uint8_t public_key[64] =
{
		220,114,233,198,133,184, 81,202,202, 10, 23,234, 25,144, 45,196,
		 80, 41,188,166, 57,158,131,237,241,  7,175, 52, 57,203,166, 48,
		 77,129,140,102, 91, 55,100, 75, 22,199,102,163,254, 39,208,  8,
		242,115,116, 26,250, 72,  8,254, 61,123,213,224,137,112,238, 69	
};
BIN_FILE_INFO bin_file;
volatile uint8_t update_flag_failed = 0;
enum{
		BIN_FILE_TOO_BIG = 0,
		PAGE_SERIAL_ID_FAILED = 1,
		BIN_FILE_HASH_ERROR = 2,
		ECDSA_SIGNATURE_ERROR = 3,
		PAGE_RECV_ERROR = 4,
		PAGE_INDEX_ID_ERROR = 5
};
static uint16_t PageIndex = 0;
static uint8_t packIndexRecord[PACK_INDEX_SIZE];

void Deal_USB_ERROR(void)
{
		GPIO_InitTypeDef GPIO_InitStruct;
		GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_12;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11|GPIO_PIN_12, GPIO_PIN_RESET);		
}

void Update_Error_Deal(u8 error)
{
		Fill_RAM(0x00);
		switch (error)
		{
				case BIN_FILE_TOO_BIG:
						Asc8_16(52,24,"Update Error 401!!!");
				break;
				case PAGE_SERIAL_ID_FAILED:
						Asc8_16(52,24,"Update Error 402!!!");
				break;
				case BIN_FILE_HASH_ERROR:
						Asc8_16(52,24,"Update Error 403!!!");
				break;
				case ECDSA_SIGNATURE_ERROR:
						Asc8_16(52,24,"Update Error 404!!!");
				break;
				case PAGE_RECV_ERROR:
						Asc8_16(52,24,"Update Error 405!!!");
				break;
				case PAGE_INDEX_ID_ERROR:
						Asc8_16(52,24,"Update Error 406!!!");
				break;
				default:
						Asc8_16(52,24,"Update Error 400!!!");
				break;
		}
		HAL_Delay(5000);
		update_flag_failed = 1;
}

uint8_t CommpareArray(uint8_t *left,uint8_t *right, int size)
{
		for(int i = 0 ; i < size ; i ++)
		{
				if( left[i]  != right[i] )
				{
						return 0;
				}
		}
		return 1;
}

void Hid_Recv_0101_Rp(uint8_t status)
{
		uint32_t crc = 0;
		uint8_t data_rp[64];
		memset(data_rp,0,64);
		
		data_rp[0] = 0x01;
		bin_file.reqSerial = Get_Serial_ID();
		data_rp[2] = bin_file.reqSerial & 0xff;
		data_rp[3] = (bin_file.reqSerial >> 8) & 0xff;
	
		bin_file.packIndex = (bin_file.size + 50 - 1)/50;
		
		if(status)
		{
				data_rp[1] = 0x10;
				data_rp[4] = 0;
				data_rp[5] = 0;
				data_rp[6] = bin_file.packIndex & 0xff;
				data_rp[7] = (bin_file.packIndex >> 8) & 0xff;
				memcpy(data_rp+8,bin_file.hash_tran,32);
				Show_Wait_Pic = 1;
		}
		else
				data_rp[1] = 0xe0;
		
		crc = command_crc32(0,data_rp,62);
		data_rp[62] = crc & 0xff;
		data_rp[63] = (crc >> 8) & 0xff;
		USBHIDSend(data_rp,64);
}

void Hid_Recv_01a3_Rp(uint8_t status,uint8_t *hash)
{
		uint32_t crc = 0;
		uint8_t data_rp[64];
		memset(data_rp,0,64);
		
		data_rp[0] = 0x01;
		if(status)
		  data_rp[1] = 0x11;
		else
			data_rp[1] = 0x12;	
		data_rp[2] = bin_file.notifySerial & 0xff;
		data_rp[3] = (bin_file.notifySerial >> 8) & 0xff;	
	
		data_rp[4] = 0;
		data_rp[5] = 0;
		data_rp[6] = bin_file.packIndex & 0xff;
		data_rp[7] = (bin_file.packIndex >> 8) & 0xff;		
		
		memmove(data_rp+8,hash,32);
		
		crc = command_crc32(0,data_rp,62);
		data_rp[62] = crc & 0xff;
		data_rp[63] = (crc >> 8) & 0xff;
		USBHIDSend(data_rp,64);
}

void Hid_Recv_020b_Rp(uint8_t status,uint16_t serialID)
{
		uint32_t crc = 0;
		uint8_t data_rp[64];
		memset(data_rp,0,64);
		
		data_rp[0] = 0x02;
		if(status)
		  data_rp[1] = 0xc3;
		else
			data_rp[1] = 0xc4;
		data_rp[2] = serialID & 0xff;
		data_rp[3] = (serialID >> 8) & 0xff;
		
		crc = command_crc32(0,data_rp,62);
		data_rp[62] = crc & 0xff;
		data_rp[63] = (crc >> 8) & 0xff;
		USBHIDSend(data_rp,64);		
}

void Hid_Recv_020c_Rp(uint8_t status,uint16_t serialID)
{
		uint32_t crc = 0;
		uint8_t data_rp[64];
		memset(data_rp,0,64);
		
		data_rp[0] = 0x02;
		if(status)
		  data_rp[1] = 0xc5;
		else
			data_rp[1] = 0xc6;
		data_rp[2] = serialID & 0xff;
		data_rp[3] = (serialID >> 8) & 0xff;
		
		crc = command_crc32(0,data_rp,62);
		data_rp[62] = crc & 0xff;
		data_rp[63] = (crc >> 8) & 0xff;
		USBHIDSend(data_rp,64);
}

void Hid_Data_Analysis(uint8_t data[],int len)
{
		int cmd = 0;
		uint16_t 	SerialID = 0;
		uint8_t 	datasharesult[32];
		uint16_t 	Pc_pageIndex = 0;
	
		if(command_verifycrc(data,len))
		{
				cmd = (data[0] << 8) | data[1];
				switch (cmd)
				{
						case 0x0101:
						{
								bin_file.notifySerial = data[2] | (data[3] << 8);
								bin_file.size = data[4] | (data[5] << 8) | (data[6] << 16) | (data[7] << 24);
								printf("bin file size:%d\n",bin_file.size);
								//将统计接收包的信息清空
								PageIndex = 0;
								memset(packIndexRecord,0,PACK_INDEX_SIZE);
								memmove(bin_file.hash_tran,data+8,32);
								if(bin_file.size < RECV_BIN_FILE_LEN)
								{
										Hid_Recv_0101_Rp(1);
								}
								else
								{
										Hid_Recv_0101_Rp(0);
										Update_Error_Deal(BIN_FILE_TOO_BIG);										
								}
						}
						break;
						case 0x01a2:
						{
								Pc_pageIndex = data[4] | (data[5] << 8);
								if(Pc_pageIndex > MAX_Page_Index)
										Update_Error_Deal(PAGE_INDEX_ID_ERROR);
								SerialID = data[2] | (data[3] << 8);
								if(SerialID == bin_file.reqSerial)
								{
										memmove(HID_RX_BUF + PageIndex * Page_Per_Size,data+6,Page_Per_Size);
										packIndexRecord[PageIndex] = 1;
										PageIndex++;
								}
								else
								{
										printf("error SerialID\n");
										Update_Error_Deal(PAGE_SERIAL_ID_FAILED);
								}
						}
						break;
						case 0x01a3:
						{
								SHA256_Data(HID_RX_BUF,bin_file.size,datasharesult,32);
								if(isAllPackGot())
								{
										if(CommpareArray(datasharesult,bin_file.hash_tran,32))
										{
												bin_file.Len_sign = HID_RX_BUF[0];
												memcpy(bin_file.signature,HID_RX_BUF+1,bin_file.Len_sign);														 									//得到hash的签名
												SHA256_Data(HID_RX_BUF+bin_file.Len_sign+1,bin_file.size-bin_file.Len_sign - 1,bin_file.hash_actual,32);//得到实际的hash
												Hid_Recv_01a3_Rp(1,datasharesult);
										}
										else
										{
												memset(&HID_RX_BUF,0,sizeof(HID_RX_BUF));
												Hid_Recv_01a3_Rp(0,datasharesult);
												HAL_Delay(500);
												Update_Error_Deal(BIN_FILE_HASH_ERROR);
										}
								}
								else
								{
										memset(&HID_RX_BUF,0,sizeof(HID_RX_BUF));
										Hid_Recv_01a3_Rp(0,datasharesult);
										HAL_Delay(500);
										Update_Error_Deal(PAGE_RECV_ERROR);
								}
						}
						break;
						case 0x020b:
						{
								SerialID = data[2] | (data[3] << 8);								
								if(Alg_ECDSASignVerify(public_key,&bin_file,bin_file.hash_actual))//ECDSA签名认证成功
								{
										STMFLASH_Erase_Sectors(FLASH_SECTOR_4);
										STMFLASH_Erase_Sectors(FLASH_SECTOR_5);
										iap_write_appbin(FLASH_APP1_ADDR,HID_RX_BUF+bin_file.Len_sign+1,bin_file.size-bin_file.Len_sign-1);
										Hid_Recv_020b_Rp(1,SerialID);
										HAL_Delay(500);
										Deal_USB_ERROR();//重新配置下USB
										Fill_RAM(0x00);
										iap_load_app(FLASH_APP1_ADDR);								
								}
								else//ECDSA签名认证失败
								{
										memset(&HID_RX_BUF,0,sizeof(HID_RX_BUF));
										Hid_Recv_020b_Rp(0,SerialID);
										Update_Error_Deal(ECDSA_SIGNATURE_ERROR);						
								}
						}
						break;
						default:
								printf("Unknow Cmd\n");
						break;
				}
		}
		else
		{
				printf("Crc Error!!!\n");
		}		
}

uint8_t isAllPackGot(void)
{
		uint16_t i;
		uint8_t value = 1;
		uint8_t index = 0;
		uint16_t err_array[20];	
		
		memset(err_array,0,20);
		for(i=0;i<bin_file.packIndex;i++)
		{
				if(!packIndexRecord[i])
				{
						err_array[index++] = i;
						value = 0;
				}
		}
		
		if(value == 0)
		{
				printf("err packid:");
				for(i=0;i<20;i++)
				{
						if(err_array[i])
								printf(" %d",err_array[i]);
				}
				printf("\n");
		}
		return value;
}

uint16_t Get_Serial_ID(void)
{
		uint32_t temp = HAL_GetTick();
		return (uint16_t)temp;
}

static void make_table()    //表是全局的
{
		int i, j;
		have_table = 1;
		for (i = 0; i < 256; i++)
				for (j = 0, table[i] = i; j < 8; j++)
						table[i] = (table[i] >> 1) ^ ((table[i] & 1) ? POLYNOMIAL : 0);
}

uint32_t command_crc32(uint32_t crc, uint8_t* buff, int len)
{
		if (!have_table) make_table();
				crc = ~crc;
		for (int i = 0; i < len; i++)
				crc = (crc >> 8) ^ table[(crc ^ buff[i]) & 0xff];
		return ~crc;		
}

uint8_t	command_verifycrc(uint8_t data[],int len)
{
		if (len != 64)
				return 0;
		uint32_t res = command_crc32(0, data, len - 2);	
		if ((data[len - 2] == (res & 0xff)) && (data[len - 1] == ((res >> 8) & 0xff))) 
		{
				return 1;
		} 
		else
		{
				return 0;	//校验失败
		}
}

void USBHIDSend(uint8_t *data,int len)
{
		USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS,data,len);
		USBD_CUSTOM_HID_HandleTypeDef     *hhid = (USBD_CUSTOM_HID_HandleTypeDef*)&hUsbDeviceFS;
		while(hhid->state == CUSTOM_HID_BUSY);
}

void Printf_array(uint8_t data[],int len)
{
		int i;
		for(i=0;i<len;i++)
		{
				printf(" 0x%x",data[i]);
		}
		printf("\n");
}

void SHA256_Data(uint8_t* input, uint32_t inLen, uint8_t* outPut, uint32_t outLen)
{
		int32_t outSize, retval;
		SHA256ctx_stt context_st;

		context_st.mFlags = E_HASH_DEFAULT;
		context_st.mTagSize = CRL_SHA256_SIZE;

		retval = SHA256_Init(&context_st);
		if (retval != HASH_SUCCESS) 
		{
				printf("256init failed\r\n");
		}

		retval = SHA256_Append(&context_st, input   , inLen);
		if (retval != HASH_SUCCESS) 
		{
				printf("256 apd failed\r\n");
		}

		retval = SHA256_Finish(&context_st, outPut, &outSize);
		if (retval != HASH_SUCCESS) 
		{
				printf("256 fnsh failed\r\n");
		}
}

void SHA256_test(void)
{
		uint8_t data_sha[32] = 
		{
			0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
			16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
		};
		uint8_t aes_out[32];		
		SHA256_Data(data_sha,32,aes_out,32);
		Printf_array(aes_out,32);
}





