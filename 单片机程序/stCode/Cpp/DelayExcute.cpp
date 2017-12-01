/*
 * DelayExcute.cpp
 *
 *  Created on: 2017Äê8ÔÂ1ÈÕ
 *      Author: Administrator
 */

#include "DelayExcute.h"
#include <string.h>

DelayExcute::DelayExcute() {
	// TODO Auto-generated constructor stub

}

void DelayExcute::Poll() {


}

bool DelayExcute::SetDelayExcute(int delay,int argcnt, void* handler,...) {
    if(argcnt < ASYNC_MAX_ARG_CNT)
    {
        Async_Excute_t exc;
        exc.handler = handler;
        exc.tick = HAL_GetTick() + delay ;
        memmove(exc.content,&handler + 1,argcnt*4);
        task_q.add(exc );
        return true;
    }
    return false;
}








