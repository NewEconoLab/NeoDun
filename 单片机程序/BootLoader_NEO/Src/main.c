/*------------------------------------------------File Info---------------------------------------------
** File Name: main.c
** Last modified Date:  2017-11-14
** Last Version: 
** Descriptions: 
**------------------------------------------------------------------------------------------------------
** Created By: 		 hkh
** Created date:   2017-11-14
** Version: 
** Descriptions: BootLoader NEODUN
**	
** 1、APP程序从扇区4开始存储，大小为80多KB，前面扇区0-3，总共64KB，留给BootLoader程序
** 2、APP程序存储在扇区4和扇区5，这两个扇区的大小为64+128KB
** 3、HID的速度改善，修改Polling Interval（轮询间隔）
** 4、用户在开机后，按下中心键进入程序升级模式，否则5s后进入正常程序
** 5、需要对APP程序进行设置，偏移地址，编译出bin文件
** 6、AW9136的灵敏度由寄存器THR2-THR4设置
** 7、扇区6开始地址存储开机密码，扇区7-扇区11，存储用户的地址和私钥对
********************************************************************************************************/
#include "main.h"
#include "stm32f4xx_hal.h"
#include "usb_device.h"
#include "aw9136.h"
#include "oled.h"
#include "iap.h"
#include "timer.h"
#include "stmflash.h"
#include "cmd.h"

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
RNG_HandleTypeDef hrng;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_RNG_Init(void);
static void GetPassportArray(int *data);

#define HID_REC_LEN 100*1024
u8 HID_RX_BUF[HID_REC_LEN] __attribute__ ((at(0X20005000)));//接收缓冲,最大USART_REC_LEN个字节,起始地址为0X20001000.    
u32 HID_RX_CNT = 0;

extern volatile unsigned char left_key_flag;
extern volatile unsigned char center_key_flag;
extern volatile unsigned char right_key_flag;
extern volatile int time_counter;
volatile int passport_flag = 0;//密码验证标志位
int num[9] = {1,2,3,4,5,6,7,8,9};//随机密码，生成数组

int main(void)
{
		u32 oldcount=0;	//老的USB接收数据值	
		u32 applenth=0;	//接收到的app代码长度
		u8 cancel_flag = 0;
	
		HAL_Init();
		SystemClock_Config();
		MX_GPIO_Init();
		MX_USART1_UART_Init();//打印信息
		MX_USB_DEVICE_Init();
		MX_USART2_UART_Init();//蓝牙串口
		TIM3_Init(5000-1,8400-1);//500ms进入一次中断
		MX_RNG_Init();	      
		OLED_Init();
		AW9136_Init();
//		HAL_Delay(5000);//等待AW9136初始化完成
		
		//开机显示随机9宫格密码
//		Asc8_16(0,0,"PassPort:");		
//		GetPassportArray(num);		
		
		//这两行代码为了调试，先不管密码
		passport_flag = 1;
//		HAL_Delay(3000);

		while(1)
		{						
				if(passport_flag)
				{						
						break;
				}
		}
		
		Fill_RAM(0x00);//清屏
		Asc12_24( 20,10 ,"Welcome Use NeoDun");
		Asc8_16(104,44,"Cancel");
		
		__HAL_RCC_TIM3_CLK_ENABLE();//开启定时器
		while(time_counter < 20)		//10s过后
		{		
				if(center_key_flag)
				{
						cancel_flag = 1;
						break;
				}
		}
		__HAL_RCC_TIM3_CLK_DISABLE();
		time_counter = 0;
	
		if(cancel_flag == 0)//没有检测到按键按下，直接跳转到APP程序
		{				
				iap_load_app(FLASH_APP1_ADDR);
		}
		cancel_flag = 0;
		
		Fill_RAM(0x00);//清屏
		Asc8_16(48,10,"Firmware Update Mode");
		Asc8_16(10,48,"updata");
		Asc8_16(112,48,"load");
		Asc8_16(206,48,"clear");
		
		//进入循环前，清除按键标志位
		left_key_flag = 0;
		center_key_flag = 0;
		right_key_flag = 0;
		
		while (1)
		{
				if(HID_RX_CNT)
				{
						if(oldcount==HID_RX_CNT)//新周期内,没有收到任何数据,认为本次数据接收完成.
						{
								applenth=HID_RX_CNT;
								oldcount=0;
								HID_RX_CNT=0;
								printf("用户程序接收完成!\r\n");
								printf("代码长度:%dBytes\r\n",applenth);
						}
						else 
								oldcount=HID_RX_CNT;			
				}
				HAL_Delay(1000);
				
				if(left_key_flag)//更新固件
				{
						iap_write_appbin(FLASH_APP1_ADDR,HID_RX_BUF,applenth);						
						Fill_Block(0,0,64,29,45);//清除这一行
						Asc8_16(76,29,"Update Finish");					
						left_key_flag = 0;
				}
				else if(center_key_flag)//加载固件
				{
						Fill_RAM(0x00);
						iap_load_app(FLASH_APP1_ADDR);
						center_key_flag = 0;
				}
				else if(right_key_flag)//清除固件
				{
						applenth=0;
						STMFLASH_Erase_Sectors(FLASH_SECTOR_4);
						STMFLASH_Erase_Sectors(FLASH_SECTOR_5);
					
						Fill_Block(0,0,64,29,45);//清除这一行
						Asc8_16(80,29,"Clear Finish");
						right_key_flag = 0;
				}	
		}
}

void GetPassportArray(int *data)
{
	uint32_t tmp = 0;
	int i = 0;
	int t = 0;
	
	HAL_RNG_GenerateRandomNumber(&hrng,&tmp);
	tmp =	tmp%9;
	if(tmp == 0)	tmp = i;
	t = data[i];
	data[i] = data[tmp];
	data[tmp] = t;
	
	Show_num(108,6,data[0],2,0);
	Show_num(124,6,data[1],2,0);
	Show_num(140,6,data[2],2,0);	
	Show_num(108,24,data[3],2,0);
	Show_num(124,24,data[4],2,0);
	Show_num(140,24,data[5],2,0);	
	Show_num(108,42,data[6],2,0);
	Show_num(124,42,data[7],2,0);
	Show_num(140,42,data[8],2,0);
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

/* RNG init function */
static void MX_RNG_Init(void)
{

  hrng.Instance = RNG;
  if (HAL_RNG_Init(&hrng) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* USART1 init function */
static void MX_USART1_UART_Init(void)
{

  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* USART2 init function */
static void MX_USART2_UART_Init(void)
{

  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
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

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4|GPIO_PIN_5, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0|GPIO_PIN_2|GPIO_PIN_4|GPIO_PIN_6, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);

  /*Configure GPIO pins : PA4 PA5 */
  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PD0 PD2 PD4 PD6 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_2|GPIO_PIN_4|GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pin : PB3 */
  GPIO_InitStruct.Pin = GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

void USB_DataReceiveHander(uint8_t *data,int len)
{
		int i = 0;
		int cmd = 0;
		int serialId = 0;		
		u32 passport[6] = {0,0,0,0,0,0};
		u32 get_passport[6];
		
		if(passport_flag)//通过密码验证
		{
				if(HID_RX_CNT<HID_REC_LEN)
				{
						for(i=0;i<len;i++)
						{
								HID_RX_BUF[i+HID_RX_CNT] = data[i];
						}
						HID_RX_CNT = HID_RX_CNT + len;
				}
		}
		else//没有通过密码验证的HID传输
		{
				printf("HID receive data:\r\n");
				PrintArray(data,len);
				if (crc_frame(data, len)) 
				{
						cmd = ReverseU16(ReadU16(data));
						serialId = ReadU16(data + 2);	
						printf("cmd = 0x%x\r\n",cmd);
						printf("serialId = 0x%x\r\n",serialId);
		
						switch (cmd) 
						{
							case CMD_SET_FIRSTPASSPORT://设置密码
								{
										memmove(passport,data+4,6);								//密码长度为6位
										STMFLASH_Erase_Sectors(FLASH_SECTOR_6);		//清除密码存储地址的值
										STMFLASH_Write(0x08040000,passport,6); 		//重新写入新的密码
										serialId = ReadU16(data + 2);							
										CreateDataQuest(CMD_SET_OK,serialId,data);//设置密码OK
										SendToPc(data);	
										break;
								}
								case CMD_VERIFY_PASSPORT://验证密码
								{		
										memcpy(get_passport,data+4,6);						//得到上位机传输的密码
										STMFLASH_Read(0x08040000,passport,6);			//读取之前设置的密码
										if(MemoryCompare(passport,get_passport,6))//比对密码
										{
												serialId = ReadU16(data + 2);
												CreateDataQuest(CMD_VERIFY_OK,serialId,data);//告知上位机密码验证通过
												SendToPc(data);
												passport_flag = 1;													 //置密码验证标志位
										}
										else
										{
												serialId = ReadU16(data + 2);
												CreateDataQuest(CMD_VERIFY_ERROR,serialId,data);//告知上位机密码验证未通过
												SendToPc(data);
												passport_flag = 0;															//不改变密码验证标志位
										}
										break;
								}
								case CMD_GET_ARRAY://上位机索取密码的序列
								{
										serialId = ReadU16(data + 2);
										CreateDataQuest(CMD_SEND_ARRAY,serialId,data);//返回给上位机密码序列
										memmove(data+4,num,9);
										SendToPc(data);											
										break;
								}
								default :
										printf("not handle CMD = %x\r\n",cmd);
								break;
						}
				} 
				else 
				{
						serialId = ReadU16(data + 2);
						CreateDataQuest(CMD_CRC_FAILED,serialId,data);//crc校验不通过
						SendToPc(data);			
						printf("crc error\r\n");
				}				
		}		
}

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
