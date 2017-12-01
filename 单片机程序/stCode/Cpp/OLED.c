/*OLED底层驱动
*@采用列行式，分辨率为128*64
*存放格式如下.
*[0]0 1 2 3 ... 127	
*[1]0 1 2 3 ... 127	
*[2]0 1 2 3 ... 127	
*[3]0 1 2 3 ... 127	
*[4]0 1 2 3 ... 127	
*[5]0 1 2 3 ... 127	
*[6]0 1 2 3 ... 127	
*[7]0 1 2 3 ... 127 
*/
//#include "stm32f10x_lib.h"
#include "stdlib.h"
#include "OLED.h"



#define OLED_XLEVEL_L		0x00
#define OLED_XLEVEL_H		0x10
#define XLevel		((OLED_XLEVEL_H&0x0F)*16+OLED_XLEVEL_L)
#define OLED_MAX_COLUMN	128
#define OLED_MAX_ROW		  64
#define	OLED_BRIGHTNESS	0xCF 


#define X_WIDTH 128
#define Y_WIDTH 64


// "sp!"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[55]^_'abcdefghijklmnopqrstuvwxyz"



void LCD_WrDat(u8 data)
{
	u8 i=8;
	//LCD_CS=0;;
	LCD_DC_SET();
  LCD_SCL_CLR();
  //asm("nop");    
  while(i--)
  {
    if(data&0x80){LCD_SDA_SET();}
    else{LCD_SDA_CLR();}
    LCD_SCL_SET(); 
    //asm("nop");;
		//asm("nop");            
    LCD_SCL_CLR();    
    data<<=1;    
  }
	//LCD_CS=1;
}
void LCD_WrCmd(u8 cmd)
{
	u8 i=8;
	
	//LCD_CS=0;;
	LCD_DC_CLR();
  LCD_SCL_CLR();
  //asm("nop");   
  while(i--)
  {
    if(cmd&0x80){LCD_SDA_SET();}
    else{LCD_SDA_CLR();}
    LCD_SCL_SET();
    //asm("nop");;
		//asm("nop");             
    LCD_SCL_CLR();    
    cmd<<=1;;   
  } 	
	//LCD_CS=1;
}

/*设置起始地址
*Y轴是按8格递进的，y轴0~63，只能按8格的倍数显示，
*因为列行式只能按8个字节进行
*/
void LCD_Set_Pos(u8 x, u8 y)
{ 
  LCD_WrCmd(0xb0+(y>>3));
  LCD_WrCmd(((x&0xf0)>>4)|0x10);
  LCD_WrCmd((x&0x0f)|0x01); 
} 
void LCD_Fill(u8 bmp_data)
{
	u8 y,x;
	
	for(y=0;y<8;y++)
	{
		LCD_WrCmd(0xb0+y);
		LCD_WrCmd(0x01);
		LCD_WrCmd(0x10);
		for(x=0;x<X_WIDTH;x++)
			LCD_WrDat(bmp_data);
	}
}
void LCD_CLS(void)
{
	u8 y,x;	
	for(y=0;y<8;y++)
	{
		LCD_WrCmd(0xb0+y);
		LCD_WrCmd(0x01);
		LCD_WrCmd(0x10); 
		for(x=0;x<X_WIDTH;x++)
			LCD_WrDat(0);
	}
}
//清除一行0-63
void LCD_CLS_y(char y)
{
	u8 x;	
	
	LCD_WrCmd(0xb0+(y>>3));
	LCD_WrCmd(0x01);
	LCD_WrCmd(0x10); 
	for(x=0;x<X_WIDTH;x++)
	{
		LCD_WrDat(0);
	}
	
}

//清除一行上的一块区域y=0-63
void LCD_CLS_line_area(u8 start_x,u8 start_y,u8 width)
{
	u8 x;	
	
	LCD_WrCmd(0xb0+(start_y>>3));
	LCD_WrCmd(0x01);
	LCD_WrCmd(0x10); 
	for(x=start_x;x<width;x++)
	{
		LCD_WrDat(0);
	}
	
}


void LCD_DLY_ms(u16 ms)
{                         
    HAL_Delay(ms);
}

void OLED_Init(void)
{
  //DDRA=0XFF;
  
	LCD_SCL_SET();
	//LCD_CS=1;	//预制SLK和SS为高电平   	
	
	LCD_RST_CLR();
	LCD_DLY_ms(50);
	LCD_RST_SET();

  LCD_WrCmd(0xae);//--turn off oled panel
  LCD_WrCmd(0x00);//---set low column address
  LCD_WrCmd(0x10);//---set high column address
  LCD_WrCmd(0x40);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
  LCD_WrCmd(0x81);//--set contrast control register
  LCD_WrCmd(0xcf); // Set SEG Output Current Brightness
  LCD_WrCmd(0xa1);//--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
  LCD_WrCmd(0xc8);//Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
  LCD_WrCmd(0xa6);//--set normal display
  LCD_WrCmd(0xa8);//--set multiplex ratio(1 to 64)
  LCD_WrCmd(0x3f);//--1/64 duty
  LCD_WrCmd(0xd3);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
  LCD_WrCmd(0x00);//-not offset
  LCD_WrCmd(0xd5);//--set display clock divide ratio/oscillator frequency
  LCD_WrCmd(0x80);//--set divide ratio, Set Clock as 100 Frames/Sec
  LCD_WrCmd(0xd9);//--set pre-charge period
  LCD_WrCmd(0xf1);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
  LCD_WrCmd(0xda);//--set com pins hardware configuration
  LCD_WrCmd(0x12);
  LCD_WrCmd(0xdb);//--set vcomh
  LCD_WrCmd(0x40);//Set VCOM Deselect Level
  LCD_WrCmd(0x20);//-Set Page Addressing Mode (0x00/0x01/0x02)
  LCD_WrCmd(0x02);//
  LCD_WrCmd(0x8d);//--set Charge Pump enable/disable
  LCD_WrCmd(0x14);//--set(0x10) disable
  LCD_WrCmd(0xa4);// Disable Entire Display On (0xa4/0xa5)
  LCD_WrCmd(0xa6);// Disable Inverse Display On (0xa6/a7) 
  LCD_WrCmd(0xaf);//--turn on oled panel
  LCD_Fill(0x00);  //初始清屏
  LCD_Set_Pos(0,0);  
	
} 
//==============================================================
//函数名： void LCD_PutPixel(u8 x,u8 y)
//功能描述：绘制一个点（x,y）
//参数：真实坐标值(x,y),x的范围0～127，y的范围0～64
//返回：无
//==============================================================
void LCD_PutPixel(u8 x,u8 y)
{
	u8 data1;  //data1当前点的数据 
	 
    //LCD_Set_Pos(x,y); 
	data1 = 0x01<<(y%8); 	
	LCD_WrCmd(0xb0+(y>>3));
	LCD_WrCmd(((x&0xf0)>>4)|0x10);
	LCD_WrCmd((x&0x0f)|0x00);
	LCD_WrDat(data1); 	 	
}
//==============================================================
//函数名： void LCD_Put_Column(u8 x,u8 y,u8 data)
//功能描述：操作一列显示，一列全显示0XFF,清除一列0X00;
//参数：真实坐标值(x,y),x的范围0～127，y的范围0～64
//返回：无
//==============================================================
void LCD_Put_Column(u8 x,u8 y,u8 data)
{	
	LCD_WrCmd(0xb0+(y>>3));
	LCD_WrCmd(((x&0xf0)>>4)|0x10);
	LCD_WrCmd((x&0x0f)|0x00);
	LCD_WrDat(data); 	 	
}


//==============================================================
//函数名： void LCD_Rectangle(u8 x1,u8 y1,
//                   u8 x2,u8 y2,u8 color,u8 gif)
//功能描述：绘制一个实心矩形
//参数：左上角坐标（x1,y1）,右下角坐标（x2，y2）
//      其中x1、x2的范围0～127，y1，y2的范围0～63，即真实坐标值
//返回：无
//==============================================================
void LCD_Rectangle(u8 x1,u8 y1,u8 x2,u8 y2,u8 gif)
{
	u8 n; 
		
	LCD_Set_Pos(x1,y1>>3);
	for(n=x1;n<=x2;n++)
	{
		LCD_WrDat(0x01<<(y1%8)); 			
		if(gif == 1) 	LCD_DLY_ms(50);
	}  
	LCD_Set_Pos(x1,y2>>3);
  for(n=x1;n<=x2;n++)
	{
		LCD_WrDat(0x01<<(y2%8)); 			
		if(gif == 1) 	LCD_DLY_ms(5);
	}
	
}  
//==============================================================
//函数名：LCD_P6x8Str(u8 x,u8 y,u8 *p)
//功能描述：写入一组标准ASCII字符串
//参数：显示的位置（x,y），y为页范围0～7，要显示的字符串
//返回：无
//==============================================================  
void LCD_P6x8Str(u8 x,u8 y,u8 *ch,const u8 *F6x8)
{
	u8 c=0,i=0,j=0;

	while (*(ch+j)!='\0')
	{    
		c =*(ch+j)-32;
		if(x>126)
		{
			x=0;
			y++;
		}
		LCD_Set_Pos(x,y);    
		for(i=0;i<6;i++) 
		{
			LCD_WrDat(*(F6x8+c*6+i));
		}
		x+=6;
		j++;
	}
}
//==============================================================
//函数名：LCD_P8x16Str(u8 x,u8 y,u8 *p)
//功能描述：写入一组标准ASCII字符串
//参数：显示的位置（x,y），y为页范围0～63，要显示的字符串
//返回：无
//==============================================================  
void LCD_P8x16Str(u8 x,u8 y,u8 *ch,const u8 *F8x16)
{
  u8 c=0,i=0,j=0;
        
  while (*(ch+j)!='\0')
  {    
    c =*(ch+j)-32;
    if(x>120)
	{	
		x=0;
		y++;
	}
    LCD_Set_Pos(x,y); 
  	for(i=0;i<8;i++) 
  	{
  	  	LCD_WrDat(*(F8x16+c*16+i));
  	}
  	LCD_Set_Pos(x,y+8);    
  	for(i=0;i<8;i++)  
  	{
  	  	LCD_WrDat(*(F8x16+c*16+i+8));  
  	}
  	x+=8;
  	j++;
  }
}
//输出汉字字符串
void LCD_P14x16Str(u8 x,u8 y,u8 ch[],const u8 *F14x16_Idx,const u8 *F14x16)
{
	u8 wm=0,ii = 0;
	u16 adder=1; 
	
	while(ch[ii] != '\0')
	{
	  	wm = 0;
	  	adder = 1;
	  	while(*(F14x16_Idx+wm) > 127)
	  	{
	  		if(*(F14x16_Idx+wm) == ch[ii])
	  		{
	  			if(*(F14x16_Idx+wm+1) == ch[ii + 1])
	  			{
	  				adder = wm * 14;
	  				break;
	  			}
	  		}
	  		wm += 2;			
	  	}
	  	if(x>118)
		{
			x=0;
			y++;
		}
	  	LCD_Set_Pos(x , y); 
	  	if(adder != 1)// 显示汉字					
	  	{
	  		LCD_Set_Pos(x , y);
	  		for(wm = 0;wm < 14;wm++)               
	  		{
	  			LCD_WrDat(*(F14x16+adder));	
	  			adder += 1;
	  		}      
	  		LCD_Set_Pos(x,y + 1); 
	  		for(wm = 0;wm < 14;wm++)          
	  		{
	  			LCD_WrDat(*(F14x16+adder));
	  			adder += 1;
	  		}   		
	  	}
	  	else			  //显示空白字符			
	  	{
	  		ii += 1;
	      	LCD_Set_Pos(x,y);
	  		for(wm = 0;wm < 16;wm++)
	  		{
	  			LCD_WrDat(0);
	  		}
	  		LCD_Set_Pos(x,y + 1);
	  		for(wm = 0;wm < 16;wm++)
	  		{   		
	  			LCD_WrDat(0);	
	  		}
	  	}
	  	x += 14;
	  	ii += 2;
	}
}
//输出汉字字符串
void LCD_P16x16Str(u8 x,u8 y,u8 *ch,const u8 *F16x16_Idx,const u8 *F16x16)
{
	u8 wm=0,ii = 0;
	u16 adder=1; 
	
	while(*(ch+ii) != '\0')
	{
  	wm = 0;
  	adder = 1;
  	while(*(F16x16_Idx+wm) > 127)
  	{
  		if(*(F16x16_Idx+wm) == *(ch+ii))
  		{
  			if(*(F16x16_Idx+wm + 1) == *(ch+ii + 1))
  			{
  				adder = wm * 16;
  				break;
  			}
  		}
  		wm += 2;			
  	}
  	if(x>118){x=0;y++;}
  	LCD_Set_Pos(x , y); 
  	if(adder != 1)// 显示汉字					
  	{
  		LCD_Set_Pos(x , y);
  		for(wm = 0;wm < 16;wm++)               
  		{
  			LCD_WrDat(*(F16x16+adder));	
  			adder += 1;
  		}      
  		LCD_Set_Pos(x,y + 8); 
  		for(wm = 0;wm < 16;wm++)          
  		{
  			LCD_WrDat(*(F16x16+adder));
  			adder += 1;
  		}   		
  	}
  	else			  //显示空白字符			
  	{
  		ii += 1;
      	LCD_Set_Pos(x,y);
  		for(wm = 0;wm < 16;wm++)
  		{
  			LCD_WrDat(0);
  		}
  		LCD_Set_Pos(x,y + 1);
  		for(wm = 0;wm < 16;wm++)
  		{   		
  			LCD_WrDat(0);	
  		}
  	}
  	x += 16;
  	ii += 2;
	}
}



//==============================================================
//函数名： void Draw_BMP(u8 x,u8 y)
//功能描述：显示BMP图片128×64
//参数：起始点坐标(x,y),x的范围0～127，y为页的范围0～63
//返回：无
//==============================================================
void Draw_BMP(u8 x,u8 y,const u8 *bmp)
{ 	
	u8 wm=0;
	u8 adder=0;
  
  	LCD_Set_Pos(x , y);
	for(wm = 0;wm < 16;wm++)               
	{
		LCD_WrDat(*(bmp+adder));	
		adder += 1;
	}      
	LCD_Set_Pos(x,y + 8); 
	for(wm = 0;wm < 16;wm++)          
	{
		LCD_WrDat(*(bmp+adder));
		adder += 1;
	} 
 /* u32 ii=0;
  u8 x,y;
  
  if(y1%8==0) y=y1/8;      
  else y=y1/8+1;
	for(y=y0;y<=y1;y++)
	{
		LCD_Set_Pos(x0,y);				
    for(x=x0;x<x1;x++)
	    {      
	    	LCD_WrDat(bmp[ii++]);	    	
	    }
	}	*/
}


