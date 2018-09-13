#include "oled281.h"
#include "stdlib.h"
#include "Asc8x16.h" 	   //ascii 8*16�ֿ�
#include "HZ12X12_S.h" 	   //12*12�����Զ��庺�ֿ�
#include "bmp.h"
#include "main_define.h"
#ifdef English
		#include "Eng_pic.h"
#endif


__INLINE static uint8_t get_Ascii_width(uint8_t ch)
{
		return AscillIndexLen[ch-32];
}

__INLINE static int get_Ascii_index(uint8_t ch)
{
		return AscillIndexNum[ch-32];
}

//��SSD1325д��һ���ֽڡ�
//dat:Ҫд�������/����
//cmd:����/�����־ 0,��ʾ����;1,��ʾ����;
void OLED_WR_Byte(uint8_t dat, uint8_t cmd)
{
		uint8_t i;
		if (cmd)
				OLED_DC_Set();
		else
				OLED_DC_Clr();
		OLED_CS_Clr();
		for(i = 0; i < 8; i++)
		{
				OLED_SCLK_Clr();
				if (dat & 0x80)
						OLED_SDIN_Set();
				else
						OLED_SDIN_Clr();
				OLED_SCLK_Set();
				dat <<= 1;
		}
		OLED_CS_Set();
		OLED_DC_Set();
}

//-----------------OLED�˿ڶ��� 4��SPI----------------  					   
/* ����IO��ģ��SPIͨѶ��ʽ����oledģ��
 CS   ~  PB3
 RES  ~  PD4
 DC   ~  PD6
 CLK  ~  PD2
 DIN  ~  PD0
 */
//��ʼ��SSD1322				    
void OLED281_Init(void) 
{
	OLED_RST_Set();
	HAL_Delay(100);
	OLED_RST_Clr();
	HAL_Delay(100);
	OLED_RST_Set();

	OLED_WR_Byte(0xAE, OLED_CMD); //	Display Off

	OLED_WR_Byte(0xFD, OLED_CMD); // Set Command Lock
	OLED_WR_Byte(0x12, OLED_DATA); //

	OLED_WR_Byte(0xB3, OLED_CMD); // Set Clock as 80 Frames/Sec
	OLED_WR_Byte(0x91, OLED_DATA); //

	OLED_WR_Byte(0xCA, OLED_CMD); // Set Multiplex Ratio
	OLED_WR_Byte(0x3F, OLED_DATA); // 1/64 Duty (0x0F~0x5F)

	OLED_WR_Byte(0xA2, OLED_CMD); // Shift Mapping RAM Counter (0x00~0x3F)
	OLED_WR_Byte(0x00, OLED_DATA); //

	OLED_WR_Byte(0xA1, OLED_CMD); // Set Mapping RAM Display Start Line (0x00~0x7F)
	OLED_WR_Byte(0x00, OLED_DATA); //

	OLED_WR_Byte(0xA0, OLED_CMD); //Set Column Address 0 Mapped to SEG0
	OLED_WR_Byte(0x14, OLED_DATA);      	//   Default => 0x40
											//     Horizontal Address Increment
	//     Column Address 0 Mapped to SEG0
	//     Disable Nibble Remap
	//     Scan from COM0 to COM[N-1]
	//     Disable COM Split Odd Even
	OLED_WR_Byte(0x11, OLED_DATA); //    Default => 0x01 (Disable Dual COM Mode)

	OLED_WR_Byte(0xB5, OLED_CMD); //  Disable GPIO Pins Input
	OLED_WR_Byte(0x00, OLED_DATA); //

	OLED_WR_Byte(0xAB, OLED_CMD); //   Enable Internal VDD Regulator
	OLED_WR_Byte(0x01, OLED_DATA); //

	OLED_WR_Byte(0xB4, OLED_CMD); //  Display Enhancement
	OLED_WR_Byte(0xA0, OLED_DATA); // Enable External VSL
	OLED_WR_Byte(0xF8, OLED_DATA); // Enhance Low Gray Scale Display Quality

	OLED_WR_Byte(0xC1, OLED_CMD); //  Set Contrast Current
	OLED_WR_Byte(0xEF, OLED_DATA); //  Default => 0x7F

	OLED_WR_Byte(0xC7, OLED_CMD); //  Master Contrast Current Control
	OLED_WR_Byte(OLED_BRIGHTNESS, OLED_DATA); //  Default => 0x0f (Maximum)

	OLED_WR_Byte(0xB8, OLED_CMD); //  	// Set Gray Scale Table
	OLED_WR_Byte(0x0C, OLED_DATA); //
	OLED_WR_Byte(0x18, OLED_DATA); //
	OLED_WR_Byte(0x24, OLED_DATA); //
	OLED_WR_Byte(0x30, OLED_DATA); //
	OLED_WR_Byte(0x3C, OLED_DATA); //
	OLED_WR_Byte(0x48, OLED_DATA); //
	OLED_WR_Byte(0x54, OLED_DATA); //
	OLED_WR_Byte(0x60, OLED_DATA); //
	OLED_WR_Byte(0x6C, OLED_DATA); //
	OLED_WR_Byte(0x78, OLED_DATA); //
	OLED_WR_Byte(0x84, OLED_DATA); //
	OLED_WR_Byte(0x90, OLED_DATA); //
	OLED_WR_Byte(0x9C, OLED_DATA); //
	OLED_WR_Byte(0xA8, OLED_DATA); //
	OLED_WR_Byte(0xB4, OLED_DATA); //
	OLED_WR_Byte(0x00, OLED_CMD); 	// Enable Gray Scale Table

	OLED_WR_Byte(0xB1, OLED_CMD); //   Set Phase 1 as 5 Clocks & Phase 2 as 14 Clocks
	OLED_WR_Byte(0xE2, OLED_DATA); //  Default => 0x74 (7 Display Clocks [Phase 2] / 9 Display Clocks [Phase 1])

	OLED_WR_Byte(0xD1, OLED_CMD); //    Enhance Driving Scheme Capability (0x00/0x20)
	OLED_WR_Byte(0xA2, OLED_DATA); //   Normal
	OLED_WR_Byte(0x20, OLED_DATA); //

	OLED_WR_Byte(0xBB, OLED_CMD); //     Set Pre-Charge Voltage Level as 0.60*VCC
	OLED_WR_Byte(0x1F, OLED_DATA); //

	OLED_WR_Byte(0xB6, OLED_CMD); //  // Set Second Pre-Charge Period as 8 Clocks
	OLED_WR_Byte(0x08, OLED_DATA); //

	OLED_WR_Byte(0xBE, OLED_CMD); //  Set Common Pins Deselect Voltage Level as 0.86*VCC
	OLED_WR_Byte(0x07, OLED_DATA); //  Default => 0x04 (0.80*VCC)

	OLED_WR_Byte(0xA6, OLED_CMD); //     Normal Display Mode (0x00/0x01/0x02/0x03)
								  //   Default => 0xA4
	//     0xA4  => Entire Display Off, All Pixels Turn Off
	//     0xA5  => Entire Display On, All Pixels Turn On at GS Level 15
	//     0xA6  => Normal Display
	//     0xA7  => Inverse Display

	OLED_WR_Byte(0xA9, OLED_CMD); //   Disable Partial Display

	Fill_RAM(0x00);
	OLED_WR_Byte(0xAF, OLED_CMD); //	Display On

}

//��
void Set_Column_Address(unsigned char a, unsigned char b) {
	OLED_WR_Byte(0x15, OLED_CMD);			// Set Column Address
	OLED_WR_Byte(a, OLED_DATA);			//   Default => 0x00
	OLED_WR_Byte(b, OLED_DATA);			//   Default => 0x77
}

//��
void Set_Row_Address(unsigned char a, unsigned char b) {
	OLED_WR_Byte(0x75, OLED_CMD);			// Set Row Address
	OLED_WR_Byte(a, OLED_DATA);			//   Default => 0x00
	OLED_WR_Byte(b, OLED_DATA);			//   Default => 0x7F
}

void Set_Write_RAM() {

	OLED_WR_Byte(0x5C, OLED_CMD);			// Enable MCU to Write into RAM
}

void Set_Read_RAM() {
	OLED_WR_Byte(0x5D, OLED_CMD);			// Enable MCU to Read from RAM
}

void Set_Remap_Format(unsigned char d) {
	OLED_WR_Byte(0xA0, OLED_CMD);  			// Set Re-Map / Dual COM Line Mode
	OLED_WR_Byte(d, OLED_DATA);  				//   Default => 0x40
	//     Horizontal Address Increment
	//     Column Address 0 Mapped to SEG0
	//     Disable Nibble Remap
	//     Scan from COM0 to COM[N-1]
	//     Disable COM Split Odd Even
	OLED_WR_Byte(0x11, OLED_DATA); 	//   Default => 0x01 (Disable Dual COM Mode)
}
void Set_Partial_Display(unsigned char a, unsigned char b, unsigned char c) {
	OLED_WR_Byte((0xA8 | a), OLED_CMD);		// Default => 0x8F

	if (a == 0x00) {
		OLED_WR_Byte(b, OLED_DATA);
		OLED_WR_Byte(c, OLED_DATA);

	}
}

void Set_Display_Offset(unsigned char d) {
	OLED_WR_Byte(0xA2, OLED_CMD);
	OLED_WR_Byte(d, OLED_DATA);

}

void Set_Start_Line(unsigned char d) {
	OLED_WR_Byte(0xA1, OLED_CMD);	// Set Vertical Scroll by RAM
	OLED_WR_Byte(d, OLED_DATA);

}

void Set_Master_Current(unsigned char d) {
	OLED_WR_Byte(0xC7, OLED_CMD);	//  Master Contrast Current Control
	OLED_WR_Byte(d, OLED_DATA);

}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Gray Scale Table Setting (Full Screen)
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Set_Gray_Scale_Table() {

	OLED_WR_Byte(0xB8, OLED_CMD); //  	// Set Gray Scale Table
	OLED_WR_Byte(0x0C, OLED_DATA); //
	OLED_WR_Byte(0x18, OLED_DATA); //
	OLED_WR_Byte(0x24, OLED_DATA); //
	OLED_WR_Byte(0x30, OLED_DATA); //
	OLED_WR_Byte(0x3C, OLED_DATA); //
	OLED_WR_Byte(0x48, OLED_DATA); //
	OLED_WR_Byte(0x54, OLED_DATA); //
	OLED_WR_Byte(0x60, OLED_DATA); //
	OLED_WR_Byte(0x6C, OLED_DATA); //
	OLED_WR_Byte(0x78, OLED_DATA); //
	OLED_WR_Byte(0x84, OLED_DATA); //
	OLED_WR_Byte(0x90, OLED_DATA); //
	OLED_WR_Byte(0x9C, OLED_DATA); //
	OLED_WR_Byte(0xA8, OLED_DATA); //
	OLED_WR_Byte(0xB4, OLED_DATA); //
	OLED_WR_Byte(0x00, OLED_CMD); 	// Enable Gray Scale Table

}
void Set_Linear_Gray_Scale_Table() {
	OLED_WR_Byte(0xB9, OLED_CMD); //	Set Default Linear Gray Scale Table

}

/*

 //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 //  Vertical Scrolling (Full Screen)
 //
 //    a: Scrolling Direction
 //       "0x00" (Upward)		���Ϲ���
 //       "0x01" (Downward)		���¹���
 //    b: Set Numbers of Row Scroll per Step	  ÿ֡����
 //    c: Set Time Interval between Each Scroll Step	ÿ֡����ʱ
 //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 void Vertical_Scroll(unsigned char a, unsigned char b, unsigned char c)
 {
 unsigned char i,j;

 Set_Partial_Display(0x00,0x00,Max_Row);

 switch(a)
 {
 case 0:
 for(i=0;i<(Max_Row+1);i+=b)
 {
 Set_Display_Offset(i+1);
 for(j=0;j<c;j++)
 {
 delay_us(200);
 }
 }
 break;
 case 1:
 for(i=0;i<(Max_Row+1);i+=b)
 {
 Set_Display_Offset(Max_Row-i);
 for(j=0;j<c;j++)
 {
 delay_us(200);
 }
 }
 break;
 }
 Set_Partial_Display(0x01,0x00,0x00);
 }

 */

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Show Regular Pattern (Full Screen)
//��������,������,������Ļ�Ǻڵ�
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Fill_RAM(unsigned char Data) {
	unsigned char i, j;

	Set_Column_Address(0x00, 0x77);
	Set_Row_Address(0x00, 0x7F);
	Set_Write_RAM();

	for (i = 0; i < 128; i++) {
		for (j = 0; j < 120; j++) {
			OLED_WR_Byte(Data, OLED_DATA);
			OLED_WR_Byte(Data, OLED_DATA);
		}
	}
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  www.lcdsoc.com 201505
//	  Data:ȡֵΪ 0x11��0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xaa,0xbb,0xcc,0xdd,0xee,0xff, ff����
//    a: Line Width		�߿�
//    b: Column Address of Start ��ʼ��
//    c: Column Address of End	 ������
//    d: Row Address of Start	 ��ʼ��
//    e: Row Address of End		 ������
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Draw_Rectangle(unsigned char Data, unsigned char a, unsigned char b,
		unsigned char c, unsigned char d, unsigned char e) {
	unsigned char i, j, k, l, b1, c1;
	b1 = b / 4;
	c1 = c / 4;
	k = a % 4;
	if (k == 0) {
		l = (a / 4) - 1;
	} else {
		l = a / 4;
	}

	Set_Column_Address(OLED_SHIFT + b1, OLED_SHIFT + c1);
	Set_Row_Address(d, (d + a - 1));
	Set_Write_RAM();
	for (i = 0; i < (c1 - b1 + 1); i++) {
		for (j = 0; j < a; j++) {
			OLED_WR_Byte(Data, OLED_DATA);
			OLED_WR_Byte(Data, OLED_DATA);
		}
	}

	Set_Column_Address(OLED_SHIFT + (c1 - l), OLED_SHIFT + c1);
	Set_Row_Address(d + a, e - a);
	Set_Write_RAM();
	for (i = 0; i < (e - d + 1); i++) {
		for (j = 0; j < (l + 1); j++) {
			if (j == 0) {
				switch (k) {
				case 0:
					OLED_WR_Byte(Data, OLED_DATA);
					OLED_WR_Byte(Data, OLED_DATA);
					break;
				case 1:
					OLED_WR_Byte(0x00, OLED_DATA);
					OLED_WR_Byte((Data & 0x0F), OLED_DATA);

					break;
				case 2:
					OLED_WR_Byte(0x00, OLED_DATA);
					OLED_WR_Byte(Data, OLED_DATA);

					break;
				case 3:
					OLED_WR_Byte((Data & 0x0F), OLED_DATA);
					OLED_WR_Byte(Data, OLED_DATA);

					break;
				}
			} else {
				OLED_WR_Byte(Data, OLED_DATA);
				OLED_WR_Byte(Data, OLED_DATA);
			}
		}
	}

	Set_Column_Address(OLED_SHIFT + b1, OLED_SHIFT + c1);
	Set_Row_Address((e - a + 1), e);
	Set_Write_RAM();
	for (i = 0; i < (c1 - b1 + 1); i++) {
		for (j = 0; j < a; j++) {
			OLED_WR_Byte(Data, OLED_DATA);
			OLED_WR_Byte(Data, OLED_DATA);

		}
	}

	Set_Column_Address(OLED_SHIFT + b1, OLED_SHIFT + (b1 + l));
	Set_Row_Address(d + a, e - a);
	Set_Write_RAM();
	for (i = 0; i < (e - d + 1); i++) {
		for (j = 0; j < (l + 1); j++) {
			if (j == l) {
				switch (k) {
				case 0:
					OLED_WR_Byte(Data, OLED_DATA);
					OLED_WR_Byte(Data, OLED_DATA);
					break;
				case 1:

					OLED_WR_Byte((Data & 0xF0), OLED_DATA);
					OLED_WR_Byte(0x00, OLED_DATA);

					break;
				case 2:
					OLED_WR_Byte(Data, OLED_DATA);
					OLED_WR_Byte(0x00, OLED_DATA);

					break;
				case 3:

					OLED_WR_Byte(Data, OLED_DATA);
					OLED_WR_Byte((Data & 0xF0), OLED_DATA);

					break;
				}
			} else {
				OLED_WR_Byte(Data, OLED_DATA);
				OLED_WR_Byte(Data, OLED_DATA);
			}
		}
	}
}
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Show Regular Pattern (Partial or Full Screen)
//
//    a: Column Address of Start
//    b: Column Address of End (Total Columns Devided by 4)
//    c: Row Address of Start
//    d: Row Address of End
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Fill_Block(unsigned char Data, unsigned char a, unsigned char b,
		unsigned char c, unsigned char d) {
	unsigned char i, j;

	Set_Column_Address(OLED_SHIFT + a, OLED_SHIFT + b);
	Set_Row_Address(c, d);
	Set_Write_RAM();

	for (i = 0; i < (d - c + 1); i++) {
		for (j = 0; j < (b - a + 1); j++) {
			OLED_WR_Byte(Data, OLED_DATA);
			OLED_WR_Byte(Data, OLED_DATA);
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
// �Ҷ�ģʽ����ʾһ��ͼƬ
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Show_Pattern(unsigned char *Data_Pointer, unsigned char a, unsigned char b,
		unsigned char c, unsigned char d) {

	unsigned char *Src_Pointer;
	unsigned char i, j;

	//ȡģʱ����������	�����ܷ�����2.7��ͬ��
	Src_Pointer = Data_Pointer;
	Set_Column_Address(OLED_SHIFT + a, OLED_SHIFT + b);
	Set_Row_Address(c, d);
	Set_Write_RAM();
	for (i = 0; i < (d - c + 1); i++) {
		for (j = 0; j < (b - a + 1); j++) {
			OLED_WR_Byte(*Src_Pointer, OLED_DATA);
			Src_Pointer++;
			OLED_WR_Byte(*Src_Pointer, OLED_DATA);
			Src_Pointer++;
		}
	}

}

/**************************************
 ����ת�����򣺽�2λ�ֳ�1���ֽڴ����Դ棬����1��seg��ʾ4��������Ҫͬʱд2���ֽڼ�4������
 uchar DATA��ȡģ������ģ����
 ****************************************/
void Con_4_byte(unsigned char DATA) 
{
		unsigned char d1_4byte[4], d2_4byte[4];
		unsigned char i;
		unsigned char d, k1, k2;
		d = DATA;

		for (i = 0; i < 2; i++)   // һ��λ�ķ�ʽд��  2*4=8λ
		{
				k1 = d & 0xc0;     		//��i=0ʱ ΪD7,D6λ ��i=1ʱ ΪD5,D4λ
				/****��4�ֿ��ܣ�16���Ҷ�,һ���ֽ����ݱ�ʾ�������أ�һ�����ض�Ӧһ���ֽڵ�4λ***/
				switch (k1)
				{
						case 0x00:
								d1_4byte[i] = 0x00;
								break;
						case 0x40:  // 0100,0000
								d1_4byte[i] = 0x0f;
								break;
						case 0x80:  //1000,0000
								d1_4byte[i] = 0xf0;
								break;
						case 0xc0:   //1100,0000
								d1_4byte[i] = 0xff;
								break;
						default:
								break;
				}

				d = d << 2;
				k2 = d & 0xc0;     //��i=0ʱ ΪD7,D6λ ��i=1ʱ ΪD5,D4λ
				/****��4�ֿ��ܣ�16���Ҷ�,һ���ֽ����ݱ�ʾ�������أ�һ�����ض�Ӧһ���ֽڵ�4λ***/

				switch (k2) 
				{
						case 0x00:
								d2_4byte[i] = 0x00;
								break;
						case 0x40:  // 0100,0000
								d2_4byte[i] = 0x0f;
								break;
						case 0x80:  //1000,0000
								d2_4byte[i] = 0xf0;
								break;
						case 0xc0:   //1100,0000
								d2_4byte[i] = 0xff;
								break;
						default:
								break;
				}
				d = d << 2;                               //������λ
				OLED_WR_Byte(d1_4byte[i], OLED_DATA);	    //дǰ2��
				OLED_WR_Byte(d2_4byte[i], OLED_DATA);     //д��2��	  ����4��
		}
}

/***************************************************************
 //  ��ʾ12*12������ 2015-05���²���ͨ��
 //  ȡģ��ʽΪ������ȡģ���λ,��������:�����Ҵ��ϵ���    16�� 12��
 //   num���������ֿ��е�λ��
 //   x: Start Column  ��ʼ�� ��Χ 0~��256-16��
 //   y: Start Row   ��ʼ�� 0~63
 ***************************************************************/
void HZ12_12(unsigned char x, unsigned char y, unsigned char num) {
	unsigned char x1, j;
	x1 = x / 4;
	Set_Column_Address(OLED_SHIFT + x1, OLED_SHIFT + x1 + 3); // ���������꣬shiftΪ��ƫ������1322������3Ϊ16/4-1
	Set_Row_Address(y, y + 11);
	Set_Write_RAM();	 //	д�Դ�

	for (j = 0; j < 24; j++) {
		Con_4_byte(HZ12X12_S[num * 24 + j]);
	}
}
//****************************************************
//   д��һ��12*12���� www.lcdsoc.com	x������ּ��dҪΪ4�ı���
//    num1,num2���������ֿ��е�λ��	 ��num1��ʾ��num2
//    x: Start Column  ��ʼ�� ��Χ 0~��255-16��
//    y: Start Row    ��ʼ�� 0~63
//    d:�ּ���� 0Ϊ�޼�� 
//*****************************************************
void Show_HZ12_12(unsigned char x, unsigned char y, unsigned char num1, unsigned char num2) {
	unsigned char i, d1;
	d1 = 16;
	for (i = num1; i < num2 + 1; i++) {
		HZ12_12(x, y, i);
		x = x + d1;
	}
}
//==============================================================
//����������д��һ���׼ASCII�ַ���	 8x16
//��������ʾ��λ�ã�x,y����ch[]Ҫ��ʾ���ַ���
//���أ���
//==============================================================  
void Asc8_16(unsigned char x, unsigned char y, unsigned char ch[]) 
{
		unsigned char x1, i = 0, j = 0;
		unsigned char width = 0;
		unsigned int index = 0;
	
		while (ch[i] != '\0') 
		{
				width = get_Ascii_width(ch[i]);
				index = get_Ascii_index(ch[i]);
			
				//�����ж�
				x1 = x / 4;
				if(x1 > 61)
				{
						x = 0;
						x1 = x / 4;
						y = y + 16;
				}
				
				//��������  д�Դ�
				if(width == 8)
						Set_Column_Address(OLED_SHIFT + x1, OLED_SHIFT + x1 + 1); // ���������꣬shiftΪ��ƫ������1322����
				else if(width == 4)
						Set_Column_Address(OLED_SHIFT + x1, OLED_SHIFT + x1); // ���������꣬shiftΪ��ƫ������1322����
				Set_Row_Address(y, y + 15);
				Set_Write_RAM();
				
				//����ת��
				if(width == 8)
				{
						for (j = 0; j < 16; j++) 
						{
								Con_4_byte(ASC8X16[index + j]);
						}
						x = x + 8;
				}
				else if(width == 4)
				{
						for (j = 0; j < 8; j++) 
						{
								Con_4_byte(ASC8X16[index + j]);
						}				
						x = x + 4;
				}
				i++;				  
		}
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Show Gray Scale Bar (Full Screen)
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Grayscale() {
	// Level 16 => Column 1~16
	Fill_Block(0xFF, 0x00, 0x03, 0x00, OLED_MAX_ROW);
	// Level 15 => Column 17~32
	Fill_Block(0xEE, 0x04, 0x07, 0x00, OLED_MAX_ROW);
	// Level 14 => Column 33~48
	Fill_Block(0xDD, 0x08, 0x0B, 0x00, OLED_MAX_ROW);
	// Level 13 => Column 49~64
	Fill_Block(0xCC, 0x0C, 0x0F, 0x00, OLED_MAX_ROW);
	// Level 12 => Column 65~80
	Fill_Block(0xBB, 0x10, 0x13, 0x00, OLED_MAX_ROW);
	// Level 11 => Column 81~96
	Fill_Block(0xAA, 0x14, 0x17, 0x00, OLED_MAX_ROW);
	// Level 10 => Column 97~112
	Fill_Block(0x99, 0x18, 0x1B, 0x00, OLED_MAX_ROW);
	// Level 9 => Column 113~128
	Fill_Block(0x88, 0x1C, 0x1F, 0x00, OLED_MAX_ROW);
	// Level 8 => Column 129~144
	Fill_Block(0x77, 0x20, 0x23, 0x00, OLED_MAX_ROW);
	// Level 7 => Column 145~160
	Fill_Block(0x66, 0x24, 0x27, 0x00, OLED_MAX_ROW);
	// Level 6 => Column 161~176
	Fill_Block(0x55, 0x28, 0x2B, 0x00, OLED_MAX_ROW);
	// Level 5 => Column 177~192
	Fill_Block(0x44, 0x2C, 0x2F, 0x00, OLED_MAX_ROW);
	// Level 4 => Column 193~208
	Fill_Block(0x33, 0x30, 0x33, 0x00, OLED_MAX_ROW);
	// Level 3 => Column 209~224
	Fill_Block(0x22, 0x34, 0x37, 0x00, OLED_MAX_ROW);
	// Level 2 => Column 225~240
	Fill_Block(0x11, 0x38, 0x3B, 0x00, OLED_MAX_ROW);
	// Level 1 => Column 241~256
	Fill_Block(0x00, 0x3C, OLED_MAX_COLUMN, 0x00, OLED_MAX_ROW);
}

void clearArea(int x, int y, int width, int height) 
{
	Fill_Block(0,x/4,(x+width-1)/4,y,y+height-1);
}

void Show_num(unsigned char x,unsigned char y,int num,unsigned char isReverse)
{
		int x1,data,j;
		int index = get_Ascii_index(num+0x30);
	
		x1 = x / 4;
		if (x1 > 61) 
		{
				x = 0;
				x1 = x / 4;
				y = y + 16;
		}  //����
		Set_Column_Address(0x1C + x1, 0x1C + x1 + 1); // ���������꣬shiftΪ��ƫ������1322����
		Set_Row_Address(y, y + 15);
		Set_Write_RAM();	 //	д�Դ�

		for (j = 0; j < 16; j++) 
		{
				data = ASC8X16[index + j];
				Con_4_byte(isReverse ? ~data : data);	//����ת��
		}
}

long long LCD_Pow(int m, int n)
{
		long long result = 1;
		while(n--)
			result *= m;
		return result;
}

void drawInt(int x, int y, int num)
{
		int x1 = x / 4;
		int index = get_Ascii_index(num+0x30);
	
		if (x1 > 61) 
		{
			x = 0;
			x1 = x / 4;
			y = y + 16;
		}  //����
		Set_Column_Address(OLED_SHIFT + x1, OLED_SHIFT + x1 + 1); // ���������꣬shiftΪ��ƫ������1322����
		Set_Row_Address(y, y + 15);
		Set_Write_RAM();	 //	д�Դ�
		
		for (int j = 0; j < 16; j++) 
		{
			int data = ASC8X16[index + j];
			Con_4_byte(data);	//����ת��
		}
}

//��ʾ����,��λΪ0,����ʾ
//x,y :�������	 
//len :���ֵ�λ��
//font:�����С 
//num:��ֵ
int drawNumber(int x, int y, long long num, int len)
{
		int t,temp;
		int enshow = 0;
		int size   = 8;
		int count  = 0;
		
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
				drawInt(x+size*(t-count),y,temp);
		}
		return len - count;
}

//��ʾ����,��λΪ0,������ʾ
//x,y :�������	 
//len :���ֵ�λ��
//font:�����С 
//num:��ֵ
int drawxNumber(int x, int y, long long num, int len)
{
		int t,temp;
		int enshow = 0;
		int size =8;
		
		for(t=0;t<len;t++)
		{
				temp = (num/LCD_Pow(10,len-t-1))%10;
				if((enshow==0)&&(t<(len-1)))
				{
						if(temp==0)
						{
								drawInt(x+size*t,y,0);
								continue;
						}
						else enshow = 1;
				}
				if((enshow == 1)&&(temp == 0))
				{
						if(!(num%LCD_Pow(10,len-t-1)))							
							return len - t;
				}				
				drawInt(x+size*t,y,temp);
		}
		return 0;
}

void Show_AscII_Picture(unsigned char x, unsigned char y,unsigned char *data, int len)
{
		unsigned char x1, i=0;   
		
		x1=x/4;
		Set_Column_Address(OLED_SHIFT+x1,OLED_SHIFT+x1+len/8-1); // ���������꣬shiftΪ��ƫ������1322���� 
		Set_Row_Address(y,y+15); 
		Set_Write_RAM();	 //	д�Դ�    
				
		for(i=0;i<len;i++)
		{	
				Con_4_byte(data[i]);	//����ת��
		}
}
