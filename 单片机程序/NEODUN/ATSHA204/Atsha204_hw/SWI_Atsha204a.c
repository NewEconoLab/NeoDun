
#include "bitBand.h"
#include "SWI_Atsha204a.h"

/* * * * * * * ����ע�ͽ����ο� * * * * * * */

extern void __delay_us(uint32_t nTime);        // ������hal_stm32_timer.c
extern void __delay_counter(uint32_t counter); // ������hal_stm32_timer.c

/*&&&&&&&&&&&&&&&&&&&        ���Ŷ��� �ļ�����Ч     &&&&&&&&&&&&&&&&&&&&&&&&&&&*/
#define  _SDA              PBout(9)
#define  _SDA_STATUS       PBin(9)

void SWI_Init(void)   
{
	//����Ϊ�գ�ϵͳ����bitBang_I2c_Init()ͬʱҲ��SWI��ʼ����,SDA���Ÿ���.
}

/*&&&&&&&&&&&&&&&&&&&        λ����       &&&&&&&&&&&&&&&&&&&&&&&&&&&*/
#define testbit(var, bit)  ((var) & (1 <<(bit)))
#define setbit(var, bit)   ((var) |= (1 << (bit)))
#define clrbit(var, bit)   ((var) &= ~(1 << (bit)))

//ʱ�䶨��

/*ʵ��ֵ�����ο�*/

/*ʵ��
264	119.1	4.198152813
266	119.1	4.198152813
268	119.1	4.198152813
270	115.7	4.321521175
272	115.7	4.321521175
����ֵ��4.34-5/9����72 = 272.48
*/
#define   _4_34_uS        __delay_counter(266)//100 - 390

/*ʵ��
2445	14.51	34.4589938
2448	14.46	34.57814661
2450	14.46	34.57814661
2455	14.46	34.57814661
2456	14.46	34.57814661
2457	14.4	34.72222222
����ֵ��34.66-5/9����72 = 2455.52
*/
#define   BIT_DELAY_7     __delay_counter(2450)//����ֵTbit - Tstart = 39-4.34 = 34.66

/*ʵ�� 
1826	19.36	25.82644628
1828	19.26	25.96053998
1830	19.26	25.96053998
����ֵ��25.98-5/9����72 = 1830.56
*/
#define   BIT_DELAY_5     __delay_counter(1824)//����ֵTbit - Tstart*3 = 39-4.34*3 = 25.98

void swi_send_wake_token(void)
{  
    _SDA = 0;
    __delay_us(65);//���ͳ���tWLO=60uS(��Сֵ)
    _SDA = 1;
}

static void swi_send_byte(uint8_t byte)
{
	//�����ڲ�ʹ�ã�����û�н����жϣ������߸���
  //���ͺ���ʹ��72M�߷ֱ�����ʱ����__delay_counter()
	 uint8_t BitCnt = 8;

   do	
    {		
        //�ȷ�LSB
        if (testbit(byte, 0)) //!< Send Logic 1 (7F)
        {

            _SDA = 0;
            _4_34_uS;
            _SDA = 1;
            BIT_DELAY_7;//����ֵTbit - Tstart = 39-4.34 = 34.66
            //λ���ͼ�϶������Ҫ**uS,��˴˴���ʱΪ **uS,��û��ϵ��������ע��
        } 
        else                 //!< Send Logic 0 (7D)
        {
            _SDA = 0;
            _4_34_uS;
            _SDA = 1;
            _4_34_uS;
            _SDA = 0;
            _4_34_uS;
            _SDA = 1;
            BIT_DELAY_5;//����ֵTbit - Tstart*3 = 39-4.34*3 = 25.98
            //λ���ͼ�϶������Ҫ**uS,��˴˴���ʱΪ **uS,��û��ϵ��������ע��
        }
    			/*
    λ���ͼ�϶��׼����һλ��ѭ���ж���Ҫ?? uS   
    ��û��ϵ�����ֲ�7.3.1��35ҳ��Tbit������ע�⣺
    If the bit time exceeds tTIMEOUT(45-65-85mS), then the ATSHA204A may enter the sleep state.
    Ҳ��������������һ������λ��(�൱��8��uart��bit)��оƬ��ȴ��������ͺ��淢�ͽ�����������λ������ȴ�ʱ��Ӧ�ü���Ϊ��
        tTIMEOUT - Tbit = 45mS - Լ39uS(ʵ�ʿ��������������ʵ�ʵķ���һ������λ����ʱ��)�൱����СֵΪ45��ms��
    �������ʱ�䣬оƬ�Ż�������ߣ�
				����5.3.1��22ҳ
				5.3.1 I/O Timeout
         After a leading transition for any data token has been received, the ATSHA204A will expect the remaining bits of
         the token to be properly received by the device within the tTIMEOUT interval. Failure to send enough bits or the
         transmission of an illegal token (a low pulse exceeding tZLO) will cause the device to enter the sleep state after
         the tTIMEOUT interval.
         The same timeout applies during the transmission of the command block. After the transmission of a legal
         command flag, the I/O timeout circuitry is enabled until the last expected data bit is received.
         Note: The timeout counter is reset after every legal token, and the total time to transmit the command may
         exceed the tTIMEOUT interval while the time between bits may not.
         The I/O timeout circuitry is disabled when the device is busy executing a command.
        �����ע�⣺оƬ���յ��κ�һ��legal token����Ḵλtimeout counter�����������������ķ��Ϳ��Գ���tTIMEOUT��
                   ����������λ�ķ��ͼ�������Գ���tTIMEOUT
        */
        byte >>= 1; //׼����һλ
    } while (--BitCnt);
    
    return;
}
void swi_send_bytes(uint8_t count, uint8_t *buffer)
{

	/*
	ת��ʱ�䣺tTURNAROUND �ֲ� P35ҳ
	After the ATSHA204A transmits the last bit of a block, the system must wait this interval before sending the first bit
  of a flag.
	оƬ�ڷ�������Ӧ�����������һ��bit������Ӧ���ٵȴ�93uS��ʵ�ʿ��Ը�С���ſ�����оƬ�����κ� I/O Flags�����־�ֽ�(������������)��	
  
	CryptoAuthlib������������������оƬ���ݺ󣬺ܿ��������ͷ���idle��sleep�����ֽڣ�����������SWI_FLAG_CMD��ʼ��������������
	������Ҫ�ȴ����ʱ��.
*/
	
    //! Wait turn around time.
    //RX_TX_DELAY;
	   __delay_us(93);
    
    __set_PRIMASK(1);   //�ر����ж� 

    do
   {   
     swi_send_byte(*buffer);            /*��������*/
     buffer++;
   }while(--count);
    
   __set_PRIMASK(0);   //�����ж� 
	 
}


static uint32_t rcv_timeout;	  //���ճ�ʱ��־λ �ڲ�ʹ�� 0��ʾ�����޳�ʱ��1��ʾ��ʱ
uint8_t swi_receive_byte(void) 
{
	  uint32_t temp; //SysTickʹ��
    uint8_t retc = 0, BitCnt = 8;   

    do {
        retc >>= 1;
       
        // <! ��ѯstart������½���
       // �ڽ����׸��ֽڵ��׸�bitʱ����ѯ�½�����Ҫ��ѯ�ϳ�ʱ�䣬��Ϊ��turnAroundʱ�䣬�����Ҫ 131-39 = 92uS �μ�����������ע��
       // ���㰴131uS����ѯ200uS�ܻ�ȵ�tSTART���½���        
        SysTick->LOAD = 9*200;//��ѯ200uS 			//��������  		 
	      SysTick->VAL=0x00;        	            //��ռ�����������LOADֵ
	      SysTick->CTRL=0x01 ;      	            //��ʼ���� 	 
	      do
	      {
					if (_SDA_STATUS == 0) goto TstartRising;
		      temp=SysTick->CTRL;
	      }while((temp&0x01)&&!(temp&(1<<16)));	//�ȴ�ʱ�䵽��   
	      SysTick->CTRL=0x00;      	 			      //�رռ���������ΪҪ�˳���
	      //SysTick->VAL =0X00;       		      //��ռ�����	 
//////////////////////////////////////////////////////////////	
  
        rcv_timeout = 1;
        break;

TstartRising:
         // <! ��ѯstart�����������
				//  64uS �㹻�ˣ���ԭ�����������ܶ�
        SysTick->CTRL=0x00;      	 		   //�رռ�����				
        SysTick->LOAD = 9*64;//��ѯ64uS  //��������  		 
	      SysTick->VAL=0x00;        		   //��ռ�����
	      SysTick->CTRL=0x01 ;      		   //��ʼ���� 	 
	      do
	      {		      
					if (_SDA_STATUS != 0) goto TzeroFailling;
					temp=SysTick->CTRL;
	      }while((temp&0x01)&&!(temp&(1<<16)));	//�ȴ�ʱ�䵽��   
	      SysTick->CTRL=0x00;      	 			      //�رռ���������ΪҪ�˳���
	      //SysTick->VAL =0X00;       		      //��ռ�����	 
//////////////////////////////////////////////////////////////	    
        rcv_timeout = 1;
        break;

        // <! ��ѯ���ܳ��ֵĵڶ����͵�ƽ���壬�����߼�0���������̲�ѯ�ļ��ʱ��  
TzeroFailling:
//������߼�1��û�������壬��ѭ��ʱ��Ϊ15uS��
//����������壬�ڿ���tZLO���½��غ�����תȥ��ѯ�����½��أ���ѭ��ʱ��Ӧ��С��15uS��
//�����Ļ������ʱ15uS�����㣺41 - 8.6 - 15 = ���17.4uSʱ������������ѯ�Ŀ�ʼ����
// markA:������ʱ�����������˵��������ȶ����Ȼ�յ���tSTART�������أ�������8.6uS(tZHI)��ῴ��tZLO���½���(����еĻ�)��
// ֻҪ��֤���1��2�εĲ�ѯ��8.6uS֮��Ӧ��û���⣡
			  SysTick->CTRL=0x00;      	 		       //�رռ�����
        SysTick->LOAD = 9*15;//���ǰ�15uS  	 //��������  		 
	      SysTick->VAL=0x00;        		       //��ռ�����
	      SysTick->CTRL=0x01 ;      		       //��ʼ���� 	 
	      do
	      {		      
					if (_SDA_STATUS == 0) goto TzeroRising;
					temp=SysTick->CTRL;
	      }while((temp&0x01)&&!(temp&(1<<16)));	//�ȴ�ʱ�䵽��   
	      //SysTick->CTRL=0x00;      	 			    //�رռ�����
	      //SysTick->VAL =0X00;       			    //��ռ�����	 //ʵ�ڿ�������������ɶ��
/////////////////////////////////////////////////////////////
        //��ѯ��15uS��û�����½��أ�˵�������д����߼�0�ĵ���������ˣ���Ϊ�յ����߼�1
        setbit(retc, 7);
        //תȥ������һ��bit
        goto nextPolling;//�������رռ�����
        
TzeroRising:
        //��Ȼ�Ѿ�������tZLO���½��أ���ô���������ѯ��������ʱ����ʱ���Ͱ�tZLO�����ֵ8.6uS���ɣ���������ѯ�Ŀ�ʼ���־��������ʱ�䣻
        //��Ҫ��֤����tZLO�������أ�������һֱ��tZLO�ĵ͵�ƽ״̬���˳��Ļ����������ѯ����Ϊ����һ����tSTART���½��أ���������
        //markB:��ʵ̫����ʱ��Ҳûɶ���壬������½��ؾ�һ�����������أ����������ؾ��˳���������ʱ����С��
			  SysTick->CTRL=0x00;      	 		    //�رռ�����
        SysTick->LOAD = 9*12;//��12uS��		//��������  		 
	      SysTick->VAL=0x00;        		    //��ռ�����
	      SysTick->CTRL=0x01;      		      //��ʼ���� 	 
	      do
	      {		      
					if (_SDA_STATUS != 0) goto nextPolling; //��Ϊ��tZLO���½��أ���һ���������������أ������������˳�����ʡʱ��!
					temp=SysTick->CTRL;
	      }while((temp&0x01)&&!(temp&(1<<16)));	//�ȴ�ʱ�䵽��   
	      //SysTick->CTRL=0x00;      	 			   //�رռ�����
	      //SysTick->VAL =0X00;       			  //��ռ�����	 
/////////////////////////////////////////////////////////////
        
        ;//�ߵ��⣬����û����tZLO�������أ�զ�죿��Ȼ��������ᷢ������ԭ����û����
               
nextPolling:
        SysTick->CTRL=0x00;      	 			//�����رռ�����
				
    } while (--BitCnt);

    return (retc);
}

ATCA_STATUS swi_receive_bytes(uint8_t count, uint8_t *buffer)
{
    uint8_t i=0;
    
    __set_PRIMASK(1); //�ر����ж� 

    rcv_timeout = 0;  //Ĭ�������޳�ʱ  
    
    swi_send_byte(SWI_FLAG_TX);//���ȷ���Transmit flag��֪оƬ��������������    
    _SDA = 1;                  //��������Ϊ���뷽ʽ����ʵ��ÿ�η����ݵ����_SDA��Ϊ1��
	
    /* ת��ʱ��Turnaround Delay tTURNAROUND��64 80 131 uS�����ֲ��35ҳ
     The ATSHA204A will initiate the first low-going transition after this time interval following the start of the last bit
     (tBIT) of the Transmit flag.
     оƬ���յ�SWI_FLAG_TX�ֽں󣬽���tTURNAROUNDʱ���Ժ�ʼ�������ݣ�
	   ʵ�ʵ���Сʱ��Ӧ�ü���Ϊ��tTURNAROUND - tBIT = 64 - 39(��������swi_send_byte��������) = 25- uS 
     �൱��??�����ڣ����ڽ������ݵĺ������״ν������polling��ʱ���㹻�ˣ�ʵ��Ϊ??us��.
	
     ͬʱ�������ʱ��Ϊ131uS��Ҳ��Ҫ�ȴ����ʱ�䣬Ӧ����������պ������������ѯtSTART���½���ʱ��ܳ�(����131uS)���������⡣
	
	   �������������պ������������������׸�tSTART���塣
	*/
   do 
   {
      *buffer++ = swi_receive_byte();
      i++;//ֻΪ���ж��ǲ��ǵ�һ���ֽھͷ���rcv_timeout
      if (rcv_timeout)  break;   
    }while (--count);//���countΪ0Ҳû��ϵ���϶��ᷢ��rcv_timeout��  
    
    __set_PRIMASK(0);   //�����ж�
    
    //���ܵķ���ֵ��ATCA_SUCCESS(��ʾ��ȷ�յ���Ԥ�ڵ�count�ֽ���)
    //ATCA_RX_TIMEOUT����ʾ���ճ�ʱ������׸��ֽھͷ���timeout���᷵��������룻
    //ATCA_RX_FAIL   ��������ն���1���ֽڷ���timeout���᷵��������룬��ܿ��ܴ�����ճɹ���һ���������ֽ�������Ӧ������Ԥ�ڵ�count��ƥ��(С��count).
    if (rcv_timeout)
    {
        if (i != 1)//if (i > 1)//�жϽ����ֽ����ǲ��Ǵ���1
        {
            //! Indicate that we timed out after having received at least one byte.
            return ATCA_RX_FAIL;
        }
        return ATCA_RX_TIMEOUT;
    }

    return ATCA_SUCCESS;
}

 
