#include "bignum.h"   
#include "string.h"
#include "stdio.h"

/******************************************************************
*	��������	  bigadd
*	����˵���� ��������ļӷ�
* ��������� adda	 ��������a
						lena	 ��������a�ĳ���
						addb	 ��������b
						lenb	 ��������b�ĳ���
						result ����ӷ�������ַ�����
* ���������
*******************************************************************/	
void bigadd(char *adda,int lena,char *addb,int lenb,char *result)
{
    int i,k,j,tmp,lensum;
    char num = '0';
    char final[BUFSIZ];
    //���ַ����������ת��Ϊ��Ӧ����
    for(i=0;i<lena;i++)
        adda[i] = adda[i] - num;
    for(i=0;i<lenb;i++)
        addb[i] = addb[i] - num;
    //����������ĳ���
    lensum = lena > lenb ? lena : lenb;
    //ѭ���ĸ�ÿһλ���ӷ�
    for(i=0,j=0;i<lena&&j<lenb;i++,j++)
        result[i] = adda[lena-i-1] + addb[lenb-i-1];
    //ʹ���жϽ��ϴ����ĸ�λҲд��������
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
    //�����������ÿһλ����10��1
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
    //ȥ��ǰ��0����������д��final������
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
*	��������	  bigsub
*	����˵���� ��������ļ���    Ҫ��a>b
* ��������� suba	 ��������a
						lena	 ��������a�ĳ���
						subb	 ��������b
						lenb	 ��������b�ĳ���
						result �������������ַ�����
* ���������
*******************************************************************/	
void bigsub(char *suba,int lena,char *subb,int lenb,char *result)
{
    char num='0';
    int i,j,k,lensum;
    char final[BUFSIZ];
    lensum=lena>lenb?lena:lenb;
    //��ASCII���������ת��Ϊ���������ִ洢�����ڼ��㡣
    for(i=0;i<lena;i++)
        suba[i]=suba[i]-num;
    for(i=0;i<lenb;i++)
        subb[i]=subb[i]-num;
    //ѭ���ĸ�ÿһλ������
    for(i=0,j=0;i<lena&&j<lenb;i++,j++)
        result[i]=suba[lena-i-1]-subb[lenb-i-1];
    //����λд��result���������
    for(i=lenb;i<lena;i++)
        result[i]=suba[lena-i-1];
    //������
    for(k=0;k<lensum-1;k++)
    {
        if(result[k]<0)
        {
            result[k]=result[k]+10;
            result[k+1] -=1;
        }
    }
    //�������ȥ��ǰ��0������final�����С�
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
*	��������	  bigmul
*	����˵���� ��������ĳ˷�    
* ��������� m	 		 ��������a
						lena	 ��������a�ĳ���
						f	 		 ��������b
						lenb	 ��������b�ĳ���
						result ����˷�������ַ�����
* ���������
*******************************************************************/	
void bigmul(char *m,int lena,char *f,int lenb,char *result)//�˷����㺯��
{
    int i,j,k,lensum,tmp_result,carry;
    char num='0';
    char final[BUFSIZ] = "";
    //ȷ���������ĳ���
    lensum=lena+lenb;
    //��ASCII��תΪ��Ӧ�����ִ洢��
    for(i=0;i<lena;i++)
        m[i]=m[i]-num;
    for(i=0;i<lenb;i++)
        f[i]=f[i]-num;
    //Ϊ��������һ�˳˷���
     for(i=0;i<lenb;i++)
     {
        for(j=0;j<lena;j++)
        {
            tmp_result=f[lenb-i-1]*m[lena-j-1];
            result[j+i]+=tmp_result;
        }
        for(k=0;k<=j+i-1;k++)//ÿ��һ�˳˷�����һ�ν�����顣
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
     //ȥ��ǰ���㽫�������final������
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
*	��������	  bigpow
*	����˵���� ���������������    
* ��������� buff	 ��������a
						len	   ��������a�ĳ���
						num	 	 �������ָ��
						result ��������������ַ�����
* ���������
*******************************************************************/	
void bigpow(char *buff,int len,int num,char *result)
{
    int i,j,templen;
    char temp_buff1[BUFSIZ] = "";
		char temp_buff2[BUFSIZ] = "";

	  templen = len;               //��¼�����ĳ���:templen
    for(i=0;i<len;i++)           //��¼��һ�μ��������:temp
        temp_buff1[i] = buff[i];
    for(i=0;i<len;i++)           //��¼�������ַ����飺temp_buff
        temp_buff2[i] = buff[i];

    if(num == 0)                        //��Ϊ0�����
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
        for(i=0;i<num-1;i++)            //ע��Ҫ��1��num-1
        {
            memset(result,0,len);//��ս������
            bigmul(temp_buff1,len,buff,templen,result);
            //��ʼ��һ�μ���ǰ�����¸�ֵ�β�
            len = strlen(result);//���ȼ���
            for(j=0;j<len;j++)   //����ν�����´�������
                temp_buff1[j] = result[j];
            for(j=0;j<templen;j++)      //����������
                buff[j] = temp_buff2[j];
        }
    }
}





