#include "OledMenu.h"
#include "aw9136.h"
#include "app_interface.h"
#include "timer.h"
#include "atsha204a.h"
#include "Algorithm.h"
#include "OLED281/oled281.h"
#include "app_hal.h"
#include "app_oled.h"

static uint8_t main_RefreshDisplay = 0;//�Ƿ�����ˢ�½����־
static uint8_t main_page_index = 0;		 //ҳ��ID

/********************************
��ʾ������
		direction
				1  ���ϵ�������
				0  ���µ�������
********************************/
void Display_Triangle(uint8_t direction)
{
		if(direction)
				Show_Pattern(&gImage_triangle_up[0],30,33,48,64);
		else
				Show_Pattern(&gImage_triangle_down[0],30,33,48,64);
}

/********************************
��ʾ��ͷ
		direction
				0  �����������
				1  ���ҵ�������
********************************/
void Display_arrow(uint8_t direction)
{
		if(direction)
				Show_Pattern(&gImage_arrow_right[0],47,50,48,64);
		else
				Show_Pattern(&gImage_arrow_left[0],13,16,48,64);
}

void Display_Usb(void)
{
		Fill_RAM(0x00);
		Show_Pattern(&gImage_Usb[0],29,34,20,44);
		clearArea(116,44,24,1);
}

void Display_Address(uint8_t state,ADDRESS *address)
{
		if(address->len_name != 0)
		{
				if(state == 0)				//left
				{
						if(address->hide)
								Show_Pattern(&gImage_Address_hide[0],11,18,0,32);
						else
								Show_Pattern(&gImage_Address[0],11,18,0,32);
						clearArea(44,32,32,1);			
						Asc8_16(60-address->len_name*4,32,(uint8_t*)address->name);
				}
				else if(state ==1)		//midddle
				{
						if(address->hide)
								Show_Pattern(&gImage_Address_hide[0],28,35,0,32);
						else
								Show_Pattern(&gImage_Address[0],28,35,0,32);
						clearArea(112,32,32,1);			
						Asc8_16(128-address->len_name*4,32,(uint8_t*)address->name);
				}
				else if(state == 2)		//right
				{
						if(address->hide)
								Show_Pattern(&gImage_Address_hide[0],45,52,0,32);
						else
								Show_Pattern(&gImage_Address[0],45,52,0,32);
						clearArea(180,32,32,1);
						Asc8_16(196-address->len_name*4,32,(uint8_t*)address->name);
				}
		}
		else
		{
				if(state == 0)				//left
				{
						if(address->hide)
								Show_Pattern(&gImage_Address_hide[0],11,18,0,32);
						else
								Show_Pattern(&gImage_Address[0],11,18,0,32);
						clearArea(44,32,32,1);			
						Asc8_16(40,32,"uname");
				}
				else if(state ==1)		//midddle
				{
						if(address->hide)
								Show_Pattern(&gImage_Address_hide[0],28,35,0,32);
						else
								Show_Pattern(&gImage_Address[0],28,35,0,32);
						clearArea(112,32,32,1);			
						Asc8_16(108,32,"uname");
				}
				else if(state == 2)		//right
				{
						if(address->hide)
								Show_Pattern(&gImage_Address_hide[0],45,52,0,32);
						else
								Show_Pattern(&gImage_Address[0],45,52,0,32);
						clearArea(180,32,32,1);	
						Asc8_16(176,32,"uname");
				}		
		}
}

void Display_MainPage(void)
{
		Sort_wallet_Address();
		Fill_RAM(0x00);
		Show_Pattern(&gImage_Set[0],28,35,0,32);
		clearArea(112,32,32,1);
		Show_HZ12_12(112,32,2,3);//����
		Display_Triangle(1);

		if(Neo_System.count)
				Display_arrow(0);
		if(Neo_System.count > 1)
				Display_arrow(1);

		Key_Control(1);
		switch(Neo_System.count)
		{
				case 0:
				break;
				case 1:
						Display_Address(0,&showaddress[0]);
				break;
				case 2:
						Display_Address(0,&showaddress[0]);
						Display_Address(2,&showaddress[1]);
				break;
				case 3:
						Display_Address(0,&showaddress[0]);
						Display_Address(2,&showaddress[2]);		
				break;
				case 4:
						Display_Address(0,&showaddress[0]);
						Display_Address(2,&showaddress[3]);					
				break;
				case 5:
						Display_Address(0,&showaddress[0]);
						Display_Address(2,&showaddress[4]);			
				break;
				default:
				break;
		}
}

void Display_MainPage_0Add(void)
{
		if(Key_Flag.flag.middle)
		{
				Key_Flag.flag.middle = 0;
				main_RefreshDisplay = 1;
				Display_Click_Set();
		}		
		if((main_page_index == 0)&&(main_RefreshDisplay == 1))
		{
				main_RefreshDisplay = 0;
				Fill_RAM(0x00);
				Show_Pattern(&gImage_Set[0],28,35,0,32);
				clearArea(112,32,32,1);
				Show_HZ12_12(112,32,2,3);//����
				Display_Triangle(1);				
		}
		if(Key_Flag.flag.right)//�Ҽ���Чʱ��ID��1
		{
				Key_Flag.flag.right = 0;
		}
		if(Key_Flag.flag.left)//�����Чʱ��ID��1
		{
				Key_Flag.flag.left = 0;
		}
}

void Display_MainPage_1Add(void)
{
		if((Key_Flag.flag.middle)&&(main_page_index == 0))
		{
				Key_Flag.flag.middle = 0;
				main_RefreshDisplay = 1;
				Display_Click_Set();
		}
		if((Key_Flag.flag.middle)&&(main_page_index == 1))
		{
				Key_Flag.flag.middle = 0;
				main_RefreshDisplay = 1;
				Display_Click_Add(1);
		}			
		if((main_page_index == 0)&&(main_RefreshDisplay == 1))
		{
				main_RefreshDisplay = 0;
				Fill_RAM(0x00);
				Show_Pattern(&gImage_Set[0],28,35,0,32);
				clearArea(112,32,32,1);
				Show_HZ12_12(112,32,2,3);//����
				Display_Triangle(1);
				Display_arrow(0);
				Display_Address(0,&showaddress[0]);				
		}
		if((main_page_index == 1)&&(main_RefreshDisplay == 1))
		{
				main_RefreshDisplay = 0;
				Fill_RAM(0x00);
				Show_Pattern(&gImage_Set[0],47,54,0,32);
				clearArea(188,32,32,1);
				Show_HZ12_12(188,32,2,3);//����
				Display_Triangle(1);
				Display_arrow(1);			
				Display_Address(1,&showaddress[0]);
		}
		
		if(Key_Flag.flag.right)//�Ҽ���Чʱ��ID��1
		{
				Key_Flag.flag.right = 0;
				if(main_page_index > 0)
				{
						main_page_index--;
						main_RefreshDisplay = 1;
				}
		}
		if(Key_Flag.flag.left)//�����Чʱ��ID��1
		{
				Key_Flag.flag.left = 0;
				if(main_page_index < 1)
				{
						main_page_index++;
						main_RefreshDisplay = 1;
				}
		}						
}

void Display_MainPage_2Add(void)
{
		if((Key_Flag.flag.middle)&&(main_page_index == 0))
		{
				Key_Flag.flag.middle = 0;
				main_RefreshDisplay = 1;
				Display_Click_Set();
		}
		if((Key_Flag.flag.middle)&&(main_page_index == 1))
		{
				Key_Flag.flag.middle = 0;
				main_RefreshDisplay = 1;
				Display_Click_Add(2);
		}			
		if((Key_Flag.flag.middle)&&(main_page_index == 2))
		{
				Key_Flag.flag.middle = 0;
				main_RefreshDisplay = 1;
				Display_Click_Add(1);
		}
		
		if((main_page_index == 0)&&(main_RefreshDisplay == 1))
		{
				main_RefreshDisplay = 0;
				Fill_RAM(0x00);
				Show_Pattern(&gImage_Set[0],28,35,0,32);
				clearArea(112,32,32,1);
				Show_HZ12_12(112,32,2,3);//����
				Display_Triangle(1);
				Display_arrow(0);
				Display_arrow(1);
				Display_Address(0,&showaddress[0]);			
				Display_Address(2,&showaddress[1]);	
		}			
		if((main_page_index == 1)&&(main_RefreshDisplay == 1))
		{
				main_RefreshDisplay = 0;
				Fill_RAM(0x00);
				Show_Pattern(&gImage_Set[0],12,19,0,32);
				clearArea(48,32,32,1);
				Show_HZ12_12(48,32,2,3);//����
				Display_Triangle(1);
				Display_arrow(0);
				Display_arrow(1);
				Display_Address(1,&showaddress[1]);	
				Display_Address(2,&showaddress[0]);
		}
		if((main_page_index == 2)&&(main_RefreshDisplay == 1))
		{
				main_RefreshDisplay = 0;
				Fill_RAM(0x00);
				Show_Pattern(&gImage_Set[0],47,54,0,32);
				clearArea(188,32,32,1);
				Show_HZ12_12(188,32,2,3);//����
				Display_Triangle(1);
				Display_arrow(0);
				Display_arrow(1);
				Display_Address(1,&showaddress[0]);
				Display_Address(0,&showaddress[1]);
		}
		if(Key_Flag.flag.left)//�����Чʱ��ID��1
		{
				Key_Flag.flag.left = 0;
				if((main_page_index == 2)&&(main_RefreshDisplay == 0))
				{
						main_page_index = 0;
						main_RefreshDisplay = 1;
				}				
				else if(main_page_index < 2)
				{
						main_page_index++;
						main_RefreshDisplay = 1;
				}
		}			
		if(Key_Flag.flag.right)//�Ҽ���Чʱ��ID��1
		{
				Key_Flag.flag.right = 0;
				if((main_page_index == 0)&&(main_RefreshDisplay == 0))
				{
						main_page_index = 2;
						main_RefreshDisplay = 1;
				}			
				else if(main_page_index > 0)
				{
						main_page_index--;
						main_RefreshDisplay = 1;
				}
		}
}

void Display_MainPage_3Add(void)
{
		if((Key_Flag.flag.middle)&&(main_page_index == 0))
		{
				Key_Flag.flag.middle = 0;
				main_RefreshDisplay = 1;
				Display_Click_Set();
		}
		if((Key_Flag.flag.middle)&&(main_page_index == 1))
		{
				Key_Flag.flag.middle = 0;
				main_RefreshDisplay = 1;
				Display_Click_Add(1);
		}			
		if((Key_Flag.flag.middle)&&(main_page_index == 2))
		{
				Key_Flag.flag.middle = 0;
				main_RefreshDisplay = 1;
				Display_Click_Add(2);
		}
		if((Key_Flag.flag.middle)&&(main_page_index == 3))
		{
				Key_Flag.flag.middle = 0;
				main_RefreshDisplay = 1;
				Display_Click_Add(3);
		}				
		
		if((main_page_index == 0)&&(main_RefreshDisplay == 1))
		{
				main_RefreshDisplay = 0;
				Fill_RAM(0x00);
				Show_Pattern(&gImage_Set[0],28,35,0,32);
				clearArea(112,32,32,1);
				Show_HZ12_12(112,32,2,3);//����
				Display_Triangle(1);
				Display_arrow(0);
				Display_arrow(1);			
				Display_Address(0,&showaddress[0]);
				Display_Address(2,&showaddress[2]);
		}
		if((main_page_index == 1)&&(main_RefreshDisplay == 1))
		{
				main_RefreshDisplay = 0;
				Fill_RAM(0x00);
				Show_Pattern(&gImage_Set[0],47,54,0,32);
				clearArea(188,32,32,1);
				Show_HZ12_12(188,32,2,3);//����
				Display_Triangle(1);
				Display_arrow(0);
				Display_arrow(1);
			
				Display_Address(0,&showaddress[1]);
				Display_Address(1,&showaddress[0]);
		}
		if((main_page_index == 2)&&(main_RefreshDisplay == 1))
		{
				main_RefreshDisplay = 0;
				Fill_RAM(0x00);
				Display_Triangle(1);
				Display_arrow(0);
				Display_arrow(1);		
				Display_Address(0,&showaddress[2]);			
				Display_Address(1,&showaddress[1]);		
				Display_Address(2,&showaddress[0]);	
		}
		if((main_page_index == 3)&&(main_RefreshDisplay == 1))
		{
				main_RefreshDisplay = 0;
				Fill_RAM(0x00);
				Show_Pattern(&gImage_Set[0],12,19,0,32);
				clearArea(48,32,32,1);
				Show_HZ12_12(48,32,2,3);//����
				Display_Triangle(1);
				Display_arrow(0);
				Display_arrow(1);			
				Display_Address(1,&showaddress[2]);				
				Display_Address(2,&showaddress[1]);	
		}				
		
		if(Key_Flag.flag.left)//�Ҽ���Чʱ��ID��1
		{
				Key_Flag.flag.left = 0;
				if(main_page_index < 4)
				{
						main_page_index++;
						main_RefreshDisplay = 1;
				}
				if(main_page_index == 4)
				{
						main_page_index = 0;
						main_RefreshDisplay = 1;
				}
		}
		if(Key_Flag.flag.right)//�����Чʱ��ID��1
		{
				Key_Flag.flag.right = 0;
				if(main_page_index > 0)
				{
						main_page_index--;
						main_RefreshDisplay = 1;
				}
				if((main_page_index == 0)&&(main_RefreshDisplay == 0))
				{
						main_page_index = 3;
						main_RefreshDisplay = 1;
				}
		}	
}

void Display_MainPage_4Add(void)
{
		if((Key_Flag.flag.middle)&&(main_page_index == 0))
		{
				Key_Flag.flag.middle = 0;
				main_RefreshDisplay = 1;
				Display_Click_Set();
		}
		if((Key_Flag.flag.middle)&&(main_page_index == 1))
		{
				Key_Flag.flag.middle = 0;
				main_RefreshDisplay = 1;
				Display_Click_Add(1);
		}			
		if((Key_Flag.flag.middle)&&(main_page_index == 2))
		{
				Key_Flag.flag.middle = 0;
				main_RefreshDisplay = 1;
				Display_Click_Add(2);
		}
		if((Key_Flag.flag.middle)&&(main_page_index == 3))
		{
				Key_Flag.flag.middle = 0;
				main_RefreshDisplay = 1;
				Display_Click_Add(3);
		}			
		if((Key_Flag.flag.middle)&&(main_page_index == 4))
		{
				Key_Flag.flag.middle = 0;
				main_RefreshDisplay = 1;
				Display_Click_Add(4);
		}		
		
		if((main_page_index == 0)&&(main_RefreshDisplay == 1))
		{
				main_RefreshDisplay = 0;
				Fill_RAM(0x00);
				Show_Pattern(&gImage_Set[0],28,35,0,32);
				clearArea(112,32,32,1);
				Show_HZ12_12(112,32,2,3);//����
				Display_Triangle(1);
				Display_arrow(0);
				Display_arrow(1);			
				Display_Address(0,&showaddress[0]);			
				Display_Address(2,&showaddress[0]);
		}
		if((main_page_index == 1)&&(main_RefreshDisplay == 1))
		{
				main_RefreshDisplay = 0;
				Fill_RAM(0x00);
				Show_Pattern(&gImage_Set[0],47,54,0,32);
				clearArea(188,32,32,1);
				Show_HZ12_12(188,32,2,3);//����
				Display_Triangle(1);
				Display_arrow(0);
				Display_arrow(1);
				Display_Address(0,&showaddress[1]);	
				Display_Address(1,&showaddress[0]);	
		}	
		if((main_page_index == 2)&&(main_RefreshDisplay == 1))
		{
				main_RefreshDisplay = 0;
				Fill_RAM(0x00);
				Display_Triangle(1);
				Display_arrow(0);
				Display_arrow(1);
				Display_Address(0,&showaddress[2]);				
				Display_Address(1,&showaddress[1]);				
				Display_Address(2,&showaddress[0]);			
		}
		if((main_page_index == 3)&&(main_RefreshDisplay == 1))
		{
				main_RefreshDisplay = 0;
				Fill_RAM(0x00);
				Display_Triangle(1);
				Display_arrow(0);
				Display_arrow(1);
				Display_Address(0,&showaddress[3]);	
				Display_Address(1,&showaddress[2]);
				Display_Address(2,&showaddress[1]);	
		}				
		if((main_page_index == 4)&&(main_RefreshDisplay == 1))
		{
				main_RefreshDisplay = 0;
				Fill_RAM(0x00);
				Show_Pattern(&gImage_Set[0],12,19,0,32);
				clearArea(48,32,32,1);
				Show_HZ12_12(48,32,2,3);//����
				Display_Triangle(1);
				Display_arrow(0);
				Display_arrow(1);
				Display_Address(1,&showaddress[3]);
				Display_Address(2,&showaddress[2]);
		}
		
		if(Key_Flag.flag.left)//�Ҽ���Чʱ��ID��1
		{
				Key_Flag.flag.left = 0;
				if(main_page_index < 5)
				{
						main_page_index++;
						main_RefreshDisplay = 1;
				}
				if(main_page_index == 5)
				{
						main_page_index = 0;
						main_RefreshDisplay = 1;
				}
		}
		if(Key_Flag.flag.right)//�����Чʱ��ID��1
		{
				Key_Flag.flag.right = 0;
				if(main_page_index > 0)
				{
						main_page_index--;
						main_RefreshDisplay = 1;
				}
				if((main_page_index == 0)&&(main_RefreshDisplay == 0))
				{
						main_page_index = 4;
						main_RefreshDisplay = 1;
				}
		}			
}

void Display_MainPage_5Add(void)
{
		if((Key_Flag.flag.middle)&&(main_page_index == 0))
		{
				Key_Flag.flag.middle = 0;
				main_RefreshDisplay = 1;
				Display_Click_Set();
		}
		if((Key_Flag.flag.middle)&&(main_page_index == 1))
		{
				Key_Flag.flag.middle = 0;
				main_RefreshDisplay = 1;
				Display_Click_Add(1);
		}			
		if((Key_Flag.flag.middle)&&(main_page_index == 2))
		{
				Key_Flag.flag.middle = 0;
				main_RefreshDisplay = 1;
				Display_Click_Add(2);
		}
		if((Key_Flag.flag.middle)&&(main_page_index == 3))
		{
				Key_Flag.flag.middle = 0;
				main_RefreshDisplay = 1;
				Display_Click_Add(3);
		}			
		if((Key_Flag.flag.middle)&&(main_page_index == 4))
		{
				Key_Flag.flag.middle = 0;
				main_RefreshDisplay = 1;
				Display_Click_Add(4);
		}		
		if((Key_Flag.flag.middle)&&(main_page_index == 5))
		{
				Key_Flag.flag.middle = 0;
				main_RefreshDisplay = 1;
				Display_Click_Add(5);
		}
		
		if((main_page_index == 0)&&(main_RefreshDisplay == 1))
		{
				main_RefreshDisplay = 0;
				Fill_RAM(0x00);
				Show_Pattern(&gImage_Set[0],28,35,0,32);
				clearArea(112,32,32,1);
				Show_HZ12_12(112,32,2,3);//����
				Display_Triangle(1);
				Display_arrow(0);
				Display_arrow(1);			
				Display_Address(0,&showaddress[0]);	
				Display_Address(2,&showaddress[4]);	
		}
		if((main_page_index == 1)&&(main_RefreshDisplay == 1))
		{
				main_RefreshDisplay = 0;
				Fill_RAM(0x00);
				Show_Pattern(&gImage_Set[0],47,54,0,32);
				clearArea(188,32,32,1);
				Show_HZ12_12(188,32,2,3);//����
				Display_Triangle(1);
				Display_arrow(0);
				Display_arrow(1);			
				Display_Address(0,&showaddress[1]);			
				Display_Address(1,&showaddress[0]);
		}	
		if((main_page_index == 2)&&(main_RefreshDisplay == 1))
		{
				main_RefreshDisplay = 0;
				Fill_RAM(0x00);
				Display_Triangle(1);
				Display_arrow(0);
				Display_arrow(1);
				Display_Address(0,&showaddress[2]);
				Display_Address(1,&showaddress[1]);
				Display_Address(2,&showaddress[0]);
		}
		if((main_page_index == 3)&&(main_RefreshDisplay == 1))
		{
				main_RefreshDisplay = 0;
				Fill_RAM(0x00);
				Display_Triangle(1);
				Display_arrow(0);
				Display_arrow(1);				
				Display_Address(0,&showaddress[3]);	
				Display_Address(1,&showaddress[2]);				
				Display_Address(2,&showaddress[1]);	
		}			
		if((main_page_index == 4)&&(main_RefreshDisplay == 1))
		{
				main_RefreshDisplay = 0;
				Fill_RAM(0x00);
				Display_Triangle(1);
				Display_arrow(0);
				Display_arrow(1);			
				Display_Address(0,&showaddress[4]);				
				Display_Address(1,&showaddress[3]);				
				Display_Address(2,&showaddress[2]);			
		}				
		if((main_page_index == 5)&&(main_RefreshDisplay == 1))
		{
				main_RefreshDisplay = 0;
				Fill_RAM(0x00);
				Show_Pattern(&gImage_Set[0],12,19,0,32);
				clearArea(48,32,32,1);
				Show_HZ12_12(48,32,2,3);//����
				Display_Triangle(1);
				Display_arrow(0);
				Display_arrow(1);				
				Display_Address(1,&showaddress[4]);			
				Display_Address(2,&showaddress[3]);
		}
		
		if(Key_Flag.flag.left)//�Ҽ���Чʱ��ID��1
		{
				Key_Flag.flag.left = 0;
				if(main_page_index < 6)
				{
						main_page_index++;
						main_RefreshDisplay = 1;
				}
				if(main_page_index == 6)
				{
						main_page_index = 0;
						main_RefreshDisplay = 1;
				}
		}
		if(Key_Flag.flag.right)//�����Чʱ��ID��1
		{
				Key_Flag.flag.right = 0;
				if(main_page_index > 0)
				{
						main_page_index--;
						main_RefreshDisplay = 1;
				}
				if((main_page_index == 0)&&(main_RefreshDisplay == 0))
				{
						main_page_index = 5;
						main_RefreshDisplay = 1;
				}
		}	
}

void Display_MainPage_judge(void)
{
		switch(Neo_System.count)
		{
			case 0:
					Display_MainPage_0Add();
			break;
			case 1:
					Display_MainPage_1Add();
			break;
			case 2:
					Display_MainPage_2Add();			
			break;
			case 3:
					Display_MainPage_3Add();				
			break;
			case 4:
					Display_MainPage_4Add();				
			break;
			case 5:
					Display_MainPage_5Add();				
			break;
			default:
			break;
		}
}

void Display_SetCode(void)
{
		while(1)
		{
				setCode();
				if(verifyCodeGetPin(1,Neo_System.pin))//��֤�������
				{
						Key_Control(1);
						Fill_RAM(0x00);
						Show_HZ12_12(56,16,4,5);//����
						Show_HZ12_12(88,16,2,3);//����
						Show_HZ12_12(120,16,35,36);//����
						Show_HZ12_12(152,16,12,12);//��
						Show_HZ12_12(168,16,34,34);//��
						Show_HZ12_12(184,16,37,37);//��
						Display_Triangle(0);
						while(Key_Flag.flag.middle==0);
						Key_Control(0);
				}
				else
						break;
		}
}

uint8_t Display_VerifyCode(void)
{
		uint8_t status = 0; 
		uint8_t count[2] = {'0','\0'};
		while(1)
		{
				status = verifyCode(1);
				if(status == 0)
						return 0;
				count[0]++;
				if(status == 5) //��ʱ����
				{
						return 1;//��ʱ����1
				}				
				if(count[0] < '5')
				{
						Key_Control(1);
						Fill_RAM(0x00);
						Show_HZ12_12(84,8,4,5);//����
						Show_HZ12_12(116,8,38,38);//��
						Show_HZ12_12(132,8,36,36);//��
						Asc8_16(148,8,count);
						Show_HZ12_12(156,8,39,39);//��
						
						Show_HZ12_12(60,28,38,38);//��
						Show_HZ12_12(76,28,36,36);//��
						Asc8_16(92,28,"5");
						Show_HZ12_12(100,28,39,39);//��
						Show_HZ12_12(116,28,32,32);//��
						Show_HZ12_12(132,28,34,34);//��
						Show_HZ12_12(148,28,3,3);//��					
						Show_HZ12_12(164,28,2,2);//��
						Show_HZ12_12(180,28,43,43);//��
						Display_Triangle(0);
						while(Key_Flag.flag.middle==0);
						Key_Control(0);
				}
				else
				{
						Key_Control(1);
						Fill_RAM(0x00);
						Show_HZ12_12(84,8,4,5);//����
						Show_HZ12_12(116,8,38,38);//��
						Show_HZ12_12(132,8,36,36);//��
						Asc8_16(148,8,"5");
						Show_HZ12_12(156,8,39,39);//��	

						Show_HZ12_12(80,28,2,2);//��
						Show_HZ12_12(96,28,43,43);//��
						Show_HZ12_12(112,28,40,41);//�ѱ�
						Show_HZ12_12(144,28,34,34);//��
						Show_HZ12_12(160,28,3,3);//��
						Display_Triangle(0);
						EmptyWallet();						 										  //���Ǯ������
						while(Key_Flag.flag.middle==0);
						Key_Flag.flag.middle=0;
						
						Fill_RAM(0x00);
						Show_HZ12_12(32,8,12,12);//��
						Show_HZ12_12(48,8,34,34);//��
						Show_HZ12_12(64,8,2,2);//��
						Show_HZ12_12(80,8,4,5);//����
						Show_HZ12_12(112,8,42,42);//��
						Show_HZ12_12(128,8,24,25);//����
						Show_HZ12_12(160,8,43,46);//�����ļ�
					
						Show_HZ12_12(16,28,47,50);//��Ҫ����
						Show_HZ12_12(80,28,12,12);//��
						Show_HZ12_12(96,28,51,52);//��½
						Asc8_16(128,28,"www.neodun.com");
						Display_Triangle(0);
						while(Key_Flag.flag.middle==0);
						Key_Control(0);
						System_Reset();//����5��ϵͳ����
				}
		}
}

uint8_t Display_VerifyCode_GetPin(uint8_t Pin[8])
{
		uint8_t status = 0; 
		uint8_t count[2] = {'0','\0'};
		while(1)
		{
				status = verifyCodeGetPin(1,Pin);
				if(status == 0)
						return 0;
				count[0]++;
				if(status == 5) //��ʱ����
				{
						return 1;//��ʱ����1
				}				
				if(count[0] < '5')
				{
						Key_Control(1);
						Fill_RAM(0x00);
						Show_HZ12_12(84,8,4,5);//����
						Show_HZ12_12(116,8,38,38);//��
						Show_HZ12_12(132,8,36,36);//��
						Asc8_16(148,8,count);
						Show_HZ12_12(156,8,39,39);//��
						
						Show_HZ12_12(60,28,38,38);//��
						Show_HZ12_12(76,28,36,36);//��
						Asc8_16(92,28,"5");
						Show_HZ12_12(100,28,39,39);//��
						Show_HZ12_12(116,28,32,32);//��
						Show_HZ12_12(132,28,34,34);//��
						Show_HZ12_12(148,28,3,3);//��
						Show_HZ12_12(164,28,17,18);//Ǯ��
						Display_Triangle(0);
						while(Key_Flag.flag.middle==0);
						Key_Control(0);
				}
				else
				{
						Key_Control(1);
						Fill_RAM(0x00);
						Show_HZ12_12(84,8,4,5);//����
						Show_HZ12_12(116,8,38,38);//��
						Show_HZ12_12(132,8,36,36);//��
						Asc8_16(148,8,"5");
						Show_HZ12_12(156,8,39,39);//��					
						
						Show_HZ12_12(80,28,17,18);//Ǯ��
						Show_HZ12_12(112,28,40,41);//�ѱ�
						Show_HZ12_12(144,28,34,34);//��
						Show_HZ12_12(160,28,3,3);//��
						Display_Triangle(0);
						EmptyWallet();						 										  //���Ǯ������
						while(Key_Flag.flag.middle==0);
						Key_Flag.flag.middle=0;
						
						Fill_RAM(0x00);
						Show_HZ12_12(32,8,12,12);//��
						Show_HZ12_12(48,8,34,34);//��
						Show_HZ12_12(64,8,2,2);//��
						Show_HZ12_12(80,8,4,5);//����
						Show_HZ12_12(112,8,42,42);//��
						Show_HZ12_12(128,8,24,25);//����
						Show_HZ12_12(160,8,43,46);//�����ļ�
					
						Show_HZ12_12(16,28,47,50);//��Ҫ����
						Show_HZ12_12(80,28,12,12);//��
						Show_HZ12_12(96,28,51,52);//��½
						Asc8_16(128,28,"www.neodun.com");
						Display_Triangle(0);
						while(Key_Flag.flag.middle==0);
						Key_Control(0);
						System_Reset();//����5��ϵͳ����
				}
		}
}

uint8_t Display_VerifyCode_PowerOn(void)
{
		uint8_t count[2] = {'0','\0'};
		while(verifyCodeGetPin(0,Neo_System.pin))//��֤�������	
		{
				count[0]++;
				if(count[0] < '5')
				{
						Key_Control(1);
						Fill_RAM(0x00);
						Show_HZ12_12(84,8,4,5);//����
						Show_HZ12_12(116,8,38,38);//��
						Show_HZ12_12(132,8,36,36);//��
						Asc8_16(148,8,count);
						Show_HZ12_12(156,8,39,39);//��
						
						Show_HZ12_12(60,28,38,38);//��
						Show_HZ12_12(76,28,36,36);//��
						Asc8_16(92,28,"5");
						Show_HZ12_12(100,28,39,39);//��
						Show_HZ12_12(116,28,32,32);//��
						Show_HZ12_12(132,28,34,34);//��
						Show_HZ12_12(148,28,3,3);//��
						Show_HZ12_12(164,28,2,2);//��
						Show_HZ12_12(180,28,43,43);//��
						Display_Triangle(0);
						while(Key_Flag.flag.middle==0);
						Key_Control(0);
				}
				else
				{
						Key_Control(1);
						Fill_RAM(0x00);
						Show_HZ12_12(84,8,4,5);//����
						Show_HZ12_12(116,8,38,38);//��
						Show_HZ12_12(132,8,36,36);//��
						Asc8_16(148,8,"5");
						Show_HZ12_12(156,8,39,39);//��					
						
						Show_HZ12_12(80,28,2,2);//��
						Show_HZ12_12(96,28,43,43);//��
						Show_HZ12_12(112,28,40,41);//�ѱ�
						Show_HZ12_12(144,28,34,34);//��
						Show_HZ12_12(160,28,3,3);//��
						Display_Triangle(0);
						Neo_System.new_wallet = 1;										  //����Ϊ��Ǯ��
						EmptyWallet();						 										  //���Ǯ������
						Update_PowerOn_SetFlag(&Set_Flag,&Neo_System);	//����ϵͳ���ñ�ʶ
						while(Key_Flag.flag.middle==0);
						Key_Flag.flag.middle=0;
						
						Fill_RAM(0x00);
						Show_HZ12_12(32,8,12,12);//��
						Show_HZ12_12(48,8,34,34);//��
						Show_HZ12_12(64,8,2,2);//��
						Show_HZ12_12(80,8,4,5);//����
						Show_HZ12_12(112,8,42,42);//��
						Show_HZ12_12(128,8,24,25);//����
						Show_HZ12_12(160,8,43,46);//�����ļ�
					
						Show_HZ12_12(16,28,47,50);//��Ҫ����
						Show_HZ12_12(80,28,12,12);//��
						Show_HZ12_12(96,28,51,52);//��½
						Asc8_16(128,28,"www.neodun.com");
						Display_Triangle(0);
						while(Key_Flag.flag.middle==0);
						Key_Control(0);
						return 1;//����5�Σ�����1
				}
		}
		return 0;//��֤�ɹ�����0
}

/******************************************************************
˵����
		AddID��������ID��1-5����ʵ�ʶ�Ӧ�ڴ��еĵ�ַ��0-4��
******************************************************************/
void Display_Click_Add(uint8_t AddID)
{
		uint8_t RefreshDisplay = 1;//�Ƿ�����ˢ�½����־
		uint8_t page_index = 0;		//ҳ��ID
		uint8_t	slot_data[32];
		Key_Control(1);
		while(1)
		{
				if((Key_Flag.flag.middle)&&(page_index == 0))
				{
						Key_Flag.flag.middle = 0;					
						Fill_RAM(0x00);
						Asc8_16(0,16,showaddress[AddID-1].address);
						Display_Triangle(0);
						while(Key_Flag.flag.middle == 0);
						Key_Control(1);
						RefreshDisplay = 1;
				}
				if((Key_Flag.flag.middle)&&(page_index == 1))
				{
						Key_Flag.flag.middle = 0;
						Fill_RAM(0x00);
						if(showaddress[AddID-1].hide == 0)
						{
								Show_HZ12_12(72,7,84,85);//����
								Show_HZ12_12(104,7,23,23);//��
								Show_HZ12_12(120,7,86,87);//����
								Show_HZ12_12(152,7,17,18);//Ǯ��
							
								Show_HZ12_12(32,28,88,90);//�ⲿ��
								Show_HZ12_12(80,28,46,46);//��
								Show_HZ12_12(96,28,32,32);//��
								Show_HZ12_12(112,28,91,92);//�޷�
								Show_HZ12_12(144,28,63,64);//����
								Show_HZ12_12(176,28,73,73);//��
								Show_HZ12_12(192,28,17,18);//Ǯ��							
						}
						else
						{
								Show_HZ12_12(80,7,65,65);//��
								Show_HZ12_12(96,7,130,130);//��
								Show_HZ12_12(112,7,84,85);//����
								Show_HZ12_12(144,7,17,18);//Ǯ��	
								
								Show_HZ12_12(40,28,131,132);//����
								Show_HZ12_12(72,28,88,90);//�ⲿ��
								Show_HZ12_12(120,28,46,46);//��
								Show_HZ12_12(136,28,63,64);//����
								Show_HZ12_12(168,28,73,73);//��
								Show_HZ12_12(184,28,17,18);//Ǯ��								
						}
						Show_HZ12_12(58,47,54,54);//��
						Show_HZ12_12(120,47,53,53);//��
						Show_HZ12_12(182,47,54,54);//��						
						while(1)
						{
								if(Key_Flag.flag.middle)
								{
										//���ص�ַ����
										Key_Control(1);
										if(showaddress[AddID-1].hide)
										{
												showaddress[AddID-1].hide = 0;
												ATSHA_read_data_slot(AddID+7,slot_data);
												slot_data[31] = slot_data[31]&0xF0;
												ATSHA_write_data_slot(AddID+7,0,slot_data,32);
										}
										else
										{
												showaddress[AddID-1].hide = 1;
												ATSHA_read_data_slot(AddID+7,slot_data);
												slot_data[31] = slot_data[31]|0x01;
												ATSHA_write_data_slot(AddID+7,0,slot_data,32);											
										}
										RefreshDisplay = 1;
										break;
								}
								if(Key_Flag.flag.left||Key_Flag.flag.right)
								{
										Key_Control(1);
										RefreshDisplay = 1;
										break;
								}
						}
				}
				if((Key_Flag.flag.middle)&&(page_index == 2))
				{
						Key_Flag.flag.middle = 0;
						return;
				}
				
				if((page_index == 0)&&(RefreshDisplay == 1))
				{
						RefreshDisplay = 0;
						Fill_RAM(0x00);
						Show_HZ12_12(96,16,78,79);//�鿴
						Show_HZ12_12(128,16,8,9);//��ַ
						if(showaddress[AddID-1].hide)
						{
								Show_HZ12_12(178,16,111,112);//ȡ��
								Show_HZ12_12(210,16,80,81);//����
						}
						else
						{
								Show_HZ12_12(178,16,80,81);//����
								Show_HZ12_12(210,16,17,18);//Ǯ��
						}
						Display_Triangle(1);
						Display_arrow(1);
				}
				if((page_index == 1)&&(RefreshDisplay == 1))
				{
						RefreshDisplay = 0;
						Fill_RAM(0x00);
						Show_HZ12_12(15,16,78,79);//�鿴
						Show_HZ12_12(47,16,8,9);//��ַ
						if(showaddress[AddID-1].hide)
						{
								Show_HZ12_12(96,16,111,112);//ȡ��
								Show_HZ12_12(128,16,80,81);//����
						}
						else
						{
								Show_HZ12_12(96,16,80,81);//����
								Show_HZ12_12(128,16,17,18);//Ǯ��
						}
						Show_HZ12_12(184,16,82,83);//����
						Display_Triangle(1);
						Display_arrow(0);
						Display_arrow(1);
				}
				if((page_index == 2)&&(RefreshDisplay == 1))
				{
						RefreshDisplay = 0;
						Fill_RAM(0x00);
						if(showaddress[AddID-1].hide)
						{
								Show_HZ12_12(15,16,111,112);//ȡ��
								Show_HZ12_12(47,16,80,81);//����
						}
						else
						{
								Show_HZ12_12(15,16,80,81);//����
								Show_HZ12_12(47,16,17,18);//Ǯ��
						}
						Show_HZ12_12(112,16,82,83);//����
						Display_Triangle(1);
						Display_arrow(0);
				}
				
				if(Key_Flag.flag.right)//�Ҽ���Чʱ��ID��1
				{
						Key_Flag.flag.right = 0;
						if(page_index<2)
						{
								page_index++;
								RefreshDisplay = 1;
						}
				}
				if(Key_Flag.flag.left)//�����Чʱ��ID��1
				{
						Key_Flag.flag.left = 0;
						if(page_index > 0)
						{
								page_index--;
								RefreshDisplay = 1;
						}
				}
				
				if((page_index == 0)&&(RefreshDisplay == 1))
				{
						RefreshDisplay = 0;
						Fill_RAM(0x00);
						Show_HZ12_12(96,16,78,79);//�鿴
						Show_HZ12_12(128,16,8,9);//��ַ
						if(showaddress[AddID].hide)
						{
								Show_HZ12_12(178,16,111,112);//ȡ��
								Show_HZ12_12(210,16,80,81);//����
						}
						else
						{
								Show_HZ12_12(178,16,80,81);//����
								Show_HZ12_12(210,16,17,18);//Ǯ��
						}
						Display_Triangle(1);
						Display_arrow(1);
				}
		}
}

void Display_Set_Coin_NEO(void)
{
		uint8_t RefreshDisplay = 1;//�Ƿ�����ˢ�½����־
		uint8_t page_index = 0;		//ҳ��ID
		while(1)
		{
				if((Key_Flag.flag.middle)&&(page_index == 0))
				{
						Key_Flag.flag.middle = 0;
						Fill_RAM(0x00);
						Show_HZ12_12(88,16,97,97);//��
						Show_HZ12_12(104,16,73,73);//��				
						Show_HZ12_12(120,16,98,98);//��
						Asc8_16(136,16,VERSION_NEO_STR);
						Display_Triangle(0);
						while(Key_Flag.flag.middle == 0);
						Key_Flag.flag.middle = 0;
						RefreshDisplay = 1;						
				}
				if((Key_Flag.flag.middle)&&(page_index == 1))
				{
						Key_Flag.flag.middle = 0;
				}			
				if((Key_Flag.flag.middle)&&(page_index == 2))
				{
						Key_Flag.flag.middle = 0;
						return;
				}
				
				if((page_index == 0)&&(RefreshDisplay == 1))
				{
						RefreshDisplay = 0;
						Fill_RAM(0x00);
					  Show_HZ12_12(104,16,97,97);//��
						Show_HZ12_12(120,16,73,73);//��
						Show_HZ12_12(136,16,98,98);//��
						
						Show_HZ12_12(184,16,99,101);//��Լǩ
						Show_HZ12_12(232,16,57,57);//��
						Display_Triangle(1);
						Display_arrow(1);					
				}
				if((page_index == 1)&&(RefreshDisplay == 1))
				{
						RefreshDisplay = 0;
						Fill_RAM(0x00);
						Show_HZ12_12(30,16,97,97);//��
						Show_HZ12_12(46,16,73,73);//��
						Show_HZ12_12(62,16,98,98);//��
						
						Show_HZ12_12(96,16,99,101);//��Լǩ
						Show_HZ12_12(144,16,57,57);//��
						Show_HZ12_12(184,16,82,83);//����
						Display_Triangle(1);	
						Display_arrow(0);	
						Display_arrow(1);
				}
				if((page_index == 2)&&(RefreshDisplay == 1))
				{
						RefreshDisplay = 0;
						Fill_RAM(0x00);
						Show_HZ12_12(20,16,99,101);//��Լǩ
						Show_HZ12_12(68,16,57,57);//��
						Show_HZ12_12(112,16,82,83);//����
						Display_Triangle(1);	
						Display_arrow(0);			
				}
				
				if(Key_Flag.flag.right)//�Ҽ���Чʱ��ID��1
				{
						Key_Flag.flag.right = 0;
						if(page_index<2)
						{
								page_index++;
								RefreshDisplay = 1;
						}
				}
				if(Key_Flag.flag.left)//�����Чʱ��ID��1
				{
						Key_Flag.flag.left = 0;
						if(page_index > 0)
						{
								page_index--;
								RefreshDisplay = 1;
						}
				}
				
				if((page_index == 0)&&(RefreshDisplay == 1))
				{
						RefreshDisplay = 0;
						Fill_RAM(0x00);
						Show_HZ12_12(104,16,97,97);//��
						Show_HZ12_12(120,16,73,73);//��
						Show_HZ12_12(136,16,98,98);//��
						
						Show_HZ12_12(184,16,99,101);//��Լǩ
						Show_HZ12_12(232,16,57,57);//��
						
						Display_Triangle(1);
						Display_arrow(1);
				}					
		}
}

void Display_Set_Coin(void)
{
		uint8_t RefreshDisplay = 1;//�Ƿ�����ˢ�½����־
		uint8_t page_index = 0;		//ҳ��ID	
		Key_Control(1);
		while(1)
		{
				if((Key_Flag.flag.middle)&&(page_index == 0))
				{
						Key_Flag.flag.middle = 0;
						Display_Set_Coin_NEO();
						RefreshDisplay = 1;
				}
				if((Key_Flag.flag.middle)&&(page_index == 1))
				{
						Key_Flag.flag.middle = 0;
						return;
				}			
				
				if((page_index == 0)&&(RefreshDisplay == 1))
				{
						RefreshDisplay = 0;
						Fill_RAM(0x00);
						Asc8_16(116,16,"NEO");
						Show_HZ12_12(184,16,82,83);//����
						Display_Triangle(1);
						Display_arrow(1);					
				}
				if((page_index == 1)&&(RefreshDisplay == 1))
				{
						RefreshDisplay = 0;
						Fill_RAM(0x00);
						Asc8_16(50,16,"NEO");
						Show_HZ12_12(112,16,82,83);//����
						Display_Triangle(1);	
						Display_arrow(0);			
				}	
				
				if(Key_Flag.flag.right)//�Ҽ���Чʱ��ID��1
				{
						Key_Flag.flag.right = 0;
						if(page_index<1)
						{
								page_index++;
								RefreshDisplay = 1;
						}
				}
				if(Key_Flag.flag.left)//�����Чʱ��ID��1
				{
						Key_Flag.flag.left = 0;
						if(page_index > 0)
						{
								page_index--;
								RefreshDisplay = 1;
						}
				}
				
				if((page_index == 0)&&(RefreshDisplay == 1))
				{
						RefreshDisplay = 0;
						Fill_RAM(0x00);
						Asc8_16(116,16,"NEO");
						Show_HZ12_12(184,16,82,83);//����
						Display_Triangle(1);
						Display_arrow(1);
				}					
		}
}

void Display_Set_About(void)
{
		uint8_t RefreshDisplay = 1;//�Ƿ�����ˢ�½����־
		uint8_t page_index = 0;		//ҳ��ID
		while(1)
		{
				if((Key_Flag.flag.middle)&&(page_index == 0))
				{
						Key_Flag.flag.middle = 0;
						Fill_RAM(0x00);
						Show_HZ12_12(88,16,97,97);//��
						Show_HZ12_12(104,16,73,73);//��				
						Show_HZ12_12(120,16,98,98);//��
						Asc8_16(136,16,VERSION_NEODUN_STR);
						Display_Triangle(0);
						while(Key_Flag.flag.middle == 0);
						Key_Flag.flag.middle = 0;
						RefreshDisplay = 1;
				}
				if((Key_Flag.flag.middle)&&(page_index == 1))
				{
						Key_Flag.flag.middle = 0;
						Fill_RAM(0x00);
						Show_HZ12_12(56,8,47,50);//��Ҫ����
						Show_HZ12_12(120,8,12,12);//��
						Show_HZ12_12(136,8,51,52);//��½
						Show_HZ12_12(168,8,104,105);//����
						Asc8_16(72,28,"www.neodun.com");
						Display_Triangle(0);
						while(Key_Flag.flag.middle == 0);
						Key_Flag.flag.middle = 0;
						RefreshDisplay = 1;
				}			
				if((Key_Flag.flag.middle)&&(page_index == 2))
				{
						Key_Flag.flag.middle = 0;
						return;
				}
				
				if((page_index == 0)&&(RefreshDisplay == 1))
				{
						RefreshDisplay = 0;
						Fill_RAM(0x00);
						Show_HZ12_12(96,16,77,77);//��
						Show_HZ12_12(112,16,46,46);//��
						Show_HZ12_12(128,16,97,97);//��
						Show_HZ12_12(144,16,73,73);//��
						Show_HZ12_12(184,16,49,50);//����
						Display_Triangle(1);
						Display_arrow(1);			
				}
				if((page_index == 1)&&(RefreshDisplay == 1))
				{
						RefreshDisplay = 0;
						Fill_RAM(0x00);
						Show_HZ12_12(20,16,77,77);//��
						Show_HZ12_12(36,16,46,46);//��
						Show_HZ12_12(52,16,97,97);//��
						Show_HZ12_12(68,16,73,73);//��
						Show_HZ12_12(112,16,49,50);//����
						Show_HZ12_12(184,16,82,83);//����
						Display_Triangle(1);	
						Display_arrow(0);	
						Display_arrow(1);
				}	
				if((page_index == 2)&&(RefreshDisplay == 1))
				{
						RefreshDisplay = 0;
						Fill_RAM(0x00);
						Show_HZ12_12(44,16,49,50);//����
						Show_HZ12_12(112,16,82,83);//����
						Display_Triangle(1);	
						Display_arrow(0);				
				}
				
				if(Key_Flag.flag.right)//�Ҽ���Чʱ��ID��1
				{
						Key_Flag.flag.right = 0;
						if(page_index<2)
						{
								page_index++;
								RefreshDisplay = 1;
						}
				}
				if(Key_Flag.flag.left)//�����Чʱ��ID��1
				{
						Key_Flag.flag.left = 0;
						if(page_index > 0)
						{
								page_index--;
								RefreshDisplay = 1;
						}
				}
				
				if((page_index == 0)&&(RefreshDisplay == 1))
				{
						RefreshDisplay = 0;
						Fill_RAM(0x00);
						Show_HZ12_12(96,16,77,77);//��
						Show_HZ12_12(112,16,46,46);//��
						Show_HZ12_12(128,16,97,97);//��
						Show_HZ12_12(144,16,73,73);//��
						Show_HZ12_12(184,16,49,50);//����
						Display_Triangle(1);
						Display_arrow(1);
				}					
		}
}

void Display_Set_Security_ResetWallet(void)
{
		Fill_RAM(0x00);
		Show_HZ12_12(64,7,68,68);//��
		Show_HZ12_12(80,7,32,32);//��
		Show_HZ12_12(96,7,55,56);//ɾ��
		Show_HZ12_12(128,7,96,96);//ȫ
		Show_HZ12_12(144,7,89,89);//��
		Show_HZ12_12(160,7,108,109);//����
	
		Show_HZ12_12(72,28,55,56);//ɾ��
		Show_HZ12_12(104,28,42,42);//��
		Show_HZ12_12(120,28,91,92);//�޷�
		Show_HZ12_12(152,28,110,110);//��
		Show_HZ12_12(168,28,83,83);//��
	
		Show_HZ12_12(46,47,111,112);//ȡ��
		Show_HZ12_12(112,47,113,114);//����
		Show_HZ12_12(184,47,111,112);//ȡ��
		
		while(1)
		{
				if(Key_Flag.flag.middle)
				{
						Key_Flag.flag.middle = 0;
						Fill_RAM(0x00);
						Show_HZ12_12(96,7,75,76);//����
						Show_HZ12_12(128,7,63,64);//����
					
						Show_HZ12_12(76,28,53,54);//�Ƿ�
						Show_HZ12_12(108,28,34,34);//��
						Show_HZ12_12(124,28,3,3);//��
						Show_HZ12_12(140,28,2,2);//��
						Show_HZ12_12(156,28,43,43);//��
						Asc8_16(172,28,"?");
					
						Show_HZ12_12(46,47,111,112);//ȡ��
						Show_HZ12_12(112,47,113,114);//����
						Show_HZ12_12(184,47,111,112);//ȡ��						
						while(1)
						{
								if(Key_Flag.flag.middle)
								{
										Key_Flag.flag.middle = 0;
										if(Display_VerifyCode() == 0)
										{
												Fill_RAM(0x00);
												Show_HZ12_12(96,8,34,34);//��
												Show_HZ12_12(112,8,3,3);//��
												Show_HZ12_12(128,8,27,27);//��
												Show_HZ12_12(144,8,115,115);//��
											
												Show_HZ12_12(72,28,116,118);//ţ����
												Show_HZ12_12(120,28,29,29);//��
												Show_HZ12_12(136,28,119,121);//��һ��
												Display_Triangle(0);
											
												Key_Control(1);
												while(Key_Flag.flag.middle == 0);
												Key_Flag.flag.middle = 0;
												EmptyWallet();				//���Ǯ������
												HAL_Delay(50);
												System_Reset();//ϵͳ����
										}
										else
												return;
								}
								if(Key_Flag.flag.left||Key_Flag.flag.right)
								{
										Key_Flag.flag.left = 0;
										Key_Flag.flag.right = 0;
										return;
								}
						}
				}
				if(Key_Flag.flag.left||Key_Flag.flag.right)
				{
						Key_Flag.flag.left = 0;
						Key_Flag.flag.right = 0;
						return;
				}
		}
}

void Display_Set_Security(void)
{
		uint8_t RefreshDisplay = 1;//�Ƿ�����ˢ�½����־
		uint8_t page_index = 0;		//ҳ��ID
		while(1)
		{
				if((Key_Flag.flag.middle)&&(page_index == 0))
				{
						Key_Flag.flag.middle = 0;
						if(Display_VerifyCode() == 0)
						{	
								Display_SetCode();
						}
						Key_Control(1);
						RefreshDisplay = 1;
				}
				if((Key_Flag.flag.middle)&&(page_index == 1))
				{
						Key_Flag.flag.middle = 0;
						Display_Set_Security_ResetWallet();						
						RefreshDisplay = 1;
				}			
				if((Key_Flag.flag.middle)&&(page_index == 2))
				{
						Key_Flag.flag.middle = 0;
						return;
				}
				
				if((page_index == 0)&&(RefreshDisplay == 1))
				{
						RefreshDisplay = 0;
						Fill_RAM(0x00);
						Show_HZ12_12(100,16,106,107);//����
						Asc8_16(132,16,"PIN");
						Show_HZ12_12(188,16,34,34);//��
						Show_HZ12_12(204,16,3,3);//��
						Show_HZ12_12(220,16,2,2);//��
						Show_HZ12_12(236,16,43,43);//��
						Display_Triangle(1);
						Display_arrow(1);			
				}
				if((page_index == 1)&&(RefreshDisplay == 1))
				{
						RefreshDisplay = 0;
						Fill_RAM(0x00);
						Show_HZ12_12(20,16,106,107);//����
						Asc8_16(52,16,"PIN");
						Show_HZ12_12(96,16,34,34);//��
						Show_HZ12_12(112,16,3,3);//��
						Show_HZ12_12(128,16,2,2);//��
						Show_HZ12_12(144,16,43,43);//��
						Show_HZ12_12(184,16,82,83);//����
						Display_Triangle(1);	
						Display_arrow(0);	
						Display_arrow(1);
				}	
				if((page_index == 2)&&(RefreshDisplay == 1))
				{
						RefreshDisplay = 0;
						Fill_RAM(0x00);
						Show_HZ12_12(30,16,34,34);//��
						Show_HZ12_12(46,16,3,3);//��
						Show_HZ12_12(62,16,2,2);//��
						Show_HZ12_12(78,16,43,43);//��
						Show_HZ12_12(112,16,82,83);//����
						Display_Triangle(1);	
						Display_arrow(0);			
				}
				
				if(Key_Flag.flag.right)//�Ҽ���Чʱ��ID��1
				{
						Key_Flag.flag.right = 0;
						if(page_index<2)
						{
								page_index++;
								RefreshDisplay = 1;
						}
				}
				if(Key_Flag.flag.left)//�����Чʱ��ID��1
				{
						Key_Flag.flag.left = 0;
						if(page_index > 0)
						{
								page_index--;
								RefreshDisplay = 1;
						}
				}
				
				if((page_index == 0)&&(RefreshDisplay == 1))
				{
						RefreshDisplay = 0;
						Fill_RAM(0x00);
						Show_HZ12_12(100,16,106,107);//����
						Asc8_16(132,16,"PIN");
						Show_HZ12_12(168,16,34,34);//��
						Show_HZ12_12(184,16,3,3);//��
						Show_HZ12_12(200,16,2,2);//��
						Show_HZ12_12(216,16,43,43);//��
						Display_Triangle(1);
						Display_arrow(1);
				}					
		}		
}

void Display_Click_Set(void)
{
		uint8_t RefreshDisplay = 1;//�Ƿ�����ˢ�½����־
		uint8_t page_index = 0;		//ҳ��ID		
		Key_Control(1);
		while(1)
		{
				if((Key_Flag.flag.middle)&&(page_index == 0))
				{
						Key_Flag.flag.middle = 0;
						Display_Set_Coin();
						RefreshDisplay = 1;
				}
				if((Key_Flag.flag.middle)&&(page_index == 1))
				{
						Key_Flag.flag.middle = 0;
						Display_Set_Security();
						RefreshDisplay = 1;
				}
				if((Key_Flag.flag.middle)&&(page_index == 2))
				{
						Key_Flag.flag.middle = 0;
						Display_Set_About();
						RefreshDisplay = 1;
				}		
				if((Key_Flag.flag.middle)&&(page_index == 3))
				{
						Key_Flag.flag.middle = 0;
						return;
				}
				
				if((page_index == 0)&&(RefreshDisplay == 1))
				{
						RefreshDisplay = 0;
						Fill_RAM(0x00);
						Show_HZ12_12(112,16,93,94);//����
						Show_HZ12_12(184,16,95,96);//��ȫ
						Display_Triangle(1);
						Display_arrow(1);					
				}
				if((page_index == 1)&&(RefreshDisplay == 1))
				{
						RefreshDisplay = 0;
						Fill_RAM(0x00);
						Show_HZ12_12(46,16,93,94);//����
						Show_HZ12_12(112,16,95,96);//��ȫ
						Show_HZ12_12(184,16,102,103);//����
						Display_Triangle(1);
						Display_arrow(0);
						Display_arrow(1);					
				}
				if((page_index == 2)&&(RefreshDisplay == 1))
				{
						RefreshDisplay = 0;
						Fill_RAM(0x00);
						Show_HZ12_12(46,16,95,96);//��ȫ
						Show_HZ12_12(112,16,102,103);//����
						Show_HZ12_12(184,16,82,83);//����
						Display_Triangle(1);
						Display_arrow(1);
						Display_arrow(0);				
				}			
				if((page_index == 3)&&(RefreshDisplay == 1))
				{
						RefreshDisplay = 0;
						Fill_RAM(0x00);
						Show_HZ12_12(46,16,102,103);//����
						Show_HZ12_12(112,16,82,83);//����
						Display_Triangle(1);
						Display_arrow(0);		
				}	
				
				if(Key_Flag.flag.right)//�Ҽ���Чʱ��ID��1
				{
						Key_Flag.flag.right = 0;
						if(page_index<3)
						{
								page_index++;
								RefreshDisplay = 1;
						}
				}
				if(Key_Flag.flag.left)//�����Чʱ��ID��1
				{
						Key_Flag.flag.left = 0;
						if(page_index > 0)
						{
								page_index--;
								RefreshDisplay = 1;
						}
				}

				if((page_index == 0)&&(RefreshDisplay == 1))
				{
						RefreshDisplay = 0;
						Fill_RAM(0x00);
						Show_HZ12_12(112,16,93,94);//����
						Show_HZ12_12(184,16,95,96);//��ȫ
						Display_Triangle(1);
						Display_arrow(1);
				}
		}
}

uint8_t Display_AddAdd(char *address)
{
		uint8_t num[2] = {'0','\0'};
		char temp = 0;
		Key_Control(1);
		Fill_RAM(0x00);
		Show_HZ12_12(0,7,53,54);//�Ƿ�
		Show_HZ12_12(32,7,24,25);//����
		Show_HZ12_12(64,7,17,18);//Ǯ��
		Asc8_16(96,7,"?");
		
		Show_HZ12_12(0,28,17,18);//Ǯ��
		Show_HZ12_12(32,28,8,9);//��ַ
		Asc8_16(64,28,":");

		Show_HZ12_12(46,47,111,112);//ȡ��
		Show_HZ12_12(112,47,113,114);//����
		Show_HZ12_12(184,47,111,112);//ȡ��
		Start_TIM(OLED_INPUT_TIME);
		while(1)
		{
				if((Get_TIM(OLED_INPUT_TIME))%INPUT_TIME_DIV == 0)
				{		
						temp = 30 - Get_TIM(OLED_INPUT_TIME)/INPUT_TIME_DIV;
						if(temp == 0)
						{
								Stop_TIM(OLED_INPUT_TIME);
								return 0;//��ʱ
						}
						num[0] = temp/10+0x30;
						Asc8_16(210,26,num);
						num[0] = temp%10+0x30;
						Asc8_16(218,26,num);
				}
				if(Key_Flag.flag.middle)
				{
						Key_Flag.flag.middle = 0;
						Fill_RAM(0x00);					
						Asc8_16(0,7,(uint8_t*)address);
						Show_HZ12_12(54,47,54,54);//��
						Show_HZ12_12(120,47,53,53);//��
						Show_HZ12_12(192,47,54,54);//��
						temp = 0;
						num[0] = '0';
						Start_TIM(OLED_INPUT_TIME);
						while(1)
						{
								if((Get_TIM(OLED_INPUT_TIME))%INPUT_TIME_DIV == 0)
								{		
										temp = 30 - Get_TIM(OLED_INPUT_TIME)/INPUT_TIME_DIV;
										if(temp == 0)
										{
												Stop_TIM(OLED_INPUT_TIME);
												return 0;//��ʱ
										}
										num[0] = temp/10+0x30;
										Asc8_16(210,26,num);
										num[0] = temp%10+0x30;
										Asc8_16(218,26,num);
								}								
								if(Key_Flag.flag.middle)
								{
										Key_Control(0);
										return 1;				//��ӵ�ַ�ɹ�
								}
								if(Key_Flag.flag.left||Key_Flag.flag.right)
								{
										Key_Control(0);
										return 0;				//��ӵ�ַʧ��
								}
						}
				}
				if(Key_Flag.flag.left||Key_Flag.flag.right)
				{
						Key_Control(0);
						return 0;
				}
		}
}

uint8_t Display_DelAdd(uint8_t AddID)
{
		unsigned char num[2] = {'0','\0'};
		char temp = 0;
		uint8_t value = 0;
		Key_Control(1);
		Fill_RAM(0x00);
		Show_HZ12_12(28,7,53,56);//�Ƿ�ɾ��		
		Asc8_16(92,7,(uint8_t*)showaddress[AddID].name);
		Show_HZ12_12(92+8*showaddress[AddID].len_name,7,59,61);//����˽
		Show_HZ12_12(140+8*showaddress[AddID].len_name,7,22,22);//Կ
		Asc8_16(156+8*showaddress[AddID].len_name,7,"?");
		Show_HZ12_12(80,28,62,67);//�ò���������
		Show_HZ12_12(58,47,54,54);//��
		Show_HZ12_12(120,47,53,53);//��
		Show_HZ12_12(182,47,54,54);//��
		Asc8_16(226,26,"s");
		Start_TIM(OLED_INPUT_TIME);
		while(1)
		{
				if((Get_TIM(OLED_INPUT_TIME))%INPUT_TIME_DIV == 0)
				{
						temp = 30 - Get_TIM(OLED_INPUT_TIME)/INPUT_TIME_DIV;
						if(temp == 0)
						{
								Stop_TIM(OLED_INPUT_TIME);
								return 0;//��ʱ
						}
						num[0] = temp/10+0x30;
						Asc8_16(210,26,num);
						num[0] = temp%10+0x30;
						Asc8_16(218,26,num);
				}
				if(Key_Flag.flag.middle)
				{
						value = 1;
						break;
				}
				if(Key_Flag.flag.left||Key_Flag.flag.right)
				{
						value = 0;
						break;
				}
		}
		Key_Control(0);
		return value;
}

uint8_t Display_SignData(SIGN_Out_Para *data,char* address,uint8_t address_name,uint8_t signdata_index)
{
		uint8_t num[2] = {'0','\0'};
		char temp = 0;
		//��ʾ��Ŀ   ��ֵΪһ��long long��  ��Ӧ��ʮ�������ĺ��λ��ΪС��
		int count_bit=0;
		int count_int = 0;//��ʾ��������ռ�õ���ʾλ��
		int count_dec = 8;//��ʾС�����ֺ�׺����ĸ���
		Key_Control(1);
		Fill_RAM(0x00);		
		Show_HZ12_12(0,0,122,122);//��
		Show_HZ12_12(16,0,8,9);//��ַ
		num[0] = address_name+0x30;
		Asc8_16(48,0,num);
		Show_HZ12_12(56,0,123,124);//����
		
		if(signdata_index != 0xff)
		{
				count_int = drawNumber(88,0,data->money[signdata_index]/100000000,8);
				if(data->money[signdata_index]%100000000)//����ֵ����Ϊ100000000����ʾBUG
				{
						Asc8_16(28+count_int*8,0,".");
						count_dec = drawxNumber(88+(count_int+1)*8,0,data->money[signdata_index]%100000000,8) - 1;//-1�ǰ�С�������ȥ
				}
				if(data->money[signdata_index] == 0)//ֵΪ0�����
						count_dec = 8;
				count_bit = 88 + (count_int + 8 -count_dec)*8 + 4; //88��ǰ��ռ�õ���ʾ��+4����ʾ��϶������
				
				if(data->coin == 0)
						Asc8_16(count_bit,0,"GAS");
				else if(data->coin == 1)
						Asc8_16(count_bit,0,"NEO");

				Show_HZ12_12(count_bit+24,0,125,125);//��
				Display_Triangle(0);
				while(Key_Flag.flag.middle == 0);
				Key_Flag.flag.middle = 0;
		}
		else
		{
				if(data->coin == 0)
						Asc8_16(88,0,"GAS");
				else if(data->coin == 1)
						Asc8_16(88,0,"NEO");
		}
		Fill_RAM(0x00);
		Asc8_16(48,0,(uint8_t*)address);
		Show_HZ12_12(46,47,111,112);//ȡ��
		Show_HZ12_12(112,47,113,114);//����
		Show_HZ12_12(184,47,111,112);//ȡ��
		Start_TIM(OLED_INPUT_TIME);
		while(1)
		{
				if((Get_TIM(OLED_INPUT_TIME))%INPUT_TIME_DIV == 0)
				{
						temp = 30 - Get_TIM(OLED_INPUT_TIME)/INPUT_TIME_DIV;
						if(temp == 0)
						{
								Stop_TIM(OLED_INPUT_TIME);
								return 0;//��ʱ
						}
						num[0] = temp/10+0x30;
						Asc8_16(210,26,num);
						num[0] = temp%10+0x30;
						Asc8_16(218,26,num);
				}
				if(Key_Flag.flag.middle)
				{
						Key_Control(0);
						if(Display_VerifyCode() == 0)//������ȷ
						{
								return 1;//ȷ������
						}
						else
						{
								return 0;
						}
				}
				if(Key_Flag.flag.left||Key_Flag.flag.right)
				{
						Key_Control(0);
						return 0;	//ȡ��
				}
		}
}

uint8_t Display_Sign_Data_Type_Identify(void)
{
		uint8_t num[2] = {'0','\0'};
		char temp = 0;
		uint8_t value = 0;
		Fill_RAM(0x00);
		Show_HZ12_12(40,20,65,65);//��
		Show_HZ12_12(56,20,53,53);//��
		Show_HZ12_12(72,20,99,100);//��Լ
		Show_HZ12_12(104,20,126,127);//����
		Show_HZ12_12(136,20,108,109);//����
		Asc8_16(168,20,",");
		Show_HZ12_12(176,20,113,114);//����
		Asc8_16(184,20,"?");
		Show_HZ12_12(46,47,111,112);//ȡ��
		Show_HZ12_12(112,47,113,114);//����
		Show_HZ12_12(184,47,111,112);//ȡ��
		Key_Control(1);
		Start_TIM(OLED_INPUT_TIME);
		while(1)
		{
				if((Get_TIM(OLED_INPUT_TIME))%INPUT_TIME_DIV == 0)
				{		
						temp = 30 - Get_TIM(OLED_INPUT_TIME)/INPUT_TIME_DIV;
						if(temp == 0)
						{
								Stop_TIM(OLED_INPUT_TIME);
								return 0;//��ʱ
						}
						num[0] = temp/10+0x30;
						Asc8_16(210,26,num);
						num[0] = temp%10+0x30;
						Asc8_16(218,26,num);
				}
				if(Key_Flag.flag.middle)//ȷ��
				{
						value = 1;
						break;
				}
				else if(Key_Flag.flag.left || Key_Flag.flag.right)//ȡ��
				{
						value = 0;
						break;
				}
		}
		Key_Control(0);	
		return value;
}

uint8_t Display_Updata_Wallet(void)
{
		uint8_t value = 0;
		Key_Control(1);
		Fill_RAM(0x00);
		Show_HZ12_12(64,7,68,68);//��
		Show_HZ12_12(80,7,32,32);//��
		Show_HZ12_12(96,7,69,70);//����
		Show_HZ12_12(128,7,17,18);//Ǯ��
		Show_HZ12_12(160,7,71,72);//����
		
		Show_HZ12_12(64,28,12,12);//��
		Show_HZ12_12(80,28,6,7);//ȷ��
		Show_HZ12_12(112,28,53,53);//��
		Show_HZ12_12(128,28,73,74);//����
		Show_HZ12_12(160,28,63,64);//����
	
		Show_HZ12_12(58,47,54,54);//��
		Show_HZ12_12(120,47,53,53);//��
		Show_HZ12_12(182,47,54,54);//��
		while(1)
		{
				if(Key_Flag.flag.middle)
				{
						Key_Flag.flag.middle = 0;
						break;
				}
				if(Key_Flag.flag.left||Key_Flag.flag.right)
				{		
						Key_Control(0);
						return 0;
				}
		}
		
		Fill_RAM(0x00);
		Show_HZ12_12(96,7,75,76);//����
		Show_HZ12_12(128,7,63,64);//����
		
		Show_HZ12_12(76,28,53,54);//�Ƿ�
		Show_HZ12_12(108,28,69,70);//����
		Show_HZ12_12(140,28,77,77);//��
		Show_HZ12_12(156,28,46,46);//��
		Asc8_16(172,28,"?");
		
		Show_HZ12_12(58,47,54,54);//��
		Show_HZ12_12(120,47,53,53);//��
		Show_HZ12_12(182,47,54,54);//��
		while(1)
		{
				if(Key_Flag.flag.middle)
				{
						if(Display_VerifyCode() == 0)//������ȷ
						{
								Neo_System.update = 1;
								value = 1;
						}
						else
							value = 0;
						break;
				}
				if(Key_Flag.flag.left||Key_Flag.flag.right)
				{		
						value = 0;
						break;
				}
		}	
		Key_Control(0);
		return value;
}


