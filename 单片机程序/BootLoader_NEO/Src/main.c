/*------------------------------------------------File Info---------------------------------------------
** File Name: main.c
** Created By: 		 hkh
** Last Version: 
** Descriptions: 
********************************************************************************************************/
#include "main.h"
#include "stm32f4xx_hal.h"
#include "usb_device.h"
#include "aw9136.h"
#include "oled.h"
#include "iap.h"
#include "stmflash.h"
#include "atsha204a.h"
#include "hw_config.h"
#include "main_define.h"
#include "app_interface.h"
#include "bmp.h"
#include "usbd_customhid.h"
#include "app_command.h"

UART_HandleTypeDef huart1;
RNG_HandleTypeDef hrng;
CRC_HandleTypeDef hcrc;

static void BSP_Init(void);
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_CRC_Init(void);
static void MX_RNG_Init(void);
static void MX_USART1_UART_Init(void);

static uint8_t	usb_init_flag = 0;
static uint8_t	aw9136_init_flag = 0;
static uint8_t	slot_data_read[32];

int main(void)
{
		//1 清除程序跳转，残留的FLASH标识
		__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP|FLASH_FLAG_OPERR|FLASH_FLAG_WRPERR|FLASH_FLAG_PGAERR
													|FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);
			
		//2 数据初始化
		DATA_Init();
		
		//3 硬件初始化
		BSP_Init();
	
		//4 加密芯片读取
		if(ReadAT204Flag(&BootFlag)==0)
		{
				Fill_RAM(0x00);
				Asc8_16(60,24,"ATSHA204 ERROR!!!");
				HAL_Delay(10000);
				return 0;
		}
		
		//5 调试和升级选择
		if(Have_App() == 0)       			//不存在APP程序
		{
				if(usb_init_flag == 0)
				{
						usb_init_flag = 1;
						MX_USB_DEVICE_Init();			//USB初始化			
				}			
				if(aw9136_init_flag == 0)					
				{
						aw9136_init_flag = 1;
						AW9136_Init();					//触摸按键
						HAL_Delay(500);
				}
				Fill_RAM(0x00);							//清屏
				Show_HZ12_12(80,7,0,0,5);		//没有安装程序
				Show_HZ12_12(80,26,0,6,9);	//是否进行
				Show_HZ12_12(144,26,0,2,3); //安装
				Show_HZ12_12(35,45,0,7,7);	//否
				Show_HZ12_12(120,45,0,6,6); //是
				Show_HZ12_12(205,45,0,7,7); //否
				Key_Control(1);							//清空按键标志位，开启按键有效
				while(1)
				{
						if(Key_Flag.flag.middle)
						{
								Key_Flag.flag.middle = 0;
								BootFlag.flag.update = 1;
								Key_Control(0);
								break;
						}
						if(Key_Flag.flag.left||Key_Flag.flag.right)
						{
								Key_Flag.flag.left  = 0;
								Key_Flag.flag.right = 0;					
								if(NEO_Test())
								{
										Key_Control(0);
										Asc8_16(88,10,"Test OK !!");
										Asc8_16(20,36,"Next ,Please Check USB Port");
										HAL_Delay(30000);
								}
								else
								{
										Key_Control(0);	
										Asc8_16(76,24,"Test Error !!");
								}
								HAL_Delay(3000);
								Fill_RAM(0x00);
								return 0;
						}
				}
		}
		
		//6 升级处理
		if(BootFlag.flag.update)//需要升级
		{
				if(usb_init_flag == 0)
				{
						usb_init_flag = 1;
						MX_USB_DEVICE_Init();			//USB初始化			
				}			
				//清除升级标志			
				BootFlag.flag.update = 0;
				ATSHA_read_data_slot(SLOT_FLAG,slot_data_read);
				slot_data_read[1] = 0;
				ATSHA_write_data_slot(SLOT_FLAG,0,slot_data_read,32);
				if(aw9136_init_flag == 0)
				{
						aw9136_init_flag = 1;
						AW9136_Init();					//触摸按键
				}
				Fill_RAM(0x00);
				if(BootFlag.flag.language == 0)
						Show_HZ12_12(96,26,0,10,13);	//等待更新
				else
				{
						Asc8_16(100,24,"Waiting");
				}			
				memset(&HID_RX_BUF,0,RECV_BIN_FILE_LEN);								
				//清除扇区，并回复上位机请求更新
				STMFLASH_Erase_Sectors(FLASH_SECTOR_5);
				STMFLASH_Erase_Sectors(FLASH_SECTOR_6);
				HAL_Delay(3000);
				Hid_Need_Updata_Rp();
				while(1)
				{
						if(hid_index_read != hid_index_write)				//收到HID数据包
						{
								Hid_Data_Analysis(hid_recv_data[hid_index_read].data,hid_recv_data[hid_index_read].len);
								memset(hid_recv_data[hid_index_read].data,0,64);
								hid_index_read++;
								if(hid_index_read == hid_index_write)
								{
									hid_index_write = 0;
									hid_index_read = 0;
								}
						}						

						if(BootFlag.flag.update_flag_failed)//一旦检测到更新失败，则退出程序
						{
								Fill_RAM(0x00);
								__set_FAULTMASK(1);
								NVIC_SystemReset();
						}
				}
		}
		//7 大厅app已安装，且不需要升级，则直接进入大厅app
		iap_load_app(FLASH_APP0_ADDR);
		return 0;
}

static void BSP_Init(void)
{
		HAL_Init();								//中断分组2
		SystemClock_Config(); 		//系统时钟168MHz
		MX_GPIO_Init();						//IO口初始化
		MX_CRC_Init();						//CRC校验
		MX_RNG_Init();						//RNG随机数
		Center_button_init();			//中间按钮初始化	
		MX_USART1_UART_Init();		//打印信息串口
		ATSHA204_Init();					//加密芯片初始化
//		MX_USB_DEVICE_Init();			//USB初始化		
		OLED_Init();							//OLED初始化	
		Show_Pattern(&gImage_logo[0],26,37,8,56);//开机logo
		clearArea(104,56,48,1);
		
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
		
		//Motor
		GPIO_InitStruct.Pin = GPIO_PIN_0;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
		
		//USB插入检测引脚
		GPIO_InitStruct.Pin = GPIO_PIN_9;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);	

		//防止开机上电，连接电脑检测，不识别的USB设备
		GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_12;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11|GPIO_PIN_12, GPIO_PIN_RESET);
}

/* CRC init function */
void MX_CRC_Init(void)
{

  hcrc.Instance = CRC;
  HAL_CRC_Init(&hcrc);

}

/* RNG init function */
void MX_RNG_Init(void)
{

  hrng.Instance = RNG;
  HAL_RNG_Init(&hrng);

}

void USB_DataReceiveHander(uint8_t *data,int len)
{
		memmove(&hid_recv_data[hid_index_write].data,data,len);
		hid_recv_data[hid_index_write].len = len;
		hid_index_write++;
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
