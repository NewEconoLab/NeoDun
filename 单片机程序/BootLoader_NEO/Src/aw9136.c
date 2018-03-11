#include "aw9136.h"
#include "myiic.h"
#include "main_define.h"
#include <string.h>

/**********************************************************
 Auto Calibration
**********************************************************/
#ifdef AW_AUTO_CALI
#define CALI_NUM			4
#define CALI_RAW_MIN	1000
#define CALI_RAW_MAX	3000
#define CNT_INT				1			//重复进入多少次中断，实际校准的宏，现定为1次

volatile unsigned char aw9136_key_flag = 0;
extern volatile unsigned char touch_motor_flag;           //1表示正在进行测试
#define  MOTOR_TIME  80

unsigned char cali_flag = 0;
unsigned char cali_num = 0;
unsigned char cali_cnt = 0;
unsigned char cali_used = 0;
unsigned char old_cali_dir[6];	//	0: no cali		1: ofr pos cali		2: ofr neg cali
unsigned int  old_ofr_cfg[6];
long Ini_sum[6];

#endif

SIGN_KEY_FLAG Key_Flag;
/**********************************************************
 Touch process variable
**********************************************************/
//static unsigned char suspend_flag = 0 ; //0: normal; 1: sleep
static int debug_level=0;
static int WorkMode = 1 ; //1: sleep, 2: normal

/**********************************************************
//在指定地址读出一个数据
//ReadAddr:开始读数的地址  
//返回值  :读到的数据
**********************************************************/
u16 I2C_read_reg(u8 ReadAddr)
{				  
	u16 temp=0;		  	    																 
  IIC2_Start();  
	IIC2_Send_Byte(0X58);	   			//发送写命令
	IIC2_Wait_Ack();
	IIC2_Send_Byte(ReadAddr);		  //发送地址
	IIC2_Wait_Ack();
	IIC2_Stop();
	IIC2_Start();  	 	   
	IIC2_Send_Byte(0X59);        	//进入接收模式			   
	IIC2_Wait_Ack();	 
  temp=IIC2_Read_Byte(1)<<8;
	temp|=IIC2_Read_Byte(0);
  IIC2_Stop();										//产生一个停止条件	    
	return temp;
}
/**********************************************************
//在指定地址写入一个数据
//WriteAddr  :写入数据的目的地址    
//DataToWrite:要写入的数据
**********************************************************/
void I2C_write_reg(u8 WriteAddr,u16 DataToWrite)
{				   	  	    																 
  IIC2_Start();  
	IIC2_Send_Byte(0X58);	    					//发送写命令
	IIC2_Wait_Ack();
	IIC2_Send_Byte(WriteAddr);						//发送地址 
	IIC2_Wait_Ack(); 	 										  		   
	IIC2_Send_Byte(DataToWrite>>8);//发送高8位字节							   
	IIC2_Wait_Ack();  		    	   
	IIC2_Send_Byte(DataToWrite%256);		//发送低8位字节
	IIC2_Wait_Ack(); 	
  IIC2_Stop();													//产生一个停止条件 
	HAL_Delay(10);	 
}
void Center_button_init(void)
{
		GPIO_InitTypeDef GPIO_Initure;

		__HAL_RCC_GPIOA_CLK_ENABLE();
	
    //PA0  中间按钮PA0 
    GPIO_Initure.Pin=GPIO_PIN_0;
    GPIO_Initure.Mode=GPIO_MODE_IT_FALLING;  		 //下降沿触发
    GPIO_Initure.Pull=GPIO_PULLUP;          		 //上拉
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);		
	
    //中断线0-PA0
    HAL_NVIC_SetPriority(EXTI0_IRQn,2,1);   //抢占优先级为2，子优先级为1
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);         //使能中断线2 

		aw9136_key_flag = 1;
}
/**********************************************************
 PC2   aw9136_INT
 PC3   aw9136_PDN
 PB10  aw9136_SCL
 PB11  aw9136_SDA
**********************************************************/
void AW9136_Init(void)
{
		u16 value1 = 0;
		u16 value2 = 0;
    GPIO_InitTypeDef GPIO_Initure;
 
    __HAL_RCC_GPIOC_CLK_ENABLE();   //使能GPIOC时钟	
    //PC2初始化设置  
    GPIO_Initure.Pin=GPIO_PIN_2;
    GPIO_Initure.Mode=GPIO_MODE_IT_FALLING;  		 //下降沿触发
    GPIO_Initure.Pull=GPIO_PULLUP;          		 //上拉
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);				
	
    //PC3初始化设置  
    GPIO_Initure.Pin=GPIO_PIN_3;
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  		 //推挽输出
    GPIO_Initure.Pull=GPIO_PULLUP;          		 //上拉
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;    			 //快速
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);		
	
    //中断线2-PC2
    HAL_NVIC_SetPriority(EXTI2_IRQn,2,3);   //抢占优先级为2，子优先级为3
    HAL_NVIC_EnableIRQ(EXTI2_IRQn);         //使能中断线2  	
	
		IIC2_Init();	
	
		AW9136_pwron();
		HAL_Delay(5);
		
		value1 = I2C_read_reg(0x00);	
#ifdef Debug_Print		
		printf("AW9136 chip ID:0x%4x\r\n",value1);
#endif		
		AW_NormalMode();
			
#ifdef AW_AUTO_CALI
		cali_flag = 1;
		cali_num = 0;
		cali_cnt = 0;
#endif		

		value2 = I2C_read_reg(0x01);	
#ifdef Debug_Print		
		printf("AW9136 GCR:0x%4x\r\n",value2);	
#endif
		AW9136_LED_ON();
}

void EXTI0_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);//调用中断处理公用函数
}

void EXTI2_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_2);//调用中断处理公用函数
}

/**********************************************************
//中断服务程序中需要做的事情
//在HAL库中所有的外部中断服务函数都会调用此函数
//GPIO_Pin:中断引脚号
**********************************************************/
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
//    HAL_Delay(100);      //消抖
		
		switch (GPIO_Pin)
		{
			case GPIO_PIN_2:
					AW9136_eint_work();
					break;
			case GPIO_PIN_0:				
					Home_Key_press();
					break;
			default:
					break;
		}	

}

void AW9136_pwron(void)
{
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_3,GPIO_PIN_RESET);
		HAL_Delay(5);
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_3,GPIO_PIN_SET);
		HAL_Delay(10);
}

void AW9136_pwroff(void)
{
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_3,GPIO_PIN_RESET);
		HAL_Delay(5);
}

/**********************************************************
 AW9136 initial register @ mobile active
**********************************************************/
void AW_NormalMode(void)
{
	I2C_write_reg(GCR,0x0000);			// disable chip

	// LED config
	I2C_write_reg(LER1,N_LER1);			// LED enable
	I2C_write_reg(LER2,N_LER2);			// LED enable

	//I2C_write_reg(CTRS1,0x0000);	// LED control RAM or I2C
	//I2C_write_reg(CTRS2,0x0000);	// LED control RAM or I2C
	I2C_write_reg(IMAX1,N_IMAX1);		// LED MAX light setting
	I2C_write_reg(IMAX2,N_IMAX2);		// LED MAX light setting
	I2C_write_reg(IMAX3,N_IMAX3);		// LED MAX light setting
	I2C_write_reg(IMAX4,N_IMAX4);		// LED MAX light setting
	I2C_write_reg(IMAX5,N_IMAX5);		// LED MAX light setting

	I2C_write_reg(LCR,N_LCR);				// LED effect control
	I2C_write_reg(IDLECR,N_IDLECR);	// IDLE time setting

	// cap-touch config
	I2C_write_reg(SLPR,N_SLPR);			// touch key enable
	I2C_write_reg(SCFG1,N_SCFG1);		// scan time setting
	I2C_write_reg(SCFG2,N_SCFG2);		// bit0~3 is sense seting

	I2C_write_reg(OFR2,N_OFR2);			// offset
	I2C_write_reg(OFR3,N_OFR3);			// offset

	I2C_write_reg(THR2, N_THR2);		// S1 press thred setting
	I2C_write_reg(THR3, N_THR3);		// S2 press thred setting
	I2C_write_reg(THR4, N_THR4);		// S3 press thred setting

	I2C_write_reg(SETCNT,N_SETCNT); // debounce
	I2C_write_reg(BLCTH,N_BLCTH);		// base trace rate 

	I2C_write_reg(AKSR,N_AKSR);			// AKS 
#ifndef AW_AUTO_CALI
	I2C_write_reg(INTER,N_INTER);	 	// signel click interrupt 
#else
	I2C_write_reg(INTER,0x0080);	 	// frame interrupt 
#endif

	I2C_write_reg(MPTR,N_MPTR);			// Long Press Time 
	I2C_write_reg(GDTR,N_GDTR);			// gesture time setting
	I2C_write_reg(GDCFGR,N_GDCFGR);	// gesture key select
	I2C_write_reg(TAPR1,N_TAPR1);		// double click 1
	I2C_write_reg(TAPR2,N_TAPR2);		// double click 2
	I2C_write_reg(0x22,N_TDTR);			// double click time

#ifndef AW_AUTO_CALI
	I2C_write_reg(GIER,N_GIER);			// gesture and double click enable
#else
	I2C_write_reg(GIER,0x0000);			// gesture and double click disable
#endif	

	I2C_write_reg(GCR,N_GCR);				// LED enable and touch scan enable

	WorkMode = 2;
#ifdef Printf_Debug		
	printf("%s Finish\n", __func__);
#endif
}

/**********************************************************
 AW9136 initial register @ mobile sleep
**********************************************************/
void AW_SleepMode(void)
{
	I2C_write_reg(GCR,0x0000);   		// disable chip

  // LED config
	I2C_write_reg(LER1,S_LER1);			// LED enable
	I2C_write_reg(LER2,S_LER2);			// LED enable

	I2C_write_reg(LCR,S_LCR);				// LED effect control
	I2C_write_reg(IDLECR,S_IDLECR);	// IDLE time setting

	I2C_write_reg(IMAX1,S_IMAX1);		// LED MAX light setting
	I2C_write_reg(IMAX2,S_IMAX2);		// LED MAX light setting
	I2C_write_reg(IMAX3,S_IMAX3);		// LED MAX light setting
	I2C_write_reg(IMAX4,S_IMAX4);		// LED MAX light setting
	I2C_write_reg(IMAX5,S_IMAX5);		// LED MAX light setting

	// cap-touch config
	I2C_write_reg(SLPR,S_SLPR);			// touch key enable

	I2C_write_reg(SCFG1,S_SCFG1);		// scan time setting
	I2C_write_reg(SCFG2,S_SCFG2);		// bit0~3 is sense seting

	I2C_write_reg(OFR2,S_OFR2);			// offset
	I2C_write_reg(OFR3,S_OFR3);			// offset

	I2C_write_reg(THR2, S_THR2);		// S1 press thred setting
	I2C_write_reg(THR3, S_THR3);		// S2 press thred setting
	I2C_write_reg(THR4, S_THR4);		// S3 press thred setting

	I2C_write_reg(SETCNT,S_SETCNT);	// debounce
	I2C_write_reg(IDLECR, S_IDLECR);// idle mode
	I2C_write_reg(BLCTH,S_BLCTH);		// base speed setting

	I2C_write_reg(AKSR,S_AKSR);			// AKS

#ifndef AW_AUTO_CALI
	I2C_write_reg(INTER,S_INTER);	 	// signel click interrupt 
#else
	I2C_write_reg(INTER,0x0080);	 	// signel click interrupt 
#endif

	I2C_write_reg(GDCFGR,S_GDCFGR);	// gesture key select
	I2C_write_reg(TAPR1,S_TAPR1);		// double click 1
	I2C_write_reg(TAPR2,S_TAPR2);		// double click 2

	I2C_write_reg(TDTR,S_TDTR);			// double click time
#ifndef AW_AUTO_CALI
	I2C_write_reg(GIER,S_GIER);			// gesture and double click enable
#else
	I2C_write_reg(GIER,0x0000);			// gesture and double click disable
#endif

	I2C_write_reg(GCR, S_GCR);   		// enable chip sensor function

	WorkMode = 1;
#ifdef Printf_Debug		
	printf("%s Finish\n", __func__);
#endif	
}

void AW9136_LED_ON(void)
{
	I2C_write_reg(IMAX1,0x3300);		// IMAX
	I2C_write_reg(IMAX2,0x0003);		// IMAX
	I2C_write_reg(LER1,0x001c);			// LEDx EN
	I2C_write_reg(CTRS1,0x00fc);		// I2C Control
	I2C_write_reg(CMDR,0xa2ff);			// LED1 ON
	I2C_write_reg(CMDR,0xa3ff);			// LED2 ON
	I2C_write_reg(CMDR,0xa4ff);			// LED3 ON
	I2C_write_reg(GCR,0x0003);			// GCR
}

void AW9136_LED_OFF(void)
{
	I2C_write_reg(IMAX1,0x3300);		// IMAX
	I2C_write_reg(IMAX2,0x0003);		// IMAX
	I2C_write_reg(LER1,0x001c);			// LEDx EN
	I2C_write_reg(CTRS1,0x00fc);		// I2C Control
	I2C_write_reg(CMDR,0xa200);			// LED1 OFF
	I2C_write_reg(CMDR,0xa300);			// LED2 OFF
	I2C_write_reg(CMDR,0xa400);			// LED3 OFF
	I2C_write_reg(GCR,0x0003);			// GCR
}

/**********************************************************
 AW91xx Auto Calibration
**********************************************************/
#ifdef AW_AUTO_CALI
unsigned char AW91xx_Auto_Cali(void)
{
	unsigned char i;
	unsigned char cali_dir[6];

	unsigned int buf[6];
	unsigned int ofr_cfg[6];
	unsigned int sen_num;

//	printf("%s Enter\n", __func__);
	
	if(cali_num == 0){
		ofr_cfg[0] = I2C_read_reg(0x13);
		ofr_cfg[1] = I2C_read_reg(0x14);
		ofr_cfg[2] = I2C_read_reg(0x15);
	}else{
		for(i=0; i<3; i++){
			ofr_cfg[i] = old_ofr_cfg[i];
		}
	}

	I2C_write_reg(0x1e,0x3);
	for(i=0; i<6; i++){
		buf[i] = I2C_read_reg(0x36+i);
	}
	sen_num = I2C_read_reg(0x02);		// SLPR

	for(i=0; i<6; i++) 
		Ini_sum[i] = (cali_cnt==0)? (0) : (Ini_sum[i] + buf[i]);

	if(cali_cnt==CNT_INT){
		for(i=0; i<6; i++){
			if((sen_num & (1<<i)) == 0)	{	// sensor used
				if((Ini_sum[i]>>2) < CALI_RAW_MIN){
					if((i%2) && ((ofr_cfg[i>>1]&0xFF00)==0x1000)){					// 0x10** -> 0x00**
						ofr_cfg[i>>1] = ofr_cfg[i>>1] & 0x00FF;
						cali_dir[i] = 2;
					}else if((i%2) && ((ofr_cfg[i>>1]&0xFF00)==0x0000)){			// 0x00**	no calibration
						cali_dir[i] = 0;
					}else if (((i%2)==0) && ((ofr_cfg[i>>1]&0x00FF)==0x0010)){		// 0x**10 -> 0x**00
						ofr_cfg[i>>1] = ofr_cfg[i>>1] & 0xFF00;
						cali_dir[i] = 2;
					}else if (((i%2)==0) && ((ofr_cfg[i>>1]&0x00FF)==0x0000)){		// 0x**00 no calibration
						cali_dir[i] = 0;
					}else{
						ofr_cfg[i>>1] = ofr_cfg[i>>1] - ((i%2)? (1<<8):1);
						cali_dir[i] = 2;
					}
				}else if((Ini_sum[i]>>2) > CALI_RAW_MAX){
					if((i%2) && ((ofr_cfg[i>>1]&0xFF00)==0x1F00)){	// 0x1F** no calibration
						cali_dir[i] = 0;
					}else if (((i%2)==0) && ((ofr_cfg[i>>1]&0x00FF)==0x001F)){	// 0x**1F no calibration
						cali_dir[i] = 0;
					}else{
						ofr_cfg[i>>1] = ofr_cfg[i>>1] + ((i%2)? (1<<8):1);
						cali_dir[i] = 1;
					}
				}else{
					cali_dir[i] = 0;
				}

				if(cali_num > 0){
					if(cali_dir[i] != old_cali_dir[i]){
						cali_dir[i] = 0;
						ofr_cfg[i>>1] = old_ofr_cfg[i>>1];
					}
				}
			}
		}

		cali_flag = 0;
		for(i=0; i<6; i++){
			if((sen_num & (1<<i)) == 0)	{	// sensor used
				if(cali_dir[i] != 0){
					cali_flag = 1;
				}
			}
		}
		if((cali_flag==0) && (cali_num==0)){
			cali_used = 0;
		}else{
			cali_used = 1;
		}

		if(cali_flag == 0){
			cali_num = 0;
			cali_cnt = 0;
			return 0;
		}

		I2C_write_reg(GCR, 0x0000);
		for(i=0; i<3; i++){
			I2C_write_reg(OFR1+i, ofr_cfg[i]);
		}
		I2C_write_reg(GCR, 0x0003);

		if(cali_num == (CALI_NUM -1)){	// no calibration
			cali_flag = 0;
			cali_num = 0;
			cali_cnt = 0;
			return 0;
		}

		for(i=0; i<6; i++){
			old_cali_dir[i] = cali_dir[i];
		}

		for(i=0; i<3; i++){
			old_ofr_cfg[i] = ofr_cfg[i];
		}

		cali_num ++;
	}

	if(cali_cnt < CNT_INT){
		cali_cnt ++;
	}else{
		cali_cnt = 0;
	}

	return 1;
}
#endif

/**********************************************************
 Function : Cap-touch main program @ mobile sleep 
            wake up after double-click/right_slip/left_slip
**********************************************************/
void AW_SleepMode_Proc(void)
{
	unsigned int buff1;
#ifdef Printf_Debug	
	printf("%s Enter\n", __func__);
#endif
#ifdef AW_AUTO_CALI
	if(cali_flag){
		AW91xx_Auto_Cali();
		if(cali_flag == 0){	
			if(cali_used){
				I2C_write_reg(GCR,0x0000);	 // disable chip
			}
			I2C_write_reg(INTER,S_INTER);
			I2C_write_reg(GIER,S_GIER);
			if(cali_used){
				I2C_write_reg(GCR,S_GCR);	 // enable chip
			}
		}
		return ;
	}
#endif
	
	if(debug_level == 0)
	{
		buff1=I2C_read_reg(0x2e);			//read gesture interupt status
		if(buff1 == 0x10)
		{
				AW_double();
		}
		else if(buff1 == 0x01)
		{
				AW_right_slip();
		}
		else if (buff1 == 0x02)
		{
				AW_left_slip();
		}
	}
}

/**********************************************************
// Function : Cap-touch main pragram @ mobile normal state
//            press/release
**********************************************************/
void AW_NormalMode_Proc(void)
{
		unsigned int buff1,buff2,buff3;
#ifdef Printf_Debug		
		printf("%s Enter\n", __func__);
//		printf("cali_flag = %d\r\n",cali_flag);	
#endif		
#ifdef AW_AUTO_CALI	
		if(cali_flag)
		{
				AW91xx_Auto_Cali();
				if(cali_flag == 0)
				{			
						if(cali_used)
						{
								I2C_write_reg(GCR,0x0000);	 // disable chip
						}
						I2C_write_reg(INTER,N_INTER);
						I2C_write_reg(GIER,N_GIER);
						if(cali_used)
						{
								I2C_write_reg(GCR,N_GCR);	 // enable chip
						}
//						AW_LedReleaseTouch();//add by hkh  在等待cali_flag为0后，再开启按键和LED关联
						aw9136_key_flag = 1;
				}
				return ;
		}
#endif
		if(debug_level == 0)
		{
				buff3=I2C_read_reg(0x2e);
				buff2=I2C_read_reg(0x32);		//read key interupt status
				buff1=I2C_read_reg(0x31);

#ifdef Printf_Debug				
//				printf("AW9136: reg0x31=0x%x, reg0x32=0x%x\n",buff1,buff2);
#endif
				if(buff3 == 0x10)
				{
						AW_double();
				}			
				if(buff2 & 0x10)//S3 click
				{						
						if(buff1 == 0x00)
						{
								AW_left_release();
						}
						else if(buff1 == 0x10)
						{
								AW_left_press();
						}
				}
				else if(buff2 & 0x08)//S2 click  实际为右键按下
				{					
						if(buff1 == 0x00)
						{
								AW_center_release();
						}
						else if (buff1 == 0x08)
						{
								AW_center_press();
						}
				}
				else if(buff2 & 0x04)//S1 click  实际为左键按下
				{					
						if(buff1 == 0x00)
						{
								 AW_right_release();
						}
						else if(buff1 == 0x04)
						{
							 AW_right_press();
						}
				}
		}
}

int AW9136_clear_intr(void) 
{
	int res;
	res = I2C_read_reg(0x32);
#ifdef Printf_Debug		
//	printf("%s: reg0x32=0x%x\n", __func__, res);
#endif	
	return res;
}

/**********************************************************
// Function : Interrupt sub-program
//            work in AW_SleepMode_Proc() or 
//            AW_NormalMode_Proc()
**********************************************************/
void AW9136_eint_work(void)
{
#ifdef Printf_Debug	
//	printf("%s Enter\n", __func__);
#endif
	switch(WorkMode){
		case 1:
			AW_SleepMode_Proc();
			break;
		case 2:
			AW_NormalMode_Proc();
			break;
		default:
			break;
	}

	AW9136_clear_intr();
}

void AW_left_press(void)
{
#ifdef Debug_Print		
		printf("AW9136 left press \n");
#endif	
}

void AW_left_release(void)
{
#ifdef Debug_Print		
		printf("AW9136 left release\n");
#endif	
}

void AW_center_press(void)
{		
		if(aw9136_key_flag&&Key_Flag.Key_Control_Flag)
		{
#ifdef Debug_Print				
				printf("AW9136 right press \n");
#endif			
				Key_Flag.Key_right_Flag = 1;
		}
		if(touch_motor_flag)
				Motor_touch(MOTOR_TIME);
}

void AW_center_release(void)
{
		if(aw9136_key_flag&&Key_Flag.Key_Control_Flag)
		{
#ifdef Debug_Print				
				printf("AW9136 right release \n");
#endif			
//				Key_Flag.Key_right_Flag = 1;
		}
}

void AW_right_press(void)
{
		if(aw9136_key_flag&&Key_Flag.Key_Control_Flag)
		{
#ifdef Debug_Print				
				printf("AW9136 left press \n");
#endif			
				Key_Flag.Key_left_Flag = 1;
		}
		if(touch_motor_flag)
				Motor_touch(MOTOR_TIME);
}
		
void AW_right_release(void)
{
		if(aw9136_key_flag&&Key_Flag.Key_Control_Flag)
		{
#ifdef Debug_Print				
				printf("AW9136 left release \n");
#endif			
//				Key_Flag.Key_left_Flag = 1;
		}
}

void AW_double(void)
{
		if(aw9136_key_flag&&Key_Flag.Key_Control_Flag)			
		{		
#ifdef Debug_Print				
				printf("AW9136 center double click \n");
#endif			
				Key_Flag.Key_double_Flag = 1;
		}
}

void AW_right_slip(void)
{
#ifdef Debug_Print		
		printf("AW9136 right slip \n");
#endif
}

void AW_left_slip(void)
{
#ifdef Debug_Print		
		printf("AW9136 left slip \n");
#endif	
}

void Home_Key_press(void)
{	
		if(aw9136_key_flag&&Key_Flag.Key_Control_Flag)			
		{		
#ifdef Debug_Print			
				printf("中间按键按下！！！\n");
#endif			
				Key_Flag.Key_center_Flag = 1;
		}
		if(touch_motor_flag)
				Motor_touch(MOTOR_TIME);
}

/*********************************
功能：让按键s1-s3，响应led1-led3
*********************************/
void AW_LedReleaseTouch(void)
{
	I2C_write_reg(IMAX1,0x3300);		// IMAX
	I2C_write_reg(IMAX2,0x0003);		// IMAX
	I2C_write_reg(LER1,0x001c);			// LEDx EN
	I2C_write_reg(CTRS1,0x00fc);		// I2C Control	
	I2C_write_reg(0x05,0x8382);		
	I2C_write_reg(0x06,0x0084);			
	I2C_write_reg(GCR,0x0003);			// GCR
}

/*********************************
value:
	0		关闭按键实际功能，关灯
	1		打开按键实际功能，开灯
*********************************/
void Key_Control(unsigned char value)
{
		memset(&Key_Flag,0,sizeof(Key_Flag));	
		if(value == 1)
		{
				AW9136_LED_ON();					
				Key_Flag.Key_Control_Flag = 1;
		}
		else
		{
				AW9136_LED_OFF();
				Key_Flag.Key_Control_Flag = 0;
		}
}	

void Motor_touch(int time)
{
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
		HAL_Delay(time);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);	
}

