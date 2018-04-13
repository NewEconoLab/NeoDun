//////////////////////////////////////////////////////////////////////////////////	 
// M31S1322_01_V1 FOR STM32 201505 WWW.LCDSOC.COM

//********************************************************************************
//接口定义参照oled.h文件
#include "oled.h"
#include <string.h>
#include "stdlib.h"
//定义ASCII库
#include "Asc5x8.h" 	   //ascii 5*8字库
#include "Asc6x12.h" 	   //ascii 8*16字库
#include "Asc8x16.h" 	   //ascii 8*16字库
#include "Asc12x24.h" 	 //ascii 12*24字库
//自定义汉字库
#include "HZ12X12_S.h" 	 //12*12宋体自定义汉字库
#include "main_define.h"
#include "aw9136.h"
#include "app_interface.h"

extern unsigned char gImage_triangle_down[72];
extern unsigned char gImage_emptypin[72];
extern unsigned char gImage_fullpin[72];
extern volatile unsigned char touch_motor_flag;    //1表示开启触摸振动，0表示关闭振动
extern SIGN_KEY_FLAG Key_Flag;
extern BOOT_FLAG BootFlag;
//向SSD1325写入一个字节。
//dat:要写入的数据/命令
//cmd:数据/命令标志 0,表示命令;1,表示数据;
void OLED_WR_Byte(u8 dat,u8 cmd)
{	
	u8 i;			  
	if(cmd)
	  OLED_DC_Set();
	else 
	  OLED_DC_Clr();		  
	OLED_CS_Clr();
	for(i=0;i<8;i++)
	{			  
		OLED_SCLK_Clr();
		if(dat&0x80)
		   OLED_SDIN_Set();
		else 
		   OLED_SDIN_Clr();
		OLED_SCLK_Set();
		dat<<=1;   
	}				 		  
	OLED_CS_Set();
	OLED_DC_Set();   	  
} 
  
static void delay_ms(int ms) {
	HAL_Delay(ms);
} 

//-----------------OLED端口定义 4线SPI----------------  					   
/* 采用IO口模拟SPI通讯方式驱动oled模块
CS   ~ PE6
RES ~ PE5
DC   ~ PE4
CLK  ~ PE3
DIN ~ PE2
*/  
//初始化SSD1322				    
void OLED_Init(void)
{ 	
  OLED_RST_Set();
	delay_ms(100);
	OLED_RST_Clr();
	delay_ms(100);
	OLED_RST_Set(); 
		
	OLED_WR_Byte(0xAE,OLED_CMD); //	Display Off
	
	OLED_WR_Byte(0xFD,OLED_CMD); // Set Command Lock
	OLED_WR_Byte(0x12,OLED_DATA); //
	
	OLED_WR_Byte(0xB3,OLED_CMD); // Set Clock as 80 Frames/Sec
	OLED_WR_Byte(0x91,OLED_DATA); //  

   	OLED_WR_Byte(0xCA,OLED_CMD); // Set Multiplex Ratio
	OLED_WR_Byte(0x3F,OLED_DATA); // 1/64 Duty (0x0F~0x5F)
   
	OLED_WR_Byte(0xA2,OLED_CMD); // Shift Mapping RAM Counter (0x00~0x3F)
	OLED_WR_Byte(0x00,OLED_DATA); //	
	
	OLED_WR_Byte(0xA1,OLED_CMD); // Set Mapping RAM Display Start Line (0x00~0x7F)
	OLED_WR_Byte(0x00,OLED_DATA); //	

	
	OLED_WR_Byte(0xA0,OLED_CMD); //Set Column Address 0 Mapped to SEG0 
	OLED_WR_Byte(0x14,OLED_DATA);      	//   Default => 0x40
						                //     Horizontal Address Increment
						//     Column Address 0 Mapped to SEG0
						//     Disable Nibble Remap
						//     Scan from COM0 to COM[N-1]
						//     Disable COM Split Odd Even
	OLED_WR_Byte(0x11,OLED_DATA);      	//    Default => 0x01 (Disable Dual COM Mode)

	
	OLED_WR_Byte(0xB5,OLED_CMD); //  Disable GPIO Pins Input
	OLED_WR_Byte(0x00,OLED_DATA); //	
	
    OLED_WR_Byte(0xAB,OLED_CMD); //   Enable Internal VDD Regulator
	OLED_WR_Byte(0x01,OLED_DATA); //

	OLED_WR_Byte(0xB4,OLED_CMD); //  Display Enhancement  
	OLED_WR_Byte(0xA0,OLED_DATA); // Enable External VSL
	OLED_WR_Byte(0xF8,OLED_DATA); // Enhance Low Gray Scale Display Quality

  OLED_WR_Byte(0xC1,OLED_CMD); //  Set Contrast Current 
	OLED_WR_Byte(0x7F,OLED_DATA); //  Default => 0x7F

    OLED_WR_Byte(0xC7,OLED_CMD); //  Master Contrast Current Control 
	OLED_WR_Byte(Brightness,OLED_DATA); //  Default => 0x0f (Maximum)

    OLED_WR_Byte(0xB8,OLED_CMD); //  	// Set Gray Scale Table 
	OLED_WR_Byte(0x0C,OLED_DATA); //
	OLED_WR_Byte(0x18,OLED_DATA); //
	OLED_WR_Byte(0x24,OLED_DATA); //
	OLED_WR_Byte(0x30,OLED_DATA); //
	OLED_WR_Byte(0x3C,OLED_DATA); //
	OLED_WR_Byte(0x48,OLED_DATA); //
	OLED_WR_Byte(0x54,OLED_DATA); //
	OLED_WR_Byte(0x60,OLED_DATA); //
	OLED_WR_Byte(0x6C,OLED_DATA); //
	OLED_WR_Byte(0x78,OLED_DATA); //
	OLED_WR_Byte(0x84,OLED_DATA); //
	OLED_WR_Byte(0x90,OLED_DATA); //
	OLED_WR_Byte(0x9C,OLED_DATA); //
	OLED_WR_Byte(0xA8,OLED_DATA); //
	OLED_WR_Byte(0xB4,OLED_DATA); //
 	OLED_WR_Byte(0x00,OLED_CMD); 	// Enable Gray Scale Table

	
	OLED_WR_Byte(0xB1,OLED_CMD); //   Set Phase 1 as 5 Clocks & Phase 2 as 14 Clocks
	OLED_WR_Byte(0xE2,OLED_DATA); //  Default => 0x74 (7 Display Clocks [Phase 2] / 9 Display Clocks [Phase 1])

    OLED_WR_Byte(0xD1,OLED_CMD); //    Enhance Driving Scheme Capability (0x00/0x20)
	OLED_WR_Byte(0xA2,OLED_DATA); //   Normal
	OLED_WR_Byte(0x20,OLED_DATA); //

   
    OLED_WR_Byte(0xBB,OLED_CMD); //     Set Pre-Charge Voltage Level as 0.60*VCC
	OLED_WR_Byte(0x1F,OLED_DATA); //

    OLED_WR_Byte(0xB6,OLED_CMD); //  // Set Second Pre-Charge Period as 8 Clocks 
	OLED_WR_Byte(0x08,OLED_DATA); //

    OLED_WR_Byte(0xBE,OLED_CMD); //  Set Common Pins Deselect Voltage Level as 0.86*VCC 
	OLED_WR_Byte(0x07,OLED_DATA); //  Default => 0x04 (0.80*VCC)

    OLED_WR_Byte(0xA6,OLED_CMD); //     Normal Display Mode (0x00/0x01/0x02/0x03)
	 								  //   Default => 0xA4
						//     0xA4  => Entire Display Off, All Pixels Turn Off
						//     0xA5  => Entire Display On, All Pixels Turn On at GS Level 15
						//     0xA6  => Normal Display
						//     0xA7  => Inverse Display
	 
	 OLED_WR_Byte(0xA9,OLED_CMD); //   Disable Partial Display
     	 	
	Fill_RAM(0x00);
	OLED_WR_Byte(0xAF,OLED_CMD); //	Display On
 

}  

void Set_Column_Address(unsigned char a, unsigned char b)
{
	OLED_WR_Byte(0x15,OLED_CMD);			// Set Column Address
	OLED_WR_Byte(a,OLED_DATA);			//   Default => 0x00
	OLED_WR_Byte(b,OLED_DATA);			//   Default => 0x77
}


void Set_Row_Address(unsigned char a, unsigned char b)
{
	OLED_WR_Byte(0x75,OLED_CMD);			// Set Row Address
	OLED_WR_Byte(a,OLED_DATA);			//   Default => 0x00
	OLED_WR_Byte(b,OLED_DATA);			//   Default => 0x7F
}

void Set_Write_RAM()
{
 
	OLED_WR_Byte(0x5C,OLED_CMD);			// Enable MCU to Write into RAM
}

void Set_Read_RAM()
{
	OLED_WR_Byte(0x5D,OLED_CMD);			// Enable MCU to Read from RAM
}


 void Set_Remap_Format(unsigned char d)
{
	 OLED_WR_Byte(0xA0,OLED_CMD);  			// Set Re-Map / Dual COM Line Mode
  OLED_WR_Byte(d,OLED_DATA);  				//   Default => 0x40
						//     Horizontal Address Increment
						//     Column Address 0 Mapped to SEG0
						//     Disable Nibble Remap
						//     Scan from COM0 to COM[N-1]
						//     Disable COM Split Odd Even
    OLED_WR_Byte(0x11,OLED_DATA); 	 			//   Default => 0x01 (Disable Dual COM Mode)
} 
  void Set_Partial_Display(unsigned char a, unsigned char b, unsigned char c)
{
  	OLED_WR_Byte((0xA8|a),OLED_CMD);		// Default => 0x8F
					
	if(a == 0x00)
	{
	    OLED_WR_Byte(b,OLED_DATA); 
		OLED_WR_Byte(c,OLED_DATA); 
	 
	}
}

void Set_Display_Offset(unsigned char d)
{
	OLED_WR_Byte(0xA2,OLED_CMD);
 	OLED_WR_Byte(d,OLED_DATA);
 
}

void Set_Start_Line(unsigned char d)
{
	OLED_WR_Byte(0xA1,OLED_CMD);	// Set Vertical Scroll by RAM
 	OLED_WR_Byte(d,OLED_DATA);
 
}

void Set_Master_Current(unsigned char d)
{
   OLED_WR_Byte(0xC7,OLED_CMD);	//  Master Contrast Current Control
 	OLED_WR_Byte(d,OLED_DATA);

}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Gray Scale Table Setting (Full Screen)
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Set_Gray_Scale_Table()
{

    OLED_WR_Byte(0xB8,OLED_CMD); //  	// Set Gray Scale Table 
	OLED_WR_Byte(0x0C,OLED_DATA); //
	OLED_WR_Byte(0x18,OLED_DATA); //
	OLED_WR_Byte(0x24,OLED_DATA); //
	OLED_WR_Byte(0x30,OLED_DATA); //
	OLED_WR_Byte(0x3C,OLED_DATA); //
	OLED_WR_Byte(0x48,OLED_DATA); //
	OLED_WR_Byte(0x54,OLED_DATA); //
	OLED_WR_Byte(0x60,OLED_DATA); //
	OLED_WR_Byte(0x6C,OLED_DATA); //
	OLED_WR_Byte(0x78,OLED_DATA); //
	OLED_WR_Byte(0x84,OLED_DATA); //
	OLED_WR_Byte(0x90,OLED_DATA); //
	OLED_WR_Byte(0x9C,OLED_DATA); //
	OLED_WR_Byte(0xA8,OLED_DATA); //
	OLED_WR_Byte(0xB4,OLED_DATA); //
 	OLED_WR_Byte(0x00,OLED_CMD); 	// Enable Gray Scale Table

}
void Set_Linear_Gray_Scale_Table()
{
   	OLED_WR_Byte(0xB9,OLED_CMD); //	Set Default Linear Gray Scale Table
 
}
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Vertical Scrolling (Full Screen)
//
//    a: Scrolling Direction
//       "0x00" (Upward)		向上滚屏
//       "0x01" (Downward)		向下滚屏
//    b: Set Numbers of Row Scroll per Step	  每帧行数
//    c: Set Time Interval between Each Scroll Step	每帧间延时
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//void Vertical_Scroll(unsigned char a, unsigned char b, unsigned char c)
//{
//unsigned char i,j;	

//	Set_Partial_Display(0x00,0x00,Max_Row);

//	switch(a)
//	{
//		case 0:
//			for(i=0;i<(Max_Row+1);i+=b)
//			{
//				Set_Display_Offset(i+1);
//				for(j=0;j<c;j++)
//				{
//					HAL_Delay(200);
//				}
//			}
//			break;
//		case 1:
//			for(i=0;i<(Max_Row+1);i+=b)
//			{
//				Set_Display_Offset(Max_Row-i);
//				for(j=0;j<c;j++)
//				{
//					HAL_Delay(200);
//				}
//			}
//			break;
//	}
//	Set_Partial_Display(0x01,0x00,0x00);
//}
  
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Show Regular Pattern (Full Screen)
 //清屏函数,清屏后,整个屏幕是黑的
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Fill_RAM(unsigned char Data)
{
unsigned char i,j;

	Set_Column_Address(0x00,0x77);
	Set_Row_Address(0x00,0x7F);
	Set_Write_RAM();

	for(i=0;i<128;i++)
	{
		for(j=0;j<120;j++)
		{
			OLED_WR_Byte(Data,OLED_DATA);
		    OLED_WR_Byte(Data,OLED_DATA); 
		}
	}
}
 


 //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  www.lcdsoc.com 201505
//	  Data:取值为 0x11，0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xaa,0xbb,0xcc,0xdd,0xee,0xff, ff最亮
//    a: Line Width		线宽
//    b: Column Address of Start 开始列
//    c: Column Address of End	 结束列
//    d: Row Address of Start	 开始行
//    e: Row Address of End		 结束行
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Draw_Rectangle(unsigned char Data, unsigned char a, unsigned char b, unsigned char c, unsigned char d, unsigned char e)
{
unsigned char i,j,k,l,b1,c1;
	b1=b/4;
	c1=c/4 ;
	k=a%4;
	if(k == 0)
	{
		l=(a/4)-1;
	}
	else
	{
		l=a/4;
	}

	Set_Column_Address(Shift+b1,Shift+c1);
	Set_Row_Address(d,(d+a-1));
	Set_Write_RAM();
	for(i=0;i<(c1-b1+1);i++)
	{
		for(j=0;j<a;j++)
		{
			OLED_WR_Byte(Data,OLED_DATA);
			OLED_WR_Byte(Data,OLED_DATA); 
		}
	}

	Set_Column_Address(Shift+(c1-l),Shift+c1);
	Set_Row_Address(d+a,e-a);
	Set_Write_RAM();
	for(i=0;i<(e-d+1);i++)
	{
		for(j=0;j<(l+1);j++)
		{
			if(j == 0)
			{
				switch(k)
				{
					case 0:
						OLED_WR_Byte(Data,OLED_DATA);
						OLED_WR_Byte(Data,OLED_DATA);
						break;
					case 1:
						OLED_WR_Byte(0x00,OLED_DATA);
						OLED_WR_Byte((Data&0x0F),OLED_DATA);
						 
						break;
					case 2:
					    OLED_WR_Byte(0x00,OLED_DATA);
						OLED_WR_Byte(Data,OLED_DATA);
					
						break;
					case 3:
						OLED_WR_Byte((Data&0x0F),OLED_DATA);
						OLED_WR_Byte(Data,OLED_DATA);
						 
						break;
				}
			}
			else
			{
						OLED_WR_Byte(Data,OLED_DATA);
		             	OLED_WR_Byte(Data,OLED_DATA); 
			}
		}
	}
   

	Set_Column_Address(Shift+b1,Shift+c1);
	Set_Row_Address((e-a+1),e);
	Set_Write_RAM();
	for(i=0;i<(c1-b1+1);i++)
	{
		for(j=0;j<a;j++)
		{
			OLED_WR_Byte(Data,OLED_DATA);
			OLED_WR_Byte(Data,OLED_DATA); 
		 
		}
	}
 
	Set_Column_Address(Shift+b1,Shift+(b1+l));
	Set_Row_Address(d+a,e-a);
	Set_Write_RAM();
	for(i=0;i<(e-d+1);i++)
	{
		for(j=0;j<(l+1);j++)
		{
			if(j == l)
			{
				switch(k)
				{
					case 0:
							OLED_WR_Byte(Data,OLED_DATA);
		                 	OLED_WR_Byte(Data,OLED_DATA); 
						break;
					case 1:
						
						OLED_WR_Byte((Data&0xF0),OLED_DATA);
						OLED_WR_Byte(0x00,OLED_DATA);
						 
						break;
					case 2:
						OLED_WR_Byte(Data,OLED_DATA);
						 OLED_WR_Byte(0x00,OLED_DATA);
					 
						break;
					case 3:
						
						OLED_WR_Byte(Data,OLED_DATA);
						OLED_WR_Byte((Data&0xF0),OLED_DATA);
				 
						break;
				}
			}
			else
			{
					OLED_WR_Byte(Data,OLED_DATA);
		         	OLED_WR_Byte(Data,OLED_DATA); 
			}
		}
	}
}
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Show Regular Pattern (Partial or Full Screen)
//
//    a: Column Address of Start  列
//    b: Column Address of End (Total Columns Devided by 4)
//    c: Row Address of Start  行
//    d: Row Address of End
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Fill_Block(unsigned char Data, unsigned char a, unsigned char b, unsigned char c, unsigned char d)
{
unsigned char i,j;
	
	Set_Column_Address(Shift+a,Shift+b);
	Set_Row_Address(c,d);
	Set_Write_RAM();

	for(i=0;i<(d-c+1);i++)
	{
		for(j=0;j<(b-a+1);j++)
		{
			OLED_WR_Byte(Data,OLED_DATA);
		    OLED_WR_Byte(Data,OLED_DATA); 
		}
	}
}
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Show Pattern (Partial or Full Screen)
//
//    a: Column Address of Start
//    b: Column Address of End (Total Columns Devided by 4)
//    c: Row Address of Start
//    d: Row Address of End
// 灰度模式下显示一副图片
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Show_Pattern(unsigned char *Data_Pointer, unsigned char a, unsigned char b, unsigned char c, unsigned char d)
{
			 
unsigned char *Src_Pointer;
unsigned char i,j;
 
  //取模时候像素正序	（不能反序与2.7不同）
    Src_Pointer=Data_Pointer;
	Set_Column_Address(Shift+a,Shift+b);
	Set_Row_Address(c,d);
	Set_Write_RAM();
  	for(i=0;i<(d-c+1);i++)
	{
		for(j=0;j<(b-a+1);j++)
		{
			OLED_WR_Byte(*Src_Pointer,OLED_DATA);
			Src_Pointer++;
			OLED_WR_Byte(*Src_Pointer,OLED_DATA);
			Src_Pointer++;
		}
	}

 }

/**************************************
  数据转换程序：将2位分成1个字节存入显存，由于1个seg表示4个列所以要同时写2个字节即4个像素
  uchar DATA：取模来的字模数据
****************************************/
void Con_4_byte(unsigned char DATA)
{
   unsigned char d1_4byte[4],d2_4byte[4];
   unsigned char i;
   unsigned char d,k1,k2;
   d=DATA;
 
  for(i=0;i<2;i++)   // 一两位的方式写入  2*4=8位
   {
     k1=d&0xc0;     //当i=0时 为D7,D6位 当i=1时 为D5,D4位

     /****有4种可能，16级灰度,一个字节数据表示两个像素，一个像素对应一个字节的4位***/

     switch(k1)
       {
	 case 0x00:
           d1_4byte[i]=0x00;
		   
         break;
     case 0x40:  // 0100,0000
           d1_4byte[i]=0x0f;
		   
         break;	
	 case 0x80:  //1000,0000
           d1_4byte[i]=0xf0;
		   
         break;
     case 0xc0:   //1100,0000
           d1_4byte[i]=0xff;
		  
         break;	 
     default:
      	 break;
	   }
     
	   d=d<<2;
	  k2=d&0xc0;     //当i=0时 为D7,D6位 当i=1时 为D5,D4位

     /****有4种可能，16级灰度,一个字节数据表示两个像素，一个像素对应一个字节的4位***/

     switch(k2)
       {
	 case 0x00:
           d2_4byte[i]=0x00;
		   
         break;
     case 0x40:  // 0100,0000
           d2_4byte[i]=0x0f;
		   
         break;	
	 case 0x80:  //1000,0000
           d2_4byte[i]=0xf0;
		 
         break;
     case 0xc0:   //1100,0000
           d2_4byte[i]=0xff;
		  
         break;	 
     default:
      	 break;
	   }
	  
	  d=d<<2;                                //左移两位
      
	 OLED_WR_Byte(d1_4byte[i],OLED_DATA);	    //写前2列
	 OLED_WR_Byte(d2_4byte[i],OLED_DATA);               //写后2列	  共计4列
   }

}
 
/***************************************************************
//  显示12*12点阵汉字 2015-05晶奥测试通过
//  取模方式为：横向取模左高位,数据排列:从左到右从上到下    16列 12行 
//   num：汉字在字库中的位置
//   x: Start Column  开始列 范围 0~（256-16）
//   y: Start Row   开始行 0~63 
***************************************************************/
void HZ12_12( unsigned char x, unsigned char y, unsigned char num)
{
	unsigned char x1,j ;
	x1=x/4; 
	Set_Column_Address(Shift+x1,Shift+x1+3); // 设置列坐标，shift为列偏移量由1322决定。3为16/4-1
	Set_Row_Address(y,y+11); 
	Set_Write_RAM();	 //	写显存
	 
	for(j=0;j<24;j++)
	{
		 Con_4_byte(HZ12X12_S[num*24+j]);
	}

}

//****************************************************
//   写入一串12*12汉字 www.lcdsoc.com	x坐标和字间距d要为4的倍数
//    num1,num2：汉字在字库中的位置	 从num1显示到num2
//    x: Start Column  开始列 范围 0~（255-16）
//    y: Start Row    开始行 0~63
//    d:字间距离 0为无间距 
//*****************************************************
void Show_HZ12_12(unsigned char  x,unsigned char  y, unsigned char  d,unsigned char num1,unsigned char num2)
{
  unsigned char  i,d1;
  d1=d+16;
  for(i=num1;i<num2+1;i++)
  {
  HZ12_12(x,y,i);
  x=x+d1;             
  }
}

 //==============================================================
//功能描述：写入一组标准ASCII字符串	 5x8
//参数：显示的位置（x,y），ch[]要显示的字符串
//返回：无
//==============================================================  
void Asc5_8(unsigned char x,unsigned char y,unsigned char ch[])
{
  unsigned char x1,c=0, i=0,j=0;      
  while (ch[i]!='\0')
  {    
    x1=x/4;
	c =ch[i]-32;
    if(x1>61)
	   {x=0;
	   x1=x/4;
	   y=y+8;}  //换行																	
    Set_Column_Address(Shift+x1,Shift+x1+1); // 设置列坐标，shift为列偏移量由1322决定 
	Set_Row_Address(y,y+7); 
	Set_Write_RAM();	 //	写显存    
  	
		for(j=0;j<8;j++)
	 		  {
				 Con_4_byte(ASC5X8[c*8+j]);	//数据转换
			   }
	 i++;
	 x=x+8;	 //字间距，8为最小
  }
}
 //==============================================================
//功能描述：写入一组标准ASCII字符串	 6x12
//参数：显示的位置（x,y），ch[]要显示的字符串
//返回：无
//==============================================================  
void Asc6_12(unsigned char x,unsigned char y,unsigned char ch[])
{
  unsigned char x1,c=0, i=0,j=0;      
  while (ch[i]!='\0')
  { 
    x1=x/4;
		c =ch[i]-32;	
    if(x1>61)
	  {
			 x=0;
			 x1=x/4;
			 y=y+12; //换行
		} 																	
    Set_Column_Address(Shift+x1,Shift+x1+1); // 设置列坐标，shift为列偏移量由1322决定 
		Set_Row_Address(y,y+11); 
		Set_Write_RAM();	 //	写显存    
  	
		for(j=0;j<12;j++)
	 	{
			Con_4_byte(ASC6X12[c*12+j]);	//数据转换
		}
	  i++;
	  x=x+8;	 //字间距，8为最小
  }
}
//==============================================================
//功能描述：写入一组标准ASCII字符串	 8x16
//参数：显示的位置（x,y），ch[]要显示的字符串
//返回：无
//==============================================================  
void Asc8_16(unsigned char x,unsigned char y,unsigned char ch[])
{
  unsigned char x1,c=0, i=0,j=0;      
  while (ch[i]!='\0')
  {    
    x1=x/4;
	c =ch[i]-32;
    if(x1>61)
	   {x=0;
	   x1=x/4;
	   y=y+16;}  //换行																	
    Set_Column_Address(Shift+x1,Shift+x1+1); // 设置列坐标，shift为列偏移量由1322决定 
	Set_Row_Address(y,y+15); 
	Set_Write_RAM();	 //	写显存    
  	
		for(j=0;j<16;j++)
	 		  {
				 Con_4_byte(ASC8X16[c*16+j]);	//数据转换
			   }
	 i++;
	 x=x+8;	 //字间距，8为最小
  }
}
 
//==============================================================
//功能描述：写入一组标准ASCII字符串	 12x24
//参数：显示的位置（x,y），ch[]要显示的字符串
//返回：无
//==============================================================  
void Asc12_24(unsigned char x,unsigned char y,unsigned char ch[])
{
  unsigned char x1,c=0, i=0,j=0;      
  while (ch[i]!='\0')
  {    
    x1=x/4;
	c =ch[i]-32;
    if(x1>59)
	   {x=0;
	   x1=x/4;
	   y=y+24;}  //换行																	
    Set_Column_Address(Shift+x1,Shift+x1+3); // 设置列坐标，shift为列偏移量由1322决定 
	Set_Row_Address(y,y+23); 
	Set_Write_RAM();	 //	写显存    
  	
		for(j=0;j<48;j++)
	 		  {
				 Con_4_byte(ASC12X24[c*48+j]);	//数据转换
			   }
	 i++;
	 x=x+12;//字间距，12为最小	
  }
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Show Gray Scale Bar (Full Screen)
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Grayscale()
{
	// Level 16 => Column 1~16
		Fill_Block(0xFF,0x00,0x03,0x00,Max_Row);
   	// Level 15 => Column 17~32
		Fill_Block(0xEE,0x04,0x07,0x00,Max_Row);
  	// Level 14 => Column 33~48
		Fill_Block(0xDD,0x08,0x0B,0x00,Max_Row);
 	// Level 13 => Column 49~64
		Fill_Block(0xCC,0x0C,0x0F,0x00,Max_Row);
  	// Level 12 => Column 65~80
		Fill_Block(0xBB,0x10,0x13,0x00,Max_Row);
 	// Level 11 => Column 81~96
		Fill_Block(0xAA,0x14,0x17,0x00,Max_Row);
 	// Level 10 => Column 97~112
		Fill_Block(0x99,0x18,0x1B,0x00,Max_Row);
 	// Level 9 => Column 113~128
		Fill_Block(0x88,0x1C,0x1F,0x00,Max_Row);
 	// Level 8 => Column 129~144
		Fill_Block(0x77,0x20,0x23,0x00,Max_Row);
  	// Level 7 => Column 145~160
		Fill_Block(0x66,0x24,0x27,0x00,Max_Row);
  	// Level 6 => Column 161~176
		Fill_Block(0x55,0x28,0x2B,0x00,Max_Row);
 	// Level 5 => Column 177~192
		Fill_Block(0x44,0x2C,0x2F,0x00,Max_Row);
  	// Level 4 => Column 193~208
		Fill_Block(0x33,0x30,0x33,0x00,Max_Row);
  	// Level 3 => Column 209~224
		Fill_Block(0x22,0x34,0x37,0x00,Max_Row);
   	// Level 2 => Column 225~240
		Fill_Block(0x11,0x38,0x3B,0x00,Max_Row);
		// Level 1 => Column 241~256
		Fill_Block(0x00,0x3C,Max_Column,0x00,Max_Row);
}

void clearArea(unsigned char x,unsigned char y,unsigned char width,unsigned char height)
{
		Fill_Block(0,x/4,(x+width-1)/4,y,y+height-1);
}

void Show_num(unsigned char x,unsigned char y,int num,unsigned char font,unsigned char isReverse)
{
		int x1,data,c,j;
		switch (font)
		{
				case 0://FONT_5X8
				{
						x1 = x / 4;
						c = num + 16;
						if (x1 > 61) 
						{
								x = 0;
								x1 = x / 4;
								y = y + 8;
						}  //换行
						Set_Column_Address(0x1C + x1, 0x1C + x1 + 1); // 设置列坐标，shift为列偏移量由1322决定
						Set_Row_Address(y, y + 7);
						Set_Write_RAM();	 //	写显存

						for (j = 0; j < 8; j++) 
						{
								data = ASC5X8[c * 8 + j];
								Con_4_byte(isReverse ? ~data : data);	//数据转换
						}
						break;
				}
				case 1://FONT_6X12
				{
						x1 = x / 4;
						c = num + 16;
						if (x1 > 61) 
						{
							  x = 0;
							  x1 = x / 4;
							  y = y + 12;
						}  //换行
						Set_Column_Address(0x1C + x1, 0x1C + x1 + 1); // 设置列坐标，shift为列偏移量由1322决定
						Set_Row_Address(y, y + 11);
						Set_Write_RAM();	 //	写显存

						for (j = 0; j < 12; j++) 
						{
								data = ASC6X12[c * 12 + j];
								Con_4_byte(isReverse ? ~data : data);	//数据转换
						}
						break;
				}
				case 2://FONT_8X16
				{
						x1 = x / 4;
						c = num + 16;
						if (x1 > 61) 
						{
								x = 0;
								x1 = x / 4;
								y = y + 16;
						}  //换行
						Set_Column_Address(0x1C + x1, 0x1C + x1 + 1); // 设置列坐标，shift为列偏移量由1322决定
						Set_Row_Address(y, y + 15);
						Set_Write_RAM();	 //	写显存

						for (j = 0; j < 16; j++) 
						{
								data = ASC8X16[c * 16 + j];
								Con_4_byte(isReverse ? ~data : data);	//数据转换
						}
						break;
				}
				case 3://FONT_12X24
				{
						x1 = x / 4;
						c = num + 16;
						if (x1 > 59)
						{
								x = 0;
								x1 = x / 4;
								y = y + 24;
						}  //换行
						Set_Column_Address(0x1C + x1, 0x1C + x1 + 3); // 设置列坐标，shift为列偏移量由1322决定
						Set_Row_Address(y, y + 23);
						Set_Write_RAM();	 //	写显存

						for (j = 0; j < 48; j++) 
						{
								data = ASC12X24[c * 48 + j];
								Con_4_byte(isReverse ? ~data : data);	//数据转换
						}
						break;
				}
				default:
						break;
		}

}

//void SetPassport(void)
//{
//    unsigned char index = 0;
//    unsigned char Center_Flag = 0;
//    unsigned char code_array[8] = {'0','0','0','0','0','0','0','0'};
//		unsigned char temp = 0;

//		Fill_RAM(0x00);//清屏
//    Asc6_12(53,8,"Pin Code:");
//		temp = code_array[0];
//		Asc6_12(125,8,&temp);
//    Asc6_12(133,8,"_");
//    Asc6_12(141,8,"_");
//    Asc6_12(149,8,"_");
//    Asc6_12(157,8,"_");
//    Asc6_12(165,8,"_");
//    Asc6_12(173,8,"_");
//    Asc6_12(181,8,"_");
//    Asc6_12(30,40,"+");
//    Asc6_12(120,40,"OK");
//    Asc6_12(226,40,"-");

//		Key_Control(1);//清空按键标志位，开启按键有效
//    while(1)
//    {
//        if(Key_Flag.Key_left_Flag)//左键按下
//        {
//						Key_Flag.Key_left_Flag = 0;
//						clearArea(125+8*index,8,6,12);//清空这个位置的显示
//            if(Center_Flag != 0)
//                Center_Flag = 0;
//            if(code_array[index] == '0')
//            {
//								Asc6_12(125+8*index,8,"9");
//                code_array[index] = '9';
//            }
//            else if(code_array[index] == '?')
//            {
//								Asc6_12(125+8*index,8,"0");
//                code_array[index] = '0';
//            }
//            else
//            {
//                code_array[index]--;
//								temp = code_array[index];
//								Asc6_12(125+8*index,8,&temp);
//            }
//        }
//        if(Key_Flag.Key_right_Flag)//右键按下
//        {
//						Key_Flag.Key_right_Flag = 0;
//						clearArea(125+8*index,8,6,12);//清空这个位置的显示
//            if(Center_Flag != 0)
//                Center_Flag = 0;
//            if(code_array[index] == '9')
//            {
//								Asc6_12(125+8*index,8,"0");
//                code_array[index] = '0';
//            }
//            else if(code_array[index] == '?')
//            {
//								Asc6_12(125+8*index,8,"0");
//                code_array[index] = '0';
//            }
//            else
//            {
//                code_array[index]++;
//								temp = code_array[index];
//								Asc6_12(125+8*index,8,&temp);
//            }
//        }
//        if(Key_Flag.Key_center_Flag)//中间键按下
//        {
//						Key_Flag.Key_center_Flag = 0;
//            Center_Flag++;
//            index++;
//            if(index < 4)
//            {
//                Center_Flag = 0;
//								clearArea(125+8*index,8,6,12);//清空这个位置的显示
//								Asc6_12(125+8*index,8,"0");
//            }
//            else
//            {
//								clearArea(125+8*index,8,6,12);//清空这个位置的显示
//								Asc6_12(125+8*index,8,"?");
//            }
//        }
//        if((index>=4)&&(Center_Flag >= 2))
//        {
//            //将密码保存起来,

//					
//					
//					
//					
//						Fill_RAM(0x00);//清除所有显示
//						
//					
//					
//            break;
//        }
//    }
//		Key_Control(0);//清空按键标志位，开启按键无效
//}

//u8 VerifyPassport(void)
//{
//    unsigned char index = 0;
//    unsigned char Center_Flag = 0;
//    unsigned char code_old[8] = {'0','0','0','0','0','0','0','0'};//从存储的地方读出来
//    unsigned char code_array[8] = {'0','0','0','0','0','0','0','0'};
//		unsigned char temp = 0;
//		
//		Fill_RAM(0x00);//清屏
//    Asc6_12(53,8,"Pin Code:");
//		temp = code_array[0];
//		Asc6_12(125,8,&temp);
//    Asc6_12(133,8,"_");
//    Asc6_12(141,8,"_");
//    Asc6_12(149,8,"_");
//    Asc6_12(157,8,"_");
//    Asc6_12(165,8,"_");
//    Asc6_12(173,8,"_");
//    Asc6_12(181,8,"_");
//    Asc6_12(30,40,"+");
//    Asc6_12(120,40,"OK");
//    Asc6_12(226,40,"-");
//		
//		//读取出老的密码
//		
//		
//		
//		
//		
//		
//		
//		Key_Control(1);//清空按键标志位，开启按键有效
//    while(1)
//    {
//        if(Key_Flag.Key_left_Flag)//左键按下
//        {
//						Key_Flag.Key_left_Flag = 0;
//						clearArea(125+8*index,8,6,12);//清空这个位置的显示
//            if(Center_Flag != 0)
//                Center_Flag = 0;
//            if(code_array[index] == '0')
//            {
//								Asc6_12(125+8*index,8,"9");
//                code_array[index] = '9';
//            }
//            else if(code_array[index] == '?')
//            {
//								Asc6_12(125+8*index,8,"0");
//                code_array[index] = '0';
//            }
//            else
//            {
//                code_array[index]--;
//								temp = code_array[index];
//								Asc6_12(125+8*index,8,&temp);
//            }
//        }
//        if(Key_Flag.Key_right_Flag)//右键按下
//        {
//						Key_Flag.Key_right_Flag = 0;
//						clearArea(125+8*index,8,6,12);//清空这个位置的显示
//            if(Center_Flag != 0)
//                Center_Flag = 0;
//            if(code_array[index] == '9')
//            {
//								Asc6_12(125+8*index,8,"0");
//                code_array[index] = '0';
//            }
//            else if(code_array[index] == '?')
//            {
//								Asc6_12(125+8*index,8,"0");
//                code_array[index] = '0';
//            }
//            else
//            {
//                code_array[index]++;
//								temp = code_array[index];
//								Asc6_12(125+8*index,8,&temp);
//            }
//        }
//        if(Key_Flag.Key_center_Flag)//中间建按下
//        {
//						Key_Flag.Key_center_Flag = 0;
//						clearArea(125+8*index,8,6,12);//清空这个位置的显示
//						Asc6_12(125+8*index,8,"*");
//            Center_Flag++;
//            index++;
//            if(index < 4)
//            {
//                Center_Flag = 0;
//								clearArea(125+8*index,8,6,12);//清空这个位置的显示
//								Asc6_12(125+8*index,8,"0");
//            }
//            else
//            {
//								clearArea(125+8*index,8,6,12);//清空这个位置的显示
//								Asc6_12(125+8*index,8,"?");
//            }
//        }
//        if((index>=4)&&(Center_Flag >= 2))
//        {
//						Fill_RAM(0x00);//清除所有显示
//					
//					

//            if(compareCharArray(code_old,code_array,index) == 1)//密码正确
//            {    
//								Key_Control(0);//清空按键标志位，开启按键无效	
//								return 1;							
//						}
//            else//密码错误
//						{
//								Key_Control(0);//清空按键标志位，开启按键无效								
//                return 0;
//						}
//        }
//    }	
//}

/***************************************************************
		以下的密码设置和验证函数都是固定长度为6位的密码
		上面注释掉的是4-8位的密码相关函数
***************************************************************/
u8 SetPassport(void)
{
    unsigned char index = 0;
    unsigned char code_array[8] = {'5','5','5','5','5','5','0','0'};
		unsigned char temp[2] = {'\0','\0'};

		Fill_RAM(0x00);//清屏
		if(BootFlag.language == 0)
		{
				//第一行显示
				Show_HZ12_12(96,7,0,21,22);//设置
				Show_HZ12_12(128,7,0,25,26);//密码
		}
		else
		{
				Asc8_16(100,7,"Set PIN");
		}
		Show_Pattern(&gImage_emptypin[0],15,17,26,38);
		Show_Pattern(&gImage_emptypin[0],21,23,26,38);
		Show_Pattern(&gImage_emptypin[0],27,29,26,38);
		Show_Pattern(&gImage_emptypin[0],33,35,26,38);
		Show_Pattern(&gImage_emptypin[0],39,41,26,38);
		Show_Pattern(&gImage_emptypin[0],45,47,26,38);
		Asc8_16(58,45,"<");
		Asc8_16(197,45,">");
		Asc8_16(124,45,"5");
		
		Key_Control(1);//清空按键标志位，开启按键有效
		while(index < 6)
    {
        if(Key_Flag.Key_left_Flag)//左键按下
        {
						Key_Flag.Key_left_Flag = 0;
            if(code_array[index] == '9')
            {
								Asc8_16(124,45,"0");
                code_array[index] = '0';
            }
            else
            {
                code_array[index]++;
								temp[0] = code_array[index];
								Asc8_16(124,45,temp);
            }
        }
        if(Key_Flag.Key_right_Flag)//右键按下
        {
						Key_Flag.Key_right_Flag = 0;
            if(code_array[index] == '0')
            {
								Asc8_16(124,45,"9");
                code_array[index] = '9';
            }
            else
            {
                code_array[index]--;
								temp[0] = code_array[index];
								Asc8_16(124,45,temp);
            }
        }
        if(Key_Flag.Key_center_Flag)//中间键按下
        {
						Key_Flag.Key_center_Flag = 0;
						Show_Pattern(&gImage_fullpin[0],15+6*index,17+6*index,26,38);
						clearArea(58+index*24,38,16,1);
            index++;
            if(index == 6)
            {		
								if(WritePinCode(code_array) == 0)//将密码保存起来
								{
										Fill_RAM(0x00);				//清屏
										Asc8_16(80,26,"Set Pin ERROR!!!");
										HAL_Delay(3000);			
										return 0;							//密码保存出错
								}
							
								Asc8_16(180,8,"OK");
								HAL_Delay(1000);
								Fill_RAM(0x00);//清除所有显示							
								break;							
            }
            else
            {
								Asc8_16(124,45,"5");
            }
        }
    }
		Key_Control(0);//清空按键标志位，开启按键无效
		return 1;
}

u8 VerifyPassport(void)
{
    unsigned char index = 0;
    unsigned char code_old[8] = {'5','5','5','5','5','5','0','0'};//从存储的地方读出来
    unsigned char code_array[6] = {'5','5','5','5','5','5'};
		unsigned char temp[2] = {'\0','\0'};
		static unsigned char Pin_error = 0;
		
		Fill_RAM(0x00);//清屏
		if(ReadPinCode(code_old) == 0)//读取出老的密码
		{
				Asc8_16(77,26,"ATSHA204 ERROR!!!");
				HAL_Delay(3000);
				Fill_RAM(0x00);//清屏
				return 0;
		}
		
		Fill_RAM(0x00);//清屏
		if(BootFlag.language == 0)
		{
				//第一行显示
				Show_HZ12_12(96,7,0,25,26);//密码
				Show_HZ12_12(128,7,0,55,56);//确认
		}
		else
		{
				Asc8_16(84,7,"Confirm PIN");
		}
		Show_Pattern(&gImage_emptypin[0],15,17,26,38);
		Show_Pattern(&gImage_emptypin[0],21,23,26,38);
		Show_Pattern(&gImage_emptypin[0],27,29,26,38);
		Show_Pattern(&gImage_emptypin[0],33,35,26,38);
		Show_Pattern(&gImage_emptypin[0],39,41,26,38);
		Show_Pattern(&gImage_emptypin[0],45,47,26,38);
		Asc8_16(58,45,"<");
		Asc8_16(197,45,">");
		Asc8_16(124,45,"5");		
				
		Key_Control(1);//清空按键标志位，开启按键有效
    while(1)
    {
        if(Key_Flag.Key_left_Flag)//左键按下
        {
						Key_Flag.Key_left_Flag = 0;
            if(code_array[index] == '9')
            {
								Asc8_16(124,45,"0");
                code_array[index] = '0';
            }
            else
            {
                code_array[index]++;
								temp[0] = code_array[index];
								Asc8_16(124,45,temp);
            }
        }
        if(Key_Flag.Key_right_Flag)//右键按下
        {
						Key_Flag.Key_right_Flag = 0;
            if(code_array[index] == '0')
            {
								Asc8_16(124,45,"9");
                code_array[index] = '9';
            }
            else
            {
                code_array[index]--;
								temp[0] = code_array[index];
								Asc8_16(124,45,temp);
            }
        }
        if(Key_Flag.Key_center_Flag)//中间建按下
        {
						Key_Flag.Key_center_Flag = 0;
						Show_Pattern(&gImage_fullpin[0],15+6*index,17+6*index,26,38);
						clearArea(58+index*24,38,16,1);
            index++;
            if(index == 6)
            {
								if(compareCharArray(code_old,code_array,index) == 1)//密码正确
								{    
										Key_Control(0);//清空按键标志位，开启按键无效
										Fill_RAM(0x00);//清除所有显示
										return 1;							
								}
								else//密码错误
								{
										Pin_error++;
										Fill_RAM(0x00);//清除所有显示
										if(BootFlag.language == 0)
										{
												//第一行显示
												Show_HZ12_12(84,7,0,25,26);//密码
												Show_HZ12_12(116,7,0,37,38);//错误
												temp[0] = Pin_error + 0x30;
												Asc8_16(148,7,temp);
												Show_HZ12_12(160,7,0,39,39);//次
												//第二行显示
												Show_HZ12_12(60,26,0,37,38);//错误
												Asc8_16(92,26,"5");
												Show_HZ12_12(104,26,0,39,39);//次
												Show_HZ12_12(120,26,0,33,33);//将
												Show_HZ12_12(136,26,0,36,36);//重
												Show_HZ12_12(152,26,0,22,22);//置
												Show_HZ12_12(168,26,0,17,18);//钱包
										}
										else
										{
												Asc8_16(52,7,"Incorrect PIN (");
												temp[0] = Pin_error + 0x30;
												Asc8_16(172,7,temp);
												Asc8_16(180,7,"/5)");
												temp[0] = 5 - Pin_error + 0x30;
												Asc8_16(68,26,temp);
												Asc8_16(76,26," try(ies) left");
										}
										Show_Pattern(&gImage_triangle_down[0],30,32,45,57);
										clearArea(120,57,12,1);
										while(Key_Flag.Key_center_Flag == 0);
										Key_Flag.Key_center_Flag = 0;
										Fill_RAM(0x00);//清除所有显示
										return 0;
								}
            }
            else
            {
								Asc8_16(124,45,"5");
            }
        }
    }	
}

u8 compareCharArray(unsigned char *left,unsigned char *right,int len)
{
		int i;
		for(i=0;i<len;i++)
		{
				if(left[i] != right[i])
						return 0;
		}
		return 1;
}

u8 NEO_Test(void)
{
		int i=0;
		int value = 0;
		Fill_RAM(0x00);
		Asc8_16( 52,4 ,"Enter NeoDun Test ?");
		Asc8_16(24,44,"Cancel");
		Asc8_16(124,44,"OK");
		Asc8_16(184,44,"Cancel");
	
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
						
						while(1) 
						{
								if(Key_Flag.Key_left_Flag)
								{
										Key_Flag.Key_left_Flag = 0;
										HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
								}
								if(Key_Flag.Key_right_Flag)
								{
										Key_Flag.Key_right_Flag = 0;
										HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);						
								}
								if(Key_Flag.Key_center_Flag)//中间按键按下，退出
								{
										Key_Flag.Key_center_Flag = 0;
										break;
								}
						}
						touch_motor_flag = 1;
						value = 1;						
						break;
				}
				else if((Key_Flag.Key_right_Flag == 1)||(Key_Flag.Key_left_Flag == 1))
				{							
						value = 0;
						break;
				}
		}
		Fill_RAM(0x00);
		return value;
}

