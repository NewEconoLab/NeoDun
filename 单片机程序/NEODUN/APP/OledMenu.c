#include "OledMenu.h"
#include "aw9136.h"
#include "app_interface.h"
#include "tick_stm32.h"
#include "atsha204a.h"
#include "Algorithm.h"
#include "OLED281/oled281.h"
#include "app_hal.h"
#include "app_oled.h"

#define INIT_PAGE_INDEX		1

/********************************
	左基准   60
	右基准		196
********************************/
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

/********************************
显示实心三角形
		direction
				0  向左的三角形
				1  向右的三角形
		pos 
				0  左
				1  中
				2  右
********************************/
void Display_Fill_Triangle(uint8_t direction,uint8_t pos)
{
		if(direction)
		{
				if(pos == 0)
						Show_Pattern(&gImage_arrow_fill_right[0],13,16,48,64);
				else if(pos == 1)
						Show_Pattern(&gImage_arrow_fill_right[0],30,33,48,64);
				else if(pos == 2)
						Show_Pattern(&gImage_arrow_fill_right[0],47,50,48,64);
		}
		else
		{
				if(pos == 0)
						Show_Pattern(&gImage_arrow_fill_left[0],13,16,48,64);
				else if(pos == 1)
						Show_Pattern(&gImage_arrow_fill_left[0],30,33,48,64);
				else if(pos == 2)
						Show_Pattern(&gImage_arrow_fill_left[0],47,50,48,64);
		}
}

void Display_Usb(void)
{
		Fill_RAM(0x00);
		Show_Pattern(&gImage_Usb[0],29,34,20,44);
		clearArea(116,44,24,1);
}

void Display_Set(uint8_t state)
{
		#ifdef Chinese
				if(state == 0)				//left
				{
						Show_Pattern(&gImage_Set[0],11,18,0,32);
						clearArea(44,32,32,1);
						Show_HZ12_12(44,32,2,3);//设置
				}
				else if(state ==1)		//midddle
				{
						Show_Pattern(&gImage_Set[0],28,35,0,32);
						clearArea(112,32,32,1);
						Show_HZ12_12(112,32,2,3);//设置
				}
				else if(state == 2)		//right
				{
						Show_Pattern(&gImage_Set[0],45,52,0,32);
						clearArea(180,32,32,1);
						Show_HZ12_12(180,32,2,3);//设置
				}		
		#endif
		#ifdef English
				if(state == 0)				//left
				{
						Show_Pattern(&gImage_Set[0],11,18,0,32);
						clearArea(44,32,32,1);
						Show_AscII_Picture(32,32,SettingsBitmapDot,sizeof(SettingsBitmapDot));
				}
				else if(state ==1)		//midddle
				{
						Show_Pattern(&gImage_Set[0],28,35,0,32);
						clearArea(112,32,32,1);
						Show_AscII_Picture(100,32,SettingsBitmapDot,sizeof(SettingsBitmapDot));
				}
				else if(state == 2)		//right
				{
						Show_Pattern(&gImage_Set[0],45,52,0,32);
						clearArea(180,32,32,1);
						Show_AscII_Picture(168,32,SettingsBitmapDot,sizeof(SettingsBitmapDot));
				}			
		#endif
}

uint8_t Display_Time_count(void)
{
		static uint8_t num[2] = {'0','\0'};
		if((Get_TIM(OLED_INPUT_TIME))%INPUT_TIME_DIV == 0)
		{
				unsigned char temp = 30 - Get_TIM(OLED_INPUT_TIME)/INPUT_TIME_DIV;
				if(temp == 0)
				{
						Stop_TIM(OLED_INPUT_TIME);
						return 0;//超时
				}
				num[0] = temp/10+0x30;
				Asc8_16(228,28,num);
				num[0] = temp%10+0x30;
				Asc8_16(236,28,num);
		}
		return 1;
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
						#ifdef Chinese
								Asc8_16(40,32,"uname");
						#endif
						#ifdef English
								Show_AscII_Picture(36,32,NoNameBitmapDot,sizeof(NoNameBitmapDot));
						#endif
				}
				else if(state ==1)		//midddle
				{
						if(address->hide)
								Show_Pattern(&gImage_Address_hide[0],28,35,0,32);
						else
								Show_Pattern(&gImage_Address[0],28,35,0,32);
						clearArea(112,32,32,1);
						#ifdef Chinese
								Asc8_16(108,32,"uname");
						#endif
						#ifdef English
								Show_AscII_Picture(104,32,NoNameBitmapDot,sizeof(NoNameBitmapDot));
						#endif						
				}
				else if(state == 2)		//right
				{
						if(address->hide)
								Show_Pattern(&gImage_Address_hide[0],45,52,0,32);
						else
								Show_Pattern(&gImage_Address[0],45,52,0,32);
						clearArea(180,32,32,1);	
						#ifdef Chinese
								Asc8_16(176,32,"uname");
						#endif
						#ifdef English
								Show_AscII_Picture(172,32,NoNameBitmapDot,sizeof(NoNameBitmapDot));
						#endif						
				}		
		}
}

static void Display_MainPage_0Add(void)
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
				Display_Set(1);
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

static void Display_MainPage_1Add(void)
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
				Display_Set(1);
				Display_Triangle(1);
				Display_arrow(0);
				Display_Address(0,&showaddress[0]);				
		}
		if((main_page_index == 1)&&(main_RefreshDisplay == 1))
		{
				main_RefreshDisplay = 0;
				Fill_RAM(0x00);
				Display_Set(2);
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

static void Display_MainPage_2Add(void)
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
		
		if((main_page_index == 0)&&(main_RefreshDisplay == 1))
		{
				main_RefreshDisplay = 0;
				Fill_RAM(0x00);
				Display_Set(1);
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
				Display_Set(2);
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
				Display_Set(0);
				Display_Triangle(1);
				Display_arrow(0);
				Display_arrow(1);
				Display_Address(1,&showaddress[1]);
				Display_Address(2,&showaddress[0]);
		}
		if(Key_Flag.flag.left)//左键有效时，ID加1
		{
				Key_Flag.flag.left = 0;			
				if(main_page_index < 2)
				{
						main_page_index++;
						main_RefreshDisplay = 1;
				}
				else if(main_page_index == 2)
				{
						main_page_index = 0;
						main_RefreshDisplay = 1;
				}					
		}
		if(Key_Flag.flag.right)//右键有效时，ID加1
		{
				Key_Flag.flag.right = 0;		
				if(main_page_index > 0)
				{
						main_page_index--;
						main_RefreshDisplay = 1;
				}
				else if(main_page_index == 0)
				{
						main_page_index = 2;
						main_RefreshDisplay = 1;
				}					
		}
}

static void Display_MainPage_3Add(void)
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
				Display_Set(1);
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
				Display_Set(2);
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
				Display_Set(0);
				Display_Triangle(1);
				Display_arrow(0);
				Display_arrow(1);			
				Display_Address(1,&showaddress[2]);				
				Display_Address(2,&showaddress[1]);	
		}				
		
		if(Key_Flag.flag.left)//右键有效时，ID加1
		{
				Key_Flag.flag.left = 0;
				if(main_page_index < 3)
				{
						main_page_index++;
						main_RefreshDisplay = 1;
				}
				else if(main_page_index == 3)
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
				else if(main_page_index == 0)
				{
						main_page_index = 3;
						main_RefreshDisplay = 1;
				}
		}	
}

static void Display_MainPage_4Add(void)
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
				Display_Set(1);
				Display_Triangle(1);
				Display_arrow(0);
				Display_arrow(1);			
				Display_Address(0,&showaddress[0]);
				Display_Address(2,&showaddress[3]);
		}
		if((main_page_index == 1)&&(main_RefreshDisplay == 1))
		{
				main_RefreshDisplay = 0;
				Fill_RAM(0x00);
				Display_Set(2);
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
				Display_Set(0);
				Display_Triangle(1);
				Display_arrow(0);
				Display_arrow(1);
				Display_Address(1,&showaddress[3]);
				Display_Address(2,&showaddress[2]);
		}
		
		if(Key_Flag.flag.left)//左键有效时，ID加1
		{
				Key_Flag.flag.left = 0;
				if(main_page_index < 4)
				{
						main_page_index++;
						main_RefreshDisplay = 1;
				}
				else if(main_page_index == 4)
				{
						main_page_index = 0;
						main_RefreshDisplay = 1;
				}
		}
		if(Key_Flag.flag.right)//右键有效时，ID减1
		{
				Key_Flag.flag.right = 0;
				if(main_page_index > 0)
				{
						main_page_index--;
						main_RefreshDisplay = 1;
				}
				else if(main_page_index == 0)
				{
						main_page_index = 4;
						main_RefreshDisplay = 1;
				}
		}			
}

static void Display_MainPage_5Add(void)
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
				Display_Set(1);
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
				Display_Set(2);
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
				Display_Set(0);
				Display_Triangle(1);
				Display_arrow(0);
				Display_arrow(1);				
				Display_Address(1,&showaddress[4]);			
				Display_Address(2,&showaddress[3]);
		}
		
		if(Key_Flag.flag.left)//右键有效时，ID加1
		{
				Key_Flag.flag.left = 0;
				if(main_page_index < 5)
				{
						main_page_index++;
						main_RefreshDisplay = 1;
				}
				else if(main_page_index == 5)
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
				else if(main_page_index == 0)
				{
						main_page_index = 5;
						main_RefreshDisplay = 1;
				}
		}	
}

void Display_MainPage(void)
{
		Sort_wallet_Address();
		Key_Control(1);
		main_RefreshDisplay = 1;
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
		}
}

void Display_MainPage_proc(void)
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

//state : 0新钱包设置密码   1旧钱包更改密码
void Display_SetCode(uint8_t state)
{
		PASSPORT_SET_INFO passport;
		passport.state = 0;
		while(1)
		{
				if(state == 0)
				{
						setCode(&passport,0);
						if(verifyCodeSetPin(&passport,Neo_System.pin))//验证密码出错
						{
								Key_Control(1);
								Fill_RAM(0x00);
								#ifdef Chinese
										Show_HZ12_12(56,16,4,5);//密码
										Show_HZ12_12(88,16,2,3);//设置
										Show_HZ12_12(120,16,35,36);//有误
										Show_HZ12_12(152,16,12,12);//请
										Show_HZ12_12(168,16,34,34);//重
										Show_HZ12_12(184,16,37,37);//试
								#endif
								#ifdef English
										Show_AscII_Picture(48,7,YourBitmapDot,sizeof(YourBitmapDot));//32
										Show_AscII_Picture(84,7,PINsBitmapDot,sizeof(PINsBitmapDot));//24
										Show_AscII_Picture(112,7,didBitmapDot,sizeof(didBitmapDot));//24
										Show_AscII_Picture(140,7,notBitmapDot,sizeof(notBitmapDot));//24
										Show_AscII_Picture(168,7,matchBitmapDot,sizeof(matchBitmapDot));//40
							
										Show_AscII_Picture(68,28,PleaseBitmapDot,sizeof(PleaseBitmapDot));//48
										Show_AscII_Picture(120,28,tryBitmapDot,sizeof(tryBitmapDot));//24
										Show_AscII_Picture(148,28,againBitmapDot,sizeof(againBitmapDot));//40
								#endif
								Display_Triangle(0);
								while(Key_Flag.flag.middle==0);
								Key_Control(0);
						}
						else
						{
								passport.state |= 0xF0;
								setCode(&passport,state);
								break;
						}
				}
				else if(state == 1)
				{
						setCode(&passport,1);
						if(verifyCodeGetPin(1,Neo_System.pin))//验证密码出错
						{
								Key_Control(1);
								Fill_RAM(0x00);
								#ifdef Chinese
										Show_HZ12_12(56,16,4,5);//密码
										Show_HZ12_12(88,16,2,3);//设置
										Show_HZ12_12(120,16,35,36);//有误
										Show_HZ12_12(152,16,12,12);//请
										Show_HZ12_12(168,16,34,34);//重
										Show_HZ12_12(184,16,37,37);//试
								#endif
								#ifdef English
										Show_AscII_Picture(48,7,YourBitmapDot,sizeof(YourBitmapDot));//32
										Show_AscII_Picture(84,7,PINsBitmapDot,sizeof(PINsBitmapDot));//24
										Show_AscII_Picture(112,7,didBitmapDot,sizeof(didBitmapDot));//24
										Show_AscII_Picture(140,7,notBitmapDot,sizeof(notBitmapDot));//24
										Show_AscII_Picture(168,7,matchBitmapDot,sizeof(matchBitmapDot));//40
							
										Show_AscII_Picture(68,28,PleaseBitmapDot,sizeof(PleaseBitmapDot));//48
										Show_AscII_Picture(120,28,tryBitmapDot,sizeof(tryBitmapDot));//24
										Show_AscII_Picture(148,28,againBitmapDot,sizeof(againBitmapDot));//40
								#endif
								Display_Triangle(0);
								while(Key_Flag.flag.middle==0);
								Key_Control(0);
						}
						else
						{
								passport.state |= 0xF0;
								setCode(&passport,state);
								break;
						}				
				}
		}
		
		if(state == 0)
		{
		}
		else if(state == 1)
		{
				Fill_RAM(0x00);
				Display_Triangle(0);
				#ifdef Chinese
						Show_HZ12_12(80,16,4,5);//密码
						Show_HZ12_12(112,16,34,34);//重
						Show_HZ12_12(128,16,2,2);//设
						Show_HZ12_12(144,16,27,27);//成
						Show_HZ12_12(160,16,115,115);//功
				#endif
				#ifdef English
						Show_AscII_Picture(38,16,YourBitmapDot,sizeof(YourBitmapDot));
						Show_AscII_Picture(74,16,newBitmapDot,sizeof(newBitmapDot));
						Show_AscII_Picture(102,16,PINBitmapDot,sizeof(PINBitmapDot));
						Show_AscII_Picture(130,16,hasBitmapDot,sizeof(hasBitmapDot));
						Show_AscII_Picture(158,16,beenBitmapDot,sizeof(beenBitmapDot));
						Show_AscII_Picture(194,16,set_BitmapDot,sizeof(set_BitmapDot));
				#endif	
			
				while(Key_Flag.flag.middle==0)
				{
						if(Set_Flag.flag.usb_offline == 0)
								return;				
				}
		}
}

uint8_t Display_VerifyCode(void)
{
		uint8_t status = 0; 
		uint8_t count[2] = {'0','\0'};
		while(1)
		{
				status = verifyCodeGetPin(0,Neo_System.pin);
				if(status == 0)
				{
						if(Neo_System.errornum)
						{
								Neo_System.errornum = 0;
								Update_Flag_ATSHA(&Set_Flag,&Neo_System);					
						}
						return 0;
				}
				Neo_System.errornum++;
				Update_Flag_ATSHA(&Set_Flag,&Neo_System);
				count[0] = Neo_System.errornum + 0x30;
				if(status == 5) //超时控制
				{
						return 1;//超时返回1
				}
				if(count[0] < '5')
				{
						Key_Control(1);
						Fill_RAM(0x00);
						#ifdef Chinese
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
						#endif
						#ifdef English					
								Show_AscII_Picture(82,7,IncorrectBitmapDot,sizeof(IncorrectBitmapDot));//64
								Show_AscII_Picture(150,7,PINBitmapDot,sizeof(PINBitmapDot));//24
								
								Show_AscII_Picture(36,28,YouBitmapDot,sizeof(YouBitmapDot));//24
								Show_AscII_Picture(64,28,haveBitmapDot,sizeof(haveBitmapDot));//32
								Asc8_16(100,28,count);
								Asc8_16(108,28,"/5");								
								Show_AscII_Picture(128,28,attemptsBitmapDot,sizeof(attemptsBitmapDot));//64
								Show_AscII_Picture(196,28,leftBitmapDot,sizeof(leftBitmapDot));//24
						#endif
						Display_Triangle(0);
						while(Key_Flag.flag.middle==0);
						Key_Control(0);
				}
				else
				{
						Key_Control(1);
						Fill_RAM(0x00);
						#ifdef Chinese
								Show_HZ12_12(84,8,4,5);//密码
								Show_HZ12_12(116,8,38,38);//错
								Show_HZ12_12(132,8,36,36);//误
								Asc8_16(148,8,"5");
								Show_HZ12_12(156,8,39,39);//次					
								
								Show_HZ12_12(80,28,17,18);//钱包
								Show_HZ12_12(112,28,40,41);//已被
								Show_HZ12_12(144,28,34,34);//重
								Show_HZ12_12(160,28,3,3);//置
						#endif
						#ifdef English					
								Show_AscII_Picture(26,7,TooBitmapDot,sizeof(TooBitmapDot));//24
								Show_AscII_Picture(54,7,manyBitmapDot,sizeof(manyBitmapDot));//40
								Show_AscII_Picture(98,7,incorrect_BitmapDot,sizeof(incorrect_BitmapDot));//64
								Show_AscII_Picture(166,7,attemptsBitmapDot,sizeof(attemptsBitmapDot));//64
								
								Show_AscII_Picture(50,28,NEODUNBitmapDot,sizeof(NEODUNBitmapDot));//48
								Show_AscII_Picture(102,28,hasBitmapDot,sizeof(hasBitmapDot));//24
								Show_AscII_Picture(130,28,beenBitmapDot,sizeof(beenBitmapDot));//32
								Show_AscII_Picture(166,28,resetBitmapDot,sizeof(resetBitmapDot));//40
						#endif
						Display_Triangle(0);
						Neo_System.errornum = 0;
						EmptyWallet();						 										  //清空钱包数据
						while(Key_Flag.flag.middle==0);
						Key_Flag.flag.middle=0;
						
						Fill_RAM(0x00);
						#ifdef Chinese
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
						#endif
						#ifdef English	
								Show_AscII_Picture(26,7,SetBitmapDot,sizeof(SetBitmapDot));//24
								Show_AscII_Picture(54,7,newBitmapDot,sizeof(newBitmapDot));//24
								Show_AscII_Picture(82,7,PINBitmapDot,sizeof(PINBitmapDot));//24
								Show_AscII_Picture(110,7,andBitmapDot,sizeof(andBitmapDot));//24
								Show_AscII_Picture(138,7,reimportBitmapDot,sizeof(reimportBitmapDot));//56
								Show_AscII_Picture(198,7,keysBitmapDot,sizeof(keysBitmapDot));//32
								
								Show_AscII_Picture(22,28,VisitBitmapDot,sizeof(VisitBitmapDot));//32
								Show_AscII_Picture(58,28,wwwBitmapDot,sizeof(wwwBitmapDot));//112
								Show_AscII_Picture(174,28,forBitmapDot,sizeof(forBitmapDot));//24
								Show_AscII_Picture(202,28,helpBitmapDot,sizeof(helpBitmapDot));//32			
						#endif
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
		while(verifyCodeGetPin(0xff,Neo_System.pin))//验证密码出错	
		{
				Neo_System.errornum++;
				Update_Flag_ATSHA(&Set_Flag,&Neo_System);
				count[0] = Neo_System.errornum + 0x30;
				if(count[0] < '5')
				{
						Key_Control(1);
						Fill_RAM(0x00);
						#ifdef Chinese
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
						#endif
						#ifdef English
								Show_AscII_Picture(82,7,IncorrectBitmapDot,sizeof(IncorrectBitmapDot));
								Show_AscII_Picture(150,7,PINBitmapDot,sizeof(PINBitmapDot));
								
								Show_AscII_Picture(36,28,YouBitmapDot,sizeof(YouBitmapDot));
								Show_AscII_Picture(64,28,haveBitmapDot,sizeof(haveBitmapDot));
								Asc8_16(100,28,count);
								Asc8_16(108,28,"/5");								
								Show_AscII_Picture(128,28,attemptsBitmapDot,sizeof(attemptsBitmapDot));
								Show_AscII_Picture(196,28,leftBitmapDot,sizeof(leftBitmapDot));
						#endif
						Display_Triangle(0);
						while(Key_Flag.flag.middle==0);
						Key_Control(0);
				}
				else
				{
						Key_Control(1);
						Fill_RAM(0x00);
						#ifdef Chinese			
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
						#endif
						#ifdef English					
								Show_AscII_Picture(26,7,TooBitmapDot,sizeof(TooBitmapDot));
								Show_AscII_Picture(54,7,manyBitmapDot,sizeof(manyBitmapDot));
								Show_AscII_Picture(98,7,incorrect_BitmapDot,sizeof(incorrect_BitmapDot));
								Show_AscII_Picture(166,7,attemptsBitmapDot,sizeof(attemptsBitmapDot));
								
								Show_AscII_Picture(50,28,NEODUNBitmapDot,sizeof(NEODUNBitmapDot));
								Show_AscII_Picture(102,28,hasBitmapDot,sizeof(hasBitmapDot));
								Show_AscII_Picture(130,28,beenBitmapDot,sizeof(beenBitmapDot));
								Show_AscII_Picture(166,28,resetBitmapDot,sizeof(resetBitmapDot));				
						#endif
						Display_Triangle(0);
						Neo_System.new_wallet = 1;										  //更改为新钱包
						Neo_System.errornum = 0;
						EmptyWallet();						 										  //清空钱包数据
						Update_PowerOn_SetFlag(&Set_Flag,&Neo_System);	//更新系统设置标识
						Update_AddressInfo_To_Ram();										
						while(Key_Flag.flag.middle==0);
						Key_Flag.flag.middle=0;
						
						Fill_RAM(0x00);
						#ifdef Chinese
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
						#endif
						#ifdef English
								Show_AscII_Picture(26,7,SetBitmapDot,sizeof(SetBitmapDot));
								Show_AscII_Picture(54,7,newBitmapDot,sizeof(newBitmapDot));
								Show_AscII_Picture(82,7,PINBitmapDot,sizeof(PINBitmapDot));
								Show_AscII_Picture(110,7,andBitmapDot,sizeof(andBitmapDot));
								Show_AscII_Picture(138,7,reimportBitmapDot,sizeof(reimportBitmapDot));
								Show_AscII_Picture(198,7,keysBitmapDot,sizeof(keysBitmapDot));
								
								Show_AscII_Picture(22,28,VisitBitmapDot,sizeof(VisitBitmapDot));
								Show_AscII_Picture(58,28,wwwBitmapDot,sizeof(wwwBitmapDot));
								Show_AscII_Picture(174,28,forBitmapDot,sizeof(forBitmapDot));
								Show_AscII_Picture(202,28,helpBitmapDot,sizeof(helpBitmapDot));				
						#endif					
						Display_Triangle(0);
						while(Key_Flag.flag.middle==0);
						Key_Control(0);
						return 1;//错误5次，返回1
				}
		}
		if(Neo_System.errornum)
		{
				Neo_System.errornum = 0;
				Update_Flag_ATSHA(&Set_Flag,&Neo_System);					
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
		uint8_t page_index = INIT_PAGE_INDEX;		//页面ID
		uint8_t	slot_data[32];
		Key_Control(1);
		while(1)
		{
				if(Set_Flag.flag.usb_offline == 0)
						return;
				if((Key_Flag.flag.middle)&&(page_index == 1))
				{
						Key_Flag.flag.middle = 0;					
						Fill_RAM(0x00);
						Asc8_16(0,16,showaddress[AddID-1].address);
						Display_Triangle(0);
						while(Key_Flag.flag.middle == 0)
						{
								if(Set_Flag.flag.usb_offline == 0)
										return;
						}
						Key_Control(1);
						RefreshDisplay = 1;
				}
				else if((Key_Flag.flag.middle)&&(page_index == 2))
				{
						Key_Flag.flag.middle = 0;
						Fill_RAM(0x00);
						if(showaddress[AddID-1].hide == 0)
						{
								#ifdef Chinese
										Show_HZ12_12(64,7,84,85);//隔离
										Show_HZ12_12(96,7,23,23);//并
										Show_HZ12_12(112,7,80,81);//隐藏
										Show_HZ12_12(144,7,73,73);//本
										Show_HZ12_12(160,7,17,18);//钱包
									
										Show_HZ12_12(24,28,17,18);//钱包
										Show_HZ12_12(56,28,9,10);//地址
										Show_HZ12_12(88,28,146,147);//只能
										Show_HZ12_12(120,28,140,140);//在
										Asc8_16(136,28,"NEODUN");
										Show_HZ12_12(184,28,148,148);//内
										Show_HZ12_12(200,28,78,79);//查看
								#endif
								#ifdef English	
										Show_AscII_Picture(46,7,IsolateBitmapDot,sizeof(IsolateBitmapDot));//48
										Show_AscII_Picture(98,7,andBitmapDot,sizeof(andBitmapDot));//24
										Show_AscII_Picture(126,7,hideBitmapDot,sizeof(hideBitmapDot));//32
										Show_AscII_Picture(162,7,walletBitmapDot,sizeof(walletBitmapDot));//40
										Asc8_16(202,28,"?");
										
										Show_AscII_Picture(24,28,ItBitmapDot,sizeof(ItBitmapDot));//16
										Show_AscII_Picture(44,28,willBitmapDot,sizeof(willBitmapDot));//24
										Show_AscII_Picture(72,28,onlyBitmapDot,sizeof(onlyBitmapDot));//32
										Show_AscII_Picture(108,28,beBitmapDot,sizeof(beBitmapDot));//16
										Show_AscII_Picture(128,28,seenBitmapDot,sizeof(seenBitmapDot));//32
										Show_AscII_Picture(164,28,inBitmapDot,sizeof(inBitmapDot));//16
										Show_AscII_Picture(184,28,NEODUNBitmapDot,sizeof(NEODUNBitmapDot));//48
								#endif
						}
						else
						{
								#ifdef Chinese
										Show_HZ12_12(68,16,65,65);//不
										Show_HZ12_12(84,16,130,130);//再
										Show_HZ12_12(100,16,80,81);//隐藏
										Show_HZ12_12(132,16,73,73);//本
										Show_HZ12_12(148,16,17,18);//钱包
										Asc8_16(180,16,"?");
								#endif
								#ifdef English
										Show_AscII_Picture(26,16,DoBitmapDot,sizeof(DoBitmapDot));//16
										Show_AscII_Picture(46,16,youBitmapDot,sizeof(youBitmapDot));//24
										Show_AscII_Picture(74,16,wantBitmapDot,sizeof(wantBitmapDot));//32
										Show_AscII_Picture(110,16,toBitmapDot,sizeof(toBitmapDot));//16
										Show_AscII_Picture(130,16,unhideBitmapDot,sizeof(unhideBitmapDot));//48
										Show_AscII_Picture(182,16,walletBitmapDot,sizeof(walletBitmapDot));//40
										Asc8_16(222,16,"?");
								#endif
						}
						#ifdef Chinese
								Show_HZ12_12(120,51,53,53);//是
								Show_HZ12_12(182,51,54,54);//否
						#endif
						#ifdef English
								Show_AscII_Picture(104,51,ConfirmBitmapDot,sizeof(ConfirmBitmapDot));
								Show_AscII_Picture(172,51,CancelBitmapDot,sizeof(CancelBitmapDot));
						#endif
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
								if(Key_Flag.flag.right)
								{
										Key_Control(1);
										RefreshDisplay = 1;
										break;
								}
								if(Set_Flag.flag.usb_offline == 0)
										return;
						}
				}
				else if((Key_Flag.flag.middle)&&(page_index == 0))
				{
						Key_Flag.flag.middle = 0;
						return;
				}
				
				if((page_index == 1)&&(RefreshDisplay == 1))
				{
						RefreshDisplay = 0;
						Fill_RAM(0x00);
						#ifdef Chinese			
								Show_HZ12_12(44,16,82,83);//返回
								Show_HZ12_12(96,16,78,79);//查看
								Show_HZ12_12(128,16,8,9);//地址
						#endif
						#ifdef English
								Show_AscII_Picture(44,16,BackBitmapDot,sizeof(BackBitmapDot));
								Show_AscII_Picture(100,16,AddressBitmapDot,sizeof(AddressBitmapDot));
						#endif
						if(showaddress[AddID-1].hide)
						{
								#ifdef Chinese									
										Show_HZ12_12(164,16,111,112);//取消
										Show_HZ12_12(196,16,80,81);//隐藏
								#endif
								#ifdef English
										Show_AscII_Picture(172,16,UnhideBitmapDot,sizeof(UnhideBitmapDot));
								#endif	
						}
						else
						{
								#ifdef Chinese
										Show_HZ12_12(164,16,80,81);//隐藏
										Show_HZ12_12(196,16,17,18);//钱包
								#endif
								#ifdef English
										Show_AscII_Picture(180,16,HideBitmapDot,sizeof(HideBitmapDot));
								#endif
						}
						Display_Triangle(1);
						Display_arrow(1);
						Display_arrow(0);
				}
				else if((page_index == 2)&&(RefreshDisplay == 1))
				{
						RefreshDisplay = 0;
						Fill_RAM(0x00);					
						#ifdef Chinese				
								Show_HZ12_12(28,16,78,79);//查看
								Show_HZ12_12(60,16,8,9);//地址
						#endif
						#ifdef English
								Show_AscII_Picture(32,16,AddressBitmapDot,sizeof(AddressBitmapDot));
						#endif
						if(showaddress[AddID-1].hide)
						{
								#ifdef Chinese									
										Show_HZ12_12(96,16,111,112);//取消
										Show_HZ12_12(128,16,80,81);//隐藏
								#endif
								#ifdef English
										Show_AscII_Picture(104,16,UnhideBitmapDot,sizeof(UnhideBitmapDot));
								#endif
						}
						else
						{		
								#ifdef Chinese
										Show_HZ12_12(96,16,80,81);//隐藏
										Show_HZ12_12(128,16,17,18);//钱包
								#endif
								#ifdef English
										Show_AscII_Picture(112,16,HideBitmapDot,sizeof(HideBitmapDot));
								#endif
						}
						Display_Triangle(1);
						Display_arrow(0);
				}
				else if((page_index == 0)&&(RefreshDisplay == 1))
				{
						RefreshDisplay = 0;
						Fill_RAM(0x00);
						#ifdef Chinese
								Show_HZ12_12(112,16,82,83);//返回
								Show_HZ12_12(164,16,78,79);//查看
								Show_HZ12_12(196,16,8,9);//地址
						#endif
						#ifdef English
								Show_AscII_Picture(112,16,BackBitmapDot,sizeof(BackBitmapDot));
								Show_AscII_Picture(168,16,AddressBitmapDot,sizeof(AddressBitmapDot));
						#endif
						Display_Triangle(1);
						Display_arrow(1);
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
				
				if((page_index == INIT_PAGE_INDEX)&&(RefreshDisplay == 1))
				{
						RefreshDisplay = 0;
						Fill_RAM(0x00);
						#ifdef Chinese
								Show_HZ12_12(44,16,82,83);//返回
								Show_HZ12_12(96,16,78,79);//查看
								Show_HZ12_12(128,16,8,9);//地址
						#endif
						#ifdef English
								Show_AscII_Picture(44,16,BackBitmapDot,sizeof(BackBitmapDot));
								Show_AscII_Picture(100,16,AddressBitmapDot,sizeof(AddressBitmapDot));
						#endif
						if(showaddress[AddID].hide)
						{
								#ifdef Chinese
										Show_HZ12_12(164,16,111,112);//取消
										Show_HZ12_12(196,16,80,81);//隐藏
								#endif
								#ifdef English
										Show_AscII_Picture(172,16,UnhideBitmapDot,sizeof(UnhideBitmapDot));
								#endif
						}
						else
						{
								#ifdef Chinese						
										Show_HZ12_12(164,16,80,81);//隐藏
										Show_HZ12_12(196,16,17,18);//钱包
								#endif
								#ifdef English
										Show_AscII_Picture(180,16,HideBitmapDot,sizeof(HideBitmapDot));
								#endif
						}
						Display_Triangle(1);
						Display_arrow(1);
						Display_arrow(0);
				}
		}
}

void Display_Set_Coin_NEO(void)
{
		uint8_t RefreshDisplay = 1;//是否重新刷新界面标志
		uint8_t page_index = INIT_PAGE_INDEX;		//页面ID
		while(1)
		{
				if(Set_Flag.flag.usb_offline == 0)
						return;
				if((Key_Flag.flag.middle)&&(page_index == 1))
				{
						Key_Flag.flag.middle = 0;
						Fill_RAM(0x00);
						#ifdef Chinese
								Show_HZ12_12(88,16,97,97);//版
								Show_HZ12_12(104,16,73,73);//本				
								Show_HZ12_12(120,16,98,98);//号						
								Asc8_16(136,16,VERSION_NEO_STR);
						#endif
						#ifdef English
								Asc8_16(112,16,VERSION_NEO_STR);
						#endif
						Display_Triangle(0);
						while(Key_Flag.flag.middle == 0)
						{
								if(Set_Flag.flag.usb_offline == 0)
										return;
						}
						Key_Flag.flag.middle = 0;
						RefreshDisplay = 1;						
				}
				if((Key_Flag.flag.middle)&&(page_index == 0))
				{
						Key_Flag.flag.middle = 0;
						return;
				}
				
				if((page_index == 1)&&(RefreshDisplay == 1))
				{
						RefreshDisplay = 0;
						Fill_RAM(0x00);
						#ifdef Chinese
								Show_HZ12_12(104,16,97,97);//版
								Show_HZ12_12(120,16,73,73);//本
								Show_HZ12_12(136,16,98,98);//号								
								Show_HZ12_12(44,16,82,83);//返回
						#endif
						#ifdef English
								Show_AscII_Picture(104,16,VersionBitmapDot,sizeof(VersionBitmapDot));
								Show_AscII_Picture(44,16,BackBitmapDot,sizeof(BackBitmapDot));
						#endif
						Display_Triangle(1);
						Display_arrow(0);					
				}
				if((page_index == 0)&&(RefreshDisplay == 1))
				{
						RefreshDisplay = 0;
						Fill_RAM(0x00);
						#ifdef Chinese				
								Show_HZ12_12(172,16,97,97);//版
								Show_HZ12_12(188,16,73,73);//本
								Show_HZ12_12(204,16,98,98);//号								
								Show_HZ12_12(112,16,82,83);//返回
						#endif
						#ifdef English
								Show_AscII_Picture(172,16,VersionBitmapDot,sizeof(VersionBitmapDot));
								Show_AscII_Picture(112,16,BackBitmapDot,sizeof(BackBitmapDot));					
						#endif
						Display_Triangle(1);
						Display_arrow(1);
				}
				
				if(Key_Flag.flag.right)//右键有效时，ID加1
				{
						Key_Flag.flag.right = 0;
						if(page_index < 1)
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
				
				if((page_index == INIT_PAGE_INDEX)&&(RefreshDisplay == 1))
				{
						RefreshDisplay = 0;
						Fill_RAM(0x00);
						#ifdef Chinese
								Show_HZ12_12(104,16,97,97);//版
								Show_HZ12_12(120,16,73,73);//本
								Show_HZ12_12(136,16,98,98);//号								
								Show_HZ12_12(44,16,82,83);//返回
						#endif
						#ifdef English
								Show_AscII_Picture(104,16,VersionBitmapDot,sizeof(VersionBitmapDot));
								Show_AscII_Picture(44,16,BackBitmapDot,sizeof(BackBitmapDot));
						#endif
						
						Display_Triangle(1);
						Display_arrow(0);
				}					
		}
}
//TBD
void Display_Set_Coin(void)
{
		uint8_t RefreshDisplay = 1;//是否重新刷新界面标志
		uint8_t page_index = INIT_PAGE_INDEX;		 //页面ID
		Key_Control(1);
		while(1)
		{
				if(Set_Flag.flag.usb_offline == 0)
						return;
				if((Key_Flag.flag.middle)&&(page_index == 1))
				{
						Key_Flag.flag.middle = 0;
						Display_Set_Coin_NEO();
						RefreshDisplay = 1;
				}
				if((Key_Flag.flag.middle)&&(page_index == 0))
				{
						Key_Flag.flag.middle = 0;
						return;
				}			
				
				if((page_index == 1)&&(RefreshDisplay == 1))
				{
						RefreshDisplay = 0;
						Fill_RAM(0x00);
						Asc8_16(116,16,"NEO");
						#ifdef Chinese
								Show_HZ12_12(44,16,82,83);//返回
						#endif
						#ifdef English
								Show_AscII_Picture(44,16,BackBitmapDot,sizeof(BackBitmapDot));
						#endif
						Display_Triangle(1);
						Display_arrow(0);					
				}
				if((page_index == 0)&&(RefreshDisplay == 1))
				{
						RefreshDisplay = 0;
						Fill_RAM(0x00);
						Asc8_16(184,16,"NEO");
						#ifdef Chinese
								Show_HZ12_12(112,16,82,83);//返回
						#endif
						#ifdef English
								Show_AscII_Picture(112,16,BackBitmapDot,sizeof(BackBitmapDot));
						#endif
						Display_Triangle(1);	
						Display_arrow(1);
				}	
				
				if(Key_Flag.flag.right)//右键有效时，ID加1
				{
						Key_Flag.flag.right = 0;
						if(page_index < 1)
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
				
				if((page_index == INIT_PAGE_INDEX)&&(RefreshDisplay == 1))
				{
						RefreshDisplay = 0;
						Fill_RAM(0x00);
						Asc8_16(116,16,"NEO");
						#ifdef Chinese
								Show_HZ12_12(44,16,82,83);//返回
						#endif
						#ifdef English
								Show_AscII_Picture(44,16,BackBitmapDot,sizeof(BackBitmapDot));
						#endif
						Display_Triangle(1);
						Display_arrow(0);
				}					
		}
}

void Display_Set_About(void)
{
		uint8_t RefreshDisplay = 1;//是否重新刷新界面标志
		uint8_t page_index = INIT_PAGE_INDEX;		//页面ID
		while(1)
		{
				if(Set_Flag.flag.usb_offline == 0)
						return;
				if((Key_Flag.flag.middle)&&(page_index == 1))
				{
						Key_Flag.flag.middle = 0;
						Fill_RAM(0x00);
						#ifdef Chinese
								Show_HZ12_12(88,16,97,97);//版
								Show_HZ12_12(104,16,73,73);//本				
								Show_HZ12_12(120,16,98,98);//号
								Asc8_16(136,16,VERSION_NEODUN_STR);
						#endif
						#ifdef English
								Asc8_16(112,16,VERSION_NEODUN_STR);
						#endif
						Display_Triangle(0);
						while(Key_Flag.flag.middle == 0)
						{
								if(Set_Flag.flag.usb_offline == 0)
										return;
						}
						Key_Flag.flag.middle = 0;
						RefreshDisplay = 1;
				}
				else if((Key_Flag.flag.middle)&&(page_index == 2))
				{
						Key_Flag.flag.middle = 0;
						Fill_RAM(0x00);					
						#ifdef Chinese
								Show_HZ12_12(56,8,47,50);//需要帮助
								Show_HZ12_12(120,8,12,12);//请
								Show_HZ12_12(136,8,51,52);//登陆
								Show_HZ12_12(168,8,104,105);//官网
								Asc8_16(72,28,"www.neodun.com");
						#endif
						#ifdef English
								Show_AscII_Picture(32,8,VisitBitmapDot,sizeof(VisitBitmapDot));
								Show_AscII_Picture(64,8,officialBitmapDot,sizeof(officialBitmapDot));
								Show_AscII_Picture(112,8,websiteBitmapDot,sizeof(websiteBitmapDot));
								Show_AscII_Picture(168,8,forBitmapDot,sizeof(forBitmapDot));
								Show_AscII_Picture(192,8,helpBitmapDot,sizeof(helpBitmapDot));
					
								Show_AscII_Picture(72,28,wwwBitmapDot,sizeof(wwwBitmapDot));
						#endif
						Display_Triangle(0);
						while(Key_Flag.flag.middle == 0)
						{
								if(Set_Flag.flag.usb_offline == 0)
										return;
						}
						Key_Flag.flag.middle = 0;
						RefreshDisplay = 1;
				}
				else if((Key_Flag.flag.middle)&&(page_index == 0))
				{
						Key_Flag.flag.middle = 0;
						return;
				}
				
				if((page_index == 1)&&(RefreshDisplay == 1))
				{
						RefreshDisplay = 0;
						Fill_RAM(0x00);
						#ifdef Chinese
								Show_HZ12_12(44,16,82,83);//返回
								Show_HZ12_12(96,16,77,77);//固
								Show_HZ12_12(112,16,46,46);//件
								Show_HZ12_12(128,16,97,97);//版
								Show_HZ12_12(144,16,73,73);//本
								Show_HZ12_12(180,16,49,50);//帮助
						#endif
						#ifdef English
								Show_AscII_Picture(44,16,BackBitmapDot,sizeof(BackBitmapDot));
								Show_AscII_Picture(100,16,FirmwareBitmapDot,sizeof(FirmwareBitmapDot));
								Show_AscII_Picture(180,16,HelpBitmapDot,sizeof(HelpBitmapDot));
						#endif
						Display_Triangle(1);
						Display_arrow(1);
						Display_arrow(0);
				}
				else if((page_index == 2)&&(RefreshDisplay == 1))
				{
						RefreshDisplay = 0;
						Fill_RAM(0x00);
						#ifdef Chinese
								Show_HZ12_12(28,16,77,77);//固
								Show_HZ12_12(44,16,46,46);//件
								Show_HZ12_12(60,16,97,97);//版
								Show_HZ12_12(76,16,73,73);//本
								Show_HZ12_12(112,16,49,50);//帮助
						#endif
						#ifdef English
								Show_AscII_Picture(32,16,FirmwareBitmapDot,sizeof(FirmwareBitmapDot));
								Show_AscII_Picture(112,16,HelpBitmapDot,sizeof(HelpBitmapDot));								
						#endif
						Display_Triangle(1);
						Display_arrow(0);
				}	
				else if((page_index == 0)&&(RefreshDisplay == 1))
				{
						RefreshDisplay = 0;
						Fill_RAM(0x00);
						#ifdef Chinese
								Show_HZ12_12(112,16,82,83);//返回
								Show_HZ12_12(164,16,77,77);//固
								Show_HZ12_12(180,16,46,46);//件
								Show_HZ12_12(196,16,97,97);//版
								Show_HZ12_12(212,16,73,73);//本			
						#endif
						#ifdef English				
								Show_AscII_Picture(112,16,BackBitmapDot,sizeof(BackBitmapDot));		
								Show_AscII_Picture(168,16,FirmwareBitmapDot,sizeof(FirmwareBitmapDot));										
						#endif
						Display_Triangle(1);	
						Display_arrow(1);
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
				
				if((page_index == INIT_PAGE_INDEX)&&(RefreshDisplay == 1))
				{
						RefreshDisplay = 0;
						Fill_RAM(0x00);
						#ifdef Chinese
								Show_HZ12_12(44,16,82,83);//返回
								Show_HZ12_12(96,16,77,77);//固
								Show_HZ12_12(112,16,46,46);//件
								Show_HZ12_12(128,16,97,97);//版
								Show_HZ12_12(144,16,73,73);//本
								Show_HZ12_12(180,16,49,50);//帮助
						#endif
						#ifdef English
								Show_AscII_Picture(44,16,BackBitmapDot,sizeof(BackBitmapDot));
								Show_AscII_Picture(100,16,FirmwareBitmapDot,sizeof(FirmwareBitmapDot));
								Show_AscII_Picture(180,16,HelpBitmapDot,sizeof(HelpBitmapDot));
						#endif				
						Display_Triangle(1);
						Display_arrow(1);
						Display_arrow(0);
				}					
		}
}

void Display_Set_Security_ResetWallet(void)
{
SecResWal:	
		Fill_RAM(0x00);
		#ifdef Chinese
				Show_HZ12_12(40,7,34,34);//重
				Show_HZ12_12(56,7,3,3);//置
				Show_HZ12_12(72,7,2,2);//设
				Show_HZ12_12(88,7,43,43);//备
				Show_HZ12_12(104,7,32,32);//将
				Show_HZ12_12(120,7,55,56);//删除
				Show_HZ12_12(152,7,96,96);//全
				Show_HZ12_12(168,7,89,89);//部
				Show_HZ12_12(184,7,108,109);//数据
			
				Show_HZ12_12(72,28,55,56);//删除
				Show_HZ12_12(104,28,42,42);//后
				Show_HZ12_12(120,28,91,92);//无法
				Show_HZ12_12(152,28,110,110);//找
				Show_HZ12_12(168,28,83,83);//回

				Display_Fill_Triangle(1,1);
				Show_HZ12_12(184,51,111,112);//取消
		#endif
		#ifdef English
				Show_AscII_Picture(12,7,ResetBitmapDot,sizeof(ResetBitmapDot));//40
				Show_AscII_Picture(56,7,willBitmapDot,sizeof(willBitmapDot));//24
				Show_AscII_Picture(84,7,eraseBitmapDot,sizeof(eraseBitmapDot));//40
				Show_AscII_Picture(128,7,allBitmapDot,sizeof(allBitmapDot));//16
				Show_AscII_Picture(148,7,keysBitmapDot,sizeof(keysBitmapDot));//32
				Show_AscII_Picture(184,7,andBitmapDot,sizeof(andBitmapDot));//24
				Show_AscII_Picture(212,7,appsBitmapDot,sizeof(appsBitmapDot));//32
				
				Show_AscII_Picture(40,28,PINBitmapDot,sizeof(PINBitmapDot));//24
				Show_AscII_Picture(68,28,isBitmapDot,sizeof(isBitmapDot));//16
				Show_AscII_Picture(88,28,requiredBitmapDot,sizeof(requiredBitmapDot));//56
				Show_AscII_Picture(148,28,toBitmapDot,sizeof(toBitmapDot));//16
				Show_AscII_Picture(168,28,confirmBitmapDot,sizeof(confirmBitmapDot));//48

				Show_AscII_Picture(172,51,CancelBitmapDot,sizeof(CancelBitmapDot));
				Display_Fill_Triangle(1,1);
		#endif
		
		while(1)
		{
				if(Key_Flag.flag.middle)
				{
						Key_Flag.flag.middle = 0;
						Fill_RAM(0x00);
						#ifdef Chinese							
								Show_HZ12_12(76,7,53,54);//是否
								Show_HZ12_12(108,7,34,34);//重
								Show_HZ12_12(124,7,3,3);//置
								Show_HZ12_12(140,7,2,2);//设
								Show_HZ12_12(156,7,43,43);//备
								Asc8_16(172,28,"?");
							
								Asc8_16(72,28,"(");
								Show_HZ12_12(80,28,47,48);//需要
								Show_HZ12_12(112,28,4,5);//密码
								Show_HZ12_12(144,28,6,7);//确认
								Asc8_16(176,28,")");
					
								Display_Fill_Triangle(0,0);
								Show_HZ12_12(120,51,53,53);//是
								Show_HZ12_12(184,51,54,54);//否
						#endif
						#ifdef English
								Show_AscII_Picture(20,7,AreBitmapDot,sizeof(AreBitmapDot));//24
								Show_AscII_Picture(48,7,youBitmapDot,sizeof(youBitmapDot));//24
								Show_AscII_Picture(76,7,sureBitmapDot,sizeof(sureBitmapDot));//32
								Show_AscII_Picture(112,7,youBitmapDot,sizeof(youBitmapDot));//24
								Show_AscII_Picture(140,7,wantBitmapDot,sizeof(wantBitmapDot));//32
								Show_AscII_Picture(176,7,toBitmapDot,sizeof(toBitmapDot));//16
								Show_AscII_Picture(196,7,resetBitmapDot,sizeof(resetBitmapDot));//40
								
								Show_AscII_Picture(50,28,allBitmapDot,sizeof(allBitmapDot));//16
								Asc8_16(66,28,"?");
								Show_AscII_Picture(78,28,ThisBitmapDot,sizeof(ThisBitmapDot));//32
								Show_AscII_Picture(114,28,isBitmapDot,sizeof(isBitmapDot));//16
								Show_AscII_Picture(134,28,irreversibleBitmapDot,sizeof(irreversibleBitmapDot));//72
														
								Show_AscII_Picture(104,51,ConfirmBitmapDot,sizeof(ConfirmBitmapDot));
								Show_AscII_Picture(172,51,CancelBitmapDot,sizeof(CancelBitmapDot));
								Display_Fill_Triangle(0,0);
						#endif
						while(1)
						{
								if(Key_Flag.flag.middle)
								{
										Key_Flag.flag.middle = 0;
										if(Display_VerifyCode() == 0)
										{
												Fill_RAM(0x00);
												#ifdef Chinese
														Show_HZ12_12(96,8,34,34);//重
														Show_HZ12_12(112,8,3,3);//置
														Show_HZ12_12(128,8,27,27);//成
														Show_HZ12_12(144,8,115,115);//功
													
														Show_HZ12_12(72,28,116,118);//牛顿忘
														Show_HZ12_12(120,28,29,29);//记
														Show_HZ12_12(136,28,119,121);//了一切
												#endif
												#ifdef English
														Show_AscII_Picture(66,7,ResetBitmapDot,sizeof(ResetBitmapDot));
														Show_AscII_Picture(110,7,successfulBitmapDot,sizeof(successfulBitmapDot));
											
														Show_AscII_Picture(28,28,AllBitmapDot,sizeof(AllBitmapDot));
														Show_AscII_Picture(56,28,dataBitmapDot,sizeof(dataBitmapDot));
														Show_AscII_Picture(92,28,erasedBitmapDot,sizeof(erasedBitmapDot));
														Show_AscII_Picture(144,28,fromBitmapDot,sizeof(fromBitmapDot));
														Show_AscII_Picture(180,28,NEODUNBitmapDot,sizeof(NEODUNBitmapDot));
												#endif
												Display_Triangle(0);
											
												Key_Control(1);
												while(Key_Flag.flag.middle == 0);
												Key_Flag.flag.middle = 0;
												EmptyWallet(); //清空钱包数据
												HAL_Delay(50);
												System_Reset();//系统重启
										}
										else
												return;
								}
								if(Key_Flag.flag.right)
								{
										Key_Flag.flag.right = 0;
										return;
								}
								if(Key_Flag.flag.left)
								{
										Key_Flag.flag.left = 0;
										goto SecResWal;
								}
						}
				}
				if(Key_Flag.flag.right)
				{
						Key_Flag.flag.right = 0;
						return;
				}
		}
}

void Display_Set_Security(void)
{
		uint8_t RefreshDisplay = 1;//是否重新刷新界面标志
		uint8_t page_index = INIT_PAGE_INDEX;		//页面ID
		while(1)
		{
				if(Set_Flag.flag.usb_offline == 0)
						return;
				if((Key_Flag.flag.middle)&&(page_index == 1))
				{
						Key_Flag.flag.middle = 0;
						if(Display_VerifyCode() == 0)
						{	
								Display_SetCode(1);
						}
						Key_Control(1);
						RefreshDisplay = 1;
				}
				else if((Key_Flag.flag.middle)&&(page_index == 2))
				{
						Key_Flag.flag.middle = 0;
						Display_Set_Security_ResetWallet();						
						RefreshDisplay = 1;
				}			
				else if((Key_Flag.flag.middle)&&(page_index == 0))
				{
						Key_Flag.flag.middle = 0;
						return;
				}
				
				if((page_index == 1)&&(RefreshDisplay == 1))
				{
						RefreshDisplay = 0;
						Fill_RAM(0x00);
						#ifdef Chinese
								Show_HZ12_12(44,16,82,83);//返回
								Show_HZ12_12(96,16,106,107);//更改
								Show_HZ12_12(128,16,4,5);//密码
								Show_HZ12_12(164,16,34,34);//重
								Show_HZ12_12(180,16,3,3);//置
								Show_HZ12_12(196,16,2,2);//设
								Show_HZ12_12(212,16,43,43);//备
						#endif
						#ifdef English
								Show_AscII_Picture(44,16,BackBitmapDot,sizeof(BackBitmapDot));
								Show_AscII_Picture(96,16,change_pinBitmapDot,sizeof(change_pinBitmapDot));
								Show_AscII_Picture(166,16,ResetallBitmapDot,sizeof(ResetallBitmapDot));
						#endif
						Display_Triangle(1);
						Display_arrow(1);		
						Display_arrow(0);
				}
				else if((page_index == 2)&&(RefreshDisplay == 1))
				{
						RefreshDisplay = 0;
						Fill_RAM(0x00);
						#ifdef Chinese
								Show_HZ12_12(28,16,106,107);//更改
								Show_HZ12_12(60,16,4,5);//密码
								Show_HZ12_12(96,16,34,34);//重
								Show_HZ12_12(112,16,3,3);//置
								Show_HZ12_12(128,16,2,2);//设
								Show_HZ12_12(144,16,43,43);//备
						#endif
						#ifdef English
								Show_AscII_Picture(28,16,change_pinBitmapDot,sizeof(change_pinBitmapDot));
								Show_AscII_Picture(98,16,ResetallBitmapDot,sizeof(ResetallBitmapDot));
						#endif
						Display_Triangle(1);	
						Display_arrow(0);
				}	
				else if((page_index == 0)&&(RefreshDisplay == 1))
				{
						RefreshDisplay = 0;
						Fill_RAM(0x00);
						#ifdef Chinese
								Show_HZ12_12(112,16,82,83);//返回
								Show_HZ12_12(164,16,106,107);//更改
								Show_HZ12_12(196,16,4,5);//密码								
						#endif
						#ifdef English
								Show_AscII_Picture(112,16,BackBitmapDot,sizeof(BackBitmapDot));							
								Show_AscII_Picture(164,16,change_pinBitmapDot,sizeof(change_pinBitmapDot));					
						#endif
						Display_Triangle(1);
						Display_arrow(1);
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
				
				if((page_index == INIT_PAGE_INDEX)&&(RefreshDisplay == 1))
				{
						RefreshDisplay = 0;
						Fill_RAM(0x00);
						#ifdef Chinese
								Show_HZ12_12(46,16,82,83);//返回
								Show_HZ12_12(96,16,106,107);//更改
								Show_HZ12_12(128,16,4,5);//密码
								Show_HZ12_12(164,16,34,34);//重
								Show_HZ12_12(180,16,3,3);//置
								Show_HZ12_12(196,16,2,2);//设
								Show_HZ12_12(212,16,43,43);//备
						#endif
						#ifdef English
								Show_AscII_Picture(44,16,BackBitmapDot,sizeof(BackBitmapDot));
								Show_AscII_Picture(96,16,change_pinBitmapDot,sizeof(change_pinBitmapDot));
								Show_AscII_Picture(166,16,ResetallBitmapDot,sizeof(ResetallBitmapDot));
						#endif
						Display_Triangle(1);
						Display_arrow(1);
						Display_arrow(0);
				}					
		}		
}

void Display_Click_Set(void)
{
		uint8_t RefreshDisplay = 1;						//是否重新刷新界面标志
		uint8_t page_index = INIT_PAGE_INDEX;	//页面ID		
		Key_Control(1);
		while(1)
		{
				if(Set_Flag.flag.usb_offline == 0)
						return;
				if((Key_Flag.flag.middle)&&(page_index == 1))
				{
						Key_Flag.flag.middle = 0;
						if(coinrecord.count != 0)
						{
								Display_Set_Coin();
								RefreshDisplay = 1;
						}
						else
						{
								Fill_RAM(0x00);
								#ifdef Chinese
										Show_HZ12_12(120,24,91,91);//无
								#endif
								#ifdef English
										Show_AscII_Picture(112,16,NoneBitmapDot,sizeof(NoneBitmapDot));
								#endif
								Display_Triangle(0);
								while(Key_Flag.flag.middle==0)
								{
										if(Set_Flag.flag.usb_offline == 0)
												return;
								}
								Key_Control(1);
								RefreshDisplay = 1;
						}
				}
				else if((Key_Flag.flag.middle)&&(page_index == 2))
				{
						Key_Flag.flag.middle = 0;
						Display_Set_Security();
						RefreshDisplay = 1;
				}				
				else if((Key_Flag.flag.middle)&&(page_index == 3))
				{
						Key_Flag.flag.middle = 0;
						Display_Set_About();
						RefreshDisplay = 1;
				}
				else if((Key_Flag.flag.middle)&&(page_index == 0))
				{
						Key_Flag.flag.middle = 0;
						return;
				}
				if((page_index == 1)&&(RefreshDisplay == 1))
				{
						RefreshDisplay = 0;
						Fill_RAM(0x00);
						#ifdef Chinese
								Show_HZ12_12(44,16,82,83);//返回
								Show_HZ12_12(112,16,93,94);//币种
								Show_HZ12_12(180,16,95,96);//安全
						#endif
						#ifdef English
								Show_AscII_Picture(44,16,BackBitmapDot,sizeof(BackBitmapDot));
								Show_AscII_Picture(116,16,APPBitmapDot,sizeof(APPBitmapDot));
								Show_AscII_Picture(168,16,SecurityBitmapDot,sizeof(SecurityBitmapDot));
						#endif
						Display_Triangle(1);
						Display_arrow(1);				
						Display_arrow(0);
				}
				else if((page_index == 2)&&(RefreshDisplay == 1))
				{
						RefreshDisplay = 0;
						Fill_RAM(0x00);
						#ifdef Chinese
								Show_HZ12_12(44,16,93,94);//币种
								Show_HZ12_12(112,16,95,96);//安全
								Show_HZ12_12(180,16,102,103);//关于
						#endif
						#ifdef English
								Show_AscII_Picture(48,16,APPBitmapDot,sizeof(APPBitmapDot));
								Show_AscII_Picture(100,16,SecurityBitmapDot,sizeof(SecurityBitmapDot));					
								Show_AscII_Picture(176,16,AboutBitmapDot,sizeof(AboutBitmapDot));
						#endif
						Display_Triangle(1);
						Display_arrow(0);
						Display_arrow(1);					
				}
				else if((page_index == 3)&&(RefreshDisplay == 1))
				{
						RefreshDisplay = 0;
						Fill_RAM(0x00);
						#ifdef Chinese
								Show_HZ12_12(44,16,95,96);//安全
								Show_HZ12_12(112,16,102,103);//关于
						#endif
						#ifdef English
								Show_AscII_Picture(32,16,SecurityBitmapDot,sizeof(SecurityBitmapDot));					
								Show_AscII_Picture(108,16,AboutBitmapDot,sizeof(AboutBitmapDot));
						#endif
						Display_Triangle(1);
						Display_arrow(0);				
				}
				else if((page_index == 0)&&(RefreshDisplay == 1))
				{
						RefreshDisplay = 0;
						Fill_RAM(0x00);
						#ifdef Chinese
								Show_HZ12_12(112,16,82,83);//返回
								Show_HZ12_12(180,16,93,94);//币种
						#endif
						#ifdef English
								Show_AscII_Picture(112,16,BackBitmapDot,sizeof(BackBitmapDot));		
								Show_AscII_Picture(184,16,APPBitmapDot,sizeof(APPBitmapDot));
						#endif
						Display_Triangle(1);
						Display_arrow(1);	
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
				if((page_index == INIT_PAGE_INDEX)&&(RefreshDisplay == 1))
				{
						RefreshDisplay = 0;
						Fill_RAM(0x00);
						#ifdef Chinese
								Show_HZ12_12(44,16,82,83);//返回
								Show_HZ12_12(112,16,93,94);//币种
								Show_HZ12_12(180,16,95,96);//安全
						#endif
						#ifdef English
								Show_AscII_Picture(44,16,BackBitmapDot,sizeof(BackBitmapDot));
								Show_AscII_Picture(116,16,APPBitmapDot,sizeof(APPBitmapDot));
								Show_AscII_Picture(168,16,SecurityBitmapDot,sizeof(SecurityBitmapDot));
						#endif
						Display_Triangle(1);
						Display_arrow(1);
						Display_arrow(0);
				}
		}
}

uint8_t Display_AddAdd(char *address)
{
ADDADDLABEL:
		Key_Control(1);
		Fill_RAM(0x00);
		#ifdef Chinese
				Show_HZ12_12(0,7,139,140);//正在
				Show_HZ12_12(32,7,24,25);//导入
				Show_HZ12_12(64,7,21,21);//新
				Show_HZ12_12(80,7,11,11);//的
				Show_HZ12_12(96,7,61,61);//私
				Show_HZ12_12(112,7,22,22);//钥
				
				Show_HZ12_12(0,28,12,12);//请
				Show_HZ12_12(16,28,141,141);//检
				Show_HZ12_12(32,28,78,78);//查
				Show_HZ12_12(48,28,23,23);//并
				Show_HZ12_12(64,28,6,7);//确认
				Show_HZ12_12(96,28,17,18);//钱包
				Show_HZ12_12(128,28,8,9);//地址

				Display_Fill_Triangle(1,1);
				Show_HZ12_12(184,51,111,112);//取消
		#endif
		#ifdef English
				Show_AscII_Picture(10,7,ImportingBitmapDot,sizeof(ImportingBitmapDot));//64
				Show_AscII_Picture(78,7,aBitmapDot,sizeof(aBitmapDot));//8
				Show_AscII_Picture(90,7,newBitmapDot,sizeof(newBitmapDot));//24
				Show_AscII_Picture(118,7,keyBitmapDot,sizeof(keyBitmapDot));//24
				Asc8_16(142,7,",");
				Show_AscII_Picture(154,7,pleaseBitmapDot,sizeof(pleaseBitmapDot));//48
				Show_AscII_Picture(206,7,checkBitmapDot,sizeof(checkBitmapDot));//40
				
				Show_AscII_Picture(24,28,theBitmapDot,sizeof(theBitmapDot));//24
				Show_AscII_Picture(52,28,addressBitmapDot,sizeof(addressBitmapDot));//56
				Show_AscII_Picture(112,28,andBitmapDot,sizeof(andBitmapDot));//24
				Show_AscII_Picture(140,28,confirmBitmapDot,sizeof(confirmBitmapDot));//48
				Show_AscII_Picture(192,28,belowBitmapDot,sizeof(belowBitmapDot));//40

				Show_AscII_Picture(172,51,CancelBitmapDot,sizeof(CancelBitmapDot));
				Display_Fill_Triangle(1,1);
		#endif
		Key_Control(0);
		Key_Control(1);
		while(1)
		{
				if(Key_Flag.flag.middle)//确认
				{
						Key_Flag.flag.middle = 0;
						break;
				}
				if(Key_Flag.flag.right)//取消
				{
						Key_Control(0);
						return NEO_USER_REFUSE;
				}
		}
		Key_Control(0);
		
		Key_Control(1);
		Fill_RAM(0x00);					
		Asc8_16(0,7,(uint8_t*)address);
		#ifdef Chinese
				Display_Fill_Triangle(0,0);
				Show_HZ12_12(112,51,6,7);//确认
				Show_HZ12_12(184,51,111,112);//取消
		#endif
		#ifdef English
				Show_AscII_Picture(104,51,ConfirmBitmapDot,sizeof(ConfirmBitmapDot));	
				Show_AscII_Picture(172,51,CancelBitmapDot,sizeof(CancelBitmapDot));
				Display_Fill_Triangle(0,0);
		#endif
		Key_Control(0);

		Key_Control(1);
		Asc8_16(244,28,"s");
		Start_TIM(OLED_INPUT_TIME);
		while(1)
		{
				if(Display_Time_count() == 0)//超时
						return NEO_TIME_OUT;
				if(Key_Flag.flag.middle)//确认
				{
						Key_Control(1);
						return NEO_SUCCESS;				//添加地址成功
				}
				if(Key_Flag.flag.right)//取消
				{
						Key_Control(1);
						return NEO_USER_REFUSE;				//添加地址失败
				}
				if(Key_Flag.flag.left)//返回
				{
						goto ADDADDLABEL;
				}
		}
}

uint8_t Display_DelAdd(uint8_t AddID)
{
		uint8_t value = 0;
		uint8_t index = 0;
	
		Key_Control(1);
		Fill_RAM(0x00);
		#ifdef Chinese
				index = 128 - (8*showaddress[AddID].len_name + 136)/2;
				Show_HZ12_12(index,7,53,56);//是否删除
				index += 64;
				Asc8_16(index,7,(uint8_t*)showaddress[AddID].name);
				index += 8*showaddress[AddID].len_name;
				Show_HZ12_12(index,7,59,61);//及其私
				index += 48;
				Show_HZ12_12(index,7,22,22);//钥
				index += 16;
				Asc8_16(index,7,"?");
	
				Show_HZ12_12(72,28,55,56);//删除
				Show_HZ12_12(104,28,42,42);//后
				Show_HZ12_12(120,28,91,92);//无法
				Show_HZ12_12(152,28,110,110);//找
				Show_HZ12_12(168,28,83,83);//回
	
				Show_HZ12_12(120,51,53,53);//是
				Show_HZ12_12(182,51,54,54);//否
		#endif
		#ifdef English
Deladd:
				Fill_RAM(0x00);
				index = 128 - (8*showaddress[AddID].len_name + 156)/2;
				Show_AscII_Picture(index,7,DoBitmapDot,sizeof(DoBitmapDot));//16
				index += 20;
				Show_AscII_Picture(index,7,youBitmapDot,sizeof(youBitmapDot));//24
				index += 28;
				Show_AscII_Picture(index,7,wantBitmapDot,sizeof(wantBitmapDot));//32
				index += 36;
				Show_AscII_Picture(index,7,toBitmapDot,sizeof(toBitmapDot));//16
				index += 20;
				Show_AscII_Picture(index,7,eraseBitmapDot,sizeof(eraseBitmapDot));//40
				index += 44;
				Asc8_16(index,7,(uint8_t*)showaddress[AddID].name);
				index += 8*showaddress[AddID].len_name;
				Asc8_16(index,7,"?");
			
				Show_AscII_Picture(36,28,PINBitmapDot,sizeof(PINBitmapDot));//24
				Show_AscII_Picture(64,28,isBitmapDot,sizeof(isBitmapDot));//16
				Show_AscII_Picture(84,28,requiredBitmapDot,sizeof(requiredBitmapDot));//56
				Show_AscII_Picture(144,28,toBitmapDot,sizeof(toBitmapDot));//16
				Show_AscII_Picture(164,28,proceedBitmapDot,sizeof(proceedBitmapDot));//56
				
				Display_Fill_Triangle(1,1);
				Show_AscII_Picture(172,51,CancelBitmapDot,sizeof(CancelBitmapDot));				
				
				Key_Flag.flag.middle = 0;
				while(Key_Flag.flag.middle == 0)
				{
						if(Key_Flag.flag.middle == 1)
						{
								Key_Control(0);
								break;
						}
						if(Key_Flag.flag.right == 1)
						{
								Key_Flag.flag.right = 0;
								return NEO_USER_REFUSE;
						}					
				}

				Fill_RAM(0x00);
				Show_AscII_Picture(8,7,AreBitmapDot,sizeof(AreBitmapDot));//24
				Show_AscII_Picture(36,7,youBitmapDot,sizeof(youBitmapDot));//24
				Show_AscII_Picture(64,7,sureBitmapDot,sizeof(sureBitmapDot));//32
				Show_AscII_Picture(100,7,youBitmapDot,sizeof(youBitmapDot));//24
				Show_AscII_Picture(128,7,wantBitmapDot,sizeof(wantBitmapDot));//32
				Show_AscII_Picture(164,7,toBitmapDot,sizeof(toBitmapDot));//16
				Show_AscII_Picture(184,7,proceedBitmapDot,sizeof(proceedBitmapDot));//56
				Asc8_16(240,7,"?");
				
				index = 128 - (8*showaddress[AddID].len_name + 148)/2;
				Show_AscII_Picture(index,28,ErasingBitmapDot,sizeof(ErasingBitmapDot));//48
				index += 52;
				Asc8_16(index,28,(uint8_t*)showaddress[AddID].name);
				index += 8*showaddress[AddID].len_name + 4;
				Show_AscII_Picture(index,28,isBitmapDot,sizeof(isBitmapDot));//16
				index += 20;
				Show_AscII_Picture(index,28,irreversibleBitmapDot,sizeof(irreversibleBitmapDot));//72
				
				Display_Fill_Triangle(0,0);
				Show_AscII_Picture(104,51,ConfirmBitmapDot,sizeof(ConfirmBitmapDot));
				Show_AscII_Picture(172,51,CancelBitmapDot,sizeof(CancelBitmapDot));
		#endif
		Asc8_16(244,28,"s");
		Key_Control(1);
		Start_TIM(OLED_INPUT_TIME);
		while(1)
		{
				if(Display_Time_count() == 0)
						return NEO_TIME_OUT;//超时
				if(Key_Flag.flag.middle)
				{
						value = NEO_SUCCESS;
						break;
				}
				if(Key_Flag.flag.right)
				{
						value = NEO_USER_REFUSE;
						break;
				}
#ifdef English
				if(Key_Flag.flag.left)
				{
						goto Deladd;
				}
#endif
		}
		Key_Control(1);
		return value;
}

uint8_t Display_Sign_ContractTran(SIGN_Out_Para *data,ADDRESS *address,uint8_t signdata_index,char *dst_address)
{
		uint8_t index = 0;
		//显示数目   该值为一个long long型  对应的十进制数的后八位数为小数
		int count_bit=0;
		int count_int = 0;//表示整数部分占用的显示位数
		int count_dec = 8;//表示小数部分后缀的零的个数
	
SignConTra:
		Key_Control(1);
		Fill_RAM(0x00);
	
		#ifdef Chinese
				Show_HZ12_12(0,0,122,122);//从
				Asc8_16(16,0,(uint8_t*)address->name);
				index = 16 + 8*address->len_name;
				Show_HZ12_12(index,0,123,124);//发送
				index += 32;
				{
						count_int = drawNumber(index,0,data->money[signdata_index]/100000000,8);
						if(data->money[signdata_index]%100000000)//消除值正好为100000000的显示BUG
						{
								Asc8_16(index+count_int*8,0,".");
								count_dec = drawxNumber(index+(count_int+1)*8,0,data->money[signdata_index]%100000000,8) - 1;//-1是把小数点算进去
						}
						if(data->money[signdata_index] == 0)//值为0的情况
								count_dec = 8;
						count_bit = index + (count_int + 8 -count_dec)*8 + 4; //index是前面占用的显示，+4是显示空隙，美观
						
						if(data->coin == 0)
								Asc8_16(count_bit,0,"GAS");
						else if(data->coin == 1)
								Asc8_16(count_bit,0,"NEO");

						Show_HZ12_12(count_bit+28,0,125,125);//到
						Display_Fill_Triangle(1,1);
						while(Key_Flag.flag.middle == 0);
						Key_Flag.flag.middle = 0;
				}
		#endif
		#ifdef English
				Show_AscII_Picture(0,7,FromBitmapDot,sizeof(FromBitmapDot));
				Asc8_16(32,7,":");
				Asc8_16(44,7,(uint8_t*)address->name);
								
				Show_AscII_Picture(0,28,SendBitmapDot,sizeof(SendBitmapDot));
				index += 36;
				{
						count_int = drawNumber(index,28,data->money[signdata_index]/100000000,8);
						if(data->money[signdata_index]%100000000)//消除值正好为100000000的显示BUG
						{
								Asc8_16(index+count_int*8,28,".");
								count_dec = drawxNumber(index+(count_int+1)*8,28,data->money[signdata_index]%100000000,8) - 1;//-1是把小数点算进去
						}
						if(data->money[signdata_index] == 0)//值为0的情况
								count_dec = 8;
						count_bit = index + (count_int + 8 -count_dec)*8 + 8; //index是前面占用的显示，+8是显示空隙，美观
						
						if(data->coin == 0)
								Asc8_16(count_bit,28,"GAS");
						else if(data->coin == 1)
								Asc8_16(count_bit,28,"NEO");
				}
				Show_AscII_Picture(count_bit+32,28,toBitmapDot,sizeof(toBitmapDot));
				Asc8_16(count_bit+48,28,":");
				Display_Fill_Triangle(1,1);
				Key_Flag.flag.middle = 0;
				while(Key_Flag.flag.middle == 0);
				Key_Flag.flag.middle = 0;
		#endif

		Fill_RAM(0x00);
		Asc8_16(0,0,(uint8_t*)dst_address);
		#ifdef Chinese
				Display_Fill_Triangle(0,0);
				Show_HZ12_12(112,51,6,7);//确认
				Show_HZ12_12(184,51,111,112);//取消
		#endif
		#ifdef English
				Display_Fill_Triangle(0,0);		
				Show_AscII_Picture(104,51,ConfirmBitmapDot,sizeof(ConfirmBitmapDot));
				Show_AscII_Picture(172,51,CancelBitmapDot,sizeof(CancelBitmapDot));
		#endif

		Asc8_16(244,28,"s");
		Start_TIM(OLED_INPUT_TIME);
		Key_Control(1);
		while(1)
		{
				if(Display_Time_count() == 0)
						return NEO_TIME_OUT;//超时
				if(Key_Flag.flag.middle)
				{
						Key_Control(0);
						if(Display_VerifyCode() == 0)//密码正确
						{
								break;
						}
						else
						{
								return NEO_TIME_OUT;
						}
				}
				if(Key_Flag.flag.right)
				{
						Key_Control(0);
						return NEO_USER_REFUSE;	//取消
				}
				if(Key_Flag.flag.left)
				{
						index = 0;
						count_bit=0;
						count_int = 0;
						count_dec = 8;
						goto SignConTra;
				}
		}		
		Key_Control(1);
		return NEO_SUCCESS;
}

uint8_t Display_Sign_Nep5(SIGN_Out_Para *data,ADDRESS *address,char *dst_address)
{
		uint8_t index = 0;
		//显示数目   该值为一个long long型  对应的十进制数的后八位数为小数
		int count_bit=0;
		int count_int = 0;//表示整数部分占用的显示位数
		int count_dec = 8;//表示小数部分后缀的零的个数
	
SignNep5:	
		Key_Control(1);
		Fill_RAM(0x00);		
	
		#ifdef Chinese
				Show_HZ12_12(0,0,122,122);//从
				Asc8_16(16,0,(uint8_t*)address->name);
				index = 16 + 8*address->len_name;
				Show_HZ12_12(index,0,123,124);//发送
				index += 32;
				{
						count_int = drawNumber(index,0,data->money[1]/100000000,8);
						if(data->money[1]%100000000)//消除值正好为100000000的显示BUG
						{
								Asc8_16(index+count_int*8,0,".");
								count_dec = drawxNumber(index+(count_int+1)*8,0,data->money[1]%100000000,8) - 1;//-1是把小数点算进去
						}
						if(data->money[1] == 0)//值为0的情况
								count_dec = 8;
						count_bit = index + (count_int + 8 -count_dec)*8 + 4; //index是前面占用的显示，+4是显示空隙，美观

						if(data->coin == 2)
								Asc8_16(count_bit,0,"NNC");
						else if(data->coin == 3)
								Asc8_16(count_bit,0,"CPX");

						Show_HZ12_12(count_bit+28,0,125,125);//到
						Display_Fill_Triangle(1,1);
						while(Key_Flag.flag.middle == 0);
						Key_Flag.flag.middle = 0;
				}
		#endif
		#ifdef English
				Show_AscII_Picture(0,7,FromBitmapDot,sizeof(FromBitmapDot));
				Asc8_16(32,7,":");
				Asc8_16(36,7,(uint8_t*)address->name);
								
				Show_AscII_Picture(0,28,SendBitmapDot,sizeof(SendBitmapDot));
				index += 36;
				{
						count_int = drawNumber(index,0,data->money[1]/100000000,8);
						if(data->money[1]%100000000)//消除值正好为100000000的显示BUG
						{
								Asc8_16(index+count_int*8,28,".");
								count_dec = drawxNumber(index+(count_int+1)*8,28,data->money[1]%100000000,8) - 1;//-1是把小数点算进去
						}
						if(data->money[1] == 0)//值为0的情况
								count_dec = 8;
						count_bit = index + (count_int + 8 -count_dec)*8 + 4; //index是前面占用的显示，+4是显示空隙，美观
						
						if(data->coin == 2)
								Asc8_16(count_bit,28,"NNC");
						else if(data->coin == 3)
								Asc8_16(count_bit,28,"CPX");
				}
				Show_AscII_Picture(count_bit+32,28,toBitmapDot,sizeof(toBitmapDot));
				Asc8_16(count_bit+48,28,":");
				Display_Fill_Triangle(1,1);
				Key_Flag.flag.middle = 0;
				while(Key_Flag.flag.middle == 0);
				Key_Flag.flag.middle = 0;
		#endif
		
		Fill_RAM(0x00);
		Asc8_16(0,0,(uint8_t*)dst_address);
		#ifdef Chinese		
				Display_Fill_Triangle(0,0);
				Show_HZ12_12(112,51,6,7);//确认
				Show_HZ12_12(184,51,111,112);//取消
		#endif
		#ifdef English
				Display_Fill_Triangle(0,0);		
				Show_AscII_Picture(104,51,ConfirmBitmapDot,sizeof(ConfirmBitmapDot));
				Show_AscII_Picture(172,51,CancelBitmapDot,sizeof(CancelBitmapDot));	
		#endif
		
		Asc8_16(244,28,"s");
		Start_TIM(OLED_INPUT_TIME);
		Key_Control(1);
		while(1)
		{
				if(Display_Time_count() == 0)
						return NEO_TIME_OUT;//超时
				if(Key_Flag.flag.middle)
				{
						Key_Control(0);
						if(Display_VerifyCode() == 0)//密码正确
						{
								break;
						}
						else
						{
								return NEO_TIME_OUT;
						}
				}
				if(Key_Flag.flag.right)
				{
						Key_Control(0);
						return NEO_USER_REFUSE;	//取消
				}
				if(Key_Flag.flag.left)
				{
						index = 0;
						count_bit=0;
						count_int = 0;
						count_dec = 8;
						goto SignNep5;
				}
		}
		Key_Control(1);
		return NEO_SUCCESS;
}

uint8_t Display_Sign_Data_Type_Identify(void)
{
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
	
		Asc8_16(244,28,"s");
		Start_TIM(OLED_INPUT_TIME);
		while(1)
		{
				if(Display_Time_count() == 0)
						return NEO_TIME_OUT;//超时
				if(Key_Flag.flag.middle)//确定
				{
						value = NEO_SUCCESS;
						break;
				}
				else if(Key_Flag.flag.left || Key_Flag.flag.right)//取消
				{
						value = NEO_USER_REFUSE;
						break;
				}
		}
		Key_Control(0);	
		return value;
}

uint8_t Display_Updata_Wallet(void)
{
		uint8_t value = 0;

UpdWal:
		Fill_RAM(0x00);
		#ifdef Chinese
				Show_HZ12_12(64,16,35,35);//有
				Show_HZ12_12(80,16,21,21);//新
				Show_HZ12_12(96,16,11,11);//的
				Show_HZ12_12(112,16,77,77);//固
				Show_HZ12_12(128,16,46,46);//件
				Show_HZ12_12(144,16,66,66);//可
				Show_HZ12_12(160,16,13,14);//使用
			
				Display_Fill_Triangle(1,1);	
				Show_HZ12_12(184,51,111,112);//取消
		#endif
		#ifdef English
				Show_AscII_Picture(28,16,ABitmapDot,sizeof(ABitmapDot));//8
				Show_AscII_Picture(40,16,firmwareBitmapDot,sizeof(firmwareBitmapDot));//56
				Show_AscII_Picture(100,16,updateBitmapDot,sizeof(updateBitmapDot));//48
				Show_AscII_Picture(152,16,isBitmapDot,sizeof(isBitmapDot));//16
				Show_AscII_Picture(172,16,availableBitmapDot,sizeof(availableBitmapDot));//56
				
				Show_AscII_Picture(172,51,CancelBitmapDot,sizeof(CancelBitmapDot));//48
				Display_Fill_Triangle(1,1);
		#endif
		
		Key_Control(1);
		while(1)
		{
				if(Key_Flag.flag.middle)
				{
						Key_Control(0);
						break;
				}
				if(Key_Flag.flag.right)
				{		
						Key_Control(0);
						return 0;
				}
		}
		
		Fill_RAM(0x00);
		#ifdef Chinese
				Show_HZ12_12(76,7,53,54);//是否
				Show_HZ12_12(108,7,69,70);//升级
				Show_HZ12_12(140,7,77,77);//固
				Show_HZ12_12(156,7,46,46);//件
				Asc8_16(172,7,"?");
			
				Asc8_16(48,28,"(");
				Show_HZ12_12(56,28,69,70);//升级
				Show_HZ12_12(88,28,142,142);//时
				Show_HZ12_12(104,28,12,12);//请
				Show_HZ12_12(120,28,143,145);//勿进行
				Show_HZ12_12(168,28,63,64);//操作
				Asc8_16(200,28,")");
		
				Display_Fill_Triangle(0,0);	
				Show_HZ12_12(120,51,53,53);//是
				Show_HZ12_12(182,51,54,54);//否
		#endif
		#ifdef English
				Show_AscII_Picture(78,7,AllowBitmapDot,sizeof(AllowBitmapDot));//40
				Show_AscII_Picture(122,7,updateBitmapDot,sizeof(updateBitmapDot));//48
				Asc8_16(170,7,"?");
				
				Show_AscII_Picture(36,28,PINBitmapDot,sizeof(PINBitmapDot));//24
				Show_AscII_Picture(64,28,isBitmapDot,sizeof(isBitmapDot));//16
				Show_AscII_Picture(84,28,requiredBitmapDot,sizeof(requiredBitmapDot));//56				
				Show_AscII_Picture(144,28,toBitmapDot,sizeof(toBitmapDot));//16
				Show_AscII_Picture(164,28,proceedBitmapDot,sizeof(proceedBitmapDot));//56
				
				Display_Fill_Triangle(0,0);
				Show_AscII_Picture(104,51,ConfirmBitmapDot,sizeof(ConfirmBitmapDot));
				Show_AscII_Picture(172,51,CancelBitmapDot,sizeof(CancelBitmapDot));
		#endif
		Key_Control(1);
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
				if(Key_Flag.flag.right)
				{		
						value = 0;
						break;
				}
				if(Key_Flag.flag.left)
				{
						Key_Flag.flag.left = 0;
						goto UpdWal;
				}
		}
		Key_Control(0);
		return value;
}


