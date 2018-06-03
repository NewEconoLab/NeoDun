#include "OledMenu.h"
#include "aw9136.h"
#include "app_interface.h"
#include "timer.h"
#include "atsha204a.h"
#include "Algorithm.h"
#include "OLED281/oled281.h"
#include "app_hal.h"
#include "app_oled.h"

static uint8_t main_RefreshDisplay = 0;//是否重新刷新界面标志
static uint8_t main_page_index = 0;		 //页面ID

/********************************
显示三角形
		direction
				1  向上的三角形
				0  向下的三角形
********************************/
void Display_Triangle(uint8_t direction)
{
		if(direction)
				Show_Pattern(&gImage_triangle_up[0],30,33,48,64);
		else
				Show_Pattern(&gImage_triangle_down[0],30,33,48,64);
}

/********************************
显示箭头
		direction
				0  向左的三角形
				1  向右的三角形
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
		Show_HZ12_12(112,32,2,3);//设置
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
				Show_HZ12_12(112,32,2,3);//设置
				Display_Triangle(1);				
		}
		if(Key_Flag.flag.right)//右键有效时，ID加1
		{
				Key_Flag.flag.right = 0;
		}
		if(Key_Flag.flag.left)//左键有效时，ID加1
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
				Show_HZ12_12(112,32,2,3);//设置
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
				Show_HZ12_12(188,32,2,3);//设置
				Display_Triangle(1);
				Display_arrow(1);			
				Display_Address(1,&showaddress[0]);
		}
		
		if(Key_Flag.flag.right)//右键有效时，ID加1
		{
				Key_Flag.flag.right = 0;
				if(main_page_index > 0)
				{
						main_page_index--;
						main_RefreshDisplay = 1;
				}
		}
		if(Key_Flag.flag.left)//左键有效时，ID加1
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
				Show_HZ12_12(112,32,2,3);//设置
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
				Show_HZ12_12(48,32,2,3);//设置
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
				Show_HZ12_12(188,32,2,3);//设置
				Display_Triangle(1);
				Display_arrow(0);
				Display_arrow(1);
				Display_Address(1,&showaddress[0]);
				Display_Address(0,&showaddress[1]);
		}
		if(Key_Flag.flag.left)//左键有效时，ID加1
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
		if(Key_Flag.flag.right)//右键有效时，ID加1
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
				Show_HZ12_12(112,32,2,3);//设置
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
				Show_HZ12_12(188,32,2,3);//设置
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
				Show_HZ12_12(48,32,2,3);//设置
				Display_Triangle(1);
				Display_arrow(0);
				Display_arrow(1);			
				Display_Address(1,&showaddress[2]);				
				Display_Address(2,&showaddress[1]);	
		}				
		
		if(Key_Flag.flag.left)//右键有效时，ID加1
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
		if(Key_Flag.flag.right)//左键有效时，ID减1
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
				Show_HZ12_12(112,32,2,3);//设置
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
				Show_HZ12_12(188,32,2,3);//设置
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
				Show_HZ12_12(48,32,2,3);//设置
				Display_Triangle(1);
				Display_arrow(0);
				Display_arrow(1);
				Display_Address(1,&showaddress[3]);
				Display_Address(2,&showaddress[2]);
		}
		
		if(Key_Flag.flag.left)//右键有效时，ID加1
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
		if(Key_Flag.flag.right)//左键有效时，ID减1
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
				Show_HZ12_12(112,32,2,3);//设置
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
				Show_HZ12_12(188,32,2,3);//设置
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
				Show_HZ12_12(48,32,2,3);//设置
				Display_Triangle(1);
				Display_arrow(0);
				Display_arrow(1);				
				Display_Address(1,&showaddress[4]);			
				Display_Address(2,&showaddress[3]);
		}
		
		if(Key_Flag.flag.left)//右键有效时，ID加1
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
		if(Key_Flag.flag.right)//左键有效时，ID减1
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
				if(verifyCodeGetPin(1,Neo_System.pin))//验证密码出错
				{
						Key_Control(1);
						Fill_RAM(0x00);
						Show_HZ12_12(56,16,4,5);//密码
						Show_HZ12_12(88,16,2,3);//设置
						Show_HZ12_12(120,16,35,36);//有误
						Show_HZ12_12(152,16,12,12);//请
						Show_HZ12_12(168,16,34,34);//重
						Show_HZ12_12(184,16,37,37);//试
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
				if(status == 5) //超时控制
				{
						return 1;//超时返回1
				}				
				if(count[0] < '5')
				{
						Key_Control(1);
						Fill_RAM(0x00);
						Show_HZ12_12(84,8,4,5);//密码
						Show_HZ12_12(116,8,38,38);//错
						Show_HZ12_12(132,8,36,36);//误
						Asc8_16(148,8,count);
						Show_HZ12_12(156,8,39,39);//次
						
						Show_HZ12_12(60,28,38,38);//错
						Show_HZ12_12(76,28,36,36);//误
						Asc8_16(92,28,"5");
						Show_HZ12_12(100,28,39,39);//次
						Show_HZ12_12(116,28,32,32);//将
						Show_HZ12_12(132,28,34,34);//重
						Show_HZ12_12(148,28,3,3);//置					
						Show_HZ12_12(164,28,2,2);//设
						Show_HZ12_12(180,28,43,43);//备
						Display_Triangle(0);
						while(Key_Flag.flag.middle==0);
						Key_Control(0);
				}
				else
				{
						Key_Control(1);
						Fill_RAM(0x00);
						Show_HZ12_12(84,8,4,5);//密码
						Show_HZ12_12(116,8,38,38);//错
						Show_HZ12_12(132,8,36,36);//误
						Asc8_16(148,8,"5");
						Show_HZ12_12(156,8,39,39);//次	

						Show_HZ12_12(80,28,2,2);//设
						Show_HZ12_12(96,28,43,43);//备
						Show_HZ12_12(112,28,40,41);//已被
						Show_HZ12_12(144,28,34,34);//重
						Show_HZ12_12(160,28,3,3);//置
						Display_Triangle(0);
						EmptyWallet();						 										  //清空钱包数据
						while(Key_Flag.flag.middle==0);
						Key_Flag.flag.middle=0;
						
						Fill_RAM(0x00);
						Show_HZ12_12(32,8,12,12);//请
						Show_HZ12_12(48,8,34,34);//重
						Show_HZ12_12(64,8,2,2);//设
						Show_HZ12_12(80,8,4,5);//密码
						Show_HZ12_12(112,8,42,42);//后
						Show_HZ12_12(128,8,24,25);//导入
						Show_HZ12_12(160,8,43,46);//备份文件
					
						Show_HZ12_12(16,28,47,50);//需要帮助
						Show_HZ12_12(80,28,12,12);//请
						Show_HZ12_12(96,28,51,52);//登陆
						Asc8_16(128,28,"www.neodun.com");
						Display_Triangle(0);
						while(Key_Flag.flag.middle==0);
						Key_Control(0);
						System_Reset();//错误5次系统重启
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
				if(status == 5) //超时控制
				{
						return 1;//超时返回1
				}				
				if(count[0] < '5')
				{
						Key_Control(1);
						Fill_RAM(0x00);
						Show_HZ12_12(84,8,4,5);//密码
						Show_HZ12_12(116,8,38,38);//错
						Show_HZ12_12(132,8,36,36);//误
						Asc8_16(148,8,count);
						Show_HZ12_12(156,8,39,39);//次
						
						Show_HZ12_12(60,28,38,38);//错
						Show_HZ12_12(76,28,36,36);//误
						Asc8_16(92,28,"5");
						Show_HZ12_12(100,28,39,39);//次
						Show_HZ12_12(116,28,32,32);//将
						Show_HZ12_12(132,28,34,34);//重
						Show_HZ12_12(148,28,3,3);//置
						Show_HZ12_12(164,28,17,18);//钱包
						Display_Triangle(0);
						while(Key_Flag.flag.middle==0);
						Key_Control(0);
				}
				else
				{
						Key_Control(1);
						Fill_RAM(0x00);
						Show_HZ12_12(84,8,4,5);//密码
						Show_HZ12_12(116,8,38,38);//错
						Show_HZ12_12(132,8,36,36);//误
						Asc8_16(148,8,"5");
						Show_HZ12_12(156,8,39,39);//次					
						
						Show_HZ12_12(80,28,17,18);//钱包
						Show_HZ12_12(112,28,40,41);//已被
						Show_HZ12_12(144,28,34,34);//重
						Show_HZ12_12(160,28,3,3);//置
						Display_Triangle(0);
						EmptyWallet();						 										  //清空钱包数据
						while(Key_Flag.flag.middle==0);
						Key_Flag.flag.middle=0;
						
						Fill_RAM(0x00);
						Show_HZ12_12(32,8,12,12);//请
						Show_HZ12_12(48,8,34,34);//重
						Show_HZ12_12(64,8,2,2);//设
						Show_HZ12_12(80,8,4,5);//密码
						Show_HZ12_12(112,8,42,42);//后
						Show_HZ12_12(128,8,24,25);//导入
						Show_HZ12_12(160,8,43,46);//备份文件
					
						Show_HZ12_12(16,28,47,50);//需要帮助
						Show_HZ12_12(80,28,12,12);//请
						Show_HZ12_12(96,28,51,52);//登陆
						Asc8_16(128,28,"www.neodun.com");
						Display_Triangle(0);
						while(Key_Flag.flag.middle==0);
						Key_Control(0);
						System_Reset();//错误5次系统重启
				}
		}
}

uint8_t Display_VerifyCode_PowerOn(void)
{
		uint8_t count[2] = {'0','\0'};
		while(verifyCodeGetPin(0,Neo_System.pin))//验证密码出错	
		{
				count[0]++;
				if(count[0] < '5')
				{
						Key_Control(1);
						Fill_RAM(0x00);
						Show_HZ12_12(84,8,4,5);//密码
						Show_HZ12_12(116,8,38,38);//错
						Show_HZ12_12(132,8,36,36);//误
						Asc8_16(148,8,count);
						Show_HZ12_12(156,8,39,39);//次
						
						Show_HZ12_12(60,28,38,38);//错
						Show_HZ12_12(76,28,36,36);//误
						Asc8_16(92,28,"5");
						Show_HZ12_12(100,28,39,39);//次
						Show_HZ12_12(116,28,32,32);//将
						Show_HZ12_12(132,28,34,34);//重
						Show_HZ12_12(148,28,3,3);//置
						Show_HZ12_12(164,28,2,2);//设
						Show_HZ12_12(180,28,43,43);//备
						Display_Triangle(0);
						while(Key_Flag.flag.middle==0);
						Key_Control(0);
				}
				else
				{
						Key_Control(1);
						Fill_RAM(0x00);
						Show_HZ12_12(84,8,4,5);//密码
						Show_HZ12_12(116,8,38,38);//错
						Show_HZ12_12(132,8,36,36);//误
						Asc8_16(148,8,"5");
						Show_HZ12_12(156,8,39,39);//次					
						
						Show_HZ12_12(80,28,2,2);//设
						Show_HZ12_12(96,28,43,43);//备
						Show_HZ12_12(112,28,40,41);//已被
						Show_HZ12_12(144,28,34,34);//重
						Show_HZ12_12(160,28,3,3);//置
						Display_Triangle(0);
						Neo_System.new_wallet = 1;										  //更改为新钱包
						EmptyWallet();						 										  //清空钱包数据
						Update_PowerOn_SetFlag(&Set_Flag,&Neo_System);	//更新系统设置标识
						while(Key_Flag.flag.middle==0);
						Key_Flag.flag.middle=0;
						
						Fill_RAM(0x00);
						Show_HZ12_12(32,8,12,12);//请
						Show_HZ12_12(48,8,34,34);//重
						Show_HZ12_12(64,8,2,2);//设
						Show_HZ12_12(80,8,4,5);//密码
						Show_HZ12_12(112,8,42,42);//后
						Show_HZ12_12(128,8,24,25);//导入
						Show_HZ12_12(160,8,43,46);//备份文件
					
						Show_HZ12_12(16,28,47,50);//需要帮助
						Show_HZ12_12(80,28,12,12);//请
						Show_HZ12_12(96,28,51,52);//登陆
						Asc8_16(128,28,"www.neodun.com");
						Display_Triangle(0);
						while(Key_Flag.flag.middle==0);
						Key_Control(0);
						return 1;//错误5次，返回1
				}
		}
		return 0;//验证成功返回0
}

/******************************************************************
说明：
		AddID传进来的ID（1-5），实际对应内存中的地址（0-4）
******************************************************************/
void Display_Click_Add(uint8_t AddID)
{
		uint8_t RefreshDisplay = 1;//是否重新刷新界面标志
		uint8_t page_index = 0;		//页面ID
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
								Show_HZ12_12(72,7,84,85);//隔离
								Show_HZ12_12(104,7,23,23);//并
								Show_HZ12_12(120,7,86,87);//冻结
								Show_HZ12_12(152,7,17,18);//钱包
							
								Show_HZ12_12(32,28,88,90);//外部软
								Show_HZ12_12(80,28,46,46);//件
								Show_HZ12_12(96,28,32,32);//将
								Show_HZ12_12(112,28,91,92);//无法
								Show_HZ12_12(144,28,63,64);//操作
								Show_HZ12_12(176,28,73,73);//本
								Show_HZ12_12(192,28,17,18);//钱包							
						}
						else
						{
								Show_HZ12_12(80,7,65,65);//不
								Show_HZ12_12(96,7,130,130);//再
								Show_HZ12_12(112,7,84,85);//隔离
								Show_HZ12_12(144,7,17,18);//钱包	
								
								Show_HZ12_12(40,28,131,132);//允许
								Show_HZ12_12(72,28,88,90);//外部软
								Show_HZ12_12(120,28,46,46);//件
								Show_HZ12_12(136,28,63,64);//操作
								Show_HZ12_12(168,28,73,73);//本
								Show_HZ12_12(184,28,17,18);//钱包								
						}
						Show_HZ12_12(58,47,54,54);//否
						Show_HZ12_12(120,47,53,53);//是
						Show_HZ12_12(182,47,54,54);//否						
						while(1)
						{
								if(Key_Flag.flag.middle)
								{
										//隐藏地址操作
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
						Show_HZ12_12(96,16,78,79);//查看
						Show_HZ12_12(128,16,8,9);//地址
						if(showaddress[AddID-1].hide)
						{
								Show_HZ12_12(178,16,111,112);//取消
								Show_HZ12_12(210,16,80,81);//隐藏
						}
						else
						{
								Show_HZ12_12(178,16,80,81);//隐藏
								Show_HZ12_12(210,16,17,18);//钱包
						}
						Display_Triangle(1);
						Display_arrow(1);
				}
				if((page_index == 1)&&(RefreshDisplay == 1))
				{
						RefreshDisplay = 0;
						Fill_RAM(0x00);
						Show_HZ12_12(15,16,78,79);//查看
						Show_HZ12_12(47,16,8,9);//地址
						if(showaddress[AddID-1].hide)
						{
								Show_HZ12_12(96,16,111,112);//取消
								Show_HZ12_12(128,16,80,81);//隐藏
						}
						else
						{
								Show_HZ12_12(96,16,80,81);//隐藏
								Show_HZ12_12(128,16,17,18);//钱包
						}
						Show_HZ12_12(184,16,82,83);//返回
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
								Show_HZ12_12(15,16,111,112);//取消
								Show_HZ12_12(47,16,80,81);//隐藏
						}
						else
						{
								Show_HZ12_12(15,16,80,81);//隐藏
								Show_HZ12_12(47,16,17,18);//钱包
						}
						Show_HZ12_12(112,16,82,83);//返回
						Display_Triangle(1);
						Display_arrow(0);
				}
				
				if(Key_Flag.flag.right)//右键有效时，ID加1
				{
						Key_Flag.flag.right = 0;
						if(page_index<2)
						{
								page_index++;
								RefreshDisplay = 1;
						}
				}
				if(Key_Flag.flag.left)//左键有效时，ID加1
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
						Show_HZ12_12(96,16,78,79);//查看
						Show_HZ12_12(128,16,8,9);//地址
						if(showaddress[AddID].hide)
						{
								Show_HZ12_12(178,16,111,112);//取消
								Show_HZ12_12(210,16,80,81);//隐藏
						}
						else
						{
								Show_HZ12_12(178,16,80,81);//隐藏
								Show_HZ12_12(210,16,17,18);//钱包
						}
						Display_Triangle(1);
						Display_arrow(1);
				}
		}
}

void Display_Set_Coin_NEO(void)
{
		uint8_t RefreshDisplay = 1;//是否重新刷新界面标志
		uint8_t page_index = 0;		//页面ID
		while(1)
		{
				if((Key_Flag.flag.middle)&&(page_index == 0))
				{
						Key_Flag.flag.middle = 0;
						Fill_RAM(0x00);
						Show_HZ12_12(88,16,97,97);//版
						Show_HZ12_12(104,16,73,73);//本				
						Show_HZ12_12(120,16,98,98);//号
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
					  Show_HZ12_12(104,16,97,97);//版
						Show_HZ12_12(120,16,73,73);//本
						Show_HZ12_12(136,16,98,98);//号
						
						Show_HZ12_12(184,16,99,101);//合约签
						Show_HZ12_12(232,16,57,57);//名
						Display_Triangle(1);
						Display_arrow(1);					
				}
				if((page_index == 1)&&(RefreshDisplay == 1))
				{
						RefreshDisplay = 0;
						Fill_RAM(0x00);
						Show_HZ12_12(30,16,97,97);//版
						Show_HZ12_12(46,16,73,73);//本
						Show_HZ12_12(62,16,98,98);//号
						
						Show_HZ12_12(96,16,99,101);//合约签
						Show_HZ12_12(144,16,57,57);//名
						Show_HZ12_12(184,16,82,83);//返回
						Display_Triangle(1);	
						Display_arrow(0);	
						Display_arrow(1);
				}
				if((page_index == 2)&&(RefreshDisplay == 1))
				{
						RefreshDisplay = 0;
						Fill_RAM(0x00);
						Show_HZ12_12(20,16,99,101);//合约签
						Show_HZ12_12(68,16,57,57);//名
						Show_HZ12_12(112,16,82,83);//返回
						Display_Triangle(1);	
						Display_arrow(0);			
				}
				
				if(Key_Flag.flag.right)//右键有效时，ID加1
				{
						Key_Flag.flag.right = 0;
						if(page_index<2)
						{
								page_index++;
								RefreshDisplay = 1;
						}
				}
				if(Key_Flag.flag.left)//左键有效时，ID加1
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
						Show_HZ12_12(104,16,97,97);//版
						Show_HZ12_12(120,16,73,73);//本
						Show_HZ12_12(136,16,98,98);//号
						
						Show_HZ12_12(184,16,99,101);//合约签
						Show_HZ12_12(232,16,57,57);//名
						
						Display_Triangle(1);
						Display_arrow(1);
				}					
		}
}

void Display_Set_Coin(void)
{
		uint8_t RefreshDisplay = 1;//是否重新刷新界面标志
		uint8_t page_index = 0;		//页面ID	
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
						Show_HZ12_12(184,16,82,83);//返回
						Display_Triangle(1);
						Display_arrow(1);					
				}
				if((page_index == 1)&&(RefreshDisplay == 1))
				{
						RefreshDisplay = 0;
						Fill_RAM(0x00);
						Asc8_16(50,16,"NEO");
						Show_HZ12_12(112,16,82,83);//返回
						Display_Triangle(1);	
						Display_arrow(0);			
				}	
				
				if(Key_Flag.flag.right)//右键有效时，ID加1
				{
						Key_Flag.flag.right = 0;
						if(page_index<1)
						{
								page_index++;
								RefreshDisplay = 1;
						}
				}
				if(Key_Flag.flag.left)//左键有效时，ID加1
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
						Show_HZ12_12(184,16,82,83);//返回
						Display_Triangle(1);
						Display_arrow(1);
				}					
		}
}

void Display_Set_About(void)
{
		uint8_t RefreshDisplay = 1;//是否重新刷新界面标志
		uint8_t page_index = 0;		//页面ID
		while(1)
		{
				if((Key_Flag.flag.middle)&&(page_index == 0))
				{
						Key_Flag.flag.middle = 0;
						Fill_RAM(0x00);
						Show_HZ12_12(88,16,97,97);//版
						Show_HZ12_12(104,16,73,73);//本				
						Show_HZ12_12(120,16,98,98);//号
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
						Show_HZ12_12(56,8,47,50);//需要帮助
						Show_HZ12_12(120,8,12,12);//请
						Show_HZ12_12(136,8,51,52);//登陆
						Show_HZ12_12(168,8,104,105);//官网
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
						Show_HZ12_12(96,16,77,77);//固
						Show_HZ12_12(112,16,46,46);//件
						Show_HZ12_12(128,16,97,97);//版
						Show_HZ12_12(144,16,73,73);//本
						Show_HZ12_12(184,16,49,50);//帮助
						Display_Triangle(1);
						Display_arrow(1);			
				}
				if((page_index == 1)&&(RefreshDisplay == 1))
				{
						RefreshDisplay = 0;
						Fill_RAM(0x00);
						Show_HZ12_12(20,16,77,77);//固
						Show_HZ12_12(36,16,46,46);//件
						Show_HZ12_12(52,16,97,97);//版
						Show_HZ12_12(68,16,73,73);//本
						Show_HZ12_12(112,16,49,50);//帮助
						Show_HZ12_12(184,16,82,83);//返回
						Display_Triangle(1);	
						Display_arrow(0);	
						Display_arrow(1);
				}	
				if((page_index == 2)&&(RefreshDisplay == 1))
				{
						RefreshDisplay = 0;
						Fill_RAM(0x00);
						Show_HZ12_12(44,16,49,50);//帮助
						Show_HZ12_12(112,16,82,83);//返回
						Display_Triangle(1);	
						Display_arrow(0);				
				}
				
				if(Key_Flag.flag.right)//右键有效时，ID加1
				{
						Key_Flag.flag.right = 0;
						if(page_index<2)
						{
								page_index++;
								RefreshDisplay = 1;
						}
				}
				if(Key_Flag.flag.left)//左键有效时，ID加1
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
						Show_HZ12_12(96,16,77,77);//固
						Show_HZ12_12(112,16,46,46);//件
						Show_HZ12_12(128,16,97,97);//版
						Show_HZ12_12(144,16,73,73);//本
						Show_HZ12_12(184,16,49,50);//帮助
						Display_Triangle(1);
						Display_arrow(1);
				}					
		}
}

void Display_Set_Security_ResetWallet(void)
{
		Fill_RAM(0x00);
		Show_HZ12_12(64,7,68,68);//即
		Show_HZ12_12(80,7,32,32);//将
		Show_HZ12_12(96,7,55,56);//删除
		Show_HZ12_12(128,7,96,96);//全
		Show_HZ12_12(144,7,89,89);//部
		Show_HZ12_12(160,7,108,109);//数据
	
		Show_HZ12_12(72,28,55,56);//删除
		Show_HZ12_12(104,28,42,42);//后
		Show_HZ12_12(120,28,91,92);//无法
		Show_HZ12_12(152,28,110,110);//找
		Show_HZ12_12(168,28,83,83);//回
	
		Show_HZ12_12(46,47,111,112);//取消
		Show_HZ12_12(112,47,113,114);//继续
		Show_HZ12_12(184,47,111,112);//取消
		
		while(1)
		{
				if(Key_Flag.flag.middle)
				{
						Key_Flag.flag.middle = 0;
						Fill_RAM(0x00);
						Show_HZ12_12(96,7,75,76);//风险
						Show_HZ12_12(128,7,63,64);//操作
					
						Show_HZ12_12(76,28,53,54);//是否
						Show_HZ12_12(108,28,34,34);//重
						Show_HZ12_12(124,28,3,3);//置
						Show_HZ12_12(140,28,2,2);//设
						Show_HZ12_12(156,28,43,43);//备
						Asc8_16(172,28,"?");
					
						Show_HZ12_12(46,47,111,112);//取消
						Show_HZ12_12(112,47,113,114);//继续
						Show_HZ12_12(184,47,111,112);//取消						
						while(1)
						{
								if(Key_Flag.flag.middle)
								{
										Key_Flag.flag.middle = 0;
										if(Display_VerifyCode() == 0)
										{
												Fill_RAM(0x00);
												Show_HZ12_12(96,8,34,34);//重
												Show_HZ12_12(112,8,3,3);//置
												Show_HZ12_12(128,8,27,27);//成
												Show_HZ12_12(144,8,115,115);//功
											
												Show_HZ12_12(72,28,116,118);//牛顿忘
												Show_HZ12_12(120,28,29,29);//记
												Show_HZ12_12(136,28,119,121);//了一切
												Display_Triangle(0);
											
												Key_Control(1);
												while(Key_Flag.flag.middle == 0);
												Key_Flag.flag.middle = 0;
												EmptyWallet();				//清空钱包数据
												HAL_Delay(50);
												System_Reset();//系统重启
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
		uint8_t RefreshDisplay = 1;//是否重新刷新界面标志
		uint8_t page_index = 0;		//页面ID
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
						Show_HZ12_12(100,16,106,107);//更改
						Asc8_16(132,16,"PIN");
						Show_HZ12_12(188,16,34,34);//重
						Show_HZ12_12(204,16,3,3);//置
						Show_HZ12_12(220,16,2,2);//设
						Show_HZ12_12(236,16,43,43);//备
						Display_Triangle(1);
						Display_arrow(1);			
				}
				if((page_index == 1)&&(RefreshDisplay == 1))
				{
						RefreshDisplay = 0;
						Fill_RAM(0x00);
						Show_HZ12_12(20,16,106,107);//更改
						Asc8_16(52,16,"PIN");
						Show_HZ12_12(96,16,34,34);//重
						Show_HZ12_12(112,16,3,3);//置
						Show_HZ12_12(128,16,2,2);//设
						Show_HZ12_12(144,16,43,43);//备
						Show_HZ12_12(184,16,82,83);//返回
						Display_Triangle(1);	
						Display_arrow(0);	
						Display_arrow(1);
				}	
				if((page_index == 2)&&(RefreshDisplay == 1))
				{
						RefreshDisplay = 0;
						Fill_RAM(0x00);
						Show_HZ12_12(30,16,34,34);//重
						Show_HZ12_12(46,16,3,3);//置
						Show_HZ12_12(62,16,2,2);//设
						Show_HZ12_12(78,16,43,43);//备
						Show_HZ12_12(112,16,82,83);//返回
						Display_Triangle(1);	
						Display_arrow(0);			
				}
				
				if(Key_Flag.flag.right)//右键有效时，ID加1
				{
						Key_Flag.flag.right = 0;
						if(page_index<2)
						{
								page_index++;
								RefreshDisplay = 1;
						}
				}
				if(Key_Flag.flag.left)//左键有效时，ID加1
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
						Show_HZ12_12(100,16,106,107);//更改
						Asc8_16(132,16,"PIN");
						Show_HZ12_12(168,16,34,34);//重
						Show_HZ12_12(184,16,3,3);//置
						Show_HZ12_12(200,16,2,2);//设
						Show_HZ12_12(216,16,43,43);//备
						Display_Triangle(1);
						Display_arrow(1);
				}					
		}		
}

void Display_Click_Set(void)
{
		uint8_t RefreshDisplay = 1;//是否重新刷新界面标志
		uint8_t page_index = 0;		//页面ID		
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
						Show_HZ12_12(112,16,93,94);//币种
						Show_HZ12_12(184,16,95,96);//安全
						Display_Triangle(1);
						Display_arrow(1);					
				}
				if((page_index == 1)&&(RefreshDisplay == 1))
				{
						RefreshDisplay = 0;
						Fill_RAM(0x00);
						Show_HZ12_12(46,16,93,94);//币种
						Show_HZ12_12(112,16,95,96);//安全
						Show_HZ12_12(184,16,102,103);//关于
						Display_Triangle(1);
						Display_arrow(0);
						Display_arrow(1);					
				}
				if((page_index == 2)&&(RefreshDisplay == 1))
				{
						RefreshDisplay = 0;
						Fill_RAM(0x00);
						Show_HZ12_12(46,16,95,96);//安全
						Show_HZ12_12(112,16,102,103);//关于
						Show_HZ12_12(184,16,82,83);//返回
						Display_Triangle(1);
						Display_arrow(1);
						Display_arrow(0);				
				}			
				if((page_index == 3)&&(RefreshDisplay == 1))
				{
						RefreshDisplay = 0;
						Fill_RAM(0x00);
						Show_HZ12_12(46,16,102,103);//关于
						Show_HZ12_12(112,16,82,83);//返回
						Display_Triangle(1);
						Display_arrow(0);		
				}	
				
				if(Key_Flag.flag.right)//右键有效时，ID加1
				{
						Key_Flag.flag.right = 0;
						if(page_index<3)
						{
								page_index++;
								RefreshDisplay = 1;
						}
				}
				if(Key_Flag.flag.left)//左键有效时，ID加1
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
						Show_HZ12_12(112,16,93,94);//币种
						Show_HZ12_12(184,16,95,96);//安全
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
		Show_HZ12_12(0,7,53,54);//是否
		Show_HZ12_12(32,7,24,25);//导入
		Show_HZ12_12(64,7,17,18);//钱包
		Asc8_16(96,7,"?");
		
		Show_HZ12_12(0,28,17,18);//钱包
		Show_HZ12_12(32,28,8,9);//地址
		Asc8_16(64,28,":");

		Show_HZ12_12(46,47,111,112);//取消
		Show_HZ12_12(112,47,113,114);//继续
		Show_HZ12_12(184,47,111,112);//取消
		Start_TIM(OLED_INPUT_TIME);
		while(1)
		{
				if((Get_TIM(OLED_INPUT_TIME))%INPUT_TIME_DIV == 0)
				{		
						temp = 30 - Get_TIM(OLED_INPUT_TIME)/INPUT_TIME_DIV;
						if(temp == 0)
						{
								Stop_TIM(OLED_INPUT_TIME);
								return 0;//超时
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
						Show_HZ12_12(54,47,54,54);//否
						Show_HZ12_12(120,47,53,53);//是
						Show_HZ12_12(192,47,54,54);//否
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
												return 0;//超时
										}
										num[0] = temp/10+0x30;
										Asc8_16(210,26,num);
										num[0] = temp%10+0x30;
										Asc8_16(218,26,num);
								}								
								if(Key_Flag.flag.middle)
								{
										Key_Control(0);
										return 1;				//添加地址成功
								}
								if(Key_Flag.flag.left||Key_Flag.flag.right)
								{
										Key_Control(0);
										return 0;				//添加地址失败
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
		Show_HZ12_12(28,7,53,56);//是否删除		
		Asc8_16(92,7,(uint8_t*)showaddress[AddID].name);
		Show_HZ12_12(92+8*showaddress[AddID].len_name,7,59,61);//及其私
		Show_HZ12_12(140+8*showaddress[AddID].len_name,7,22,22);//钥
		Asc8_16(156+8*showaddress[AddID].len_name,7,"?");
		Show_HZ12_12(80,28,62,67);//该操作不可逆
		Show_HZ12_12(58,47,54,54);//否
		Show_HZ12_12(120,47,53,53);//是
		Show_HZ12_12(182,47,54,54);//否
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
								return 0;//超时
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
		//显示数目   该值为一个long long型  对应的十进制数的后八位数为小数
		int count_bit=0;
		int count_int = 0;//表示整数部分占用的显示位数
		int count_dec = 8;//表示小数部分后缀的零的个数
		Key_Control(1);
		Fill_RAM(0x00);		
		Show_HZ12_12(0,0,122,122);//从
		Show_HZ12_12(16,0,8,9);//地址
		num[0] = address_name+0x30;
		Asc8_16(48,0,num);
		Show_HZ12_12(56,0,123,124);//发送
		
		if(signdata_index != 0xff)
		{
				count_int = drawNumber(88,0,data->money[signdata_index]/100000000,8);
				if(data->money[signdata_index]%100000000)//消除值正好为100000000的显示BUG
				{
						Asc8_16(28+count_int*8,0,".");
						count_dec = drawxNumber(88+(count_int+1)*8,0,data->money[signdata_index]%100000000,8) - 1;//-1是把小数点算进去
				}
				if(data->money[signdata_index] == 0)//值为0的情况
						count_dec = 8;
				count_bit = 88 + (count_int + 8 -count_dec)*8 + 4; //88是前面占用的显示，+4是显示空隙，美观
				
				if(data->coin == 0)
						Asc8_16(count_bit,0,"GAS");
				else if(data->coin == 1)
						Asc8_16(count_bit,0,"NEO");

				Show_HZ12_12(count_bit+24,0,125,125);//到
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
		Show_HZ12_12(46,47,111,112);//取消
		Show_HZ12_12(112,47,113,114);//继续
		Show_HZ12_12(184,47,111,112);//取消
		Start_TIM(OLED_INPUT_TIME);
		while(1)
		{
				if((Get_TIM(OLED_INPUT_TIME))%INPUT_TIME_DIV == 0)
				{
						temp = 30 - Get_TIM(OLED_INPUT_TIME)/INPUT_TIME_DIV;
						if(temp == 0)
						{
								Stop_TIM(OLED_INPUT_TIME);
								return 0;//超时
						}
						num[0] = temp/10+0x30;
						Asc8_16(210,26,num);
						num[0] = temp%10+0x30;
						Asc8_16(218,26,num);
				}
				if(Key_Flag.flag.middle)
				{
						Key_Control(0);
						if(Display_VerifyCode() == 0)//密码正确
						{
								return 1;//确定交易
						}
						else
						{
								return 0;
						}
				}
				if(Key_Flag.flag.left||Key_Flag.flag.right)
				{
						Key_Control(0);
						return 0;	//取消
				}
		}
}

uint8_t Display_Sign_Data_Type_Identify(void)
{
		uint8_t num[2] = {'0','\0'};
		char temp = 0;
		uint8_t value = 0;
		Fill_RAM(0x00);
		Show_HZ12_12(40,20,65,65);//不
		Show_HZ12_12(56,20,53,53);//是
		Show_HZ12_12(72,20,99,100);//合约
		Show_HZ12_12(104,20,126,127);//交易
		Show_HZ12_12(136,20,108,109);//数据
		Asc8_16(168,20,",");
		Show_HZ12_12(176,20,113,114);//继续
		Asc8_16(184,20,"?");
		Show_HZ12_12(46,47,111,112);//取消
		Show_HZ12_12(112,47,113,114);//继续
		Show_HZ12_12(184,47,111,112);//取消
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
								return 0;//超时
						}
						num[0] = temp/10+0x30;
						Asc8_16(210,26,num);
						num[0] = temp%10+0x30;
						Asc8_16(218,26,num);
				}
				if(Key_Flag.flag.middle)//确定
				{
						value = 1;
						break;
				}
				else if(Key_Flag.flag.left || Key_Flag.flag.right)//取消
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
		Show_HZ12_12(64,7,68,68);//即
		Show_HZ12_12(80,7,32,32);//将
		Show_HZ12_12(96,7,69,70);//升级
		Show_HZ12_12(128,7,17,18);//钱包
		Show_HZ12_12(160,7,71,72);//程序
		
		Show_HZ12_12(64,28,12,12);//请
		Show_HZ12_12(80,28,6,7);//确认
		Show_HZ12_12(112,28,53,53);//是
		Show_HZ12_12(128,28,73,74);//本人
		Show_HZ12_12(160,28,63,64);//操作
	
		Show_HZ12_12(58,47,54,54);//否
		Show_HZ12_12(120,47,53,53);//是
		Show_HZ12_12(182,47,54,54);//否
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
		Show_HZ12_12(96,7,75,76);//风险
		Show_HZ12_12(128,7,63,64);//操作
		
		Show_HZ12_12(76,28,53,54);//是否
		Show_HZ12_12(108,28,69,70);//升级
		Show_HZ12_12(140,28,77,77);//固
		Show_HZ12_12(156,28,46,46);//件
		Asc8_16(172,28,"?");
		
		Show_HZ12_12(58,47,54,54);//否
		Show_HZ12_12(120,47,53,53);//是
		Show_HZ12_12(182,47,54,54);//否
		while(1)
		{
				if(Key_Flag.flag.middle)
				{
						if(Display_VerifyCode() == 0)//密码正确
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


