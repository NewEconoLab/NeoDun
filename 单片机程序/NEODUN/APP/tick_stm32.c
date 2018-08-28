#include "tick_stm32.h"
#include "main_define.h"
#include "aw9136.h"

static		volatile 	uint32_t 	input_time_counter = 0;
static 		volatile  uint8_t 	input_time_flag = 0;
static		volatile	uint32_t	center_key_count = 0;
static		volatile	uint32_t	scan_usb_count = 0;

void HAL_SYSTICK_Callback(void)
{
		//ϵͳʱ������
		system_base_time++;

		//1s�����ʱ
		if(system_base_time % 1000 == 0)
				task_1s_flag = 1;
		
		//�����ʱ����
		if(input_time_flag)								
		{
				if(input_time_counter < 30000)
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
		//USB����ɨ��
		if(system_base_time >= scan_usb_count)
		{
				if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_9))//rising
						Set_Flag.flag.usb_offline 	= 0;		//USB����
				else//failing
						Set_Flag.flag.usb_offline 	= 1;		//USB�Ͽ�
				scan_usb_count += 10;
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

