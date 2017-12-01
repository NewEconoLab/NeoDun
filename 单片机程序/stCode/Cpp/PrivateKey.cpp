/*
 * PrivateKey.cpp
 *
 *  Created on: 2017Äê9ÔÂ3ÈÕ
 *      Author: Administrator
 */

#include "PrivateKey.h"
#include <string.h>
#include "Utils.h"

extern "C"{
#include "Algorithm.h"
}

PrivateKey::PrivateKey(u8* _data) 
{
		memmove(this->data,_data,PrivateKey::KEY_SIZE);
}

bool PrivateKey::Compare(PrivateKey& right) 
{
		return Utils::MemoryCompare(this->data,right.data,PrivateKey::KEY_SIZE);
}

PublicKey PrivateKey::GetPublicKey(bool t) 
{
		PublicKey key ;
		int res = Alg_GetPublicFromPrivate(this->data,key.getData()+1);
	
		if(t)
		{
				if(key.data[64]%2)
						key.data[0] = 0x03;
				else
						key.data[0] = 0x02;
		}
		else
		{
				key.data[0] = 0x04;
		}
		
		if(res != 0 )
		{
				printf("get public key error =%d\r\n",res);
		}		
		return key;
}

PrivateKey::PrivateKey() 
{
}

u8* PrivateKey::getData() const 
{
		return (u8*)data;
}

PrivateKey PrivateKey::ParseFromWIF(char* wif ) 
{
		PrivateKey key  ;
		int res = Alg_GetPrivateFromWIF(wif, key.getData( ));
		if(res != 0 )
		{
				printf("get private key error =%d\r\n",res);
		}
		return key ;
}
