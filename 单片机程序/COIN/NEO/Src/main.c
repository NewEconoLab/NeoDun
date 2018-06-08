#include "main.h"
#include "stm32f4xx_hal.h"
#include <string.h>
#include <stdio.h>
#include "sys.h"
#include "ecdsa.h"
#include "showsign.h"
#include "getaddress.h" 
#include "stmflash.h"

CRC_HandleTypeDef hcrc;
RNG_HandleTypeDef hrng;
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_CRC_Init(void);
static void MX_RNG_Init(void);

//#define Debug
//为了调试
//#define FLASH_NEODUN_ADDRESS			0x08000000
//#define FLASH_PACK_ADDRESS				0x08080000
//#define FLASH_RESULT_sign_ADDRESS	0x0808F000
//#define FLASH_RESULT_alg_ADDRESS	0x0808F800

#define FLASH_NEO_ADDRESS					0x08060000
#define FLASH_NEODUN_ADDRESS			0x08020000
#define FLASH_PACK_ADDRESS				0x08010000
#define FLASH_RESULT_sign_ADDRESS	0x0801F000
#define FLASH_RESULT_alg_ADDRESS	0x0801F800

uint8_t privateKey_flash[32];
uint8_t data_pack[512];
uint32_t pack_len = 0;

typedef  void (*iapfun)(void);
iapfun jump2app;
//跳转到指定应用程序段
//appxaddr:用户代码起始地址.
void jump_to_app(uint32_t appxaddr)
{
		//跳转之前失能所有的时钟和中断
		HAL_DeInit();
		HAL_NVIC_DisableIRQ(SysTick_IRQn);
		if(((*(vu32*)appxaddr)&0x2FFE0000)==0x20000000)	//检查栈顶地址是否合法.
		{ 
				jump2app=(iapfun)*(vu32*)(appxaddr+4);		//用户代码区第二个字为程序开始地址(复位地址)		
				MSR_MSP(*(vu32*)appxaddr);								//初始化APP堆栈指针(用户代码区的第一个字用于存放栈顶地址)
				jump2app();																//跳转到APP.
		}
}

int main(void)
{
		SCB->VTOR = FLASH_BASE | 0x60000;//设置偏移量
		//清除程序跳转，残留的FLASH标识
		__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP|FLASH_FLAG_OPERR|FLASH_FLAG_WRPERR|FLASH_FLAG_PGAERR
													|FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);
	
		uint8_t resultsignRecord[98];
		int 		len_sign_alg = 0;
		uint8_t result_SignData[64];
		SIGN_Out_Para Sign;
		uint8_t PublicKey_Flash[65];
		uint8_t PubKey_Flash[33];

		//硬件初始化
		HAL_Init();

		SystemClock_Config();
		MX_GPIO_Init();
		MX_CRC_Init();
		MX_RNG_Init();

#ifdef Debug
		Test_Ecc_Sign_Data();
#endif

		//数据初始化
		memset(&data_pack,0,512);
		pack_len = STMFLASH_ReadWord(FLASH_PACK_ADDRESS);
		if(pack_len == 0xffffffff)
		{
				STMFLASH_WriteWord(FLASH_RESULT_sign_ADDRESS,0);
				jump_to_app(FLASH_NEODUN_ADDRESS);//出错跳回大厅APP		
		}
	
		STMFLASH_Read(FLASH_PACK_ADDRESS+4,(uint32_t*)privateKey_flash,32);
		STMFLASH_Read(FLASH_PACK_ADDRESS+36,(uint32_t*)data_pack,pack_len);
		memset(&Sign,0,sizeof(Sign));
		memset(&result_SignData,0,64);
		memset(&resultsignRecord,0,98);
		
		//解析数据包
		if(Alg_ShowSignData(data_pack,pack_len,&Sign))
		{
				STMFLASH_WriteWord(FLASH_RESULT_sign_ADDRESS,0);
				jump_to_app(FLASH_NEODUN_ADDRESS);//出错跳回大厅APP
		}
		//签名数据包
		if(Alg_ECDSASignData(data_pack,pack_len,result_SignData,&len_sign_alg,privateKey_flash))
		{
				STMFLASH_WriteWord(FLASH_RESULT_sign_ADDRESS,0);
				jump_to_app(FLASH_NEODUN_ADDRESS);//出错跳回大厅APP
		}
		
		//组合最终的数据
		Alg_GetPublicFromPrivate(privateKey_flash,PublicKey_Flash,1);
		memmove(PubKey_Flash,PublicKey_Flash,33);
		resultsignRecord[0] = 33;
		memmove(resultsignRecord+1,PubKey_Flash,33);
		memmove(resultsignRecord+34,result_SignData,64);
		
		//将结果写入FLASH
		{
				//记录解释签名结果
				STMFLASH_WriteByte(FLASH_RESULT_sign_ADDRESS,Sign.type);
				STMFLASH_WriteByte(FLASH_RESULT_sign_ADDRESS+1,Sign.version);
				STMFLASH_WriteByte(FLASH_RESULT_sign_ADDRESS+2,Sign.coin);
				STMFLASH_WriteByte(FLASH_RESULT_sign_ADDRESS+3,Sign.countAttributes);
				STMFLASH_WriteByte(FLASH_RESULT_sign_ADDRESS+4,Sign.countInputs);
				STMFLASH_WriteByte(FLASH_RESULT_sign_ADDRESS+5,Sign.countOutputs);
				STMFLASH_Write_ByteArray(FLASH_RESULT_sign_ADDRESS+6,Sign.address[0],25);
				STMFLASH_Write_ByteArray(FLASH_RESULT_sign_ADDRESS+31,Sign.address[1],25);
				STMFLASH_Write_ByteArray(FLASH_RESULT_sign_ADDRESS+56,Sign.assetid[0],25);
				STMFLASH_Write_ByteArray(FLASH_RESULT_sign_ADDRESS+88,Sign.assetid[1],25);
				STMFLASH_WriteByte(FLASH_RESULT_sign_ADDRESS+120,(uint8_t)Sign.money[0]);
				STMFLASH_WriteByte(FLASH_RESULT_sign_ADDRESS+121,(uint8_t)(Sign.money[0]>>8));
				STMFLASH_WriteByte(FLASH_RESULT_sign_ADDRESS+122,(uint8_t)(Sign.money[0]>>16));
				STMFLASH_WriteByte(FLASH_RESULT_sign_ADDRESS+123,(uint8_t)(Sign.money[0]>>24));
				STMFLASH_WriteByte(FLASH_RESULT_sign_ADDRESS+124,(uint8_t)(Sign.money[0]>>32));
				STMFLASH_WriteByte(FLASH_RESULT_sign_ADDRESS+125,(uint8_t)(Sign.money[0]>>40));
				STMFLASH_WriteByte(FLASH_RESULT_sign_ADDRESS+126,(uint8_t)(Sign.money[0]>>48));
				STMFLASH_WriteByte(FLASH_RESULT_sign_ADDRESS+127,(uint8_t)(Sign.money[0]>>56));
				STMFLASH_WriteByte(FLASH_RESULT_sign_ADDRESS+128,(uint8_t)Sign.money[1]);
				STMFLASH_WriteByte(FLASH_RESULT_sign_ADDRESS+129,(uint8_t)(Sign.money[1]>>8));
				STMFLASH_WriteByte(FLASH_RESULT_sign_ADDRESS+130,(uint8_t)(Sign.money[1]>>16));
				STMFLASH_WriteByte(FLASH_RESULT_sign_ADDRESS+131,(uint8_t)(Sign.money[1]>>24));
				STMFLASH_WriteByte(FLASH_RESULT_sign_ADDRESS+132,(uint8_t)(Sign.money[1]>>32));
				STMFLASH_WriteByte(FLASH_RESULT_sign_ADDRESS+133,(uint8_t)(Sign.money[1]>>40));
				STMFLASH_WriteByte(FLASH_RESULT_sign_ADDRESS+134,(uint8_t)(Sign.money[1]>>48));
				STMFLASH_WriteByte(FLASH_RESULT_sign_ADDRESS+135,(uint8_t)(Sign.money[1]>>56));			
		}
		STMFLASH_Write_ByteArray(FLASH_RESULT_alg_ADDRESS,resultsignRecord,98);	//记录签名数据结果
		
		//回跳到NEODUN
		jump_to_app(FLASH_NEODUN_ADDRESS);//跳回大厅APP
		while(1)
		{
		
		}
}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

    /**Configure the main internal regulator output voltage 
    */
  __HAL_RCC_PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* CRC init function */
static void MX_CRC_Init(void)
{

  hcrc.Instance = CRC;
  if (HAL_CRC_Init(&hcrc) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* RNG init function */
static void MX_RNG_Init(void)
{

  hrng.Instance = RNG;
  if (HAL_RNG_Init(&hrng) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
static void MX_GPIO_Init(void)
{

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void _Error_Handler(char * file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1) 
  {
  }
  /* USER CODE END Error_Handler_Debug */ 
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
