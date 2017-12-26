/*
 * KeyScan.cpp
 *
 *  Created on: 2017年7月12日
 *      Author: Administrator
 */

#include "KeyScan.h"

namespace view {

KeyScan::KeyScan(char port , int pin , void (*onPressed)(int eventType) )
{
		this->state = 2;
		this->port = port;
		this->pin = pin;
		this->onPressed = onPressed;
		this->pressTime = 0;
		this->preValue = 0 ;
}


void KeyScan::Scan()
{
		//使用状态机
		switch(state )
		{
				case STATE_INIT:
				{
						preValue = Hal::getInstance().ReadPin(port,pin);
						state = STATE_WAITE_CHANGE;
						break;
				}
				case STATE_WAITE_CHANGE:
				{
						u8 current = Hal::getInstance().ReadPin(port,pin);
						if(current != preValue)
						{
								//有变化了
								pressTime = HAL_GetTick();
								preValue = current;
								state = STATE_WAITE_UP;
						}
						break;
				}
				case STATE_WAITE_UP:
				{
						u8 current = Hal::getInstance().ReadPin(port,pin);
						if(current != preValue)
						{
								preValue = current;
								//有变化了
								if( HAL_GetTick() - pressTime >300 ) //间隔300ms
								{
										if( this->onPressed != NULL)
										{
												onPressed(EVENT_LONG_CLICK);
										}
								}
								else
								{
										if( this->onPressed != NULL)
										{
												onPressed(EVENT_SHORT_CLICK);
										}
								}
								state = STATE_INIT;
						}
						break;
				}
		}
}


} /* namespace view */
