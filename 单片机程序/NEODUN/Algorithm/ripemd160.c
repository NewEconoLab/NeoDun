#include "ripemd160.h"
#include "string.h"
//#include "usart.h"

static int ripemd160_count = 0;
static uint32_t ripemd160_stateMD160[5] = {0x67452301,0xefcdab89,0x98badcfe,0x10325476,0xc3d2e1f0};
static uint32_t ripemd160_blockDWords[16];
static uint8_t ripemd160_buffer[64];

/******************************************************************
*	函数名：	 Ripemd160
*	函数说明：进行Ripemd160 哈希算法
* 输入参数：buff		要计算的数组
					 result	输出的结果数组
* 输出参数：
*******************************************************************/	
void Ripemd160(uint8_t *buff,uint8_t *result)
{
		memset(ripemd160_blockDWords,0,sizeof(ripemd160_blockDWords));
		memset(ripemd160_buffer,0,sizeof(ripemd160_buffer));
		memset(ripemd160_stateMD160,0,sizeof(ripemd160_stateMD160));
		ripemd160_count = 0;
	
		ripemd160_stateMD160[0] = 0x67452301;
		ripemd160_stateMD160[1] = 0xefcdab89;
		ripemd160_stateMD160[2] = 0x98badcfe;
		ripemd160_stateMD160[3] = 0x10325476;
		ripemd160_stateMD160[4] = 0xc3d2e1f0;
	
		ripemd160_HashData(buff,0,32);
		ripemd160_EndHash(result);
}

void ripemd160_HashData(uint8_t *partIn, int ibStart, int cbSize)
{
		int i;
		int bufferLen;
		int partInLen = cbSize;
		int partInBase = ibStart;

		/* Compute length of buffer */
		bufferLen = ripemd160_count & 0x3f;

		/* Update number of bytes */
		ripemd160_count += partInLen;

		if ((bufferLen > 0) && (bufferLen + partInLen >= 64))
		{	
				for(i=0;i<64 - bufferLen;i++)
				{			
						ripemd160_buffer[i+bufferLen] = partIn[i+partInBase];
				}	
				partInBase += (64 - bufferLen);
				partInLen -= (64 - bufferLen);
				MDTransform(ripemd160_blockDWords, ripemd160_stateMD160, ripemd160_buffer);
				bufferLen = 0;
		}

		/* Copy input to temporary buffer and hash */
		while (partInLen >= 64)
		{	
				for(i=0;i<64;i++)
				{			
						ripemd160_buffer[i] = partIn[i+partInBase];
				}			
				partInBase += 64;
				partInLen -= 64;
				MDTransform(ripemd160_blockDWords, ripemd160_stateMD160, ripemd160_buffer);
		}

		if (partInLen > 0)
		{
				for(i=0;i<partInLen;i++)
				{			
						ripemd160_buffer[i+bufferLen] = partIn[i+partInBase];
				}			
		}
}

void ripemd160_EndHash(uint8_t *hash)
{
		int padLen = 32;
		long long bitCount;
		uint8_t pad[32];

		/* Compute padding: 80 00 00 ... 00 00 <bit count>
		*/
		memset(pad,0,32);
		pad[0] = 0x80;

		//  Convert count to bit count
		bitCount = ripemd160_count * 8;

		// The convention for RIPEMD is little endian (the same as MD4)
		pad[padLen - 1] = (bitCount >> 56) & 0xff;
		pad[padLen - 2] = (bitCount >> 48) & 0xff;
		pad[padLen - 3] = (bitCount >> 40) & 0xff;
		pad[padLen - 4] = (bitCount >> 32) & 0xff;
		pad[padLen - 5] = (bitCount >> 24) & 0xff;
		pad[padLen - 6] = (bitCount >> 16) & 0xff;
		pad[padLen - 7] = (bitCount >> 8) & 0xff;
		pad[padLen - 8] = (bitCount >> 0) & 0xff;

		/* Digest padding */
		ripemd160_HashData(pad, 0, 32);

		/* Store digest */
		DWORDToLittleEndian(hash, ripemd160_stateMD160, 5);
}

void MDTransform(uint32_t *blockDWords, uint32_t *state, uint8_t *block)
{
		uint32_t aa = state[0];
		uint32_t bb = state[1];
		uint32_t cc = state[2];
		uint32_t dd = state[3];
		uint32_t ee = state[4];

		uint32_t aaa = aa;
		uint32_t bbb = bb;
		uint32_t ccc = cc;
		uint32_t ddd = dd;
		uint32_t eee = ee;

		DWORDFromLittleEndian(blockDWords, 16, block);

		/*
			As we don't have macros in C# and we don't want to pay the cost of a function call
			(which BTW is quite important here as we would have to pass 5 args by ref in 
			16 * 10 = 160 function calls)
			we'll prefer a less compact code to a less performant code
		*/

		// Left Round 1 
		// FF(ref aa, ref bb, ref cc, ref dd, ref ee, blockDWords[0], 11);
		aa += blockDWords[0] + ripemd160_F(bb, cc, dd);
		aa = (aa << 11 | aa >> (32 - 11)) + ee;
		cc = (cc << 10 | cc >> (32 - 10));

		// FF(ref ee, ref aa, ref bb, ref cc, ref dd, blockDWords[1], 14);
		ee += blockDWords[1] + ripemd160_F(aa, bb, cc);
		ee = (ee << 14 | ee >> (32 - 14)) + dd;
		bb = (bb << 10 | bb >> (32 - 10));

		// FF(ref dd, ref ee, ref aa, ref bb, ref cc, blockDWords[2], 15);
		dd += blockDWords[2] + ripemd160_F(ee, aa, bb);
		dd = (dd << 15 | dd >> (32 - 15)) + cc;
		aa = (aa << 10 | aa >> (32 - 10));

		// FF(ref cc, ref dd, ref ee, ref aa, ref bb, blockDWords[3], 12);
		cc += blockDWords[3] + ripemd160_F(dd, ee, aa);
		cc = (cc << 12 | cc >> (32 - 12)) + bb;
		ee = (ee << 10 | ee >> (32 - 10));

		// FF(ref bb, ref cc, ref dd, ref ee, ref aa, blockDWords[4], 5);
		bb += blockDWords[4] + ripemd160_F(cc, dd, ee);
		bb = (bb << 5 | bb >> (32 - 5)) + aa;
		dd = (dd << 10 | dd >> (32 - 10));

		// FF(ref aa, ref bb, ref cc, ref dd, ref ee, blockDWords[5], 8);
		aa += blockDWords[5] + ripemd160_F(bb, cc, dd);
		aa = (aa << 8 | aa >> (32 - 8)) + ee;
		cc = (cc << 10 | cc >> (32 - 10));

		// FF(ref ee, ref aa, ref bb, ref cc, ref dd, blockDWords[6], 7);
		ee += blockDWords[6] + ripemd160_F(aa, bb, cc);
		ee = (ee << 7 | ee >> (32 - 7)) + dd;
		bb = (bb << 10 | bb >> (32 - 10));

		// FF(ref dd, ref ee, ref aa, ref bb, ref cc, blockDWords[7], 9);
		dd += blockDWords[7] + ripemd160_F(ee, aa, bb);
		dd = (dd << 9 | dd >> (32 - 9)) + cc;
		aa = (aa << 10 | aa >> (32 - 10));

		// FF(ref cc, ref dd, ref ee, ref aa, ref bb, blockDWords[8], 11);
		cc += blockDWords[8] + ripemd160_F(dd, ee, aa);
		cc = (cc << 11 | cc >> (32 - 11)) + bb;
		ee = (ee << 10 | ee >> (32 - 10));

		// FF(ref bb, ref cc, ref dd, ref ee, ref aa, blockDWords[9], 13);
		bb += blockDWords[9] + ripemd160_F(cc, dd, ee);
		bb = (bb << 13 | bb >> (32 - 13)) + aa;
		dd = (dd << 10 | dd >> (32 - 10));

		// FF(ref aa, ref bb, ref cc, ref dd, ref ee, blockDWords[10], 14);
		aa += blockDWords[10] + ripemd160_F(bb, cc, dd);
		aa = (aa << 14 | aa >> (32 - 14)) + ee;
		cc = (cc << 10 | cc >> (32 - 10));

		// FF(ref ee, ref aa, ref bb, ref cc, ref dd, blockDWords[11], 15);
		ee += blockDWords[11] + ripemd160_F(aa, bb, cc);
		ee = (ee << 15 | ee >> (32 - 15)) + dd;
		bb = (bb << 10 | bb >> (32 - 10));

		// FF(ref dd, ref ee, ref aa, ref bb, ref cc, blockDWords[12], 6);
		dd += blockDWords[12] + ripemd160_F(ee, aa, bb);
		dd = (dd << 6 | dd >> (32 - 6)) + cc;
		aa = (aa << 10 | aa >> (32 - 10));

		// FF(ref cc, ref dd, ref ee, ref aa, ref bb, blockDWords[13], 7);
		cc += blockDWords[13] + ripemd160_F(dd, ee, aa);
		cc = (cc << 7 | cc >> (32 - 7)) + bb;
		ee = (ee << 10 | ee >> (32 - 10));

		// FF(ref bb, ref cc, ref dd, ref ee, ref aa, blockDWords[14], 9);
		bb += blockDWords[14] + ripemd160_F(cc, dd, ee);
		bb = (bb << 9 | bb >> (32 - 9)) + aa;
		dd = (dd << 10 | dd >> (32 - 10));

		// FF(ref aa, ref bb, ref cc, ref dd, ref ee, blockDWords[15], 8);
		aa += blockDWords[15] + ripemd160_F(bb, cc, dd);
		aa = (aa << 8 | aa >> (32 - 8)) + ee;
		cc = (cc << 10 | cc >> (32 - 10));

		// Left Round 2 
		// GG(ref ee, ref aa, ref bb, ref cc, ref dd, blockDWords[7], 7);
		ee += ripemd160_G(aa, bb, cc) + blockDWords[7] + 0x5a827999;
		ee = (ee << 7 | ee >> (32 - 7)) + dd;
		bb = (bb << 10 | bb >> (32 - 10));

		// GG(ref dd, ref ee, ref aa, ref bb, ref cc, blockDWords[4], 6);
		dd += ripemd160_G(ee, aa, bb) + blockDWords[4] + 0x5a827999;
		dd = (dd << 6 | dd >> (32 - 6)) + cc;
		aa = (aa << 10 | aa >> (32 - 10));

		// GG(ref cc, ref dd, ref ee, ref aa, ref bb, blockDWords[13], 8);
		cc += ripemd160_G(dd, ee, aa) + blockDWords[13] + 0x5a827999;
		cc = (cc << 8 | cc >> (32 - 8)) + bb;
		ee = (ee << 10 | ee >> (32 - 10));

		// GG(ref bb, ref cc, ref dd, ref ee, ref aa, blockDWords[1], 13);
		bb += ripemd160_G(cc, dd, ee) + blockDWords[1] + 0x5a827999;
		bb = (bb << 13 | bb >> (32 - 13)) + aa;
		dd = (dd << 10 | dd >> (32 - 10));

		// GG(ref aa, ref bb, ref cc, ref dd, ref ee, blockDWords[10], 11);
		aa += ripemd160_G(bb, cc, dd) + blockDWords[10] + 0x5a827999;
		aa = (aa << 11 | aa >> (32 - 11)) + ee;
		cc = (cc << 10 | cc >> (32 - 10));

		// GG(ref ee, ref aa, ref bb, ref cc, ref dd, blockDWords[6], 9);
		ee += ripemd160_G(aa, bb, cc) + blockDWords[6] + 0x5a827999;
		ee = (ee << 9 | ee >> (32 - 9)) + dd;
		bb = (bb << 10 | bb >> (32 - 10));

		// GG(ref dd, ref ee, ref aa, ref bb, ref cc, blockDWords[15], 7);
		dd += ripemd160_G(ee, aa, bb) + blockDWords[15] + 0x5a827999;
		dd = (dd << 7 | dd >> (32 - 7)) + cc;
		aa = (aa << 10 | aa >> (32 - 10));

		// GG(ref cc, ref dd, ref ee, ref aa, ref bb, blockDWords[3], 15);
		cc += ripemd160_G(dd, ee, aa) + blockDWords[3] + 0x5a827999;
		cc = (cc << 15 | cc >> (32 - 15)) + bb;
		ee = (ee << 10 | ee >> (32 - 10));

		// GG(ref bb, ref cc, ref dd, ref ee, ref aa, blockDWords[12], 7);
		bb += ripemd160_G(cc, dd, ee) + blockDWords[12] + 0x5a827999;
		bb = (bb << 7 | bb >> (32 - 7)) + aa;
		dd = (dd << 10 | dd >> (32 - 10));

		// GG(ref aa, ref bb, ref cc, ref dd, ref ee, blockDWords[0], 12);
		aa += ripemd160_G(bb, cc, dd) + blockDWords[0] + 0x5a827999;
		aa = (aa << 12 | aa >> (32 - 12)) + ee;
		cc = (cc << 10 | cc >> (32 - 10));

		// GG(ref ee, ref aa, ref bb, ref cc, ref dd, blockDWords[9], 15);
		ee += ripemd160_G(aa, bb, cc) + blockDWords[9] + 0x5a827999;
		ee = (ee << 15 | ee >> (32 - 15)) + dd;
		bb = (bb << 10 | bb >> (32 - 10));

		// GG(ref dd, ref ee, ref aa, ref bb, ref cc, blockDWords[5], 9);
		dd += ripemd160_G(ee, aa, bb) + blockDWords[5] + 0x5a827999;
		dd = (dd << 9 | dd >> (32 - 9)) + cc;
		aa = (aa << 10 | aa >> (32 - 10));

		// GG(ref cc, ref dd, ref ee, ref aa, ref bb, blockDWords[2], 11);
		cc += ripemd160_G(dd, ee, aa) + blockDWords[2] + 0x5a827999;
		cc = (cc << 11 | cc >> (32 - 11)) + bb;
		ee = (ee << 10 | ee >> (32 - 10));

		// GG(ref bb, ref cc, ref dd, ref ee, ref aa, blockDWords[14], 7);
		bb += ripemd160_G(cc, dd, ee) + blockDWords[14] + 0x5a827999;
		bb = (bb << 7 | bb >> (32 - 7)) + aa;
		dd = (dd << 10 | dd >> (32 - 10));

		// GG(ref aa, ref bb, ref cc, ref dd, ref ee, blockDWords[11], 13);
		aa += ripemd160_G(bb, cc, dd) + blockDWords[11] + 0x5a827999;
		aa = (aa << 13 | aa >> (32 - 13)) + ee;
		cc = (cc << 10 | cc >> (32 - 10));

		// GG(ref ee, ref aa, ref bb, ref cc, ref dd, blockDWords[8], 12);
		ee += ripemd160_G(aa, bb, cc) + blockDWords[8] + 0x5a827999;
		ee = (ee << 12 | ee >> (32 - 12)) + dd;
		bb = (bb << 10 | bb >> (32 - 10));

		// Left Round 3 
		// HH(ref dd, ref ee, ref aa, ref bb, ref cc, blockDWords[3], 11);
		dd += ripemd160_H(ee, aa, bb) + blockDWords[3] + 0x6ed9eba1;
		dd = (dd << 11 | dd >> (32 - 11)) + cc;
		aa = (aa << 10 | aa >> (32 - 10));

		// HH(ref cc, ref dd, ref ee, ref aa, ref bb, blockDWords[10], 13);
		cc += ripemd160_H(dd, ee, aa) + blockDWords[10] + 0x6ed9eba1;
		cc = (cc << 13 | cc >> (32 - 13)) + bb;
		ee = (ee << 10 | ee >> (32 - 10));

		// HH(ref bb, ref cc, ref dd, ref ee, ref aa, blockDWords[14], 6);
		bb += ripemd160_H(cc, dd, ee) + blockDWords[14] + 0x6ed9eba1;
		bb = (bb << 6 | bb >> (32 - 6)) + aa;
		dd = (dd << 10 | dd >> (32 - 10));

		// HH(ref aa, ref bb, ref cc, ref dd, ref ee, blockDWords[4], 7);
		aa += ripemd160_H(bb, cc, dd) + blockDWords[4] + 0x6ed9eba1;
		aa = (aa << 7 | aa >> (32 - 7)) + ee;
		cc = (cc << 10 | cc >> (32 - 10));

		// HH(ref ee, ref aa, ref bb, ref cc, ref dd, blockDWords[9], 14);
		ee += ripemd160_H(aa, bb, cc) + blockDWords[9] + 0x6ed9eba1;
		ee = (ee << 14 | ee >> (32 - 14)) + dd;
		bb = (bb << 10 | bb >> (32 - 10));

		// HH(ref dd, ref ee, ref aa, ref bb, ref cc, blockDWords[15], 9);
		dd += ripemd160_H(ee, aa, bb) + blockDWords[15] + 0x6ed9eba1;
		dd = (dd << 9 | dd >> (32 - 9)) + cc;
		aa = (aa << 10 | aa >> (32 - 10));

		// HH(ref cc, ref dd, ref ee, ref aa, ref bb, blockDWords[8], 13);
		cc += ripemd160_H(dd, ee, aa) + blockDWords[8] + 0x6ed9eba1;
		cc = (cc << 13 | cc >> (32 - 13)) + bb;
		ee = (ee << 10 | ee >> (32 - 10));

		// HH(ref bb, ref cc, ref dd, ref ee, ref aa, blockDWords[1], 15);
		bb += ripemd160_H(cc, dd, ee) + blockDWords[1] + 0x6ed9eba1;
		bb = (bb << 15 | bb >> (32 - 15)) + aa;
		dd = (dd << 10 | dd >> (32 - 10));

		// HH(ref aa, ref bb, ref cc, ref dd, ref ee, blockDWords[2], 14);
		aa += ripemd160_H(bb, cc, dd) + blockDWords[2] + 0x6ed9eba1;
		aa = (aa << 14 | aa >> (32 - 14)) + ee;
		cc = (cc << 10 | cc >> (32 - 10));

		// HH(ref ee, ref aa, ref bb, ref cc, ref dd, blockDWords[7], 8);
		ee += ripemd160_H(aa, bb, cc) + blockDWords[7] + 0x6ed9eba1;
		ee = (ee << 8 | ee >> (32 - 8)) + dd;
		bb = (bb << 10 | bb >> (32 - 10));

		// HH(ref dd, ref ee, ref aa, ref bb, ref cc, blockDWords[0], 13);
		dd += ripemd160_H(ee, aa, bb) + blockDWords[0] + 0x6ed9eba1;
		dd = (dd << 13 | dd >> (32 - 13)) + cc;
		aa = (aa << 10 | aa >> (32 - 10));

		// HH(ref cc, ref dd, ref ee, ref aa, ref bb, blockDWords[6], 6);
		cc += ripemd160_H(dd, ee, aa) + blockDWords[6] + 0x6ed9eba1;
		cc = (cc << 6 | cc >> (32 - 6)) + bb;
		ee = (ee << 10 | ee >> (32 - 10));

		// HH(ref bb, ref cc, ref dd, ref ee, ref aa, blockDWords[13], 5);
		bb += ripemd160_H(cc, dd, ee) + blockDWords[13] + 0x6ed9eba1;
		bb = (bb << 5 | bb >> (32 - 5)) + aa;
		dd = (dd << 10 | dd >> (32 - 10));

		// HH(ref aa, ref bb, ref cc, ref dd, ref ee, blockDWords[11], 12);
		aa += ripemd160_H(bb, cc, dd) + blockDWords[11] + 0x6ed9eba1;
		aa = (aa << 12 | aa >> (32 - 12)) + ee;
		cc = (cc << 10 | cc >> (32 - 10));

		// HH(ref ee, ref aa, ref bb, ref cc, ref dd, blockDWords[5], 7);
		ee += ripemd160_H(aa, bb, cc) + blockDWords[5] + 0x6ed9eba1;
		ee = (ee << 7 | ee >> (32 - 7)) + dd;
		bb = (bb << 10 | bb >> (32 - 10));

		// HH(ref dd, ref ee, ref aa, ref bb, ref cc, blockDWords[12], 5);
		dd += ripemd160_H(ee, aa, bb) + blockDWords[12] + 0x6ed9eba1;
		dd = (dd << 5 | dd >> (32 - 5)) + cc;
		aa = (aa << 10 | aa >> (32 - 10));

		// Left Round 4 
		// II(ref cc, ref dd, ref ee, ref aa, ref bb, blockDWords[1], 11);
		cc += ripemd160_I(dd, ee, aa) + blockDWords[1] + 0x8f1bbcdc;
		cc = (cc << 11 | cc >> (32 - 11)) + bb;
		ee = (ee << 10 | ee >> (32 - 10));

		// II(ref bb, ref cc, ref dd, ref ee, ref aa, blockDWords[9], 12);
		bb += ripemd160_I(cc, dd, ee) + blockDWords[9] + 0x8f1bbcdc;
		bb = (bb << 12 | bb >> (32 - 12)) + aa;
		dd = (dd << 10 | dd >> (32 - 10));

		// II(ref aa, ref bb, ref cc, ref dd, ref ee, blockDWords[11], 14);
		aa += ripemd160_I(bb, cc, dd) + blockDWords[11] + 0x8f1bbcdc;
		aa = (aa << 14 | aa >> (32 - 14)) + ee;
		cc = (cc << 10 | cc >> (32 - 10));

		// II(ref ee, ref aa, ref bb, ref cc, ref dd, blockDWords[10], 15);
		ee += ripemd160_I(aa, bb, cc) + blockDWords[10] + 0x8f1bbcdc;
		ee = (ee << 15 | ee >> (32 - 15)) + dd;
		bb = (bb << 10 | bb >> (32 - 10));

		// II(ref dd, ref ee, ref aa, ref bb, ref cc, blockDWords[0], 14);
		dd += ripemd160_I(ee, aa, bb) + blockDWords[0] + 0x8f1bbcdc;
		dd = (dd << 14 | dd >> (32 - 14)) + cc;
		aa = (aa << 10 | aa >> (32 - 10));

		// II(ref cc, ref dd, ref ee, ref aa, ref bb, blockDWords[8], 15);
		cc += ripemd160_I(dd, ee, aa) + blockDWords[8] + 0x8f1bbcdc;
		cc = (cc << 15 | cc >> (32 - 15)) + bb;
		ee = (ee << 10 | ee >> (32 - 10));

		// II(ref bb, ref cc, ref dd, ref ee, ref aa, blockDWords[12], 9);
		bb += ripemd160_I(cc, dd, ee) + blockDWords[12] + 0x8f1bbcdc;
		bb = (bb << 9 | bb >> (32 - 9)) + aa;
		dd = (dd << 10 | dd >> (32 - 10));

		// II(ref aa, ref bb, ref cc, ref dd, ref ee, blockDWords[4], 8);
		aa += ripemd160_I(bb, cc, dd) + blockDWords[4] + 0x8f1bbcdc;
		aa = (aa << 8 | aa >> (32 - 8)) + ee;
		cc = (cc << 10 | cc >> (32 - 10));

		// II(ref ee, ref aa, ref bb, ref cc, ref dd, blockDWords[13], 9);
		ee += ripemd160_I(aa, bb, cc) + blockDWords[13] + 0x8f1bbcdc;
		ee = (ee << 9 | ee >> (32 - 9)) + dd;
		bb = (bb << 10 | bb >> (32 - 10));

		// II(ref dd, ref ee, ref aa, ref bb, ref cc, blockDWords[3], 14);
		dd += ripemd160_I(ee, aa, bb) + blockDWords[3] + 0x8f1bbcdc;
		dd = (dd << 14 | dd >> (32 - 14)) + cc;
		aa = (aa << 10 | aa >> (32 - 10));

		// II(ref cc, ref dd, ref ee, ref aa, ref bb, blockDWords[7], 5);
		cc += ripemd160_I(dd, ee, aa) + blockDWords[7] + 0x8f1bbcdc;
		cc = (cc << 5 | cc >> (32 - 5)) + bb;
		ee = (ee << 10 | ee >> (32 - 10));

		// II(ref bb, ref cc, ref dd, ref ee, ref aa, blockDWords[15], 6);
		bb += ripemd160_I(cc, dd, ee) + blockDWords[15] + 0x8f1bbcdc;
		bb = (bb << 6 | bb >> (32 - 6)) + aa;
		dd = (dd << 10 | dd >> (32 - 10));

		// II(ref aa, ref bb, ref cc, ref dd, ref ee, blockDWords[14], 8);
		aa += ripemd160_I(bb, cc, dd) + blockDWords[14] + 0x8f1bbcdc;
		aa = (aa << 8 | aa >> (32 - 8)) + ee;
		cc = (cc << 10 | cc >> (32 - 10));

		// II(ref ee, ref aa, ref bb, ref cc, ref dd, blockDWords[5], 6);
		ee += ripemd160_I(aa, bb, cc) + blockDWords[5] + 0x8f1bbcdc;
		ee = (ee << 6 | ee >> (32 - 6)) + dd;
		bb = (bb << 10 | bb >> (32 - 10));

		// II(ref dd, ref ee, ref aa, ref bb, ref cc, blockDWords[6], 5);
		dd += ripemd160_I(ee, aa, bb) + blockDWords[6] + 0x8f1bbcdc;
		dd = (dd << 5 | dd >> (32 - 5)) + cc;
		aa = (aa << 10 | aa >> (32 - 10));

		// II(ref cc, ref dd, ref ee, ref aa, ref bb, blockDWords[2], 12);
		cc += ripemd160_I(dd, ee, aa) + blockDWords[2] + 0x8f1bbcdc;
		cc = (cc << 12 | cc >> (32 - 12)) + bb;
		ee = (ee << 10 | ee >> (32 - 10));

		// Left Round 5 
		// JJ(ref bb, ref cc, ref dd, ref ee, ref aa, blockDWords[4], 9);
		bb += ripemd160_J(cc, dd, ee) + blockDWords[4] + 0xa953fd4e;
		bb = (bb << 9 | bb >> (32 - 9)) + aa;
		dd = (dd << 10 | dd >> (32 - 10));

		// JJ(ref aa, ref bb, ref cc, ref dd, ref ee, blockDWords[0], 15);
		aa += ripemd160_J(bb, cc, dd) + blockDWords[0] + 0xa953fd4e;
		aa = (aa << 15 | aa >> (32 - 15)) + ee;
		cc = (cc << 10 | cc >> (32 - 10));

		// JJ(ref ee, ref aa, ref bb, ref cc, ref dd, blockDWords[5], 5);
		ee += ripemd160_J(aa, bb, cc) + blockDWords[5] + 0xa953fd4e;
		ee = (ee << 5 | ee >> (32 - 5)) + dd;
		bb = (bb << 10 | bb >> (32 - 10));

		// JJ(ref dd, ref ee, ref aa, ref bb, ref cc, blockDWords[9], 11);
		dd += ripemd160_J(ee, aa, bb) + blockDWords[9] + 0xa953fd4e;
		dd = (dd << 11 | dd >> (32 - 11)) + cc;
		aa = (aa << 10 | aa >> (32 - 10));

		// JJ(ref cc, ref dd, ref ee, ref aa, ref bb, blockDWords[7], 6);
		cc += ripemd160_J(dd, ee, aa) + blockDWords[7] + 0xa953fd4e;
		cc = (cc << 6 | cc >> (32 - 6)) + bb;
		ee = (ee << 10 | ee >> (32 - 10));

		// JJ(ref bb, ref cc, ref dd, ref ee, ref aa, blockDWords[12], 8);
		bb += ripemd160_J(cc, dd, ee) + blockDWords[12] + 0xa953fd4e;
		bb = (bb << 8 | bb >> (32 - 8)) + aa;
		dd = (dd << 10 | dd >> (32 - 10));

		// JJ(ref aa, ref bb, ref cc, ref dd, ref ee, blockDWords[2], 13);
		aa += ripemd160_J(bb, cc, dd) + blockDWords[2] + 0xa953fd4e;
		aa = (aa << 13 | aa >> (32 - 13)) + ee;
		cc = (cc << 10 | cc >> (32 - 10));

		// JJ(ref ee, ref aa, ref bb, ref cc, ref dd, blockDWords[10], 12);
		ee += ripemd160_J(aa, bb, cc) + blockDWords[10] + 0xa953fd4e;
		ee = (ee << 12 | ee >> (32 - 12)) + dd;
		bb = (bb << 10 | bb >> (32 - 10));

		// JJ(ref dd, ref ee, ref aa, ref bb, ref cc, blockDWords[14], 5);
		dd += ripemd160_J(ee, aa, bb) + blockDWords[14] + 0xa953fd4e;
		dd = (dd << 5 | dd >> (32 - 5)) + cc;
		aa = (aa << 10 | aa >> (32 - 10));

		// JJ(ref cc, ref dd, ref ee, ref aa, ref bb, blockDWords[1], 12);
		cc += ripemd160_J(dd, ee, aa) + blockDWords[1] + 0xa953fd4e;
		cc = (cc << 12 | cc >> (32 - 12)) + bb;
		ee = (ee << 10 | ee >> (32 - 10));

		// JJ(ref bb, ref cc, ref dd, ref ee, ref aa, blockDWords[3], 13);
		bb += ripemd160_J(cc, dd, ee) + blockDWords[3] + 0xa953fd4e;
		bb = (bb << 13 | bb >> (32 - 13)) + aa;
		dd = (dd << 10 | dd >> (32 - 10));

		// JJ(ref aa, ref bb, ref cc, ref dd, ref ee, blockDWords[8], 14);
		aa += ripemd160_J(bb, cc, dd) + blockDWords[8] + 0xa953fd4e;
		aa = (aa << 14 | aa >> (32 - 14)) + ee;
		cc = (cc << 10 | cc >> (32 - 10));

		// JJ(ref ee, ref aa, ref bb, ref cc, ref dd, blockDWords[11], 11);
		ee += ripemd160_J(aa, bb, cc) + blockDWords[11] + 0xa953fd4e;
		ee = (ee << 11 | ee >> (32 - 11)) + dd;
		bb = (bb << 10 | bb >> (32 - 10));

		// JJ(ref dd, ref ee, ref aa, ref bb, ref cc, blockDWords[6], 8);
		dd += ripemd160_J(ee, aa, bb) + blockDWords[6] + 0xa953fd4e;
		dd = (dd << 8 | dd >> (32 - 8)) + cc;
		aa = (aa << 10 | aa >> (32 - 10));

		// JJ(ref cc, ref dd, ref ee, ref aa, ref bb, blockDWords[15], 5);
		cc += ripemd160_J(dd, ee, aa) + blockDWords[15] + 0xa953fd4e;
		cc = (cc << 5 | cc >> (32 - 5)) + bb;
		ee = (ee << 10 | ee >> (32 - 10));

		// JJ(ref bb, ref cc, ref dd, ref ee, ref aa, blockDWords[13], 6);
		bb += ripemd160_J(cc, dd, ee) + blockDWords[13] + 0xa953fd4e;
		bb = (bb << 6 | bb >> (32 - 6)) + aa;
		dd = (dd << 10 | dd >> (32 - 10));

		// Parallel Right Round 1 
		// JJJ(ref aaa, ref bbb, ref ccc, ref ddd, ref eee, blockDWords[5], 8);
		aaa += ripemd160_J(bbb, ccc, ddd) + blockDWords[5] + 0x50a28be6;
		aaa = (aaa << 8 | aaa >> (32 - 8)) + eee;
		ccc = (ccc << 10 | ccc >> (32 - 10));

		// JJJ(ref eee, ref aaa, ref bbb, ref ccc, ref ddd, blockDWords[14], 9);
		eee += ripemd160_J(aaa, bbb, ccc) + blockDWords[14] + 0x50a28be6;
		eee = (eee << 9 | eee >> (32 - 9)) + ddd;
		bbb = (bbb << 10 | bbb >> (32 - 10));

		// JJJ(ref ddd, ref eee, ref aaa, ref bbb, ref ccc, blockDWords[7], 9);
		ddd += ripemd160_J(eee, aaa, bbb) + blockDWords[7] + 0x50a28be6;
		ddd = (ddd << 9 | ddd >> (32 - 9)) + ccc;
		aaa = (aaa << 10 | aaa >> (32 - 10));

		// JJJ(ref ccc, ref ddd, ref eee, ref aaa, ref bbb, blockDWords[0], 11);
		ccc += ripemd160_J(ddd, eee, aaa) + blockDWords[0] + 0x50a28be6;
		ccc = (ccc << 11 | ccc >> (32 - 11)) + bbb;
		eee = (eee << 10 | eee >> (32 - 10));

		// JJJ(ref bbb, ref ccc, ref ddd, ref eee, ref aaa, blockDWords[9], 13);
		bbb += ripemd160_J(ccc, ddd, eee) + blockDWords[9] + 0x50a28be6;
		bbb = (bbb << 13 | bbb >> (32 - 13)) + aaa;
		ddd = (ddd << 10 | ddd >> (32 - 10));

		// JJJ(ref aaa, ref bbb, ref ccc, ref ddd, ref eee, blockDWords[2], 15);
		aaa += ripemd160_J(bbb, ccc, ddd) + blockDWords[2] + 0x50a28be6;
		aaa = (aaa << 15 | aaa >> (32 - 15)) + eee;
		ccc = (ccc << 10 | ccc >> (32 - 10));

		// JJJ(ref eee, ref aaa, ref bbb, ref ccc, ref ddd, blockDWords[11], 15);
		eee += ripemd160_J(aaa, bbb, ccc) + blockDWords[11] + 0x50a28be6;
		eee = (eee << 15 | eee >> (32 - 15)) + ddd;
		bbb = (bbb << 10 | bbb >> (32 - 10));

		// JJJ(ref ddd, ref eee, ref aaa, ref bbb, ref ccc, blockDWords[4], 5);
		ddd += ripemd160_J(eee, aaa, bbb) + blockDWords[4] + 0x50a28be6;
		ddd = (ddd << 5 | ddd >> (32 - 5)) + ccc;
		aaa = (aaa << 10 | aaa >> (32 - 10));

		// JJJ(ref ccc, ref ddd, ref eee, ref aaa, ref bbb, blockDWords[13], 7);
		ccc += ripemd160_J(ddd, eee, aaa) + blockDWords[13] + 0x50a28be6;
		ccc = (ccc << 7 | ccc >> (32 - 7)) + bbb;
		eee = (eee << 10 | eee >> (32 - 10));

		// JJJ(ref bbb, ref ccc, ref ddd, ref eee, ref aaa, blockDWords[6], 7);
		bbb += ripemd160_J(ccc, ddd, eee) + blockDWords[6] + 0x50a28be6;
		bbb = (bbb << 7 | bbb >> (32 - 7)) + aaa;
		ddd = (ddd << 10 | ddd >> (32 - 10));

		// JJJ(ref aaa, ref bbb, ref ccc, ref ddd, ref eee, blockDWords[15], 8);
		aaa += ripemd160_J(bbb, ccc, ddd) + blockDWords[15] + 0x50a28be6;
		aaa = (aaa << 8 | aaa >> (32 - 8)) + eee;
		ccc = (ccc << 10 | ccc >> (32 - 10));

		// JJJ(ref eee, ref aaa, ref bbb, ref ccc, ref ddd, blockDWords[8], 11);
		eee += ripemd160_J(aaa, bbb, ccc) + blockDWords[8] + 0x50a28be6;
		eee = (eee << 11 | eee >> (32 - 11)) + ddd;
		bbb = (bbb << 10 | bbb >> (32 - 10));

		// JJJ(ref ddd, ref eee, ref aaa, ref bbb, ref ccc, blockDWords[1], 14);
		ddd += ripemd160_J(eee, aaa, bbb) + blockDWords[1] + 0x50a28be6;
		ddd = (ddd << 14 | ddd >> (32 - 14)) + ccc;
		aaa = (aaa << 10 | aaa >> (32 - 10));

		// JJJ(ref ccc, ref ddd, ref eee, ref aaa, ref bbb, blockDWords[10], 14);
		ccc += ripemd160_J(ddd, eee, aaa) + blockDWords[10] + 0x50a28be6;
		ccc = (ccc << 14 | ccc >> (32 - 14)) + bbb;
		eee = (eee << 10 | eee >> (32 - 10));

		// JJJ(ref bbb, ref ccc, ref ddd, ref eee, ref aaa, blockDWords[3], 12);
		bbb += ripemd160_J(ccc, ddd, eee) + blockDWords[3] + 0x50a28be6;
		bbb = (bbb << 12 | bbb >> (32 - 12)) + aaa;
		ddd = (ddd << 10 | ddd >> (32 - 10));

		// JJJ(ref aaa, ref bbb, ref ccc, ref ddd, ref eee, blockDWords[12], 6);
		aaa += ripemd160_J(bbb, ccc, ddd) + blockDWords[12] + 0x50a28be6;
		aaa = (aaa << 6 | aaa >> (32 - 6)) + eee;
		ccc = (ccc << 10 | ccc >> (32 - 10));

		// Parallel Right Round 2 
		// III(ref eee, ref aaa, ref bbb, ref ccc, ref ddd, blockDWords[6], 9); 
		eee += ripemd160_I(aaa, bbb, ccc) + blockDWords[6] + 0x5c4dd124;
		eee = (eee << 9 | eee >> (32 - 9)) + ddd;
		bbb = (bbb << 10 | bbb >> (32 - 10));

		// III(ref ddd, ref eee, ref aaa, ref bbb, ref ccc, blockDWords[11], 13);
		ddd += ripemd160_I(eee, aaa, bbb) + blockDWords[11] + 0x5c4dd124;
		ddd = (ddd << 13 | ddd >> (32 - 13)) + ccc;
		aaa = (aaa << 10 | aaa >> (32 - 10));

		// III(ref ccc, ref ddd, ref eee, ref aaa, ref bbb, blockDWords[3], 15);
		ccc += ripemd160_I(ddd, eee, aaa) + blockDWords[3] + 0x5c4dd124;
		ccc = (ccc << 15 | ccc >> (32 - 15)) + bbb;
		eee = (eee << 10 | eee >> (32 - 10));

		// III(ref bbb, ref ccc, ref ddd, ref eee, ref aaa, blockDWords[7], 7);
		bbb += ripemd160_I(ccc, ddd, eee) + blockDWords[7] + 0x5c4dd124;
		bbb = (bbb << 7 | bbb >> (32 - 7)) + aaa;
		ddd = (ddd << 10 | ddd >> (32 - 10));

		// III(ref aaa, ref bbb, ref ccc, ref ddd, ref eee, blockDWords[0], 12);
		aaa += ripemd160_I(bbb, ccc, ddd) + blockDWords[0] + 0x5c4dd124;
		aaa = (aaa << 12 | aaa >> (32 - 12)) + eee;
		ccc = (ccc << 10 | ccc >> (32 - 10));

		// III(ref eee, ref aaa, ref bbb, ref ccc, ref ddd, blockDWords[13], 8);
		eee += ripemd160_I(aaa, bbb, ccc) + blockDWords[13] + 0x5c4dd124;
		eee = (eee << 8 | eee >> (32 - 8)) + ddd;
		bbb = (bbb << 10 | bbb >> (32 - 10));

		// III(ref ddd, ref eee, ref aaa, ref bbb, ref ccc, blockDWords[5], 9);
		ddd += ripemd160_I(eee, aaa, bbb) + blockDWords[5] + 0x5c4dd124;
		ddd = (ddd << 9 | ddd >> (32 - 9)) + ccc;
		aaa = (aaa << 10 | aaa >> (32 - 10));

		// III(ref ccc, ref ddd, ref eee, ref aaa, ref bbb, blockDWords[10], 11);
		ccc += ripemd160_I(ddd, eee, aaa) + blockDWords[10] + 0x5c4dd124;
		ccc = (ccc << 11 | ccc >> (32 - 11)) + bbb;
		eee = (eee << 10 | eee >> (32 - 10));

		// III(ref bbb, ref ccc, ref ddd, ref eee, ref aaa, blockDWords[14], 7);
		bbb += ripemd160_I(ccc, ddd, eee) + blockDWords[14] + 0x5c4dd124;
		bbb = (bbb << 7 | bbb >> (32 - 7)) + aaa;
		ddd = (ddd << 10 | ddd >> (32 - 10));

		// III(ref aaa, ref bbb, ref ccc, ref ddd, ref eee, blockDWords[15], 7);
		aaa += ripemd160_I(bbb, ccc, ddd) + blockDWords[15] + 0x5c4dd124;
		aaa = (aaa << 7 | aaa >> (32 - 7)) + eee;
		ccc = (ccc << 10 | ccc >> (32 - 10));

		// III(ref eee, ref aaa, ref bbb, ref ccc, ref ddd, blockDWords[8], 12);
		eee += ripemd160_I(aaa, bbb, ccc) + blockDWords[8] + 0x5c4dd124;
		eee = (eee << 12 | eee >> (32 - 12)) + ddd;
		bbb = (bbb << 10 | bbb >> (32 - 10));

		// III(ref ddd, ref eee, ref aaa, ref bbb, ref ccc, blockDWords[12], 7);
		ddd += ripemd160_I(eee, aaa, bbb) + blockDWords[12] + 0x5c4dd124;
		ddd = (ddd << 7 | ddd >> (32 - 7)) + ccc;
		aaa = (aaa << 10 | aaa >> (32 - 10));

		// III(ref ccc, ref ddd, ref eee, ref aaa, ref bbb, blockDWords[4], 6);
		ccc += ripemd160_I(ddd, eee, aaa) + blockDWords[4] + 0x5c4dd124;
		ccc = (ccc << 6 | ccc >> (32 - 6)) + bbb;
		eee = (eee << 10 | eee >> (32 - 10));

		// III(ref bbb, ref ccc, ref ddd, ref eee, ref aaa, blockDWords[9], 15);
		bbb += ripemd160_I(ccc, ddd, eee) + blockDWords[9] + 0x5c4dd124;
		bbb = (bbb << 15 | bbb >> (32 - 15)) + aaa;
		ddd = (ddd << 10 | ddd >> (32 - 10));

		// III(ref aaa, ref bbb, ref ccc, ref ddd, ref eee, blockDWords[1], 13);
		aaa += ripemd160_I(bbb, ccc, ddd) + blockDWords[1] + 0x5c4dd124;
		aaa = (aaa << 13 | aaa >> (32 - 13)) + eee;
		ccc = (ccc << 10 | ccc >> (32 - 10));

		// III(ref eee, ref aaa, ref bbb, ref ccc, ref ddd, blockDWords[2], 11);
		eee += ripemd160_I(aaa, bbb, ccc) + blockDWords[2] + 0x5c4dd124;
		eee = (eee << 11 | eee >> (32 - 11)) + ddd;
		bbb = (bbb << 10 | bbb >> (32 - 10));

		// Parallel Right Round 3
		// HHH(ref ddd, ref eee, ref aaa, ref bbb, ref ccc, blockDWords[15], 9);
		ddd += ripemd160_H(eee, aaa, bbb) + blockDWords[15] + 0x6d703ef3;
		ddd = (ddd << 9 | ddd >> (32 - 9)) + ccc;
		aaa = (aaa << 10 | aaa >> (32 - 10));

		// HHH(ref ccc, ref ddd, ref eee, ref aaa, ref bbb, blockDWords[5], 7);
		ccc += ripemd160_H(ddd, eee, aaa) + blockDWords[5] + 0x6d703ef3;
		ccc = (ccc << 7 | ccc >> (32 - 7)) + bbb;
		eee = (eee << 10 | eee >> (32 - 10));

		// HHH(ref bbb, ref ccc, ref ddd, ref eee, ref aaa, blockDWords[1], 15);
		bbb += ripemd160_H(ccc, ddd, eee) + blockDWords[1] + 0x6d703ef3;
		bbb = (bbb << 15 | bbb >> (32 - 15)) + aaa;
		ddd = (ddd << 10 | ddd >> (32 - 10));

		// HHH(ref aaa, ref bbb, ref ccc, ref ddd, ref eee, blockDWords[3], 11);
		aaa += ripemd160_H(bbb, ccc, ddd) + blockDWords[3] + 0x6d703ef3;
		aaa = (aaa << 11 | aaa >> (32 - 11)) + eee;
		ccc = (ccc << 10 | ccc >> (32 - 10));

		// HHH(ref eee, ref aaa, ref bbb, ref ccc, ref ddd, blockDWords[7], 8);
		eee += ripemd160_H(aaa, bbb, ccc) + blockDWords[7] + 0x6d703ef3;
		eee = (eee << 8 | eee >> (32 - 8)) + ddd;
		bbb = (bbb << 10 | bbb >> (32 - 10));

		// HHH(ref ddd, ref eee, ref aaa, ref bbb, ref ccc, blockDWords[14], 6);
		ddd += ripemd160_H(eee, aaa, bbb) + blockDWords[14] + 0x6d703ef3;
		ddd = (ddd << 6 | ddd >> (32 - 6)) + ccc;
		aaa = (aaa << 10 | aaa >> (32 - 10));

		// HHH(ref ccc, ref ddd, ref eee, ref aaa, ref bbb, blockDWords[6], 6);
		ccc += ripemd160_H(ddd, eee, aaa) + blockDWords[6] + 0x6d703ef3;
		ccc = (ccc << 6 | ccc >> (32 - 6)) + bbb;
		eee = (eee << 10 | eee >> (32 - 10));

		// HHH(ref bbb, ref ccc, ref ddd, ref eee, ref aaa, blockDWords[9], 14);
		bbb += ripemd160_H(ccc, ddd, eee) + blockDWords[9] + 0x6d703ef3;
		bbb = (bbb << 14 | bbb >> (32 - 14)) + aaa;
		ddd = (ddd << 10 | ddd >> (32 - 10));

		// HHH(ref aaa, ref bbb, ref ccc, ref ddd, ref eee, blockDWords[11], 12);
		aaa += ripemd160_H(bbb, ccc, ddd) + blockDWords[11] + 0x6d703ef3;
		aaa = (aaa << 12 | aaa >> (32 - 12)) + eee;
		ccc = (ccc << 10 | ccc >> (32 - 10));

		// HHH(ref eee, ref aaa, ref bbb, ref ccc, ref ddd, blockDWords[8], 13);
		eee += ripemd160_H(aaa, bbb, ccc) + blockDWords[8] + 0x6d703ef3;
		eee = (eee << 13 | eee >> (32 - 13)) + ddd;
		bbb = (bbb << 10 | bbb >> (32 - 10));

		// HHH(ref ddd, ref eee, ref aaa, ref bbb, ref ccc, blockDWords[12], 5);
		ddd += ripemd160_H(eee, aaa, bbb) + blockDWords[12] + 0x6d703ef3;
		ddd = (ddd << 5 | ddd >> (32 - 5)) + ccc;
		aaa = (aaa << 10 | aaa >> (32 - 10));

		// HHH(ref ccc, ref ddd, ref eee, ref aaa, ref bbb, blockDWords[2], 14);
		ccc += ripemd160_H(ddd, eee, aaa) + blockDWords[2] + 0x6d703ef3;
		ccc = (ccc << 14 | ccc >> (32 - 14)) + bbb;
		eee = (eee << 10 | eee >> (32 - 10));

		// HHH(ref bbb, ref ccc, ref ddd, ref eee, ref aaa, blockDWords[10], 13);
		bbb += ripemd160_H(ccc, ddd, eee) + blockDWords[10] + 0x6d703ef3;
		bbb = (bbb << 13 | bbb >> (32 - 13)) + aaa;
		ddd = (ddd << 10 | ddd >> (32 - 10));

		// HHH(ref aaa, ref bbb, ref ccc, ref ddd, ref eee, blockDWords[0], 13);
		aaa += ripemd160_H(bbb, ccc, ddd) + blockDWords[0] + 0x6d703ef3;
		aaa = (aaa << 13 | aaa >> (32 - 13)) + eee;
		ccc = (ccc << 10 | ccc >> (32 - 10));

		// HHH(ref eee, ref aaa, ref bbb, ref ccc, ref ddd, blockDWords[4], 7);
		eee += ripemd160_H(aaa, bbb, ccc) + blockDWords[4] + 0x6d703ef3;
		eee = (eee << 7 | eee >> (32 - 7)) + ddd;
		bbb = (bbb << 10 | bbb >> (32 - 10));

		// HHH(ref ddd, ref eee, ref aaa, ref bbb, ref ccc, blockDWords[13], 5);
		ddd += ripemd160_H(eee, aaa, bbb) + blockDWords[13] + 0x6d703ef3;
		ddd = (ddd << 5 | ddd >> (32 - 5)) + ccc;
		aaa = (aaa << 10 | aaa >> (32 - 10));

		// Parallel Right Round 4
		// GGG(ref ccc, ref ddd, ref eee, ref aaa, ref bbb, blockDWords[8], 15);
		ccc += ripemd160_G(ddd, eee, aaa) + blockDWords[8] + 0x7a6d76e9;
		ccc = (ccc << 15 | ccc >> (32 - 15)) + bbb;
		eee = (eee << 10 | eee >> (32 - 10));

		// GGG(ref bbb, ref ccc, ref ddd, ref eee, ref aaa, blockDWords[6], 5);
		bbb += ripemd160_G(ccc, ddd, eee) + blockDWords[6] + 0x7a6d76e9;
		bbb = (bbb << 5 | bbb >> (32 - 5)) + aaa;
		ddd = (ddd << 10 | ddd >> (32 - 10));

		// GGG(ref aaa, ref bbb, ref ccc, ref ddd, ref eee, blockDWords[4], 8);
		aaa += ripemd160_G(bbb, ccc, ddd) + blockDWords[4] + 0x7a6d76e9;
		aaa = (aaa << 8 | aaa >> (32 - 8)) + eee;
		ccc = (ccc << 10 | ccc >> (32 - 10));

		// GGG(ref eee, ref aaa, ref bbb, ref ccc, ref ddd, blockDWords[1], 11);
		eee += ripemd160_G(aaa, bbb, ccc) + blockDWords[1] + 0x7a6d76e9;
		eee = (eee << 11 | eee >> (32 - 11)) + ddd;
		bbb = (bbb << 10 | bbb >> (32 - 10));

		// GGG(ref ddd, ref eee, ref aaa, ref bbb, ref ccc, blockDWords[3], 14);
		ddd += ripemd160_G(eee, aaa, bbb) + blockDWords[3] + 0x7a6d76e9;
		ddd = (ddd << 14 | ddd >> (32 - 14)) + ccc;
		aaa = (aaa << 10 | aaa >> (32 - 10));

		// GGG(ref ccc, ref ddd, ref eee, ref aaa, ref bbb, blockDWords[11], 14);
		ccc += ripemd160_G(ddd, eee, aaa) + blockDWords[11] + 0x7a6d76e9;
		ccc = (ccc << 14 | ccc >> (32 - 14)) + bbb;
		eee = (eee << 10 | eee >> (32 - 10));

		// GGG(ref bbb, ref ccc, ref ddd, ref eee, ref aaa, blockDWords[15], 6);
		bbb += ripemd160_G(ccc, ddd, eee) + blockDWords[15] + 0x7a6d76e9;
		bbb = (bbb << 6 | bbb >> (32 - 6)) + aaa;
		ddd = (ddd << 10 | ddd >> (32 - 10));

		// GGG(ref aaa, ref bbb, ref ccc, ref ddd, ref eee, blockDWords[0], 14);
		aaa += ripemd160_G(bbb, ccc, ddd) + blockDWords[0] + 0x7a6d76e9;
		aaa = (aaa << 14 | aaa >> (32 - 14)) + eee;
		ccc = (ccc << 10 | ccc >> (32 - 10));

		// GGG(ref eee, ref aaa, ref bbb, ref ccc, ref ddd, blockDWords[5], 6);
		eee += ripemd160_G(aaa, bbb, ccc) + blockDWords[5] + 0x7a6d76e9;
		eee = (eee << 6 | eee >> (32 - 6)) + ddd;
		bbb = (bbb << 10 | bbb >> (32 - 10));

		// GGG(ref ddd, ref eee, ref aaa, ref bbb, ref ccc, blockDWords[12], 9);
		ddd += ripemd160_G(eee, aaa, bbb) + blockDWords[12] + 0x7a6d76e9;
		ddd = (ddd << 9 | ddd >> (32 - 9)) + ccc;
		aaa = (aaa << 10 | aaa >> (32 - 10));

		// GGG(ref ccc, ref ddd, ref eee, ref aaa, ref bbb, blockDWords[2], 12);
		ccc += ripemd160_G(ddd, eee, aaa) + blockDWords[2] + 0x7a6d76e9;
		ccc = (ccc << 12 | ccc >> (32 - 12)) + bbb;
		eee = (eee << 10 | eee >> (32 - 10));

		// GGG(ref bbb, ref ccc, ref ddd, ref eee, ref aaa, blockDWords[13], 9);
		bbb += ripemd160_G(ccc, ddd, eee) + blockDWords[13] + 0x7a6d76e9;
		bbb = (bbb << 9 | bbb >> (32 - 9)) + aaa;
		ddd = (ddd << 10 | ddd >> (32 - 10));

		// GGG(ref aaa, ref bbb, ref ccc, ref ddd, ref eee, blockDWords[9], 12);
		aaa += ripemd160_G(bbb, ccc, ddd) + blockDWords[9] + 0x7a6d76e9;
		aaa = (aaa << 12 | aaa >> (32 - 12)) + eee;
		ccc = (ccc << 10 | ccc >> (32 - 10));

		// GGG(ref eee, ref aaa, ref bbb, ref ccc, ref ddd, blockDWords[7], 5);
		eee += ripemd160_G(aaa, bbb, ccc) + blockDWords[7] + 0x7a6d76e9;
		eee = (eee << 5 | eee >> (32 - 5)) + ddd;
		bbb = (bbb << 10 | bbb >> (32 - 10));

		// GGG(ref ddd, ref eee, ref aaa, ref bbb, ref ccc, blockDWords[10], 15);
		ddd += ripemd160_G(eee, aaa, bbb) + blockDWords[10] + 0x7a6d76e9;
		ddd = (ddd << 15 | ddd >> (32 - 15)) + ccc;
		aaa = (aaa << 10 | aaa >> (32 - 10));

		// GGG(ref ccc, ref ddd, ref eee, ref aaa, ref bbb, blockDWords[14], 8);
		ccc += ripemd160_G(ddd, eee, aaa) + blockDWords[14] + 0x7a6d76e9;
		ccc = (ccc << 8 | ccc >> (32 - 8)) + bbb;
		eee = (eee << 10 | eee >> (32 - 10));

		// Parallel Right Round 5 
		// FFF(ref bbb, ref ccc, ref ddd, ref eee, ref aaa, blockDWords[12], 8);
		bbb += ripemd160_F(ccc, ddd, eee) + blockDWords[12];
		bbb = (bbb << 8 | bbb >> (32 - 8)) + aaa;
		ddd = (ddd << 10 | ddd >> (32 - 10));

		// FFF(ref aaa, ref bbb, ref ccc, ref ddd, ref eee, blockDWords[15], 5);
		aaa += ripemd160_F(bbb, ccc, ddd) + blockDWords[15];
		aaa = (aaa << 5 | aaa >> (32 - 5)) + eee;
		ccc = (ccc << 10 | ccc >> (32 - 10));

		// FFF(ref eee, ref aaa, ref bbb, ref ccc, ref ddd, blockDWords[10], 12);
		eee += ripemd160_F(aaa, bbb, ccc) + blockDWords[10];
		eee = (eee << 12 | eee >> (32 - 12)) + ddd;
		bbb = (bbb << 10 | bbb >> (32 - 10));

		// FFF(ref ddd, ref eee, ref aaa, ref bbb, ref ccc, blockDWords[4], 9);
		ddd += ripemd160_F(eee, aaa, bbb) + blockDWords[4];
		ddd = (ddd << 9 | ddd >> (32 - 9)) + ccc;
		aaa = (aaa << 10 | aaa >> (32 - 10));

		// FFF(ref ccc, ref ddd, ref eee, ref aaa, ref bbb, blockDWords[1], 12);
		ccc += ripemd160_F(ddd, eee, aaa) + blockDWords[1];
		ccc = (ccc << 12 | ccc >> (32 - 12)) + bbb;
		eee = (eee << 10 | eee >> (32 - 10));

		// FFF(ref bbb, ref ccc, ref ddd, ref eee, ref aaa, blockDWords[5], 5);
		bbb += ripemd160_F(ccc, ddd, eee) + blockDWords[5];
		bbb = (bbb << 5 | bbb >> (32 - 5)) + aaa;
		ddd = (ddd << 10 | ddd >> (32 - 10));

		// FFF(ref aaa, ref bbb, ref ccc, ref ddd, ref eee, blockDWords[8], 14);
		aaa += ripemd160_F(bbb, ccc, ddd) + blockDWords[8];
		aaa = (aaa << 14 | aaa >> (32 - 14)) + eee;
		ccc = (ccc << 10 | ccc >> (32 - 10));

		// FFF(ref eee, ref aaa, ref bbb, ref ccc, ref ddd, blockDWords[7], 6);
		eee += ripemd160_F(aaa, bbb, ccc) + blockDWords[7];
		eee = (eee << 6 | eee >> (32 - 6)) + ddd;
		bbb = (bbb << 10 | bbb >> (32 - 10));

		// FFF(ref ddd, ref eee, ref aaa, ref bbb, ref ccc, blockDWords[6], 8);
		ddd += ripemd160_F(eee, aaa, bbb) + blockDWords[6];
		ddd = (ddd << 8 | ddd >> (32 - 8)) + ccc;
		aaa = (aaa << 10 | aaa >> (32 - 10));

		// FFF(ref ccc, ref ddd, ref eee, ref aaa, ref bbb, blockDWords[2], 13);
		ccc += ripemd160_F(ddd, eee, aaa) + blockDWords[2];
		ccc = (ccc << 13 | ccc >> (32 - 13)) + bbb;
		eee = (eee << 10 | eee >> (32 - 10));

		// FFF(ref bbb, ref ccc, ref ddd, ref eee, ref aaa, blockDWords[13], 6);
		bbb += ripemd160_F(ccc, ddd, eee) + blockDWords[13];
		bbb = (bbb << 6 | bbb >> (32 - 6)) + aaa;
		ddd = (ddd << 10 | ddd >> (32 - 10));

		// FFF(ref aaa, ref bbb, ref ccc, ref ddd, ref eee, blockDWords[14], 5);
		aaa += ripemd160_F(bbb, ccc, ddd) + blockDWords[14];
		aaa = (aaa << 5 | aaa >> (32 - 5)) + eee;
		ccc = (ccc << 10 | ccc >> (32 - 10));

		// FFF(ref eee, ref aaa, ref bbb, ref ccc, ref ddd, blockDWords[0], 15);
		eee += ripemd160_F(aaa, bbb, ccc) + blockDWords[0];
		eee = (eee << 15 | eee >> (32 - 15)) + ddd;
		bbb = (bbb << 10 | bbb >> (32 - 10));

		// FFF(ref ddd, ref eee, ref aaa, ref bbb, ref ccc, blockDWords[3], 13);
		ddd += ripemd160_F(eee, aaa, bbb) + blockDWords[3];
		ddd = (ddd << 13 | ddd >> (32 - 13)) + ccc;
		aaa = (aaa << 10 | aaa >> (32 - 10));

		// FFF(ref ccc, ref ddd, ref eee, ref aaa, ref bbb, blockDWords[9], 11);
		ccc += ripemd160_F(ddd, eee, aaa) + blockDWords[9];
		ccc = (ccc << 11 | ccc >> (32 - 11)) + bbb;
		eee = (eee << 10 | eee >> (32 - 10));

		// FFF(ref bbb, ref ccc, ref ddd, ref eee, ref aaa, blockDWords[11], 11);
		bbb += ripemd160_F(ccc, ddd, eee) + blockDWords[11];
		bbb = (bbb << 11 | bbb >> (32 - 11)) + aaa;
		ddd = (ddd << 10 | ddd >> (32 - 10));

		// Update the state of the hash object
		ddd += cc + state[1];
		state[1] = state[2] + dd + eee;
		state[2] = state[3] + ee + aaa;
		state[3] = state[4] + aa + bbb;
		state[4] = state[0] + bb + ccc;
		state[0] = ddd;
}

// The five basic functions
uint32_t ripemd160_F(uint32_t x, uint32_t y, uint32_t z)
{
    return (x ^ y ^ z);
}

uint32_t ripemd160_G(uint32_t x, uint32_t y, uint32_t z)
{
    return ((x & y) | (~x & z));
}

uint32_t ripemd160_H(uint32_t x, uint32_t y, uint32_t z)
{
    return ((x | ~y) ^ z);
}

uint32_t ripemd160_I(uint32_t x, uint32_t y, uint32_t z)
{
    return ((x & z) | (y & ~z));
}

uint32_t ripemd160_J(uint32_t x, uint32_t y, uint32_t z)
{
    return (x ^ (y | ~z));
}

void DWORDFromLittleEndian(uint32_t *x, int digits, uint8_t *block)
{
		int i;
		int j;

		for (i = 0, j = 0; i < digits; i++, j += 4)
				x[i] = (uint32_t)(block[j] | (block[j + 1] << 8) | (block[j + 2] << 16) | (block[j + 3] << 24));
}

void DWORDToLittleEndian(uint8_t *block, uint32_t *x, int digits)
{
		int i;
		int j;

		for (i = 0, j = 0; i < digits; i++, j += 4)
		{
				block[j] = x[i] & 0xff;
				block[j + 1] = (x[i] >> 8) & 0xff;
				block[j + 2] = (x[i] >> 16) & 0xff;
				block[j + 3] = (x[i] >> 24) & 0xff;
		}
}
