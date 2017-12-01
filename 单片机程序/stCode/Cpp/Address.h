/*
 * Address.h
 *
 *  Created on: 2017Äê9ÔÂ2ÈÕ
 *      Author: Administrator
 */

#ifndef CPP_ADDRESS_H_
#define CPP_ADDRESS_H_

#include "myType.h"

class Address 
{
public:
		static const int ADDR_SIZE = 40;
		static const u16 ADDR_XiaoYi = 0x0101;

private:
		char content[ADDR_SIZE];
		u16 addressType;
		u32 dataId0;
		u32 dataId1;
		u32 dataId2;
		u32 dataId3;
		//add by hkh
		u8 privatekey[32];
public:
		Address();
		Address(char *_content, u8 *_privatekey,u16 _addressType, u32 _dataId0, u32 _dataId1, u32 _dataId2, u32 _dataId3);
		static Address ParseFromBase58Data(u8 * data , int len );
		u16 getAddressType() const;
		char* getContent() const;
		u8* getPrivatekey() const;
		u32 getDataId0() const;
		u32 getDataId1() const;
		u32 getDataId2() const;
		u32 getDataId3() const;
};

#endif /* CPP_ADDRESS_H_ */
