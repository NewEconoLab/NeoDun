/*
 * DelayExcute.h
 *
 *  Created on: 2017Äê8ÔÂ1ÈÕ
 *      Author: Administrator
 */

#ifndef CPP_DELAYEXCUTE_H_
#define CPP_DELAYEXCUTE_H_
#include "queue.h"

#define   ASYNC_MAX_ARG_CNT     6

typedef struct
{
	u32 		tick ;
    void        *handler;
    void        *content[ASYNC_MAX_ARG_CNT];

}Async_Excute_t;

class DelayExcute {
private:
	static queue<Async_Excute_t,30> task_q;
public:
	DelayExcute();
	static void Poll();
	bool SetDelayExcute( int delay , int argCnt,void * function , ... );

};

#endif /* CPP_DELAYEXCUTE_H_ */
