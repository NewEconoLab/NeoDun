#ifndef _MYIIC_H
#define _MYIIC_H

#include "sys.h"

#ifdef __cplusplus
 extern "C" {
#endif

////IO��������
//#define SDA1_IN()  {GPIOB->MODER&=~(3<<(9*2));GPIOB->MODER|=0<<9*2;} //PB9����ģʽ
//#define SDA1_OUT() {GPIOB->MODER&=~(3<<(9*2));GPIOB->MODER|=1<<9*2;} //PB9���ģʽ

#define SDA2_IN()  {GPIOB->MODER&=~(3<<(11*2));GPIOB->MODER|=0<<11*2;} //PB11����ģʽ
#define SDA2_OUT() {GPIOB->MODER&=~(3<<(11*2));GPIOB->MODER|=1<<11*2;} //PB11���ģʽ


////���Ŷ���
//#define IIC1_SCL    PBout(8) //SCL
//#define IIC1_SDA    PBout(9) //SDA
//#define READ1_SDA   PBin(9)  //����SDA

#define IIC2_SCL    PBout(10) //SCL
#define IIC2_SDA    PBout(11) //SDA
#define READ2_SDA   PBin(11)  //����SDA


////IIC���в�������
//void IIC1_Init(void);                		              
//void IIC1_Start(void);								//����IIC��ʼ�ź�
//void IIC1_Stop(void);	  						  //����IICֹͣ�ź�
//void IIC1_Send_Byte(u8 txd);					//IIC����һ���ֽ�
//u8 IIC1_Read_Byte(unsigned char ack); //IIC��ȡһ���ֽ�
//u8 IIC1_Wait_Ack(void); 							//IIC�ȴ�ACK�ź�
//void IIC1_Ack(void);									//IIC����ACK�ź�
//void IIC1_NAck(void);								  //IIC������ACK�ź�

void IIC2_Init(void); 
void IIC2_Start(void);								//����IIC��ʼ�ź�
void IIC2_Stop(void);	  						  //����IICֹͣ�ź�
void IIC2_Send_Byte(u8 txd);					//IIC����һ���ֽ�
u8 IIC2_Read_Byte(unsigned char ack); //IIC��ȡһ���ֽ�
u8 IIC2_Wait_Ack(void); 							//IIC�ȴ�ACK�ź�
void IIC2_Ack(void);									//IIC����ACK�ź�
void IIC2_NAck(void);								  //IIC������ACK�ź�

#ifdef __cplusplus
 }
#endif

#endif

