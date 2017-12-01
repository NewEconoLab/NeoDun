/*
 * FingerPrintPacket.h
 *
 *  Created on: 2017年9月4日
 *      Author: Administrator
 */

#ifndef CPP_FINGERPRINT_FINGERPRINTPACKET_H_
#define CPP_FINGERPRINT_FINGERPRINTPACKET_H_

#include "../myType.h"

class FingerPrintPacket {
public:
	static const int CMD_VERIFY = 0x13;					//	1 0x13 验证口令
	static const int CMD_SET = 0x12;					//	2 0x12 设置口令
	static const int CMD_SET_MODULE_ADDRESS = 0x15;		//	3 0x15 设置模块地址
	static const int CMD_SET_SYSTEM_CONFIG = 0x0e;		//	4 0x0e 设置模块系统参数(波特率，最长帧长)
	static const int CMD_GET_CONFIG = 0x0f;				//	5 0x0f 获取模块系统参数
	static const int CMD_GET_FINGER_LIST = 0x1f;		//	6 0x1f 获取指纹索引列表
	static const int CMD_GET_FINGER_COUNT = 0x1d;		//	7 0x1d 获取指纹数量
	static const int CMD_REG_FIGPRINT = 0x01;			//	8 0x01 注册指纹
	static const int CMD_UPLOAD_IMAGE = 0x0a;			//	9 0x0a 上传图像
	static const int CMD_DOWNLOAD_IMAGE = 0x0b;			//	10 0x0b 下载图像
	static const int CMD_MATCH = 0x04;					//	11 0x04 匹配指纹
	static const int CMD_DELETE = 0x0c;					//	12 0x0c 删除指纹
	static const int CMD_CLEAR = 0x0d;					//	13 0x0d 清空指纹库
	static const int CMD_INIT_CONFIG = 0x21;			//	14 0x21 恢复出厂设置
	static const int CMD_INIT_OK = 0x22;				//	15 0x22 初始化成功
	static const int CMD_HAND_SHAKE = 0x23;				//	16 0x23 握手
	static const int CMD_ENBALE_PRINT = 0x40;			//	17 0x40 使能打印功能
	static const int CMD_PRINT = 0x41;					//	18 0x41 打印功能
	static const int CMD_GET_PARAM = 0x42;				//	19 0x42 获取参数

	//------------------------------ 错误代码 -------------------------------------------------------

	static const int ERR_OK   = 0x00 	;				//指令执行成功
	static const int ERR_CONMUNICTAE   = 0x01 	;		//通信错误
	static const int ERR_GET_IMAGE   = 0x03 	;		//获取图像失败
	static const int ERR_MATCH   = 0x09 	;			//匹配失败
	static const int ERR_OUT_OF_BOUNDS   = 0x0b 	;	//指纹 ID 越界
	static const int ERR_OVER_MAX_SIZE   = 0x0e 	;	//接收帧长度大于最长长度
	static const int ERR_UP_CAPTURE   = 0x0f	;		//上传/采集图像错误
	static const int ERR_ID_FAILED   = 0x10 	;		//删除失败，指纹 ID 错误
	static const int ERR_CLEAR   = 0x11 	;			//清空指纹库失败
	static const int ERR_VAERIFY   = 0x13 	;			//验证口令失败
	static const int ERR_REG_CONFIG   = 0x1a 	;		//错误的寄存器/设置系统参数序号错误
	static const int ERR_REGISTE   = 0x1e 	;			//注册错误
	static const int ERR_ADDRESS   = 0x20 	;			//器件地址错误
	static const int ERR_MUST_VERIFY_CMD   = 0x21 	;	//必须验证口令
	static const int ERR_LIST_AND_SO   = 0x22	;		//获取指纹索引列表参数错误；录入时传感器错误；设置模块系统参数对应的内容出错；上传图像类型出错；下载图像时参数错误，图像数据长度超出可以接收的范围
	static const int ERR_ID_HOLD   = 0x24 	;			//指纹 ID 被占用
	static const int ERR_SINGLE_OK   = 0x25 	;		//单次采集成功
	static const int ERR_TIME_OUT   = 0x26 	;			//录入/匹配超时
	static const int ERR_DOWNLOAD   = 0x27 	;			//下载失败
	static const int ERR_CMD   = 0x28 	;				//命令错误
	static const int ERR_FLAG   = 0x29 	;				//包标识错误

private:
	static const u32 ADDRESS_DEFAULT = 0xffffffff;
private:
	u8 fsmCRC(u8 *dat, u32 len);
public:
	FingerPrintPacket();
	FingerPrintPacket(u8 packFlag );
};

#endif /* CPP_FINGERPRINT_FINGERPRINTPACKET_H_ */
