/*
 * DisplayMem.cpp
 *
 *  Created on: 2017年7月9日
 *      Author: Administrator
 */

#include "DisplayMem.h"
#include <string.h>
#include "OLED281/oled281.h"
#include "main_define.h"

//声明ASCII库
extern unsigned char ASC5X8[];
extern unsigned char ASC6X12[];
extern unsigned char ASC8X16[];
extern unsigned char ASC12X24[];
extern unsigned char ASC20X40[];
extern unsigned char  HZ12X12_S[];
extern unsigned char  HZ16X16_S[];
extern unsigned char  HZ24X24_S[];
extern RNG_HandleTypeDef hrng;
extern SIGN_KEY_FLAG Key_Flag;


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

int DisplayMem::drawChar(int x, int y, char ch, FONT_t font, bool isReverse) {
	int value = 0;
	switch (font) {
	case FONT_5X8: {
		int x1;
		x1 = x / 4;
		int c = ch - 32;
		if (x1 > 61) {
			x = 0;
			x1 = x / 4;
			y = y + 8;
			value = 1;
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
		printf("%d ",x1);	
		if (x1 > 61) {
			x = 0;
			x1 = x / 4;
			y = y + 12;
			value = 1;
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
			value = 1;
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
			value = 1;
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
	return value;
}

void DisplayMem::clearArea(int x, int y, int width, int height) {
	Fill_Block(0,x/4,(x+width-1)/4,y,y+height-1);
}

void DisplayMem::drawString(int x, int y, char* c) {
	for (int i = 0; c[i] != '\0'; i++) 
	{
		if(this->drawChar(x, y, c[i], FONT_8X16, false))
		{	
			x = 0;
			y+=12;
		}
		x += 8;
	}
}

void DisplayMem::drawString(int x, int y, char* c, FONT_t font) {
	for (int i = 0; c[i] != '\0'; i++) {
		if(this->drawChar(x, y, c[i], font, false))
		{
			y+=12;
			x = 0;
		}
		switch (font) {
		case FONT_5X8:
			x += 8;
			break;
		case FONT_6X12:
			x += 8;
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
		if(this->drawChar(x, y, c[i], FONT_8X16, isReverse))
		{
			x = 0;
			y+=12;
		}	
		x += 8;
	}
}

void DisplayMem::drawString(int x, int y, char* c, FONT_t font,
		bool isReverse) {
	for (int i = 0; c[i] != '\0'; i++) {
		if(this->drawChar(x, y, c[i], font, isReverse))
		{
				x = 0;
				y+=12;
		}
		switch (font) {
		case FONT_5X8:
			x += 8;
			break;
		case FONT_6X12:
			x += 8;
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
		w += 8;
		break;
	case FONT_6X12:
		w += 8;
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
					x += 16;
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
					x += 16;
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

int DisplayMem::GetPassportArray(int *data)
{
		HAL_StatusTypeDef status = HAL_OK;	
		uint32_t tmp = 0;
		int i = 0;
		int t = 0;
		
		for(i=0;i<9;i++)
		{			
				status = HAL_RNG_GenerateRandomNumber(&hrng,&tmp);
				if(status != 0)//随机数出错
						return 0;
				tmp =	tmp%9;
				if(tmp == 0)	tmp = i;
				t = data[i];
				data[i] = data[tmp];
				data[tmp] = t;
		}
		
		Show_num(108,6,data[0],2,0);
		Show_num(124,6,data[1],2,0);
		Show_num(140,6,data[2],2,0);	
		Show_num(108,24,data[3],2,0);
		Show_num(124,24,data[4],2,0);
		Show_num(140,24,data[5],2,0);	
		Show_num(108,42,data[6],2,0);
		Show_num(124,42,data[7],2,0);
		Show_num(140,42,data[8],2,0);
		
		return 1;
}

void DisplayMem::GetPassportFromString(u8 *data,int *src,u32 *desc)
{
		int i;
		for(i=0;i<6;i++)
		{
				desc[i] = src[data[i]-49];
		}
}

void DisplayMem::SetCode(void)
{
    unsigned char index = 0;
    unsigned char Center_Flag = 0;
    unsigned char code_array[8] = {'0','0','0','0','0','0','0','0'};
		
		clearAll();
    drawString(53,8,"Pin Code:");
		drawChar(125,8,code_array[0],FONT_6X12,false);
    drawString(133,8,"_");
    drawString(141,8,"_");
    drawString(149,8,"_");
    drawString(157,8,"_");
    drawString(165,8,"_");
    drawString(173,8,"_");
    drawString(181,8,"_");
    drawString(30,40,"+");
    drawString(120,40,"OK");
    drawString(226,40,"-");

		memset(&Key_Flag,0,sizeof(Key_Flag));
		Key_Flag.Sign_Key_Flag = 1;//按键有效
    while(1)
    {
        if(Key_Flag.Sign_Key_left_Flag)//左键按下
        {
						Key_Flag.Sign_Key_left_Flag = 0;
						clearArea(125+8*index,8,6,12);//清空这个位置的显示
            if(Center_Flag != 0)
                Center_Flag = 0;
            if(code_array[index] == '0')
            {
								drawChar(125+8*index,8,'9',FONT_6X12,false);
                code_array[index] = '9';
            }
            else if(code_array[index] == '?')
            {
								drawChar(125+8*index,8,'0',FONT_6X12,false);
                code_array[index] = '0';
            }
            else
            {
                code_array[index]--;
								drawChar(125+8*index,8,code_array[index],FONT_6X12,false);
            }
        }
        if(Key_Flag.Sign_Key_right_Flag)//右键按下
        {           
						Key_Flag.Sign_Key_right_Flag = 0;
						clearArea(125+8*index,8,6,12);//清空这个位置的显示
            if(Center_Flag != 0)
                Center_Flag = 0;
            if(code_array[index] == '9')
            {
								drawChar(125+8*index,8,'0',FONT_6X12,false);
                code_array[index] = '0';
            }
            else if(code_array[index] == '?')
            {
								drawChar(125+8*index,8,'0',FONT_6X12,false);
                code_array[index] = '0';
            }
            else
            {
                code_array[index]++;
								drawChar(125+8*index,8,code_array[index],FONT_6X12,false);
            }
        }
        if(Key_Flag.Sign_Key_center_Flag)//中间建按下
        {
						Key_Flag.Sign_Key_center_Flag = 0;
            Center_Flag++;
            index++;
            if(index < 4)
            {
                Center_Flag = 0;
								clearArea(125+8*index,8,6,12);//清空这个位置的显示
								drawChar(125+8*index,8,'0',FONT_6X12,false);
            }
            else
            {
								clearArea(125+8*index,8,6,12);//清空这个位置的显示
								drawChar(125+8*index,8,'?',FONT_6X12,false);
            }
        }
        if((index>=4)&&(Center_Flag >= 2))
        {
            //将密码保存起来,

					
					
					
					
						clearAll();
						
					
					
            break;
        }
    }
		Key_Flag.Sign_Key_Flag = 0;//按键无效
		memset(&Key_Flag,0,sizeof(Key_Flag));
}

/************************************************
密码验证函数
返回值： 0  密码错误
				1  密码正确
*************************************************/
u8 DisplayMem::VerifyCode(void)
{
    unsigned char index = 0;
    unsigned char Center_Flag = 0;
    unsigned char code_old[8] = {'0','0','0','0','0','0','0','0'};//从存储的地方读出来
    unsigned char code_array[8] = {'0','0','0','0','0','0','0','0'};

		clearAll();
    drawString(53,8,"Pin Code:");
		drawChar(125,8,code_array[0],FONT_6X12,false);
    drawString(133,8,"_");
    drawString(141,8,"_");
    drawString(149,8,"_");
    drawString(157,8,"_");
    drawString(165,8,"_");
    drawString(173,8,"_");
    drawString(181,8,"_");
    drawString(30,40,"+");
    drawString(120,40,"OK");
    drawString(226,40,"-");
		
		//读取出老的密码
		
		
		
		
		
		
		memset(&Key_Flag,0,sizeof(Key_Flag));
		Key_Flag.Sign_Key_Flag = 1;//按键有效
    while(1)
    {
        if(Key_Flag.Sign_Key_left_Flag)//左键按下
        {
						Key_Flag.Sign_Key_left_Flag = 0;
						clearArea(125+8*index,8,6,12);//清空这个位置的显示
            if(Center_Flag != 0)
                Center_Flag = 0;
            if(code_array[index] == '0')
            {
								drawChar(125+8*index,8,'9',FONT_6X12,false);
                code_array[index] = '9';
            }
            else if(code_array[index] == '?')
            {
								drawChar(125+8*index,8,'0',FONT_6X12,false);
                code_array[index] = '0';
            }
            else
            {
                code_array[index]--;
								drawChar(125+8*index,8,code_array[index],FONT_6X12,false);
            }
        }
        if(Key_Flag.Sign_Key_right_Flag)//右键按下
        {
						Key_Flag.Sign_Key_right_Flag = 0;
						clearArea(125+8*index,8,6,12);//清空这个位置的显示
            if(Center_Flag != 0)
                Center_Flag = 0;
            if(code_array[index] == '9')
            {
								drawChar(125+8*index,8,'0',FONT_6X12,false);
                code_array[index] = '0';
            }
            else if(code_array[index] == '?')
            {
								drawChar(125+8*index,8,'0',FONT_6X12,false);
                code_array[index] = '0';
            }
            else
            {
                code_array[index]++;
								drawChar(125+8*index,8,code_array[index],FONT_6X12,false);
            }
        }
        if(Key_Flag.Sign_Key_center_Flag)//中间建按下
        {
						Key_Flag.Sign_Key_center_Flag = 0;
						clearArea(125+8*index,8,6,12);//清空这个位置的显示
						drawChar(125+8*index,8,'*',FONT_6X12,false);
            Center_Flag++;
            index++;
            if(index < 4)
            {
                Center_Flag = 0;
								clearArea(125+8*index,8,6,12);//清空这个位置的显示
								drawChar(125+8*index,8,'0',FONT_6X12,false);
            }
            else
            {
								clearArea(125+8*index,8,6,12);//清空这个位置的显示
								drawChar(125+8*index,8,'?',FONT_6X12,false);
            }
        }
        if((index>=4)&&(Center_Flag >= 2))
        {
            //清除所有显示
						clearAll();
					
					

            if(compareCharArray(code_old,code_array,index) == 1)//密码正确
            {    
								Key_Flag.Sign_Key_Flag = 0;//按键无效
		            memset(&Key_Flag,0,sizeof(Key_Flag));	
								return 1;							
						}
            else//密码错误
						{
								Key_Flag.Sign_Key_Flag = 0;//按键无效
		            memset(&Key_Flag,0,sizeof(Key_Flag));									
                return 0;
						}
        }
    }	
}

u8 DisplayMem::compareCharArray(unsigned char *left,unsigned char *right,int len)
{
		for(int i=0;i<len;i++)
		{
				if(left[i] != right[i])
						return 0;
		}
		return 1;
}

}
/* namespace view */

