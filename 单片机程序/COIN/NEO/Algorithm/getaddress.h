#ifndef __GETADDRESS_H
#define __GETADDRESS_H
#include "sys.h"	

void GetAddressFromPublickey (uint8_t *publickey,int len);
void Base58_Encode(uint8_t *buff,int lenin,char *result,int *lenout);
void Base58_Decode(char *buff,uint8_t *result);
void Alg_Base58Encode(uint8_t *dataIn , int dataInLen , char *dataOut , int *dataoutLen );

void	Base58_25Bytes(uint8_t *buff,int lenin,uint8_t result[25]);
	 				    
#endif
