#ifndef __BIGNUM_H
#define	__BIGNUM_H

#include "sys.h"	

void bigadd(char *adda,int lena,char *addb,int lenb,char *result);
void bigsub(char *suba,int lena,char *subb,int lenb,char *result);
void bigmul(char *m,int lena,char *f,int lenb,char *result);
void bigpow(char *buff,int len,int num,char *result);



#endif 
