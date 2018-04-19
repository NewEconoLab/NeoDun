#ifndef _MYIIC_H
#define _MYIIC_H

#include "sys.h"


////IO方向设置
//#define SDA1_IN()  {GPIOB->MODER&=~(3<<(9*2));GPIOB->MODER|=0<<9*2;} //PB9输入模式
//#define SDA1_OUT() {GPIOB->MODER&=~(3<<(9*2));GPIOB->MODER|=1<<9*2;} //PB9输出模式

#define SDA2_IN()  {GPIOB->MODER&=~(3<<(11*2));GPIOB->MODER|=0<<11*2;} //PB11输入模式
#define SDA2_OUT() {GPIOB->MODER&=~(3<<(11*2));GPIOB->MODER|=1<<11*2;} //PB11输出模式


////引脚定义
//#define IIC1_SCL    PBout(8) //SCL
//#define IIC1_SDA    PBout(9) //SDA
//#define READ1_SDA   PBin(9)  //输入SDA

#define IIC2_SCL    PBout(10) //SCL
#define IIC2_SDA    PBout(11) //SDA
#define READ2_SDA   PBin(11)  //输入SDA


////IIC所有操作函数
//void IIC1_Init(void);                		              
//void IIC1_Start(void);								//发送IIC开始信号
//void IIC1_Stop(void);	  						  //发送IIC停止信号
//void IIC1_Send_Byte(uint8_t txd);					//IIC发送一个字节
//uint8_t IIC1_Read_Byte(uint8_t ack); //IIC读取一个字节
//uint8_t IIC1_Wait_Ack(void); 							//IIC等待ACK信号
//void IIC1_Ack(void);									//IIC发送ACK信号
//void IIC1_NAck(void);								  //IIC不发送ACK信号

void IIC2_Init(void); 
void IIC2_Start(void);								//发送IIC开始信号
void IIC2_Stop(void);	  						  //发送IIC停止信号
void IIC2_Send_Byte(uint8_t txd);					//IIC发送一个字节
uint8_t IIC2_Read_Byte(uint8_t ack); //IIC读取一个字节
uint8_t IIC2_Wait_Ack(void); 							//IIC等待ACK信号
void IIC2_Ack(void);									//IIC发送ACK信号
void IIC2_NAck(void);								  //IIC不发送ACK信号

#endif

