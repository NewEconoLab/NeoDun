
#include "bitBang_I2c.h"
#include "bitBand.h"

extern void __delay_us(uint32_t nTime);// ������hal_stm32_timer.c

/*&&&&&&&&&&&&&&&&&&&        ���Ŷ��� �ļ�����Ч   &&&&&&&&&&&&&&&&&&&&&&&&&&&*/
#define  _I2C_PORT         GPIOB
#define  _SCL_PIN_NO       GPIO_PIN_8
#define  _SDA_PIN_NO       GPIO_PIN_9

#define _SCL          PBout(8)
#define _SDA          PBout(9)
#define _SDA_STATUS   PBin(9)

void bitBang_I2c_Init(void)
{
	//ע��˿�ʱ������ǰʹ��
    GPIO_InitTypeDef GPIO_InitStructure;	
    // Configure I2C pins: SCL and SDA
    GPIO_InitStructure.Pin = _SCL_PIN_NO | _SDA_PIN_NO;
    GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;//GPIO_Speed_50MHz;//Ϊ����EMI����
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_OD; //��©���
		GPIO_InitStructure.Pull  = GPIO_NOPULL;
    HAL_GPIO_Init(_I2C_PORT, &GPIO_InitStructure);
	//�ͷ�SCL SDA
	  _SCL =1;
    _SDA =1;	
}

/*&&&&&&&&&&&&&&&&&&&&&&&    bitBang_i2c      &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&*/

/*&&&&&&&&&&&&&&&&&&&        λ����       &&&&&&&&&&&&&&&&&&&&&&&&&&&*/
#define testbit(var, bit)  ((var) & (1 <<(bit)))
#define setbit(var, bit)   ((var) |= (1 << (bit)))
#define clrbit(var, bit)   ((var) &= ~(1 << (bit)))

/*&&&&&&&&&&&&&&&&&&&        ��ʱ�궨��       &&&&&&&&&&&&&&&&&&&&&&&&&&&*/
#define _1uS   __delay_us(1)
#define _2uS   __delay_us(2)
#define _3uS   __delay_us(3)
#define _4uS   __delay_us(4)
#define _5uS   __delay_us(5)
/*&&&&&&&&&&&&&&&&&&&        ������ַ       &&&&&&&&&&&&&&&&&&&&&&&&&&&*/
#define ATSHA204A_ADDR_W   0xC8
#define ATSHA204A_ADDR_R   0xC9
/*&&&&&&&&&&&&&&&&&&&        ȫ�ֱ�־       &&&&&&&&&&&&&&&&&&&&&&&&&&&*/
static uint32_t ack;	         	//Ӧ���־λ �ڲ�ʹ�� 0��ʾ�յ���Ӧ��1��ʾû�յ�

static void Start_I2c()
{
  _SDA=1;   	//������ʼ�����������ź�
  _1uS;
  _SCL=1;
  _5uS;         //��ʼ��������ʱ�����4.7us,��ʱ
  
  _SDA=0;       //������ʼ�ź�
  _4uS;    	    // ��ʼ��������ʱ�����4��s
       
  _SCL=0;   	//ǯסI2C���ߣ�׼�����ͻ�������� 
  _2uS;
}

static void Stop_I2c()
{
  _SDA=0;  	//���ͽ��������������ź�
  _1uS;   	//���ͽ���������ʱ���ź�
  _SCL=1;  	//������������ʱ�����4��s
  _4uS;
 
  _SDA=1;  	//����I2C���߽����ź�
  _4uS;     //ֹͣ����������֮������߿���ʱ��
}


static void  SendByte(uchar c)
{   //���뺯��2cy
    uchar BitCnt = 9;//2cy
	while (--BitCnt)//2cy,���ж��������״ν����ʱ��ӵ���ʱ
    {
        //�жϷ���λ���ڲ���SDA�ߵ�֮ǰ����3��4cy
        if (testbit(c, 7))  _SDA = 1; //3cy
        else                _SDA = 0; //4cy   
        _1uS;     //���ݽ���ʱ��
        c <<= 1;  //׼����һλ��2cy��ռ��SCL�͵�ƽʱ�䣬�����ݽ���ʱ�䣬��Ϊ������SDA�ȶ�
        _SCL=1;   //��ʱ����Ϊ�ߣ��ͳ�SDA����:֪ͨ��������ʼ��������λ
        _4uS;     //��֤ʱ�Ӹߵ�ƽ���ڴ���4��s       
        _SCL=0;//goto����ѭ����ͷ����һЩָ��ʱ�䣬��ʹSCL�͵�ƽ���ȶ�...Ȼ�����޸�SDA�������ʱ�䲻��̫��
    }          //SCL���ͺ��޸�SDAΪ0��Ϊ1��ʱ��Ϊ���ݱ���ʱ��tHD;STA(�з����)����СֵΪ0��
               //�������ֵ3.45uS������˵���SCL�͵�ƽʱ���ӳ��Ļ����Բ����������ֵ   
    _2uS;
    _SDA=1;                //8λ��������ͷ������ߣ�׼������Ӧ��λ
    _2uS; 
    _SCL=1;
    _3uS;
    if(_SDA_STATUS)    {ack=1;Stop_I2c();return;}//��Ҫ�޸�--��Ӧ��ֱ�ӷ���ֹͣλȻ��������     

    ack=0;//��Ӧ��
    _SCL=0;
    _2uS;
}

static uchar  RcvByte()
{
  uchar retc,BitCnt;

  _SDA=1;            //��������Ϊ���뷽ʽ
  retc=0;//�����ź��棬��_SDA=1�Ժ�һ����ʱ,
         //����SCL���ͺ�(�п������ǵ�)SDA�����ᱻ���������ƣ��п��ܳ��ּ��
  BitCnt = 9;
   
  while (--BitCnt)//4cy,���ж��������״ν����ʱ��ӵ���ʱ
   {
        _1uS;
        _SCL=0; //��ʱ����Ϊ�ͣ�׼����������λ
        _5uS;     //ʱ�ӵ͵�ƽ���ڴ���4.7��s
        _SCL=1; //��ʱ����Ϊ��ʹ��������������Ч/
        _2uS;
        retc <<= 1;//2cy,ע��0 ����LSb
        //retc |= _SDA_STATUS; //4cy������λ,���յ�����λ����retc��
        if(_SDA_STATUS) setbit(retc,0);//2cy������λ,���յ�����λ����retc��
        _2uS;
    }
    
  _SCL=0;// ����ķ���Ӧ����û�н�SCL���;��޸�SDA�ˣ��������ͱ����    
  _2uS;
  return(retc);
}

static void Ack_I2c(uchar a)//0Ӧ��1��Ӧ��
{ 
   _SDA=a; //������ʡ��ָ�ע��ֻȡbit0
  /*if(a==0)  _SDA=0;     	     //�ڴ˷���Ӧ����Ӧ���ź�
    else      _SDA=1;*/
  _3uS;
    
  _SCL=1;   //����Ӧ��λ
  _4uS;                    //ʱ�Ӹߵ�ƽ���ڴ���4��s
  
  _SCL=0;                  //��ʱ���ߣ�ǯסI2C�����Ա��������
  _2uS;
}

/*******************************************************************
                 �ⲿ�û��ӿں���   
*******************************************************************/
void ATSHA_i2c_send_wake_token(void)
{
	_SDA=0;
	__delay_us(80);//���ͳ���tWLO=60uS(��Сֵ)
	_SDA=1;
}

ATCA_STATUS ATSHA_ISendNBytes(uchar *s,uchar no)
{
   Start_I2c();                     //��������
   SendByte( ATSHA204A_ADDR_W );    //����������ַ,д
   if(ack) return(ATCA_TX_TIMEOUT); //��ַ��Ӧ��  //SendByte����һ���ֽ�û�յ�Ӧ���Զ�����ֹͣλ

   while(no !=0 )
   {   
     SendByte(*s);                  //��������
     if(ack) return(ATCA_TX_FAIL);  //������Ӧ�� //SendByte����һ���ֽ�û�յ�Ӧ���Զ�����ֹͣλ
     s++;
     no--;
   } 
   Stop_I2c();                      //�������� 
	 
   return(ATCA_SUCCESS);
}

ATCA_STATUS ATSHA_IRcvNBytes(uchar *s,uchar no)
{
   uint8_t count;
   
   Start_I2c();
   SendByte(ATSHA204A_ADDR_R);       //����������ַ,��
   if(ack) return(ATCA_TX_TIMEOUT);  //��ַ��Ӧ��  //SendByte����һ���ֽ�û�յ�Ӧ���Զ�����ֹͣλ

    *s = RcvByte();//�ȶ���һ���ֽڣ���ʾ�������ص��ֽ���
    Ack_I2c(0);    //������Ӧ��
    
    count = s[0];
    if ( (count < 4) || (count > no) ) //�ֽ�����ƥ��
    {			
        Stop_I2c();
        return (ATCA_INVALID_SIZE); 
    }

   //�������µ��ֽ�
   count--;
   s++;  
   while(count !=0 )
   {  
      *s++=RcvByte(); 
      if(--count) Ack_I2c(0);//�������һ���ֽڣ�������Ӧ��λ,0��ʾ������Ӧ��
      else        Ack_I2c(1);//���һ���ֽڶ��귢�͸�Ӧ��
   }    
	 
   Stop_I2c();               //�������� 
	 
   return(ATCA_SUCCESS);
}


