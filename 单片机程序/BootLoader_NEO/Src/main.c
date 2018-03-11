/*------------------------------------------------File Info---------------------------------------------
** File Name: main.c
** Last modified Date:  2017-11-14
** Last Version: 
** Descriptions: 
**------------------------------------------------------------------------------------------------------
** Created By: 		 hkh
** Created date:   2017-11-14
** Version: 
** Descriptions: NEODUN BootLoader 
**	
** 1、APP程序从扇区4开始存储，大小为80多KB，前面扇区0-3，总共64KB，留给BootLoader程序
** 2、APP程序存储在扇区4和扇区5，这两个扇区的大小为64+128KB
** 3、HID的速度改善，修改Polling Interval（轮询间隔）
** 4、用户在开机后，按下中心键进入程序升级模式，否则2s后进入正常程序
** 5、需要对APP程序进行设置，偏移地址，编译出bin文件
** 6、AW9136的灵敏度由寄存器THR2-THR4设置
** 7、扇区6开始地址存储开机密码，扇区11开始地址存储总的计数的值
** 8、扇区7-扇区10，总共512KB，存储用户的地址和私钥对
** 9、中断分组2
** 10、可以考虑将开机密码、计数值和设置的标志位，都记录在同一个扇区，当任意其中一个值改变时，先读出并保存这三个值，
			然后擦除该扇区，再重新写入这三个值，这样节省一个扇区的空间
** 11、最终FLASH分配：
				扇区0-3：  64KB    BootLoader程序
				扇区4-5：  192KB		 APP程序
				扇区6：    128KB   开机密码、设置标识、计数总数
				扇区7-11： 640KB   私钥地址对
** 12、修改N_THR2、N_THR3、N_THR4值，可更改按键S1-S3的灵敏度  默认值为0x2328  现改为0x0808
** 13、插入USB开机出现异常，修改BootLoader主流程，只有进入升级状态，才枚举USBHID设备，调整主流程
** 14、由于STM32擦除内部FLASH的速度很慢，重新调整FLASH分配
				扇区0-2：  48KB     BootLoader程序
				扇区3：		 16KB		 开机密码、设置标识、计数总数
				扇区4-5：  192KB		 APP程序 
				扇区6-11： 768KB   私钥地址对
** 15、VID一致  PID不同，PID为22352时，表示这个是钱包
			 VID表示供应商识别码，PID表示产品识别码
** 16、BootLoader问题，如果BootLoader停留的时间不够AW9136初始化，会导致程序进入AW9136的sleep模式，导致速度变慢
				解决办法：只有进入升级界面的时候，才初始化AW9136，不然只初始化，中间按键有效
** 17、通过写入0x1111进寄存器（N_OFR1~3）以及定义宏CNT_INT来提高AW9136的初始化速度
** 18、增加触摸按键双击的识别N_TAPR1-2、N_TDTR、N_GIER寄存器的值,但是检测到双击操作时，单击也会被检测到
** 19、去掉aw9136按键释放时的标志位改变
** 20、增加密码的设置和验证
** 21、增加加密芯片的驱动
** 22、重新调整存储分配，FLASH中只存储程序代码，加密芯片来存储用户私钥和地址，以及程序中的变量
			 加密芯片存储规划 0-15 一共16个槽，每个槽为32字节，分配如下：
						0-4   存储用户私钥
						5     存储重要的标识和PIN码（4字节PIN码+4字节BootLoader标识+其它）  
						6-14  存储其它程序标量
						15    存储该芯片进行加密读写时要用的私钥
** 23、更改BootLoader主逻辑,以及界面的显示						
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

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
RNG_HandleTypeDef hrng;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
int NEO_Test(void);

//HID接收缓冲设置
#define HID_REC_LEN 110*1024
u8 HID_RX_BUF[HID_REC_LEN] __attribute__ ((at(0X20003000)));//接收缓冲,最大USART_REC_LEN个字节,起始地址为0X20001000.    
u32 HID_RX_CNT = 0;

//全局变量
extern SIGN_KEY_FLAG Key_Flag;
BOOT_FLAG BootFlag;
volatile unsigned char touch_motor_flag = 1;    //1表示开启触摸振动，0表示关闭振动

int main(void)
{
		u32 oldcount=0;						//老的USB接收数据值	
		u32 applenth=0;						//接收到的app代码长度
		u8 HID_begin = 1;
		u8 HID_end = 0;
		u8 error_code = 0;
		memset(&BootFlag,0,sizeof(BootFlag));
		memset(&Key_Flag,0,sizeof(Key_Flag));//清空按键标志位，开启按键有效
	
		HAL_Init();								//中断分组2
		SystemClock_Config(); 		//系统时钟168MHz
		MX_GPIO_Init();						//IO口初始化
		Center_button_init();			//中间按钮初始化
		OLED_Init();							//OLED初始化	
		Show_Pattern(&gImage_logo[0],26,37,8,56);//开机logo
		clearArea(104,56,48,1);	
#ifdef Debug_Print	
		MX_USART1_UART_Init();		//打印信息串口
#endif
		Set_System();			
		MX_USART2_UART_Init();		//蓝牙串口
		TIM3_Init(5000-1,8400-1);	//500ms进入一次中断计数      
		MX_USB_DEVICE_Init();			//USB初始化
		AW9136_Init();	
		if(ReadAT204Flag(&BootFlag)==0)
		{
				Asc8_16(60,24,"ATSHA204 ERROR!!!");
				HAL_Delay(10000);
				return 0;
		}	
			
//		HAL_Delay(200);									//加延时给按键芯片足够的时间来完成初始化		
		
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
								break;
						}
						if(Key_Flag.Key_left_Flag||Key_Flag.Key_right_Flag)
						{
								NEO_Test();
								Fill_RAM(0x00);
								return 0;
						}		
				}
		}
		
		if(BootFlag.update)//需要升级
		{
				Fill_RAM(0x00);								//清屏
				if(BootFlag.language == 0)
						Show_HZ12_12(96,26,0,10,13);	//等待更新
				else
				{
				
					
					
				}
				applenth=0;
				memset(HID_RX_BUF,0,HID_REC_LEN);
				STMFLASH_Erase_Sectors(FLASH_SECTOR_4);
				STMFLASH_Erase_Sectors(FLASH_SECTOR_5);
				while(1)
				{
						if(HID_RX_CNT)
						{
								if(oldcount==HID_RX_CNT)//新周期内,没有收到任何数据,认为本次数据接收完成.
								{
										applenth=HID_RX_CNT;
										oldcount=0;
										HID_RX_CNT=0;
										HID_end = 1;
		#ifdef Debug_Print							
										printf("len of code:%dBytes\r\n",applenth);
		#endif							
								}
								else 
										oldcount=HID_RX_CNT;
								if(HID_begin)
								{
										HID_begin = 0;
										Fill_RAM(0x00);
										Show_Pattern(&gImage_wait[0],30,33,24,40);
										clearArea(120,40,16,1);
								}
						}
						HAL_Delay(50);
						
						if(HID_end)
						{
								iap_write_appbin(FLASH_APP1_ADDR,HID_RX_BUF,applenth);
								iap_load_app(FLASH_APP1_ADDR);
						}
				}
		}
		
		if(BootFlag.new_wallet) //新钱包
		{
				Fill_RAM(0x00);
				if(BootFlag.language == 0)
				{
						Show_HZ12_12(16,16,0,14,16);//检测为
						Show_HZ12_12(64,16,0,13,13);//新
						Show_HZ12_12(80,16,0,17,26);//钱包，请设置您的密码
				}
				else
				{
				
				
				}
				Show_Pattern(&gImage_triangle_down[0],30,32,45,57);
				clearArea(120,57,12,1);
				Key_Control(1);							//清空按键标志位，开启按键有效
				while(Key_Flag.Key_center_Flag == 0);
				Key_Flag.Key_center_Flag = 0;
				while(1)
				{
						if(SetPassport())
						{
								if(VerifyPassport())
								{
										ChangeWallet();
										break;		
								}
						}
				}
				Fill_RAM(0x00);
				if(BootFlag.language == 0)
				{
						//第一行显示
						Show_HZ12_12(80,7,0,25,26);//密码	
						Show_HZ12_12(112,7,0,21,22);//设置
						Show_HZ12_12(144,7,0,27,28);//完成
						//第二行显示
						Show_HZ12_12(8,26,0,20,20);//请
						Show_HZ12_12(24,26,0,29,30);//牢记
						Show_HZ12_12(56,26,0,25,26);//密码
						Show_HZ12_12(88,26,0,19,19);//，
						Show_HZ12_12(104,26,0,31,35);//丢失将导致
						Show_HZ12_12(184,26,0,17,18);//钱包
						Show_HZ12_12(216,26,0,36,36);//重
						Show_HZ12_12(232,26,0,22,22);//置
				}
				else
				{				
				
				}
				Show_Pattern(&gImage_triangle_down[0],30,32,45,57);
				clearArea(120,57,12,1);
				Key_Control(1);							//清空按键标志位，开启按键有效
				while(Key_Flag.Key_center_Flag == 0);
				Key_Flag.Key_center_Flag = 0;
				iap_load_app(FLASH_APP1_ADDR);
		}
		else//旧钱包，验证密码
		{
				while(VerifyPassport() == 0)
				{
						error_code++;
						if(error_code >= 5)
						{							
								if(BootFlag.language == 0)
								{
										//第一行显示
										Show_HZ12_12(84,7,0,25,26);//密码
										Show_HZ12_12(116,7,0,37,38);//错误
										Asc6_12(148,8,"5");
										Show_HZ12_12(160,7,0,39,39);//次								
										//第二行显示
										Show_HZ12_12(80,26,0,17,18);//钱包	
										Show_HZ12_12(112,26,0,40,41);//已被
										Show_HZ12_12(144,26,0,36,36);//重
										Show_HZ12_12(160,26,0,22,22);//置
								}
								else
								{
								
								}
								Show_Pattern(&gImage_triangle_down[0],30,32,45,57);
								clearArea(120,57,12,1);
								EmptyWallet();
								while(Key_Flag.Key_center_Flag == 0);
								Key_Flag.Key_center_Flag = 0;
								Fill_RAM(0x00);
								if(BootFlag.language == 0)
								{
										//第一行显示
										Show_HZ12_12(16,7,0,20,20);//请
										Show_HZ12_12(32,7,0,36,36);//重
										Show_HZ12_12(48,7,0,54,54);//启
										Show_HZ12_12(64,7,0,21,22);//设置
										Show_HZ12_12(96,7,0,25,26);//密码
										Show_HZ12_12(128,7,0,42,42);//后
										Show_HZ12_12(144,7,0,34,34);//导
										Show_HZ12_12(160,7,0,43,43);//入
										Show_HZ12_12(178,7,0,44,47);//备份文件	
										//第二行显示
										Show_HZ12_12(16,26,0,48,51);//需要帮助
										Show_HZ12_12(80,26,0,20,20);//请
										Show_HZ12_12(96,26,0,52,53);//登陆
										Asc6_12(128,26,"www.neodun.com");
								}
								else
								{
								
								}
								Show_Pattern(&gImage_triangle_down[0],30,32,45,57);
								clearArea(120,57,12,1);
								while(Key_Flag.Key_center_Flag == 0);
								Key_Flag.Key_center_Flag = 0;
								Fill_RAM(0x00);
								EmptyWallet();
								return 0;
						}
				}
				iap_load_app(FLASH_APP1_ADDR);
		}
		
		while (1)
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
}

void USB_DataReceiveHander(uint8_t *data,int len)
{
		int i = 0;
		if(HID_RX_CNT<HID_REC_LEN)
		{
				for(i=0;i<len;i++)
				{
						HID_RX_BUF[i+HID_RX_CNT] = data[i];
				}
				HID_RX_CNT = HID_RX_CNT + len;
		}	
}

int NEO_Test(void)
{
		int i=0;
		int value = 0;
		Fill_RAM(0x00);
		Asc8_16( 52,4 ,"Enter NeoDun Test ?");
		Asc8_16(24,44,"Cancel");
		Asc8_16(124,44,"OK");
		Asc8_16(184,44,"Cancel");
	
		Key_Control(1);				//清空按键标志位，开启按键有效
		while(1)
		{														
				if(Key_Flag.Key_center_Flag)
				{
						touch_motor_flag = 0;
						Key_Control(0);				//清空按键标志位，开启按键无效

						Fill_RAM(0x00);
						Asc8_16( 84,4 ,"NeoDun Test");		
						HAL_Delay(2000);
						
						for(i=0;i<5;i++)
						{
								Fill_RAM(0xFF);
								HAL_Delay(1000);
								Fill_RAM(0x00);
								HAL_Delay(1000);
						}
				/******************************************
										NeoDun Test
									Motor and Key Test
						On						Exit					Off
				******************************************/		
						Asc8_16( 84,4 ,"NeoDun Test");
						Asc8_16( 56,24 ,"Motor and Key Test");
						Asc8_16( 30,44 ,"On");
						Asc8_16( 112,44 ,"Exit");
						Asc8_16( 206,44 ,"Off");
						Key_Control(1);				//清空按键标志位，开启按键有效
						
						while(!Key_Flag.Key_center_Flag) //中间按键按下，退出
						{
								if(Key_Flag.Key_left_Flag)
								{
										Key_Flag.Key_left_Flag = 0;
										HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
								}
								else if(Key_Flag.Key_right_Flag)
								{
										Key_Flag.Key_right_Flag = 0;
										HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);						
								}
						}
						Key_Flag.Key_center_Flag = 0;				
						touch_motor_flag = 1;
						value = 1;
						break;
				}
				else if((Key_Flag.Key_right_Flag == 1)||(Key_Flag.Key_left_Flag == 1))
				{	
						Key_Control(0);				//清空按键标志位，开启按键无效
						value = 0;
						break;
				}
		}
		return value;
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
