#include "app_utils.h"
#include "crypto.h"
#include "HASH/SHA256/sha256.h"

extern RNG_HandleTypeDef hrng;
extern IWDG_HandleTypeDef hiwdg;

//Local variable
static const uint32_t  POLYNOMIAL = 0xEDB88320;
static uint8_t have_table = 0;
static uint32_t table[256];

static void make_table()    //表是全局的
{
		int i, j;
		have_table = 1;
		for (i = 0; i < 256; i++)
				for (j = 0, table[i] = i; j < 8; j++)
						table[i] = (table[i] >> 1) ^ ((table[i] & 1) ? POLYNOMIAL : 0);
}

uint32_t Utils_crc32(uint32_t crc, uint8_t* buff, int len)
{
		if (!have_table) make_table();
				crc = ~crc;
		for (int i = 0; i < len; i++)
				crc = (crc >> 8) ^ table[(crc ^ buff[i]) & 0xff];
		return ~crc;		
}

uint8_t	Utils_verifycrc(uint8_t data[],int len)
{
		uint32_t res = Utils_crc32(0, data, len - 2);	
		if ((data[len - 2] == (res & 0xff)) && (data[len - 1] == ((res >> 8) & 0xff))) 
		{
				return 1;
		} 
		else
		{
				return 0;	//校验失败
		}
}

uint16_t RandomInteger(void)
{
		uint32_t temp = HAL_GetTick() ;
		return (uint16_t)temp;
}

uint8_t		ArrayCompare(uint8_t* left, uint8_t* right, int len)
{
		for (int i = 0; i < len; i++) 
		{
				if (left[i] != right[i])
						return 0;
		}
		return 1;
}

void RandomArray(uint8_t *array,uint8_t len)
{
		uint8_t i;
		for(i=0;i<len;i++)
		{
				array[i] = (HAL_RNG_GetRandomNumber(&hrng))%256;
		}
}

void  SHA256_Data(uint8_t* input, uint32_t inLen, uint8_t* outPut, uint32_t outLen)
{
		int32_t outSize, retval;
		SHA256ctx_stt context_st;

		context_st.mFlags = E_HASH_DEFAULT;
		context_st.mTagSize = CRL_SHA256_SIZE;

		retval = SHA256_Init(&context_st);
		if (retval != HASH_SUCCESS) 
		{
#ifdef Debug_Print				
				printf("256init failed\r\n");
#endif			
		}

		retval = SHA256_Append(&context_st, input   , inLen);
		if (retval != HASH_SUCCESS) 
		{
#ifdef Debug_Print				
				printf("256 apd failed\r\n");
#endif
		}

		retval = SHA256_Finish(&context_st, outPut, &outSize);
		if (retval != HASH_SUCCESS) 
		{
#ifdef Debug_Print			
				printf("256 fnsh failed\r\n");
#endif
		}
}

void  PrintArray(uint8_t data[], int len) 
{
		for(int i = 0 ; i<len ; i ++)
		{
				printf("%02x ",data[i]);
		}
		printf("\r\n");
}

typedef  void (*iapfun)(void);
iapfun jump2app;
//跳转到指定应用程序段
//appxaddr:用户代码起始地址.
void jump_to_app(uint32_t appxaddr)
{
		//跳转之前失能所有的时钟和中断
		HAL_DeInit();
		HAL_NVIC_DisableIRQ(SysTick_IRQn);
		HAL_NVIC_DisableIRQ(USART2_IRQn);
		HAL_NVIC_DisableIRQ(EXTI2_IRQn);
		HAL_NVIC_DisableIRQ(OTG_FS_IRQn);
	
		if(((*(vu32*)appxaddr)&0x2FFE0000)==0x20000000)	//检查栈顶地址是否合法.
		{ 
				jump2app=(iapfun)*(vu32*)(appxaddr+4);		//用户代码区第二个字为程序开始地址(复位地址)		
				MSR_MSP(*(vu32*)appxaddr);								//初始化APP堆栈指针(用户代码区的第一个字用于存放栈顶地址)
				jump2app();																//跳转到APP.
		}
}

void IWDG_Feed(void)
{   
    HAL_IWDG_Refresh(&hiwdg); 	//喂狗
}

void Power_ON_BLE(void)
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
	HAL_Delay(100);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
}

void Power_OFF_BLE(void)
{
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);
	HAL_Delay(100);	
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET);
}
