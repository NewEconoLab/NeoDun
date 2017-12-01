/*
 * KeyScan.h
 *
 *  Created on: 2017Äê7ÔÂ12ÈÕ
 *      Author: Administrator
 */

#ifndef CPP_KEYSCAN_H_
#define CPP_KEYSCAN_H_

#include "Hal.h"

namespace view 
{

class KeyScan 
{
		char port;
		u8   pin;
		u8   state :3;
		u8   preValue :1;
		u32  pressTime;
		void (*onPressed)(int eventType);
		static const int STATE_INIT = 0;
		static const int STATE_WAITE_UP = 1;
		static const int STATE_WAITE_CHANGE = 2;

public:
		static const int EVENT_SHORT_CLICK = 0;
		static const int EVENT_LONG_CLICK = 1;
		KeyScan(char port , int pin , void (*onPressed)(int eventType) );
		void Scan();
};

} /* namespace view */

#endif /* CPP_KEYSCAN_H_ */
