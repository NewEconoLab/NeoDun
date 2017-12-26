/*
 * ReceiveAnalysis.h
 *
 *  Created on: 2017年7月30日
 *      Author: Administrator
 */

#ifndef CPP_RECEIVEANALYSIS_H_
#define CPP_RECEIVEANALYSIS_H_

#include "myType.h"

class ReceiveAnalysis 
{
private:
		static const int CON_STATE_IDLE				=0;
		static const int CON_START_PULL	 			=1;
		static const int DATA_BLOCK_SIZE      =64*1024;
		static const int DATA_PACK_SIZE       =50;
		ReceiveAnalysis();
		ReceiveAnalysis(const ReceiveAnalysis& copy);
		bool crc_frame(u8 data[],int len);
		void clearData();
		bool isAllPackGot();
		bool hashCompare(u8 * hash1 , u8 * hash2);
		int state;
		int packIndex;
		int packCount;
		u16	reqSerial;
		u16 notifySerial;		//通知下位机的时候的串号
		u32 dataLen;
		u8  dataSave[DATA_BLOCK_SIZE];
		bool packIndexRecord[(DATA_BLOCK_SIZE+DATA_PACK_SIZE-1)/DATA_PACK_SIZE ];
		u8 hashRecord[32];	//PC传过来的hash值的记录
		u8 resultsignRecord[98];
		
public:
		static ReceiveAnalysis & GetInstance();
		void PackDataFromPcCallback(u8  data[] ,int len );
		void Poll();
};


#endif /* CPP_RECEIVEANALYSIS_H_ */
