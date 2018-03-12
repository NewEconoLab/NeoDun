
#include "bitBand.h"
#include "SWI_Atsha204a.h"

/* * * * * * * 代码注释仅供参考 * * * * * * */

extern void __delay_us(uint32_t nTime);        // 定义在hal_stm32_timer.c
extern void __delay_counter(uint32_t counter); // 定义在hal_stm32_timer.c

/*&&&&&&&&&&&&&&&&&&&        引脚定义 文件内有效     &&&&&&&&&&&&&&&&&&&&&&&&&&&*/
#define  _SDA              PBout(9)
#define  _SDA_STATUS       PBin(9)

void SWI_Init(void)   
{
	//函数为空，系统调用bitBang_I2c_Init()同时也将SWI初始化了,SDA引脚复用.
}

/*&&&&&&&&&&&&&&&&&&&        位操作       &&&&&&&&&&&&&&&&&&&&&&&&&&&*/
#define testbit(var, bit)  ((var) & (1 <<(bit)))
#define setbit(var, bit)   ((var) |= (1 << (bit)))
#define clrbit(var, bit)   ((var) &= ~(1 << (bit)))

//时间定义

/*实测值仅供参考*/

/*实测
264	119.1	4.198152813
266	119.1	4.198152813
268	119.1	4.198152813
270	115.7	4.321521175
272	115.7	4.321521175
理论值（4.34-5/9）×72 = 272.48
*/
#define   _4_34_uS        __delay_counter(266)//100 - 390

/*实测
2445	14.51	34.4589938
2448	14.46	34.57814661
2450	14.46	34.57814661
2455	14.46	34.57814661
2456	14.46	34.57814661
2457	14.4	34.72222222
理论值（34.66-5/9）×72 = 2455.52
*/
#define   BIT_DELAY_7     __delay_counter(2450)//典型值Tbit - Tstart = 39-4.34 = 34.66

/*实测 
1826	19.36	25.82644628
1828	19.26	25.96053998
1830	19.26	25.96053998
理论值（25.98-5/9）×72 = 1830.56
*/
#define   BIT_DELAY_5     __delay_counter(1824)//典型值Tbit - Tstart*3 = 39-4.34*3 = 25.98

void swi_send_wake_token(void)
{  
    _SDA = 0;
    __delay_us(65);//拉低超过tWLO=60uS(最小值)
    _SDA = 1;
}

static void swi_send_byte(uint8_t byte)
{
	//函数内部使用，里面没有禁用中断，调用者负责
  //发送函数使用72M高分辨率延时函数__delay_counter()
	 uint8_t BitCnt = 8;

   do	
    {		
        //先发LSB
        if (testbit(byte, 0)) //!< Send Logic 1 (7F)
        {

            _SDA = 0;
            _4_34_uS;
            _SDA = 1;
            BIT_DELAY_7;//典型值Tbit - Tstart = 39-4.34 = 34.66
            //位发送间隙处理需要**uS,因此此处延时为 **uS,但没关系，见下面注解
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
            BIT_DELAY_5;//典型值Tbit - Tstart*3 = 39-4.34*3 = 25.98
            //位发送间隙处理需要**uS,因此此处延时为 **uS,但没关系，见下面注解
        }
    			/*
    位发送间隙：准备下一位及循环判断需要?? uS   
    但没关系：见手册7.3.1第35页对Tbit参数的注解：
    If the bit time exceeds tTIMEOUT(45-65-85mS), then the ATSHA204A may enter the sleep state.
    也即：主机发送完一个数据位后(相当于8个uart的bit)，芯片会等待主机发送后面发送接下来的数据位，这个等待时间应该计算为：
        tTIMEOUT - Tbit = 45mS - 约39uS(实际可由上面代码计算出实际的发送一个数据位的总时间)相当于最小值为45个ms，
    超过这个时间，芯片才会进入休眠；
				而且5.3.1第22页
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
        有这个注解：芯片在收到任何一个legal token后均会复位timeout counter，所以整个数据流的发送可以超过tTIMEOUT！
                   但两个数据位的发送间隔不可以超过tTIMEOUT
        */
        byte >>= 1; //准备下一位
    } while (--BitCnt);
    
    return;
}
void swi_send_bytes(uint8_t count, uint8_t *buffer)
{

	/*
	转身时间：tTURNAROUND 手册 P35页
	After the ATSHA204A transmits the last bit of a block, the system must wait this interval before sending the first bit
  of a flag.
	芯片在发送完响应数据流的最后一个bit后，主机应至少等待93uS（实际可以更小）才可以向芯片发送任何 I/O Flags命令标志字节(及后续数据流)；	
  
	CryptoAuthlib函数库在主机接收完芯片数据后，很可能立即就发送idle或sleep命令字节，或发送其他以SWI_FLAG_CMD开始的命令数据流，
	所以需要等待这个时间.
*/
	
    //! Wait turn around time.
    //RX_TX_DELAY;
	   __delay_us(93);
    
    __set_PRIMASK(1);   //关闭总中断 

    do
   {   
     swi_send_byte(*buffer);            /*发送数据*/
     buffer++;
   }while(--count);
    
   __set_PRIMASK(0);   //打开总中断 
	 
}


static uint32_t rcv_timeout;	  //接收超时标志位 内部使用 0表示正常无超时，1表示超时
uint8_t swi_receive_byte(void) 
{
	  uint32_t temp; //SysTick使用
    uint8_t retc = 0, BitCnt = 8;   

    do {
        retc >>= 1;
       
        // <! 查询start脉冲的下降沿
       // 在接收首个字节的首个bit时，查询下降沿需要轮询较长时间，因为有turnAround时间，最大需要 131-39 = 92uS 参见主调函数标注；
       // 就算按131uS，轮询200uS总会等到tSTART的下降沿        
        SysTick->LOAD = 9*200;//查询200uS 			//计数加载  		 
	      SysTick->VAL=0x00;        	            //清空计数器，加载LOAD值
	      SysTick->CTRL=0x01 ;      	            //开始倒数 	 
	      do
	      {
					if (_SDA_STATUS == 0) goto TstartRising;
		      temp=SysTick->CTRL;
	      }while((temp&0x01)&&!(temp&(1<<16)));	//等待时间到达   
	      SysTick->CTRL=0x00;      	 			      //关闭计数器，因为要退出了
	      //SysTick->VAL =0X00;       		      //清空计数器	 
//////////////////////////////////////////////////////////////	
  
        rcv_timeout = 1;
        break;

TstartRising:
         // <! 查询start脉冲的上升沿
				//  64uS 足够了，但原代码比这个长很多
        SysTick->CTRL=0x00;      	 		   //关闭计数器				
        SysTick->LOAD = 9*64;//查询64uS  //计数加载  		 
	      SysTick->VAL=0x00;        		   //清空计数器
	      SysTick->CTRL=0x01 ;      		   //开始倒数 	 
	      do
	      {		      
					if (_SDA_STATUS != 0) goto TzeroFailling;
					temp=SysTick->CTRL;
	      }while((temp&0x01)&&!(temp&(1<<16)));	//等待时间到达   
	      SysTick->CTRL=0x00;      	 			      //关闭计数器，因为要退出了
	      //SysTick->VAL =0X00;       		      //清空计数器	 
//////////////////////////////////////////////////////////////	    
        rcv_timeout = 1;
        break;

        // <! 查询可能出现的第二个低电平脉冲，代表逻辑0，尽量缩短查询的间隔时间  
TzeroFailling:
//如果是逻辑1，没有零脉冲，总循环时间为15uS；
//如果有零脉冲，在看到tZLO的下降沿后立即转去查询它的下降沿，总循环时间应该小于15uS！
//这样的话，最长延时15uS，计算：41 - 8.6 - 15 = 最短17.4uS时间留给后面轮询的开始部分
// markA:关于总时间的设置以上说法可以商榷：既然收到了tSTART的上升沿，则最晚8.6uS(tZHI)后会看到tZLO的下降沿(如果有的话)，
// 只要保证最后1到2次的查询在8.6uS之后应该没问题！
			  SysTick->CTRL=0x00;      	 		       //关闭计数器
        SysTick->LOAD = 9*15;//还是按15uS  	 //计数加载  		 
	      SysTick->VAL=0x00;        		       //清空计数器
	      SysTick->CTRL=0x01 ;      		       //开始倒数 	 
	      do
	      {		      
					if (_SDA_STATUS == 0) goto TzeroRising;
					temp=SysTick->CTRL;
	      }while((temp&0x01)&&!(temp&(1<<16)));	//等待时间到达   
	      //SysTick->CTRL=0x00;      	 			    //关闭计数器
	      //SysTick->VAL =0X00;       			    //清空计数器	 //实在看不出来这行有啥用
/////////////////////////////////////////////////////////////
        //轮询的15uS还没看到下降沿，说明不会有代表逻辑0的低脉冲出现了，认为收到了逻辑1
        setbit(retc, 7);
        //转去接收下一个bit
        goto nextPolling;//负责最后关闭计数器
        
TzeroRising:
        //既然已经看到了tZLO的下降沿，那么下面这个查询上升沿延时的总时长就按tZLO的最大值8.6uS即可，给后面轮询的开始部分尽量留多点时间；
        //但要保证看到tZLO的上升沿！！否则一直在tZLO的低电平状态就退出的话，后面的轮询会认为是下一个的tSTART的下降沿！！！！！
        //markB:其实太纠结时间也没啥意义，这里，有下降沿就一定会有上升沿，看到上升沿就退出，所以总时间会更小！
			  SysTick->CTRL=0x00;      	 		    //关闭计数器
        SysTick->LOAD = 9*12;//按12uS吧		//计数加载  		 
	      SysTick->VAL=0x00;        		    //清空计数器
	      SysTick->CTRL=0x01;      		      //开始倒数 	 
	      do
	      {		      
					if (_SDA_STATUS != 0) goto nextPolling; //认为有tZLO的下降沿，就一定能有他的上升沿，看到它即可退出，节省时间!
					temp=SysTick->CTRL;
	      }while((temp&0x01)&&!(temp&(1<<16)));	//等待时间到达   
	      //SysTick->CTRL=0x00;      	 			   //关闭计数器
	      //SysTick->VAL =0X00;       			  //清空计数器	 
/////////////////////////////////////////////////////////////
        
        ;//走到这，代表没看到tZLO的上升沿，咋办？虽然这基本不会发生，但原代码没处理
               
nextPolling:
        SysTick->CTRL=0x00;      	 			//最后负责关闭计数器
				
    } while (--BitCnt);

    return (retc);
}

ATCA_STATUS swi_receive_bytes(uint8_t count, uint8_t *buffer)
{
    uint8_t i=0;
    
    __set_PRIMASK(1); //关闭总中断 

    rcv_timeout = 0;  //默认正常无超时  
    
    swi_send_byte(SWI_FLAG_TX);//首先发送Transmit flag告知芯片接下来返回数据    
    _SDA = 1;                  //置数据线为输入方式，其实在每次发数据的最后_SDA都为1；
	
    /* 转身时间Turnaround Delay tTURNAROUND（64 80 131 uS）：手册第35页
     The ATSHA204A will initiate the first low-going transition after this time interval following the start of the last bit
     (tBIT) of the Transmit flag.
     芯片在收到SWI_FLAG_TX字节后，将在tTURNAROUND时间以后开始发送数据；
	   实际的最小时间应该计算为：tTURNAROUND - tBIT = 64 - 39(根据上面swi_send_byte函数计算) = 25- uS 
     相当于??个周期，对于接收数据的函数的首次进入接收polling，时间足够了（实际为??us）.
	
     同时，就算最长时间为131uS，也不要等待这个时间，应立即进入接收函数，函数里查询tSTART的下降沿时间很长(超过131uS)，不会问题。
	
	   综上所述，接收函数不会错过数据流的首个tSTART脉冲。
	*/
   do 
   {
      *buffer++ = swi_receive_byte();
      i++;//只为了判断是不是第一个字节就发生rcv_timeout
      if (rcv_timeout)  break;   
    }while (--count);//如果count为0也没关系，肯定会发生rcv_timeout，  
    
    __set_PRIMASK(0);   //打开总中断
    
    //可能的返回值：ATCA_SUCCESS(表示正确收到了预期的count字节数)
    //ATCA_RX_TIMEOUT：表示接收超时，如果首个字节就发生timeout，会返回这个代码；
    //ATCA_RX_FAIL   ：如果接收多于1个字节发生timeout，会返回这个代码，这很可能代表接收成功了一定数量的字节数，但应该是与预期的count不匹配(小于count).
    if (rcv_timeout)
    {
        if (i != 1)//if (i > 1)//判断接收字节数是不是大于1
        {
            //! Indicate that we timed out after having received at least one byte.
            return ATCA_RX_FAIL;
        }
        return ATCA_RX_TIMEOUT;
    }

    return ATCA_SUCCESS;
}

 
