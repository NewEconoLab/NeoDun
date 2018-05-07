#include "timer.h"
#include "main_define.h"
#include "aw9136.h"

//local
static		volatile 	uint32_t 	input_time_counter = 0;
static 		volatile  uint8_t 	input_time_flag = 0;
static		volatile 	uint32_t 	system_base_time = 0;
static		volatile	uint32_t	center_key_count = 0;

TIM_HandleTypeDef TIM3_Handler;      //��ʱ����� 

//ͨ�ö�ʱ��3�жϳ�ʼ��
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//��ʱ�����ʱ����㷽��:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=��ʱ������Ƶ��,��λ:Mhz
//����ʹ�õ��Ƕ�ʱ��3!(��ʱ��3����APB1�ϣ�ʱ��ΪHCLK/2)
void TIM3_Init(uint16_t arr,uint16_t psc)
{  
    TIM3_Handler.Instance=TIM3;                          		//ͨ�ö�ʱ��3
    TIM3_Handler.Init.Prescaler=psc;                     		//��Ƶϵ��
    TIM3_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;    		//���ϼ�����
    TIM3_Handler.Init.Period=arr;                        		//�Զ�װ��ֵ
    TIM3_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;	//ʱ�ӷ�Ƶ����
    HAL_TIM_Base_Init(&TIM3_Handler);
    
    HAL_TIM_Base_Start_IT(&TIM3_Handler); //ʹ�ܶ�ʱ��3�Ͷ�ʱ��3�����жϣ�TIM_IT_UPDATE   
		__HAL_RCC_TIM3_CLK_ENABLE();					//������ʱ��
}


//��ʱ���ײ�����������ʱ�ӣ������ж����ȼ�
//�˺����ᱻHAL_TIM_Base_Init()��������
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
    if(htim->Instance==TIM3)
	{
		__HAL_RCC_TIM3_CLK_ENABLE();            //ʹ��TIM3ʱ��
		HAL_NVIC_SetPriority(TIM3_IRQn,1,3);    //�����ж����ȼ�����ռ���ȼ�1�������ȼ�3
		HAL_NVIC_EnableIRQ(TIM3_IRQn);          //����ITM3�ж�   
	}
}


//��ʱ��3�жϷ�����
void TIM3_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&TIM3_Handler);
}


//�ص���������ʱ���жϷ���������		10ms����һ���жϼ���
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim==(&TIM3_Handler))
    {
				//ϵͳʱ������		1000s
				if(system_base_time < 100000)
						system_base_time++;
				else
						system_base_time = 0;
				//1s�����ʱ
				if(system_base_time % 100 == 0)
						task_1s_flag = 1;
				//�����ʱ����
				if(input_time_flag)								
				{
						if(input_time_counter < 3000)
								input_time_counter++;
						else
								input_time_counter = 0;
				}
				//��ﴦ��
				if(moter_delay)
				{
						moter_delay--;
						HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
						if(moter_delay == 0)
						{
								HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
						}
				}
				//��������
				if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0) == 0) //�а�������
				{
						center_key_count++;
				}
				else																				//û����ʱ�ж�״̬
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


