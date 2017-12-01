#include "oled281.h"
#include "bmp.h"

void oled281_test() {
	Fill_RAM(0x00);
	Asc8_16(0, 0, "Welcom Use Neo");	   // 8*16点阵ASCII码
	while (1) {
	}

}
void oled281_test2() {
	OLED_CS_Set();
	OLED_RST_Set();
	OLED_DC_Set();
	OLED_SCLK_Set();
	OLED_SDIN_Set();


	OLED281_Init();
	Fill_RAM(0x00);
	while (1) {
		//ASCII码演示
		Draw_Rectangle(0xff, 2, 0, 255, 0, 63);	//画灰度值 0xff （0，0）~（255，63）线宽为2像素的方框
		Asc12_24(72, 19, "STM32_DEMO");  // 12*24点阵ASCII码
		HAL_Delay(1864);					   // 延时
		HAL_Delay(1864);
		Fill_RAM(0x00);
		Draw_Rectangle(0xff, 1, 0, 255, 0, 63);	//画灰度值 0xff （0，0）~（255，63）线宽为1像素的方框
		Asc5_8(88, 2, "5*8 ASCII");	   // 5*8点阵ASCII码
		Asc6_12(84, 11, "6*12 ASCII");	   // 6*12点阵ASCII码
		Asc8_16(84, 23, "8*16 ASCII");	   // 8*16点阵ASCII码
		Asc12_24(56, 39, "12*24 ASCII");  // 12*24点阵ASCII码
		HAL_Delay(1864);					   // 延时
		HAL_Delay(1864);
		Fill_RAM(0x00);
		Draw_Rectangle(0xff, 3, 0, 255, 0, 63); //画灰度值 0xff（0，0）~（255，63）线宽为3像素的方框
		Asc20_40(15, 12, "20*40 ASCII");  // 20*40点阵ASCII码
		HAL_Delay(1864);
		HAL_Delay(1864);
		Fill_RAM(0x00);

		//汉字演示
		Draw_Rectangle(0xff, 1, 0, 255, 0, 63);	//画灰度值 0xff （0，0）~（255，63）线宽为1像素的方框
		Show_HZ12_12(4, 3, 0, 0, 3);	// 12*12宋体
		Show_HZ16_16(64, 17, 4, 0, 3);	// 坐标（64，17），字间距4，字库号0~3“晶奥科技”，16*16宋体
		Show_HZ24_24(140, 32, 4, 0, 3); //24*24宋体
		HAL_Delay(1864);
		HAL_Delay(1864);
		HAL_Delay(1864);
		Fill_RAM(0x00);

		//灰度图片显示测试201411通过
		Draw_Rectangle(0xff, 2, 88, 164, 2, 61); //画灰度值 0xff（88，2）~（164，61）线宽为3像素的方框
		Show_Pattern(&logo_60[0], 24, 38, 6, 56); //显示60*50像素16阶灰度图像，24开始列坐标(实际列坐标为112，24*4)，38为[96+60）/4]-1	,6开始行，56结束行。开始列坐标要能被4整除
		HAL_Delay(1864);
		HAL_Delay(1864);
		Fill_RAM(0x00);

	}

}
