/*
 * PrivateKey.h
 *
 *  Created on: 2017Äê9ÔÂ3ÈÕ
 *      Author: Administrator
 */

#ifndef CPP_PRIVATEKEY_H_
#define CPP_PRIVATEKEY_H_

#include "PublicKey.h"

class PrivateKey 
{
public:
		static int const KEY_SIZE =32;
private:
		u8 data[KEY_SIZE];
public:
		PrivateKey(u8 * _data);
		PrivateKey( );
		bool Compare(PrivateKey & right) ;
		PublicKey GetPublicKey(bool t);

		static PrivateKey ParseFromWIF( char *wif ) ;
		u8* getData() const;
};

#endif /* CPP_PRIVATEKEY_H_ */
