#ifndef __RIPEMD160_H
#define __RIPEMD160_H
//#include "sys.h"
#include <stdint.h> 

void Ripemd160(uint8_t *buff,uint8_t *result);
void ripemd160_HashData(uint8_t *partIn, int ibStart, int cbSize);
void ripemd160_EndHash(uint8_t *buff);
void MDTransform(uint32_t *blockDWords, uint32_t *state, uint8_t *block);
uint32_t ripemd160_F(uint32_t x, uint32_t y, uint32_t z);
uint32_t ripemd160_G(uint32_t x, uint32_t y, uint32_t z);
uint32_t ripemd160_H(uint32_t x, uint32_t y, uint32_t z);
uint32_t ripemd160_I(uint32_t x, uint32_t y, uint32_t z);
uint32_t ripemd160_J(uint32_t x, uint32_t y, uint32_t z);
void DWORDFromLittleEndian(uint32_t *x, int digits, uint8_t *block);
void DWORDToLittleEndian(uint8_t *block, uint32_t *x, int digits);

#endif
