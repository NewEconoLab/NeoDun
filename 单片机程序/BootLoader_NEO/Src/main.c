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
** 1��APP���������4��ʼ�洢����СΪ80��KB��ǰ������0-3���ܹ�64KB������BootLoader����
** 2��APP����洢������4������5�������������Ĵ�СΪ64+128KB
** 3��HID���ٶȸ��ƣ��޸�Polling Interval����ѯ�����
** 4���û��ڿ����󣬰������ļ������������ģʽ������5s�������������
** 5����Ҫ��APP����������ã�ƫ�Ƶ�ַ�������bin�ļ�
** 6��AW9136���������ɼĴ���THR2-THR4����
** 7������6��ʼ��ַ�洢�������룬����7-����11���洢�û��ĵ�ַ��˽Կ��
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
u8 HID_RX_BUF[HID_REC_LEN] __attribute__ ((at(0X20005000)));//���ջ���,���USART_REC_LEN���ֽ�,��ʼ��ַΪ0X20001000.    
u32 HID_RX_CNT = 0;

extern volatile unsigned char left_key_flag;
extern volatile unsigned char center_key_flag;
extern volatile unsigned char right_key_flag;
extern volatile int time_counter;
volatile int passport_flag = 0;//������֤��־λ
int num[9] = {1,2,3,4,5,6,7,8,9};//������룬��������

int main(void)
{
		u32 oldcount=0;	//�ϵ�USB��������ֵ	
		u32 applenth=0;	//���յ���app���볤��
		u8 cancel_flag = 0;
	
		HAL_Init();
		SystemClock_Config();
		MX_GPIO_Init();
		MX_USART1_UART_Init();//��ӡ��Ϣ
		MX_USB_DEVICE_Init();
		MX_USART2_UART_Init();//��������
		TIM3_Init(5000-1,8400-1);//500ms����һ���ж�
		MX_RNG_Init();	      
		OLED_Init();
		AW9136_Init();
//		HAL_Delay(5000);//�ȴ�AW9136��ʼ�����
		
		//������ʾ���9��������
//		Asc8_16(0,0,"PassPort:");		
//		GetPassportArray(num);		
		
		//�����д���Ϊ�˵��ԣ��Ȳ�������
		passport_flag = 1;
//		HAL_Delay(3000);

		while(1)
		{						
				if(passport_flag)
				{						
						break;
				}
		}
		
		Fill_RAM(0x00);//����
		Asc12_24( 20,10 ,"Welcome Use NeoDun");
		Asc8_16(104,44,"Cancel");
		
		__HAL_RCC_TIM3_CLK_ENABLE();//������ʱ��
		while(time_counter < 20)		//10s����
		{		
				if(center_key_flag)
				{
						cancel_flag = 1;
						break;
				}
		}
		__HAL_RCC_TIM3_CLK_DISABLE();
		time_counter = 0;
	
		if(cancel_flag == 0)//û�м�⵽�������£�ֱ����ת��APP����
		{				
				iap_load_app(FLASH_APP1_ADDR);
		}
		cancel_flag = 0;
		
		Fill_RAM(0x00);//����
		Asc8_16(48,10,"Firmware Update Mode");
		Asc8_16(10,48,"updata");
		Asc8_16(112,48,"load");
		Asc8_16(206,48,"clear");
		
		//����ѭ��ǰ�����������־λ
		left_key_flag = 0;
		center_key_flag = 0;
		right_key_flag = 0;
		
		while (1)
		{
				if(HID_RX_CNT)
				{
						if(oldcount==HID_RX_CNT)//��������,û���յ��κ�����,��Ϊ�������ݽ������.
						{
								applenth=HID_RX_CNT;
								oldcount=0;
								HID_RX_CNT=0;
								printf("�û�����������!\r\n");
								printf("���볤��:%dBytes\r\n",applenth);
						}
						else 
								oldcount=HID_RX_CNT;			
				}
				HAL_Delay(1000);
				
				if(left_key_flag)//���¹̼�
				{
						iap_write_appbin(FLASH_APP1_ADDR,HID_RX_BUF,applenth);						
						Fill_Block(0,0,64,29,45);//�����һ��
						Asc8_16(76,29,"Update Finish");					
						left_key_flag = 0;
				}
				else if(center_key_flag)//���ع̼�
				{
						Fill_RAM(0x00);
						iap_load_app(FLASH_APP1_ADDR);
						center_key_flag = 0;
				}
				else if(right_key_flag)//����̼�
				{
						applenth=0;
						STMFLASH_Erase_Sectors(FLASH_SECTOR_4);
						STMFLASH_Erase_Sectors(FLASH_SECTOR_5);
					
						Fill_Block(0,0,64,29,45);//�����һ��
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
		
		if(passport_flag)//ͨ��������֤
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
		else//û��ͨ��������֤��HID����
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
							case CMD_SET_FIRSTPASSPORT://��������
								{
										memmove(passport,data+4,6);								//���볤��Ϊ6λ
										STMFLASH_Erase_Sectors(FLASH_SECTOR_6);		//�������洢��ַ��ֵ
										STMFLASH_Write(0x08040000,passport,6); 		//����д���µ�����
										serialId = ReadU16(data + 2);							
										CreateDataQuest(CMD_SET_OK,serialId,data);//��������OK
										SendToPc(data);	
										break;
								}
								case CMD_VERIFY_PASSPORT://��֤����
								{		
										memcpy(get_passport,data+4,6);						//�õ���λ�����������
										STMFLASH_Read(0x08040000,passport,6);			//��ȡ֮ǰ���õ�����
										if(MemoryCompare(passport,get_passport,6))//�ȶ�����
										{
												serialId = ReadU16(data + 2);
												CreateDataQuest(CMD_VERIFY_OK,serialId,data);//��֪��λ��������֤ͨ��
												SendToPc(data);
												passport_flag = 1;													 //��������֤��־λ
										}
										else
										{
												serialId = ReadU16(data + 2);
												CreateDataQuest(CMD_VERIFY_ERROR,serialId,data);//��֪��λ��������֤δͨ��
												SendToPc(data);
												passport_flag = 0;															//���ı�������֤��־λ
										}
										break;
								}
								case CMD_GET_ARRAY://��λ����ȡ���������
								{
										serialId = ReadU16(data + 2);
										CreateDataQuest(CMD_SEND_ARRAY,serialId,data);//���ظ���λ����������
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
						CreateDataQuest(CMD_CRC_FAILED,serialId,data);//crcУ�鲻ͨ��
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
