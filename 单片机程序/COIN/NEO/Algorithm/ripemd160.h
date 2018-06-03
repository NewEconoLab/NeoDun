#ifndef __RIPEMD160_H
#define __RIPEMD160_H
#include "sys.h"

void Ripemd160(uint8_t *buff,uint8_t *result);
void _HashData(uint8_t *partIn, int ibStart, int cbSize);
void _EndHash(uint8_t *buff);
void MDTransform(uint32_t *blockDWords, uint32_t *state, uint32_t *block);
uint32_t F(uint32_t x, uint32_t y, uint32_t z);
uint32_t G(uint32_t x, uint32_t y, uint32_t z);
uint32_t H(uint32_t x, uint32_t y, uint32_t z);
uint32_t I(uint32_t x, uint32_t y, uint32_t z);
uint32_t J(uint32_t x, uint32_t y, uint32_t z);
void DWORDFromLittleEndian(uint32_t *x, int digits, uint32_t *block);
void DWORDToLittleEndian(uint8_t *block, uint32_t *x, int digits);

#endif
