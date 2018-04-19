
#include "bitBang_I2c.h"
#include "bitBand.h"

extern void __delay_us(uint32_t nTime);// 定义在hal_stm32_timer.c

/*&&&&&&&&&&&&&&&&&&&        引脚定义 文件内有效   &&&&&&&&&&&&&&&&&&&&&&&&&&&*/
#define  _I2C_PORT         GPIOB
#define  _SCL_PIN_NO       GPIO_PIN_8
#define  _SDA_PIN_NO       GPIO_PIN_9

#define _SCL          PBout(8)
#define _SDA          PBout(9)
#define _SDA_STATUS   PBin(9)

void bitBang_I2c_Init(void)
{
	//注意端口时钟已提前使能
    GPIO_InitTypeDef GPIO_InitStructure;	
    // Configure I2C pins: SCL and SDA
    GPIO_InitStructure.Pin = _SCL_PIN_NO | _SDA_PIN_NO;
    GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;//GPIO_Speed_50MHz;//为减少EMI噪声
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_OD; //开漏输出
		GPIO_InitStructure.Pull  = GPIO_NOPULL;
    HAL_GPIO_Init(_I2C_PORT, &GPIO_InitStructure);
	//释放SCL SDA
	  _SCL =1;
    _SDA =1;	
}

/*&&&&&&&&&&&&&&&&&&&&&&&    bitBang_i2c      &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&*/

/*&&&&&&&&&&&&&&&&&&&        位操作       &&&&&&&&&&&&&&&&&&&&&&&&&&&*/
#define testbit(var, bit)  ((var) & (1 <<(bit)))
#define setbit(var, bit)   ((var) |= (1 << (bit)))
#define clrbit(var, bit)   ((var) &= ~(1 << (bit)))

/*&&&&&&&&&&&&&&&&&&&        延时宏定义       &&&&&&&&&&&&&&&&&&&&&&&&&&&*/
#define _1uS   __delay_us(1)
#define _2uS   __delay_us(2)
#define _3uS   __delay_us(3)
#define _4uS   __delay_us(4)
#define _5uS   __delay_us(5)
/*&&&&&&&&&&&&&&&&&&&        器件地址       &&&&&&&&&&&&&&&&&&&&&&&&&&&*/
#define ATSHA204A_ADDR_W   0xC8
#define ATSHA204A_ADDR_R   0xC9
/*&&&&&&&&&&&&&&&&&&&        全局标志       &&&&&&&&&&&&&&&&&&&&&&&&&&&*/
static uint32_t ack;	         	//应答标志位 内部使用 0表示收到了应答，1表示没收到

static void Start_I2c()
{
  _SDA=1;   	//发送起始条件的数据信号
  _1uS;
  _SCL=1;
  _5uS;         //起始条件建立时间大于4.7us,延时
  
  _SDA=0;       //发送起始信号
  _4uS;    	    // 起始条件锁定时间大于4μs
       
  _SCL=0;   	//钳住I2C总线，准备发送或接收数据 
  _2uS;
}

static void Stop_I2c()
{
  _SDA=0;  	//发送结束条件的数据信号
  _1uS;   	//发送结束条件的时钟信号
  _SCL=1;  	//结束条件建立时间大于4μs
  _4uS;
 
  _SDA=1;  	//发送I2C总线结束信号
  _4uS;     //停止和启动条件之间的总线空闲时间
}


static void  SendByte(uchar c)
{   //进入函数2cy
    uchar BitCnt = 9;//2cy
	while (--BitCnt)//2cy,先判断有利于首次进入的时候加点延时
    {
        //判断发送位，在操作SDA高低之前，有3或4cy
        if (testbit(c, 7))  _SDA = 1; //3cy
        else                _SDA = 0; //4cy   
        _1uS;     //数据建立时间
        c <<= 1;  //准备下一位，2cy，占用SCL低电平时间，及数据建立时间，认为有利于SDA稳定
        _SCL=1;   //置时钟线为高，送出SDA；即:通知被控器开始接收数据位
        _4uS;     //保证时钟高电平周期大于4μs       
        _SCL=0;//goto加上循环体头部有一些指令时间，可使SCL低电平便稳定...然后再修改SDA，但这个时间不能太长
    }          //SCL拉低后到修改SDA为0或为1的时间为数据保持时间tHD;STA(叫法奇怪)，最小值为0，
               //但有最大值3.45uS，但据说如果SCL低电平时间延长的话可以不必遵守最大值   
    _2uS;
    _SDA=1;                //8位发送完后释放数据线，准备接收应答位
    _2uS; 
    _SCL=1;
    _3uS;
    if(_SDA_STATUS)    {ack=1;Stop_I2c();return;}//重要修改--无应答直接发送停止位然后函数返回     

    ack=0;//有应答
    _SCL=0;
    _2uS;
}

static uchar  RcvByte()
{
  uchar retc,BitCnt;

  _SDA=1;            //置数据线为输入方式
  retc=0;//把这句放后面，给_SDA=1以后一点延时,
         //否则SCL拉低后(有可能已是低)SDA立即会被子器件控制，有可能出现尖峰
  BitCnt = 9;
   
  while (--BitCnt)//4cy,先判断有利于首次进入的时候加点延时
   {
        _1uS;
        _SCL=0; //置时钟线为低，准备接收数据位
        _5uS;     //时钟低电平周期大于4.7μs
        _SCL=1; //置时钟线为高使数据线上数据有效/
        _2uS;
        retc <<= 1;//2cy,注意0 移入LSb
        //retc |= _SDA_STATUS; //4cy读数据位,接收的数据位放入retc中
        if(_SDA_STATUS) setbit(retc,0);//2cy读数据位,接收的数据位放入retc中
        _2uS;
    }
    
  _SCL=0;// 后面的发送应答函数没有将SCL拉低就修改SDA了，在这拉低必须的    
  _2uS;
  return(retc);
}

static void Ack_I2c(uchar a)//0应答，1非应答
{ 
   _SDA=a; //比下面省俩指令，注意只取bit0
  /*if(a==0)  _SDA=0;     	     //在此发出应答或非应答信号
    else      _SDA=1;*/
  _3uS;
    
  _SCL=1;   //发出应答位
  _4uS;                    //时钟高电平周期大于4μs
  
  _SCL=0;                  //清时钟线，钳住I2C总线以便继续接收
  _2uS;
}

/*******************************************************************
                 外部用户接口函数   
*******************************************************************/
void ATSHA_i2c_send_wake_token(void)
{
	_SDA=0;
	__delay_us(80);//拉低超过tWLO=60uS(最小值)
	_SDA=1;
}

ATCA_STATUS ATSHA_ISendNBytes(uchar *s,uchar no)
{
   Start_I2c();                     //启动总线
   SendByte( ATSHA204A_ADDR_W );    //发送器件地址,写
   if(ack) return(ATCA_TX_TIMEOUT); //地址无应答  //SendByte发送一个字节没收到应答自动发送停止位

   while(no !=0 )
   {   
     SendByte(*s);                  //发送数据
     if(ack) return(ATCA_TX_FAIL);  //数据无应答 //SendByte发送一个字节没收到应答自动发送停止位
     s++;
     no--;
   } 
   Stop_I2c();                      //结束总线 
	 
   return(ATCA_SUCCESS);
}

ATCA_STATUS ATSHA_IRcvNBytes(uchar *s,uchar no)
{
   uint8_t count;
   
   Start_I2c();
   SendByte(ATSHA204A_ADDR_R);       //发送器件地址,读
   if(ack) return(ATCA_TX_TIMEOUT);  //地址无应答  //SendByte发送一个字节没收到应答自动发送停止位

    *s = RcvByte();//先读第一个字节，表示器件返回的字节数
    Ack_I2c(0);    //发送正应答
    
    count = s[0];
    if ( (count < 4) || (count > no) ) //字节数不匹配
    {			
        Stop_I2c();
        return (ATCA_INVALID_SIZE); 
    }

   //接收余下的字节
   count--;
   s++;  
   while(count !=0 )
   {  
      *s++=RcvByte(); 
      if(--count) Ack_I2c(0);//不是最后一个字节，发送正应答位,0表示发送正应答
      else        Ack_I2c(1);//最后一个字节读完发送负应答
   }    
	 
   Stop_I2c();               //结束总线 
	 
   return(ATCA_SUCCESS);
}


