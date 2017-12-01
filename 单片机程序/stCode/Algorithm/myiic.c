#include "myiic.h"

//void delay_us(int value)
//{
//		int t = 7;
//		for(;value>0;value--)
//				for(;t>0;t--);
//}

void delay_us(int dwTime)
{
    uint32_t dwCurCounter=0;                                //��ǰʱ�����ֵ
    uint32_t dwPreTickVal=SysTick->VAL;                     //��һ��SYSTICK����ֵ
    uint32_t dwCurTickVal;                                  //��һ��SYSTICK����ֵ
    dwTime=dwTime*(HAL_RCC_GetHCLKFreq()/1000000);    			//����ʱʱ�䣬������ʱ�����
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

////IIC1��ʼ��
//void IIC1_Init(void)
//{
//    GPIO_InitTypeDef GPIO_Initure;
//    
//    __HAL_RCC_GPIOB_CLK_ENABLE();   //ʹ��GPIOBʱ��
//    
//    //PB8,9��ʼ������
//    GPIO_Initure.Pin=GPIO_PIN_8|GPIO_PIN_9;
//    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  		 //�������
//    GPIO_Initure.Pull=GPIO_PULLUP;          		 //����
//    GPIO_Initure.Speed=GPIO_SPEED_HIGH;    			 //����
//    HAL_GPIO_Init(GPIOB,&GPIO_Initure);
//    
//    IIC1_SDA=1;
//    IIC1_SCL=1;  
//}
////����IIC��ʼ�ź�
//void IIC1_Start(void)
//{
//		SDA1_OUT();     //sda�����
//		IIC1_SDA=1;	  	  
//		IIC1_SCL=1;
//		delay_us(4);
//		IIC1_SDA=0;//START:when CLK is high,DATA change form high to low 
//		delay_us(4);
//		IIC1_SCL=0;//ǯסI2C���ߣ�׼�����ͻ�������� 
//}	  
////����IICֹͣ�ź�
//void IIC1_Stop(void)
//{
//		SDA1_OUT();//sda�����
//		IIC1_SCL=0;
//		IIC1_SDA=0;//STOP:when CLK is high DATA change form low to high
//		delay_us(4);
//		IIC1_SCL=1; 
//		IIC1_SDA=1;//����I2C���߽����ź�
//		delay_us(4);							   	
//}
////�ȴ�Ӧ���źŵ���
////����ֵ��1������Ӧ��ʧ��
////        0������Ӧ��ɹ�
//u8 IIC1_Wait_Ack(void)
//{
//		u8 ucErrTime=0;
//		SDA1_IN();      //SDA����Ϊ����  
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
//		IIC1_SCL=0;//ʱ�����0 	   
//		return 0;  
//} 
////����ACKӦ��
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
////������ACKӦ��		    
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
////IIC����һ���ֽ�
////���شӻ�����Ӧ��
////1����Ӧ��
////0����Ӧ��			  
//void IIC1_Send_Byte(u8 txd)
//{                        
//    u8 t;   
//		SDA1_OUT(); 	    
//    IIC1_SCL=0;//����ʱ�ӿ�ʼ���ݴ���
//    for(t=0;t<8;t++)
//    {              
//        IIC1_SDA=(txd&0x80)>>7;
//        txd<<=1; 	  
//				delay_us(2);   //��TEA5767��������ʱ���Ǳ����
//				IIC1_SCL=1;
//				delay_us(2); 
//				IIC1_SCL=0;	
//				delay_us(2);
//    }	 
//} 	    
////��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
//u8 IIC1_Read_Byte(unsigned char ack)
//{
//		unsigned char i,receive=0;
//		SDA1_IN();//SDA����Ϊ����
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
//        IIC1_NAck();//����nACK
//    else
//        IIC1_Ack(); //����ACK   
//    return receive;
//}

//IIC2��ʼ��
void IIC2_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    
//    __HAL_RCC_GPIOB_CLK_ENABLE();   //ʹ��GPIOBʱ��
    
    //PB10,11��ʼ������
    GPIO_Initure.Pin=GPIO_PIN_10|GPIO_PIN_11;
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  		 //�������
    GPIO_Initure.Pull=GPIO_PULLUP;          		 //����
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;    			 //����
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);
    
    IIC2_SDA=1;
    IIC2_SCL=1;  
}
//����IIC��ʼ�ź�
void IIC2_Start(void)
{
		SDA2_OUT();     //sda�����
		IIC2_SDA=1;	  	  
		IIC2_SCL=1;
		delay_us(4);
		IIC2_SDA=0;//START:when CLK is high,DATA change form high to low 
		delay_us(4);
		IIC2_SCL=0;//ǯסI2C���ߣ�׼�����ͻ�������� 
}	  
//����IICֹͣ�ź�
void IIC2_Stop(void)
{
		SDA2_OUT();//sda�����
		IIC2_SCL=0;
		IIC2_SDA=0;//STOP:when CLK is high DATA change form low to high
		delay_us(4);
		IIC2_SCL=1; 
		IIC2_SDA=1;//����I2C���߽����ź�
		delay_us(4);							   	
}
//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
u8 IIC2_Wait_Ack(void)
{
		u8 ucErrTime=0;
		SDA2_IN();      //SDA����Ϊ����  
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
		IIC2_SCL=0;//ʱ�����0 	   
		return 0;  
} 
//����ACKӦ��
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
//������ACKӦ��		    
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
//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��			  
void IIC2_Send_Byte(u8 txd)
{                        
    u8 t;   
		SDA2_OUT(); 	    
    IIC2_SCL=0;//����ʱ�ӿ�ʼ���ݴ���
    for(t=0;t<8;t++)
    {              
        IIC2_SDA=(txd&0x80)>>7;
        txd<<=1; 	  
				delay_us(2);   //��TEA5767��������ʱ���Ǳ����
				IIC2_SCL=1;
				delay_us(2); 
				IIC2_SCL=0;	
				delay_us(2);
    }	 
} 	    
//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
u8 IIC2_Read_Byte(unsigned char ack)
{
		unsigned char i,receive=0;
		SDA2_IN();//SDA����Ϊ����
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
        IIC2_NAck();//����nACK
    else
        IIC2_Ack(); //����ACK   
    return receive;
}


