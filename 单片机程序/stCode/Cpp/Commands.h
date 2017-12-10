/*
 * Commands.h
 *
 *  Created on: 2017Äê7ÔÂ15ÈÕ
 *      Author: Administrator
 */

#ifndef CPP_COMMANDS_H_
#define CPP_COMMANDS_H_

#include "myType.h"

#define CMD_DATA_PACK_LEN      64


class Commands 
{
private:
		static u8   data[CMD_DATA_PACK_LEN];
		int index;
public:
		Commands(  );
		Commands( u16 cmd , u16 serial );
		void CreateDataQuest(u16 cmd, u16 serial,u16 firstIndex,u16 lastIndex,u8 *hash,u32 hashLen);
		void AppendU16(u16 data);
		void AppendU32(u16 data);
		void AppendBytes(u8 *data , int len);
		void SendToPc();
};


#endif /* CPP_COMMANDS_H_ */
