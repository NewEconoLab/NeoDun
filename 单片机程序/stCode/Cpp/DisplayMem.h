/*
 * DisplayMem.h
 *
 *  Created on: 2017年7月9日
 *      Author: Administrator
 */

#ifndef CPP_DISPLAYMEM_H_
#define CPP_DISPLAYMEM_H_

#include "myType.h"


namespace view {

typedef enum
{
	FONT_5X8  = 0,
	FONT_6X12,
	FONT_8X16,
	FONT_12X24,
	FONT_20X40,
}FONT_t;

typedef enum
{
	FONT_12X12  = 0,
	FONT_16X16,
	FONT_24X24,
}FONT_HZ_t;

//12864 显存类
class DisplayMem {
private:
	DisplayMem();
	DisplayMem(const DisplayMem & copy);
	void drawChar(int x, int y , char c,FONT_t font,bool isReverse);
	//add by hkh
	void drawHZ(int x, int y, int num, FONT_HZ_t font, bool isReverse);
public:
	static DisplayMem & getInstance();
	void clearAll();	//清除全部显存
	void drawRectangel(int x , int y , int width , int height );
	void clearArea(int x , int y , int width , int height ); //清除显示

	void drawString(int x, int y , char* c);
	void drawString(int x, int y , char* c,FONT_t font);
	void drawString(int x, int y , char* c,bool isReverse);	//是不是反色显示
	void drawString(int x, int y , char* c,FONT_t font,bool isReverse);
	void drawLineSpace(  int y , FONT_t font  );
	//add by hkh
	void drawHZString(int x, int y , int num1, int num2);
	void drawHZString(int x, int y , int num1, int num2, FONT_HZ_t font);
	void drawHZString(int x, int y , int num1, int num2, bool isReverse);	//是不是反色显示
	void drawHZString(int x, int y , int num1, int num2, FONT_HZ_t font, bool isReverse);	

	void drawPicture(unsigned char *data,int a,int b,int c,int d);
	long long LCD_Pow(int m, int n);
	void drawInt(int x, int y, int num, FONT_t font,bool isReverse);
	int drawNumber(int x, int y, long long num, int len,FONT_t font);
	int drawxNumber(int x, int y, long long num, int len,FONT_t font);
	int GetPassportArray(int *data);
	void GetPassportFromString(u8 *data,int *src,u32 *desc);
	
};

} /* namespace view */

#endif /* CPP_DISPLAYMEM_H_ */
