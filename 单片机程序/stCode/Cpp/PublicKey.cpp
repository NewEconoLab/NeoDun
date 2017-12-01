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
		memmove(this->data,_data,PublicKey::KEY_SIZE);
}

u8* PublicKey::getData() const 
{	
		return (u8*)data;
}

PublicKey::PublicKey() 
{

}

Address PublicKey::GetAddress() 
{
		Address address ;
		int len = 0;
		Alg_GetAddressFromPublic(this->data,(char*)address.getContent(),len);
		return address;
}
