/*
 * PublicKey.h
 *
 *  Created on: 2017Äê9ÔÂ3ÈÕ
 *      Author: Administrator
 */

#ifndef CPP_PUBLICKEY_H_
#define CPP_PUBLICKEY_H_

#include "myType.h"

class PublicKey {
public:
	static int const KEY_SIZE = 65;
	u8 data[KEY_SIZE];
	PublicKey(u8 *_data);
	PublicKey( );
	u8* getData() const;
};

#endif /* CPP_PUBLICKEY_H_ */
