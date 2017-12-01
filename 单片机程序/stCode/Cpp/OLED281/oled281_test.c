#include "oled281.h"
#include "bmp.h"

void oled281_test() {
	Fill_RAM(0x00);
	Asc8_16(0, 0, "Welcom Use Neo");	   // 8*16����ASCII��
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
		//ASCII����ʾ
		Draw_Rectangle(0xff, 2, 0, 255, 0, 63);	//���Ҷ�ֵ 0xff ��0��0��~��255��63���߿�Ϊ2���صķ���
		Asc12_24(72, 19, "STM32_DEMO");  // 12*24����ASCII��
		HAL_Delay(1864);					   // ��ʱ
		HAL_Delay(1864);
		Fill_RAM(0x00);
		Draw_Rectangle(0xff, 1, 0, 255, 0, 63);	//���Ҷ�ֵ 0xff ��0��0��~��255��63���߿�Ϊ1���صķ���
		Asc5_8(88, 2, "5*8 ASCII");	   // 5*8����ASCII��
		Asc6_12(84, 11, "6*12 ASCII");	   // 6*12����ASCII��
		Asc8_16(84, 23, "8*16 ASCII");	   // 8*16����ASCII��
		Asc12_24(56, 39, "12*24 ASCII");  // 12*24����ASCII��
		HAL_Delay(1864);					   // ��ʱ
		HAL_Delay(1864);
		Fill_RAM(0x00);
		Draw_Rectangle(0xff, 3, 0, 255, 0, 63); //���Ҷ�ֵ 0xff��0��0��~��255��63���߿�Ϊ3���صķ���
		Asc20_40(15, 12, "20*40 ASCII");  // 20*40����ASCII��
		HAL_Delay(1864);
		HAL_Delay(1864);
		Fill_RAM(0x00);

		//������ʾ
		Draw_Rectangle(0xff, 1, 0, 255, 0, 63);	//���Ҷ�ֵ 0xff ��0��0��~��255��63���߿�Ϊ1���صķ���
		Show_HZ12_12(4, 3, 0, 0, 3);	// 12*12����
		Show_HZ16_16(64, 17, 4, 0, 3);	// ���꣨64��17�����ּ��4���ֿ��0~3�����¿Ƽ�����16*16����
		Show_HZ24_24(140, 32, 4, 0, 3); //24*24����
		HAL_Delay(1864);
		HAL_Delay(1864);
		HAL_Delay(1864);
		Fill_RAM(0x00);

		//�Ҷ�ͼƬ��ʾ����201411ͨ��
		Draw_Rectangle(0xff, 2, 88, 164, 2, 61); //���Ҷ�ֵ 0xff��88��2��~��164��61���߿�Ϊ3���صķ���
		Show_Pattern(&logo_60[0], 24, 38, 6, 56); //��ʾ60*50����16�׻Ҷ�ͼ��24��ʼ������(ʵ��������Ϊ112��24*4)��38Ϊ[96+60��/4]-1	,6��ʼ�У�56�����С���ʼ������Ҫ�ܱ�4����
		HAL_Delay(1864);
		HAL_Delay(1864);
		Fill_RAM(0x00);

	}

}
