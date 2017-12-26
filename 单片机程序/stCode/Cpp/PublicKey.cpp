/*
 * PublicKey.cpp
 *
 *  Created on: 2017Äê9ÔÂ3ÈÕ
 *      Author: Administrator
 */

#include "PublicKey.h"
#include <string.h>

extern "C"
{
#include "Algorithm.h"
}

PublicKey::PublicKey(u8* _data) 
{
		memset(this->data,0,PublicKey::KEY_SIZE);	
		memmove(this->data,_data,PublicKey::KEY_SIZE);
}

u8* PublicKey::getData() const 
{	
		return (u8*)data;
}

PublicKey::PublicKey() 
{

}

