//////////////////////////////////////////////////////////////////////////////////	 
//使用SysTick的普通计数模式对延迟进行管理
//********************************************************************************
#ifndef OLED_281_H_
#define OLED_281_H_

#include "stdlib.h"
#include <stdint.h>
#include <stm32f4xx_hal.h>


#define OLED_MODE 				0
#define OLED_SIZE 				16
#define OLED_XLEVEL_L			0x02
#define OLED_XLEVEL_H			0x10
#define OLED_MAX_COLUMN		0x3F			// 256/4-1
#define OLED_MAX_ROW			0x3F			// 64-1
#define	OLED_BRIGHTNESS		0xFF
#define	OLED_SHIFT				0x1C

//-----------------OLED端口定义 4线SPI----------------  					   
/* 采用IO口模拟SPI通讯方式驱动oled模块
 CS   ~  PB3
 RES  ~  PD4
 DC   ~  PD6
 CLK  ~  PD2
 DIN  ~  PD0
 */
#define OLED_CS_Clr()  	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3,GPIO_PIN_RESET)
#define OLED_CS_Set()  	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3,GPIO_PIN_SET)

#define OLED_RST_Clr() 	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_4,GPIO_PIN_RESET)
#define OLED_RST_Set() 	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_4,GPIO_PIN_SET)

#define OLED_DC_Clr()  	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_6,GPIO_PIN_RESET)
#define OLED_DC_Set()  	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_6,GPIO_PIN_SET)

#define OLED_SCLK_Clr() HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_RESET)
#define OLED_SCLK_Set() HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_SET)

#define OLED_SDIN_Clr() HAL_GPIO_WritePin(GPIOD,GPIO_PIN_0,GPIO_PIN_RESET)
#define OLED_SDIN_Set() HAL_GPIO_WritePin(GPIOD,GPIO_PIN_0,GPIO_PIN_SET)

#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据
//OLED控制用函数
void OLED_WR_Byte(uint8_t dat, uint8_t cmd);
void OLED281_Init(void);
void Set_Column_Address(unsigned char a, unsigned char b);
void Set_Row_Address(unsigned char a, unsigned char b);
void Set_Write_RAM(void);
void Set_Read_RAM(void);
void Fill_RAM(unsigned char Data);
void Draw_Rectangle(unsigned char Data, unsigned char a, unsigned char b,unsigned char c, unsigned char d, unsigned char e);
void Fill_Block(unsigned char Data, unsigned char a, unsigned char b,unsigned char c, unsigned char d);
void Grayscale(void);
void Set_Remap_Format(unsigned char d);
void Show_Pattern(unsigned char *Data_Pointer, unsigned char a, unsigned char b,unsigned char c, unsigned char d);
void Con_4_byte(unsigned char DATA);
void Set_Partial_Display(unsigned char a, unsigned char b, unsigned char c);
void Set_Display_Offset(unsigned char d);
void Set_Start_Line(unsigned char d);
void Set_Master_Current(unsigned char d);
//void Vertical_Scroll(unsigned char a, unsigned char b, unsigned char c);
void Set_Gray_Scale_Table(void);
void Set_Linear_Gray_Scale_Table(void);

void clearArea(int x, int y, int width, int height);

//以下ASCII程序
void Asc6_12(unsigned char x, unsigned char y, unsigned char ch[]);
void Asc8_16(unsigned char x, unsigned char y, unsigned char ch[]);
//以下汉字程序
void HZ12_12(unsigned char x, unsigned char y, unsigned char num);
void Show_HZ12_12(unsigned char x, unsigned char y, unsigned char num1, unsigned char num2);
void Show_num(unsigned char x,unsigned char y,int num,unsigned char isReverse);

long long LCD_Pow(int m, int n);
void drawInt(int x, int y, int num);
int drawNumber(int x, int y, long long num, int len);
int drawxNumber(int x, int y, long long num, int len);
	

#endif
