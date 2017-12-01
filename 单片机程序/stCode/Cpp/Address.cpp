/*
 * Address.cpp
 *
 *  Created on: 2017Äê9ÔÂ2ÈÕ
 *      Author: Administrator
 */

#include "Address.h"
#include <string.h>

Address::Address() 
{
		
}

u16 Address::getAddressType() const 
{
		return addressType;
}

char* Address::getContent() const 
{
		return (char*)content;
}

//add by hkh
u8* Address::getPrivatekey() const
{
		return (u8*)privatekey;
}

u32 Address::getDataId0() const 
{
		return dataId0;
}

u32 Address::getDataId1() const 
{
		return dataId1;
}

u32 Address::getDataId2() const 
{
		return dataId2;
}

u32 Address::getDataId3() const 
{
		return dataId3;
}

Address::Address(char *_content, u8 *_privatekey,u16 _addressType, u32 _dataId0, u32 _dataId1, u32 _dataId2, u32 _dataId3)
{
		memmove(this->content,_content,Address::ADDR_SIZE);
		memmove(this->privatekey,_privatekey,32);
		this->addressType = _addressType;
		this->dataId0 = _dataId0;
		this->dataId1 = _dataId1;
		this->dataId2 = _dataId2;
		this->dataId3 = _dataId3;
}



