/*
 * DisplayMem.cpp
 *
 *  Created on: 2017年7月9日
 *      Author: Administrator
 */

#include "DisplayMem.h"
#include <string.h>
#include "OLED281/oled281.h"
//声明ASCII库
extern unsigned char ASC5X8[];
extern unsigned char ASC6X12[];
extern unsigned char ASC8X16[];
extern unsigned char ASC12X24[];
extern unsigned char ASC20X40[];
extern unsigned char  HZ12X12_S[];
extern unsigned char  HZ16X16_S[];
extern unsigned char  HZ24X24_S[];

namespace view {

DisplayMem::DisplayMem() {

}

DisplayMem::DisplayMem(const DisplayMem& copy) {
}

void DisplayMem::clearAll() {
		Fill_RAM(0x00);
}


void DisplayMem::drawRectangel(int x, int y, int width, int height) {
	Draw_Rectangle(0xff,1,x,x+width-1,y,y+height-1);
}

DisplayMem& DisplayMem::getInstance() {
	static DisplayMem mem;
	return mem;
}

void DisplayMem::drawChar(int x, int y, char ch, FONT_t font, bool isReverse) {
	switch (font) {
	case FONT_5X8: {
		int x1;
		x1 = x / 4;
		int c = ch - 32;
		if (x1 > 61) {
			x = 0;
			x1 = x / 4;
			y = y + 8;
		}  //换行
		Set_Column_Address(OLED_SHIFT + x1, OLED_SHIFT + x1 + 1); // 设置列坐标，shift为列偏移量由1322决定
		Set_Row_Address(y, y + 7);
		Set_Write_RAM();	 //	写显存

		for (int j = 0; j < 8; j++) {
			int data = ASC5X8[c * 8 + j];
			Con_4_byte(isReverse ? ~data : data);	//数据转换
		}
		break;
	}
	case FONT_6X12: {
		int x1 = x / 4;
		int c = ch - 32;
		if (x1 > 61) {
			x = 0;
			x1 = x / 4;
			y = y + 12;
		}  //换行
		Set_Column_Address(OLED_SHIFT + x1, OLED_SHIFT + x1 + 1); // 设置列坐标，shift为列偏移量由1322决定
		Set_Row_Address(y, y + 11);
		Set_Write_RAM();	 //	写显存

		for (int j = 0; j < 12; j++) {
			int data = ASC6X12[c * 12 + j];
			Con_4_byte(isReverse ? ~data : data);	//数据转换
		}
		break;
	}
	case FONT_8X16: {
		int x1 = x / 4;
		int c = ch - 32;
		if (x1 > 61) {
			x = 0;
			x1 = x / 4;
			y = y + 16;
		}  //换行
		Set_Column_Address(OLED_SHIFT + x1, OLED_SHIFT + x1 + 1); // 设置列坐标，shift为列偏移量由1322决定
		Set_Row_Address(y, y + 15);
		Set_Write_RAM();	 //	写显存

		for (int j = 0; j < 16; j++) {
			int data = ASC8X16[c * 16 + j];
			Con_4_byte(isReverse ? ~data : data);	//数据转换
		}
		break;
	}
	case FONT_12X24: {
		int x1 = x / 4;
		int c = ch - 32;
		if (x1 > 59) {
			x = 0;
			x1 = x / 4;
			y = y + 24;
		}  //换行
		Set_Column_Address(OLED_SHIFT + x1, OLED_SHIFT + x1 + 3); // 设置列坐标，shift为列偏移量由1322决定
		Set_Row_Address(y, y + 23);
		Set_Write_RAM();	 //	写显存

		for (int j = 0; j < 48; j++) {
			int data = ASC12X24[c * 48 + j];
			Con_4_byte(isReverse ? ~data : data);	//数据转换
		}
		break;
	}
	case FONT_20X40: {
		int x1 = x / 4;
		int c = ch - 32;
		/* if(x1>10)
		 {x=0;
		 x1=x/4;
		 }  //只能显示一行		*/
		Set_Column_Address(OLED_SHIFT + x1, OLED_SHIFT + x1 + 5); // 设置列坐标，shift为列偏移量由1322决定
		Set_Row_Address(y, y + 39);
		Set_Write_RAM();	 //	写显存

		for (int j = 0; j < 120; j++) {
			int data = ASC20X40[c * 120 + j];
			Con_4_byte(isReverse ? ~data : data);	//数据转换
		}
		break;
	}

	}
}

void DisplayMem::clearArea(int x, int y, int width, int height) {
	Fill_Block(0,x,(x+width-1)/4,y,y+height-1);
}

void DisplayMem::drawString(int x, int y, char* c) {
	for (int i = 0; c[i] != '\0'; i++) {
		this->drawChar(x, y, c[i], FONT_8X16, false);
		x += 8;
	}
}

void DisplayMem::drawString(int x, int y, char* c, FONT_t font) {
	for (int i = 0; c[i] != '\0'; i++) {
		this->drawChar(x, y, c[i], font, false);
		switch (font) {
		case FONT_5X8:
			x += 5;
			break;
		case FONT_6X12:
			x += 6;
			break;
		case FONT_8X16:
			x += 8;
			break;
		case FONT_12X24:
			x += 12;
			break;
		case FONT_20X40:
			x += 20;
			break;
		}
	}
}

void DisplayMem::drawString(int x, int y, char* c, bool isReverse) {
	for (int i = 0; c[i] != '\0'; i++) {
		this->drawChar(x, y, c[i], FONT_8X16, isReverse);
		x += 8;
	}
}

void DisplayMem::drawString(int x, int y, char* c, FONT_t font,
		bool isReverse) {
	for (int i = 0; c[i] != '\0'; i++) {
		this->drawChar(x, y, c[i], font, isReverse);
		switch (font) {
		case FONT_5X8:
			x += 5;
			break;
		case FONT_6X12:
			x += 6;
			break;
		case FONT_8X16:
			x += 8;
			break;
		case FONT_12X24:
			x += 12;
			break;
		case FONT_20X40:
			x += 20;
			break;
		}
	}

}

void DisplayMem::drawLineSpace( int y, FONT_t font) {
	int w = 0;
	switch (font) {
	case FONT_5X8:
		w += 5;
		break;
	case FONT_6X12:
		w += 6;
		break;
	case FONT_8X16:
		w += 8;
		break;
	case FONT_12X24:
		w += 12;
		break;
	case FONT_20X40:
		w += 20;
		break;
	}
	for(int i = 0; i < 256/w ; i++)
	{
		drawChar(i*w,y,' ',font,false);
	}
}

void DisplayMem::drawHZ(int x, int y, int num, FONT_HZ_t font, bool isReverse)
{
	switch (font) 
	{
		case FONT_12X12: 
		{
			int x1 = x / 4;
			if (x1 > 61) 
			{
				x = 0;
				x1 = x / 4;
				y = y + 12;
			}  //换行
			Set_Column_Address(OLED_SHIFT + x1, OLED_SHIFT + x1 + 3); // 设置列坐标，shift为列偏移量由1322决定
			Set_Row_Address(y, y + 11);
			Set_Write_RAM();	 //	写显存

			for (int j = 0; j < 24; j++) 
			{
				int data = HZ12X12_S[num * 24 + j];
				Con_4_byte(isReverse ? ~data : data);	//数据转换
			}
			break;
		}
		case FONT_16X16: 
		{
			int x1 = x / 4;
			if (x1 > 60) 
			{
				x = 0;
				x1 = x / 4;
				y = y + 16;
			}  //换行
			Set_Column_Address(OLED_SHIFT + x1, OLED_SHIFT + x1 + 3); // 设置列坐标，shift为列偏移量由1322决定
			Set_Row_Address(y, y + 15);
			Set_Write_RAM();	 //	写显存

			for (int j = 0; j < 32; j++) 
			{
				int data = HZ16X16_S[num * 32 + j];
				Con_4_byte(isReverse ? ~data : data);	//数据转换
			}
			break;
		}
		case FONT_24X24: 
		{
			int x1 = x / 4;
			if (x1 > 58) 
			{
				x = 0;
				x1 = x / 4;
				y = y + 24;
			}  //换行
			Set_Column_Address(OLED_SHIFT + x1, OLED_SHIFT + x1 + 5); // 设置列坐标，shift为列偏移量由1322决定
			Set_Row_Address(y, y + 23);
			Set_Write_RAM();	 //	写显存

			for (int j = 0; j < 72; j++) 
			{
				int data = HZ24X24_S[num * 72 + j];
				Con_4_byte(isReverse ? ~data : data);	//数据转换
			}
			break;
		}
	}	
}	

void DisplayMem::drawHZString(int x, int y , int num1, int num2)
{
		for (int i = num1; i < num2+1; i++) 
		{
				this->drawHZ(x, y, i, FONT_12X12, false);
				x += 12;
		}
}

void DisplayMem::drawHZString(int x, int y , int num1, int num2, FONT_HZ_t font)
{
		for (int i = num1; i < num2+1; i++) 
		{
				this->drawHZ(x, y, i, font, false);
				switch (font) 
				{
				case FONT_12X12:
					x += 12;
					break;
				case FONT_16X16:
					x += 16;
					break;
				case FONT_24X24:
					x += 24;
					break;
				}
		}
}

void DisplayMem::drawHZString(int x, int y , int num1, int num2, bool isReverse)	//是不是反色显示
{
		for (int i = num1; i < num2+1; i++) 
		{
				this->drawHZ(x, y, i, FONT_12X12, isReverse);
				x += 12;
		}
}

void DisplayMem::drawHZString(int x, int y , int num1, int num2, FONT_HZ_t font, bool isReverse)
{
		for (int i = num1; i < num2+1; i++) 
		{
				this->drawHZ(x, y, i, font, isReverse);
				switch (font) 
				{
				case FONT_12X12:
					x += 12;
					break;
				case FONT_16X16:
					x += 16;
					break;
				case FONT_24X24:
					x += 24;
					break;
				}
		}
}

void DisplayMem::drawPicture(unsigned char *data,int a,int b,int c,int d)
{
		Show_Pattern(data,a,b,c,d);
}

long long DisplayMem::LCD_Pow(int m, int n)
{
		long long result = 1;
		while(n--)
			result *= m;
		return result;
}

void DisplayMem::drawInt(int x, int y, int num, FONT_t font,bool isReverse)
{
	switch (font) {
	case FONT_5X8: {
		int x1;
		x1 = x / 4;
		int c = num + 16;
		if (x1 > 61) {
			x = 0;
			x1 = x / 4;
			y = y + 8;
		}  //换行
		Set_Column_Address(OLED_SHIFT + x1, OLED_SHIFT + x1 + 1); // 设置列坐标，shift为列偏移量由1322决定
		Set_Row_Address(y, y + 7);
		Set_Write_RAM();	 //	写显存

		for (int j = 0; j < 8; j++) {
			int data = ASC5X8[c * 8 + j];
			Con_4_byte(isReverse ? ~data : data);	//数据转换
		}
		break;
	}
	case FONT_6X12: {
		int x1 = x / 4;
		int c = num + 16;
		if (x1 > 61) {
			x = 0;
			x1 = x / 4;
			y = y + 12;
		}  //换行
		Set_Column_Address(OLED_SHIFT + x1, OLED_SHIFT + x1 + 1); // 设置列坐标，shift为列偏移量由1322决定
		Set_Row_Address(y, y + 11);
		Set_Write_RAM();	 //	写显存

		for (int j = 0; j < 12; j++) {
			int data = ASC6X12[c * 12 + j];
			Con_4_byte(isReverse ? ~data : data);	//数据转换
		}
		break;
	}
	case FONT_8X16: {
		int x1 = x / 4;
		int c = num + 16;
		if (x1 > 61) {
			x = 0;
			x1 = x / 4;
			y = y + 16;
		}  //换行
		Set_Column_Address(OLED_SHIFT + x1, OLED_SHIFT + x1 + 1); // 设置列坐标，shift为列偏移量由1322决定
		Set_Row_Address(y, y + 15);
		Set_Write_RAM();	 //	写显存

		for (int j = 0; j < 16; j++) {
			int data = ASC8X16[c * 16 + j];
			Con_4_byte(isReverse ? ~data : data);	//数据转换
		}
		break;
	}
	case FONT_12X24: {
		int x1 = x / 4;
		int c = num + 16;
		if (x1 > 59) {
			x = 0;
			x1 = x / 4;
			y = y + 24;
		}  //换行
		Set_Column_Address(OLED_SHIFT + x1, OLED_SHIFT + x1 + 3); // 设置列坐标，shift为列偏移量由1322决定
		Set_Row_Address(y, y + 23);
		Set_Write_RAM();	 //	写显存

		for (int j = 0; j < 48; j++) {
			int data = ASC12X24[c * 48 + j];
			Con_4_byte(isReverse ? ~data : data);	//数据转换
		}
		break;
	}
	case FONT_20X40: {
		int x1 = x / 4;
		int c = num + 16;
		/* if(x1>10)
		 {x=0;
		 x1=x/4;
		 }  //只能显示一行		*/
		Set_Column_Address(OLED_SHIFT + x1, OLED_SHIFT + x1 + 5); // 设置列坐标，shift为列偏移量由1322决定
		Set_Row_Address(y, y + 39);
		Set_Write_RAM();	 //	写显存

		for (int j = 0; j < 120; j++) {
			int data = ASC20X40[c * 120 + j];
			Con_4_byte(isReverse ? ~data : data);	//数据转换
		}
		break;
	}

	}		

}

//显示数字,高位为0,则不显示
//x,y :起点坐标	 
//len :数字的位数
//font:字体大小 
//num:数值
int DisplayMem::drawNumber(int x, int y, long long num, int len,FONT_t font)
{
		int t,temp;
		int enshow = 0;
		int size =0;
		int count = 0;
	
		switch (font)
		{
		case FONT_5X8:
				size = 5;
				break;
		case FONT_6X12:
				size = 6;
				break;
		case FONT_8X16:
				size = 8;	
				break;
		case FONT_12X24:
				size = 12;
				break;
		case FONT_20X40:
				size = 20;
				break;
		}
		
		for(t=0;t<len;t++)
		{
				temp = (num/LCD_Pow(10,len-t-1))%10;
				if((enshow==0)&&(t<(len-1)))
				{
						if(temp==0)
						{
								count++;
								continue;
						}
						else enshow = 1;
				}
				drawInt(x+size*(t-count),y,temp,font,false);
		}
		return len - count;
}

//显示数字,高位为0,还是显示
//x,y :起点坐标	 
//len :数字的位数
//font:字体大小 
//num:数值
int DisplayMem::drawxNumber(int x, int y, long long num, int len,FONT_t font)
{
		int t,temp;
		int enshow = 0;
		int size =0;
	
		switch (font)
		{
		case FONT_5X8:
				size = 5;
				break;
		case FONT_6X12:
				size = 6;
				break;
		case FONT_8X16:
				size = 8;	
				break;
		case FONT_12X24:
				size = 12;
				break;
		case FONT_20X40:
				size = 20;
				break;
		}
		
		for(t=0;t<len;t++)
		{
				temp = (num/LCD_Pow(10,len-t-1))%10;
				if((enshow==0)&&(t<(len-1)))
				{
						if(temp==0)
						{
								drawInt(x+size*t,y,0,font,false);
								continue;
						}
						else enshow = 1;
				}
				if((enshow == 1)&&(temp == 0))
				{
						if(!(num%LCD_Pow(10,len-t-1)))							
							return len - t;
				}				
				drawInt(x+size*t,y,temp,font,false);
		}
		return 0;
}

}
/* namespace view */

