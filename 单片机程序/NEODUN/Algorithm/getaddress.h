#ifndef __GETADDRESS_H
#define __GETADDRESS_H
#include <stdint.h>


uint8_t Base58_Encode(uint8_t *buff,int lenin,char *result,int *lenout);
void	Base58_25Bytes(uint8_t *buff,int lenin,uint8_t result[25]);

    
#endif
