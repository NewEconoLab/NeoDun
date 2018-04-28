#include "timer.h"
#include "main_define.h"
#include "aw9136.h"

//local
static		volatile 	uint32_t 	input_time_counter = 0;
static 		volatile  uint8_t 	input_time_flag = 0;
static		volatile 	uint32_t 	system_base_time = 0;
static		volatile	uint32_t	center_key_count = 0;

TIM_HandleTypeDef TIM3_Handler;      //定时器句柄 

//通用定时器3中断初始化
//arr：自动重装值。
//psc：时钟预分频数
//定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=定时器工作频率,单位:Mhz
//这里使用的是定时器3!(定时器3挂在APB1上，时钟为HCLK/2)
void TIM3_Init(uint16_t arr,uint16_t psc)
{  
    TIM3_Handler.Instance=TIM3;                          		//通用定时器3
    TIM3_Handler.Init.Prescaler=psc;                     		//分频系数
    TIM3_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;    		//向上计数器
    TIM3_Handler.Init.Period=arr;                        		//自动装载值
    TIM3_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;	//时钟分频因子
    HAL_TIM_Base_Init(&TIM3_Handler);
    
    HAL_TIM_Base_Start_IT(&TIM3_Handler); //使能定时器3和定时器3更新中断：TIM_IT_UPDATE   
		__HAL_RCC_TIM3_CLK_ENABLE();					//开启定时器
}


//定时器底册驱动，开启时钟，设置中断优先级
//此函数会被HAL_TIM_Base_Init()函数调用
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
    if(htim->Instance==TIM3)
	{
		__HAL_RCC_TIM3_CLK_ENABLE();            //使能TIM3时钟
		HAL_NVIC_SetPriority(TIM3_IRQn,1,3);    //设置中断优先级，抢占优先级1，子优先级3
		HAL_NVIC_EnableIRQ(TIM3_IRQn);          //开启ITM3中断   
	}
}


//定时器3中断服务函数
void TIM3_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&TIM3_Handler);
}


//回调函数，定时器中断服务函数调用		10ms进入一次中断计数
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim==(&TIM3_Handler))
    {
				//系统时基更新		1000s
				if(system_base_time < 100000)
						system_base_time++;
				else
						system_base_time = 0;
				//1s任务计时
				if(system_base_time % 100 == 0)
						task_1s_flag = 1;
				//输入计时处理
				if(input_time_flag)								
				{
						if(input_time_counter < 3000)
								input_time_counter++;
						else
								input_time_counter = 0;
				}
				//马达处理
				if(moter_delay)
				{
						moter_delay--;
						HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
						if(moter_delay == 0)
						{
								HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
						}
				}
				//按键处理
				if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0) == 0) //有按键按下
				{
						center_key_count++;
				}
				else																				//没按键时判断状态
				{
						if(center_key_count <= KEY_VALID_TIME)
						{
								center_key_count = 0;
						}
						else
						{
								center_key_count = 0;
								Key_Flag.flag.middle = 1;
								Motor_touch(MOTOR_TIME);
						}
				}
    }
}

void Start_TIM(uint8_t type)
{
		if(type == OLED_INPUT_TIME)
		{		
				input_time_flag = 1;
				input_time_counter = 0;
		}
		if(type == KEY_TIME)
		{
				
		}
}

void Stop_TIM(uint8_t type)
{
		if(type == OLED_INPUT_TIME)
		{			
				input_time_flag = 0;
				input_time_counter = 0;
		}
		if(type == KEY_TIME)
		{
				
		}		
}

int Get_TIM(uint8_t type)
{
		if(type == OLED_INPUT_TIME)
		{
				return input_time_counter;
		}
		if(type == KEY_TIME)
		{	
				return 0;
		}
		return 0;
}


