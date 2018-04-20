/*------------------------------------------------File Info---------------------------------------------
** File Name: main.c
** Last modified Date:  2018-04-19
** Last Version: 
** Descriptions: 
**------------------------------------------------------------------------------------------------------
** Created By: 		 hkh
** Created date:   2017-11-14
** Version: 
** Descriptions: NEODUN BootLoader 
**	
** 1、APP程序从扇区4开始存储，前面扇区0-3，总共64KB，留给BootLoader程序
** 2、APP程序存储在扇区4和扇区5，这两个扇区的大小为64+128KB
** 3、HID的速度改善，修改Polling Interval（轮询间隔）
** 4、AW9136的灵敏度由寄存器THR2-THR4设置
** 5、修改N_THR2、N_THR3、N_THR4值，可更改按键S1-S3的灵敏度  默认值为0x2328  现改为0x0808
** 6、插入USB开机出现异常，修改BootLoader主流程，只有进入升级状态，才枚举USBHID设备，调整主流程
** 7、VID一致  PID不同，PID为22352时，表示这个是钱包;VID表示供应商识别码，PID表示产品识别码
** 8、BootLoader问题，如果BootLoader停留的时间不够AW9136初始化，会导致程序进入AW9136的sleep模式，导致速度变慢
				解决办法：只有进入升级界面的时候，才初始化AW9136，不然只初始化，中间按键有效
** 9、通过写入0x1111进寄存器（N_OFR1~3）以及定义宏CNT_INT来提高AW9136的初始化速度
** 10、增加触摸按键双击的识别N_TAPR1-2、N_TDTR、N_GIER寄存器的值,但是检测到双击操作时，单击也会被检测到
** 11、亮度调整在OLED的0xC1处修改，可调范围：0-0xff
** 12、注意USB的主从控制引脚，导致USB设备一直有效
** 13、代码重构
********************************************************************************************************/
#include "main.h"
#include "stm32f4xx_hal.h"
#include "usb_device.h"
#include "aw9136.h"
#include "oled.h"
#include "iap.h"
#include "timer.h"
#include "stmflash.h"
#include "atsha204a.h"
#include "hw_config.h"
#include "main_define.h"
#include "app_interface.h"
#include "test_jpg.h"
#include "usbd_customhid.h"
#include "app_command.h"

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
RNG_HandleTypeDef hrng;
CRC_HandleTypeDef hcrc;

static void DATA_Init(void);
static void BSP_Init(void);
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_CRC_Init(void);
static void MX_RNG_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);

/***************************************** Global **************************************************/
extern BIN_FILE_INFO Bin_File;
extern SIGN_KEY_FLAG Key_Flag;
BOOT_SYS_FLAG BootFlag;
//HID接收缓冲设置
uint8_t HID_RX_BUF[RECV_BIN_FILE_LEN] __attribute__ ((at(0X20003000)));//接收缓冲,最大USART_REC_LEN个字节,起始地址为0X20001000.
//HID数据处理变量
volatile uint8_t 	HID_PAGE_RECV[64];
volatile int 			HID_RECV_LEN = 0;
volatile uint8_t 	hid_recv_flag = 0;
/***************************************************************************************************/

int main(void)
{
		//数据初始化
		DATA_Init();
		
		//硬件初始化
		BSP_Init();
	
		//加密芯片判断
		if(ReadAT204Flag(&BootFlag)==0)
		{
				Fill_RAM(0x00);
				Asc8_16(60,24,"ATSHA204 ERROR!!!");
				HAL_Delay(10000);
				return 0;
		}

		//加延时给按键芯片足够的时间来完成初始化
		HAL_Delay(500);
		
		//调试和升级选择
		if(Have_App() == 0)       			//不存在APP程序
		{
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
						if(Key_Flag.Key_center_Flag)
						{
								Key_Flag.Key_center_Flag = 0;
								BootFlag.update = 1;
								MX_USB_DEVICE_Init();			//USB初始化
								Key_Control(0);
								break;
						}
						if(Key_Flag.Key_left_Flag||Key_Flag.Key_right_Flag)
						{
								Key_Flag.Key_left_Flag = 0;
								Key_Flag.Key_right_Flag = 0;
								MX_USB_DEVICE_Init();			//USB初始化							
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
		
		//升级处理
#ifdef Debug_Print
		printf("Update Flag:%d\n",BootFlag.update);
#endif
		if(BootFlag.update)//需要升级
		{
				Fill_RAM(0x00);
				if(BootFlag.language == 0)
						Show_HZ12_12(96,26,0,10,13);	//等待更新
				else
				{
						Asc8_16(100,24,"Waiting");
				}
				MX_USB_DEVICE_Init();							//USB初始化
				memset(&HID_RX_BUF,0,RECV_BIN_FILE_LEN);
				while(1)
				{
						if(hid_recv_flag)			//接收数据则处理
						{
								Hid_Data_Analysis((uint8_t *)HID_PAGE_RECV,HID_RECV_LEN);
								hid_recv_flag = 0;
						}
						if(BootFlag.update_flag_failed)//一旦检测到更新失败，则退出程序
						{
								Fill_RAM(0x00);
								__set_FAULTMASK(1);
								NVIC_SystemReset();
						}
				}
		}
		
		//大厅app已安装，且不需要升级，则直接进入大厅app
		iap_load_app(FLASH_APP1_ADDR);
		
		return 0;
}

static void DATA_Init(void)
{
		memset(&BootFlag,0,sizeof(BOOT_SYS_FLAG));
		memset(&Key_Flag,0,sizeof(SIGN_KEY_FLAG));//清空按键标志位，开启按键有效
		memset(&Bin_File,0,sizeof(BIN_FILE_INFO));
}

static void BSP_Init(void)
{
		HAL_Init();								//中断分组2
		SystemClock_Config(); 		//系统时钟168MHz
		MX_GPIO_Init();						//IO口初始化
		MX_CRC_Init();						//CRC校验
		MX_RNG_Init();						//RNG随机数
		Center_button_init();			//中间按钮初始化
		OLED_Init();							//OLED初始化	
		Show_Pattern(&gImage_logo[0],26,37,8,56);//开机logo
		clearArea(104,56,48,1);
		MX_USART1_UART_Init();		//打印信息串口
		ATSHA204_Init();
		MX_USART2_UART_Init();		//蓝牙串口
		TIM3_Init(5000-1,8400-1);	//500ms进入一次中断计数
		AW9136_Init();						//触摸按键
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
		HID_RECV_LEN = len;
		memmove((uint8_t *)HID_PAGE_RECV,data,len);
		hid_recv_flag = 1;
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
