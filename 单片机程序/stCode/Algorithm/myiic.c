#include "myiic.h"

//void delay_us(int value)
//{
//		int t = 7;
//		for(;value>0;value--)
//				for(;t>0;t--);
//}

void delay_us(int dwTime)
{
    uint32_t dwCurCounter=0;                                //当前时间计数值
    uint32_t dwPreTickVal=SysTick->VAL;                     //上一次SYSTICK计数值
    uint32_t dwCurTickVal;                                  //上一次SYSTICK计数值
    dwTime=dwTime*(HAL_RCC_GetHCLKFreq()/1000000);    			//需延时时间，共多少时间节拍
    for(;;)
		{
        dwCurTickVal=SysTick->VAL;
        if(dwCurTickVal<dwPreTickVal)
				{
            dwCurCounter=dwCurCounter+dwPreTickVal-dwCurTickVal;
        }
        else
				{
            dwCurCounter=dwCurCounter+dwPreTickVal+SysTick->LOAD-dwCurTickVal;
        }
        dwPreTickVal=dwCurTickVal;
        if(dwCurCounter>=dwTime)
				{
            return;
        }
    }
}

////IIC1初始化
//void IIC1_Init(void)
//{
//    GPIO_InitTypeDef GPIO_Initure;
//    
//    __HAL_RCC_GPIOB_CLK_ENABLE();   //使能GPIOB时钟
//    
//    //PB8,9初始化设置
//    GPIO_Initure.Pin=GPIO_PIN_8|GPIO_PIN_9;
//    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  		 //推挽输出
//    GPIO_Initure.Pull=GPIO_PULLUP;          		 //上拉
//    GPIO_Initure.Speed=GPIO_SPEED_HIGH;    			 //快速
//    HAL_GPIO_Init(GPIOB,&GPIO_Initure);
//    
//    IIC1_SDA=1;
//    IIC1_SCL=1;  
//}
////产生IIC起始信号
//void IIC1_Start(void)
//{
//		SDA1_OUT();     //sda线输出
//		IIC1_SDA=1;	  	  
//		IIC1_SCL=1;
//		delay_us(4);
//		IIC1_SDA=0;//START:when CLK is high,DATA change form high to low 
//		delay_us(4);
//		IIC1_SCL=0;//钳住I2C总线，准备发送或接收数据 
//}	  
////产生IIC停止信号
//void IIC1_Stop(void)
//{
//		SDA1_OUT();//sda线输出
//		IIC1_SCL=0;
//		IIC1_SDA=0;//STOP:when CLK is high DATA change form low to high
//		delay_us(4);
//		IIC1_SCL=1; 
//		IIC1_SDA=1;//发送I2C总线结束信号
//		delay_us(4);							   	
//}
////等待应答信号到来
////返回值：1，接收应答失败
////        0，接收应答成功
//u8 IIC1_Wait_Ack(void)
//{
//		u8 ucErrTime=0;
//		SDA1_IN();      //SDA设置为输入  
//		IIC1_SDA=1;delay_us(1);	   
//		IIC1_SCL=1;delay_us(1);	 
//		while(READ1_SDA)
//		{
//				ucErrTime++;
//				if(ucErrTime>250)
//				{
//						IIC1_Stop();
//						return 1;
//				}
//		}
//		IIC1_SCL=0;//时钟输出0 	   
//		return 0;  
//} 
////产生ACK应答
//void IIC1_Ack(void)
//{
//		IIC1_SCL=0;
//		SDA1_OUT();
//		IIC1_SDA=0;
//		delay_us(2);
//		IIC1_SCL=1;
//		delay_us(2);
//		IIC1_SCL=0;
//}
////不产生ACK应答		    
//void IIC1_NAck(void)
//{
//		IIC1_SCL=0;
//		SDA1_OUT();
//		IIC1_SDA=1;
//		delay_us(2);
//		IIC1_SCL=1;
//		delay_us(2);
//		IIC1_SCL=0;
//}					 				     
////IIC发送一个字节
////返回从机有无应答
////1，有应答
////0，无应答			  
//void IIC1_Send_Byte(u8 txd)
//{                        
//    u8 t;   
//		SDA1_OUT(); 	    
//    IIC1_SCL=0;//拉低时钟开始数据传输
//    for(t=0;t<8;t++)
//    {              
//        IIC1_SDA=(txd&0x80)>>7;
//        txd<<=1; 	  
//				delay_us(2);   //对TEA5767这三个延时都是必须的
//				IIC1_SCL=1;
//				delay_us(2); 
//				IIC1_SCL=0;	
//				delay_us(2);
//    }	 
//} 	    
////读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
//u8 IIC1_Read_Byte(unsigned char ack)
//{
//		unsigned char i,receive=0;
//		SDA1_IN();//SDA设置为输入
//    for(i=0;i<8;i++ )
//		{
//        IIC1_SCL=0; 
//        delay_us(2);
//				IIC1_SCL=1;
//        receive<<=1;
//        if(READ1_SDA)receive++;   
//				delay_us(1); 
//    }					 
//    if (!ack)
//        IIC1_NAck();//发送nACK
//    else
//        IIC1_Ack(); //发送ACK   
//    return receive;
//}

//IIC2初始化
void IIC2_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    
//    __HAL_RCC_GPIOB_CLK_ENABLE();   //使能GPIOB时钟
    
    //PB10,11初始化设置
    GPIO_Initure.Pin=GPIO_PIN_10|GPIO_PIN_11;
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  		 //推挽输出
    GPIO_Initure.Pull=GPIO_PULLUP;          		 //上拉
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;    			 //快速
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);
    
    IIC2_SDA=1;
    IIC2_SCL=1;  
}
//产生IIC起始信号
void IIC2_Start(void)
{
		SDA2_OUT();     //sda线输出
		IIC2_SDA=1;	  	  
		IIC2_SCL=1;
		delay_us(4);
		IIC2_SDA=0;//START:when CLK is high,DATA change form high to low 
		delay_us(4);
		IIC2_SCL=0;//钳住I2C总线，准备发送或接收数据 
}	  
//产生IIC停止信号
void IIC2_Stop(void)
{
		SDA2_OUT();//sda线输出
		IIC2_SCL=0;
		IIC2_SDA=0;//STOP:when CLK is high DATA change form low to high
		delay_us(4);
		IIC2_SCL=1; 
		IIC2_SDA=1;//发送I2C总线结束信号
		delay_us(4);							   	
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
u8 IIC2_Wait_Ack(void)
{
		u8 ucErrTime=0;
		SDA2_IN();      //SDA设置为输入  
		IIC2_SDA=1;delay_us(1);	   
		IIC2_SCL=1;delay_us(1);	 
		while(READ2_SDA)
		{
				ucErrTime++;
				if(ucErrTime>250)
				{
						IIC2_Stop();
						return 1;
				}
		}
		IIC2_SCL=0;//时钟输出0 	   
		return 0;  
} 
//产生ACK应答
void IIC2_Ack(void)
{
		IIC2_SCL=0;
		SDA2_OUT();
		IIC2_SDA=0;
		delay_us(2);
		IIC2_SCL=1;
		delay_us(2);
		IIC2_SCL=0;
}
//不产生ACK应答		    
void IIC2_NAck(void)
{
		IIC2_SCL=0;
		SDA2_OUT();
		IIC2_SDA=1;
		delay_us(2);
		IIC2_SCL=1;
		delay_us(2);
		IIC2_SCL=0;
}					 				     
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void IIC2_Send_Byte(u8 txd)
{                        
    u8 t;   
		SDA2_OUT(); 	    
    IIC2_SCL=0;//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
        IIC2_SDA=(txd&0x80)>>7;
        txd<<=1; 	  
				delay_us(2);   //对TEA5767这三个延时都是必须的
				IIC2_SCL=1;
				delay_us(2); 
				IIC2_SCL=0;	
				delay_us(2);
    }	 
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
u8 IIC2_Read_Byte(unsigned char ack)
{
		unsigned char i,receive=0;
		SDA2_IN();//SDA设置为输入
    for(i=0;i<8;i++ )
		{
        IIC2_SCL=0; 
        delay_us(2);
				IIC2_SCL=1;
        receive<<=1;
        if(READ2_SDA)receive++;   
				delay_us(1); 
    }					 
    if (!ack)
        IIC2_NAck();//发送nACK
    else
        IIC2_Ack(); //发送ACK   
    return receive;
}


