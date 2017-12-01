#include "bignum.h"   
#include "string.h"
#include "stdio.h"

/******************************************************************
*	函数名：	  bigadd
*	函数说明： 大数运算的加法
* 输入参数： adda	 输入数组a
						lena	 输入数组a的长度
						addb	 输入数组b
						lenb	 输入数组b的长度
						result 输出加法结果的字符数组
* 输出参数：
*******************************************************************/	
void bigadd(char *adda,int lena,char *addb,int lenb,char *result)
{
    int i,k,j,tmp,lensum;
    char num = '0';
    char final[BUFSIZ];
    //将字符编码的数字转换为对应的数
    for(i=0;i<lena;i++)
        adda[i] = adda[i] - num;
    for(i=0;i<lenb;i++)
        addb[i] = addb[i] - num;
    //求出结果数组的长度
    lensum = lena > lenb ? lena : lenb;
    //循环的给每一位作加法
    for(i=0,j=0;i<lena&&j<lenb;i++,j++)
        result[i] = adda[lena-i-1] + addb[lenb-i-1];
    //使用判断将较大数的高位也写入结果数组
    if(lena>lenb)
    {
        for(i=lenb;i<lena;i++)
            result[i] = adda[lena-i-1];
    }
    if(lena<lenb)
    {
        for(i=lena;i<lenb;i++)
            result[i] = addb[lenb-i-1];
    }
    //整理结果数组的每一位，满10进1
    for(k=0;k<lensum;k++)
    {
        if(result[k]>9)
        {
            tmp = result[k]/10;
            result[k] = result[k]%10;
            result[k+1] += tmp;
        }
    }
    j=0;
    //去掉前导0将结果处理后写到final数组中
    if(result[lensum]!=0)
    {
        final[j] = result[lensum] + num;
        j++;
    }
    for(i = lensum-1;i>=0;i--)
        final[j++] = result[i] + num;
    for(i=0;i<strlen(final);i++)
        result[i] = final[i];
}

/******************************************************************
*	函数名：	  bigsub
*	函数说明： 大数运算的减法    要求a>b
* 输入参数： suba	 输入数组a
						lena	 输入数组a的长度
						subb	 输入数组b
						lenb	 输入数组b的长度
						result 输出减法结果的字符数组
* 输出参数：
*******************************************************************/	
void bigsub(char *suba,int lena,char *subb,int lenb,char *result)
{
    char num='0';
    int i,j,k,lensum;
    char final[BUFSIZ];
    lensum=lena>lenb?lena:lenb;
    //将ASCII编码的数字转换为真正的数字存储，便于计算。
    for(i=0;i<lena;i++)
        suba[i]=suba[i]-num;
    for(i=0;i<lenb;i++)
        subb[i]=subb[i]-num;
    //循环的给每一位作减法
    for(i=0,j=0;i<lena&&j<lenb;i++,j++)
        result[i]=suba[lena-i-1]-subb[lenb-i-1];
    //将高位写入result结果数组中
    for(i=lenb;i<lena;i++)
        result[i]=suba[lena-i-1];
    //整理结果
    for(k=0;k<lensum-1;k++)
    {
        if((result[k]+num) < 0x30)
        {
            result[k]=result[k]+10;
            result[k+1] -=1;
        }
    }
    //将结果集去除前导0后整理到final数组中。
    j=0;
    if(result[lensum-1]!=0)
    {
        final[j]=result[lensum-1]+num;
        j++;
    }
    for(i=lensum-2;i>=0;i--)
        final[j++]=result[i]+num;
    for(i=0;i<strlen(final);i++)
        result[i] = final[i];
}

/******************************************************************
*	函数名：	  bigmul
*	函数说明： 大数运算的乘法    
* 输入参数： m	 		 输入数组a
						lena	 输入数组a的长度
						f	 		 输入数组b
						lenb	 输入数组b的长度
						result 输出乘法结果的字符数组
* 输出参数：
*******************************************************************/	
void bigmul(char *m,int lena,char *f,int lenb,char *result)//乘法运算函数
{
    int i,j,k,lensum,tmp_result,carry;
    char num='0';
    char final[BUFSIZ] = "";
    //确定结果数组的长度
    lensum=lena+lenb;
    //将ASCII码转为对应的数字存储。
    for(i=0;i<lena;i++)
        m[i]=m[i]-num;
    for(i=0;i<lenb;i++)
        f[i]=f[i]-num;
    //为被乘数作一趟乘法。
     for(i=0;i<lenb;i++)
     {
        for(j=0;j<lena;j++)
        {
            tmp_result=f[lenb-i-1]*m[lena-j-1];
            result[j+i]+=tmp_result;
        }
        for(k=0;k<=j+i-1;k++)//每作一趟乘法整理一次结果数组。
        {
            if(result[k]>9)
            {
                carry=result[k]/10;
                result[k]=result[k]%10;
                result[k+1] += carry;
            }
        }
     }
     j=0;
     //去除前导零将结果整理到final数组中
     if(result[lensum-1]!=0)
     {
        final[j]=result[lensum-1]+num;
        j++;
     }
    for(i=lensum-2;i>=0;i--)
        final[j++]=result[i]+num;
    for(i=0;i<strlen(final);i++)
        result[i] = final[i];
}

/******************************************************************
*	函数名：	  bigpow
*	函数说明： 大数运算的幂运算    
* 输入参数： buff	 输入数组a
						len	   输入数组a的长度
						num	 	 幂运算的指数
						result 输出幂运算结果的字符数组
* 输出参数：
*******************************************************************/	
void bigpow(char *buff,int len,int num,char *result)
{
    int i,j,templen;
    char temp_buff1[BUFSIZ] = "";
		char temp_buff2[BUFSIZ] = "";

	  templen = len;               //记录底数的长度:templen
    for(i=0;i<len;i++)           //记录第一次计算的数组:temp
        temp_buff1[i] = buff[i];
    for(i=0;i<len;i++)           //记录底数的字符数组：temp_buff
        temp_buff2[i] = buff[i];

    if(num == 0)                        //幂为0的情况
    {
        memset(result,0,strlen(result));
        result[0] = '1';
        return;
    }
		else if(num == 1)
		{
				for(i=0;i<len;i++)
					result[i] = buff[i];
				return;
		}
    else
    {
        for(i=0;i<num-1;i++)            //注意要减1，num-1
        {
            memset(result,0,len);//清空结果数组
            bigmul(temp_buff1,len,buff,templen,result);
            //开始下一次计算前，重新赋值形参
            len = strlen(result);//长度计算
            for(j=0;j<len;j++)   //将这次结果给下次做乘数
                temp_buff1[j] = result[j];
            for(j=0;j<templen;j++)      //将乘数更新
                buff[j] = temp_buff2[j];
        }
    }
}





