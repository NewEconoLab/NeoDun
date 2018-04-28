/*
 * CMD.h
 *
 *  Created on: 2017年7月30日
 *      Author: Administrator
 */

#ifndef CPP_CMD_H_
#define CPP_CMD_H_


#define CMD_CRC_FAILED							0x00e0		//CRC校验出错

#define	CMD_NOTIFY_DATA							0x0101		//
#define	CMD_PULL_DATA								0x0110		//

#define	CMD_PULL_DATA_REQ						0x01a2		//
#define	CMD_PULL_DATA_REQ_OVER			0x01a3		//

#define CMD_ERR_DATA_TOO_BIG				0x01e0		//
#define CMD_ERR_HASH								0x0112		//
#define CMD_ACK_HASH_OK							0x0111		//

#define CMD_DEL_ADDRESS							0x0203		//删除地址
#define CMD_DEL_ADDRESS_OK					0x02c1
#define CMD_DEL_ADDRESS_FAILED			0x02c2

#define CMD_ADD_ADDRESS							0x0204		//增加地址
#define CMD_ADDR_ADD_OK							0x02b1		//
#define CMD_ADDR_ADD_FAILED					0x02b2		//

#define CMD_GET_PRIKEY							0x0206		//获取安全信息
#define CMD_RETURN_MESSAGE					0x02a4
#define CMD_GET_PRIKEY_FAILED				0x02e0

#define CMD_QUERY_ADDRESS						0x0201		//查询设备地址情况
#define CMD_RETURN_ADDRESS					0x02a0		//返回一条地址
#define CMD_RETURN_ADDRESS_OK				0x02a1		//返回地址结束

#define CMD_SIGN_DATA								0x020a		//数据签名
#define CMD_SIGN_OK									0x02a8
#define CMD_SIGN_FAILED							0x02e3

#define CMD_SET_PASSPORT						0x020b		//设置密码
#define CMD_SET_OK									0x02c3
#define CMD_SET_FAILED							0x02c4

#define CMD_VERIFY_PASSPORT					0x020c		//验证密码
#define CMD_VERIFY_OK								0x02c5
#define CMD_VERIFY_FAILED						0x02c6

//#define CMD_WALLET_ISNEW						0x020d		//判断钱包是否是新钱包
//#define CMD_WALLET_NEW							0x02b5
//#define CMD_WALLET_OLD							0x02b6

#define CMD_SET_INFO								0x021a		//设置配置信息
#define CMD_SET_INFO_OK							0x02d3
#define CMD_SET_INFO_FAILED					0x02d4

#define CMD_GET_INFO								0x021b		//获取配置信息
#define CMD_GET_INFO_OK							0x02d1
#define CMD_GET_INFO_FAILED					0x02d2

#define CMD_SHOW_PASSPORT						0x021c		//上位机发起密码验证，告诉下位机显示密码界面
#define CMD_SHOW_OK									0x02d5
#define CMD_SHOW_FAILED							0x02e2

//#define CMD_CLEAR_ALL_CACHE					0x0200		//清除所有缓存数据
//#define CMD_TRANSMIT_BLOCK_DATA			0x0201		//传输一块数据
//#define CMD_VERIFY_BLOCK_DATA				0x0202		//验证一块数据
//#define CMD_GET_HARDWARE_INFO				0x0300		//查询硬件信息
//#define CMD_GET_ADDR_LIST 					0x0301		//查询地址列表
//#define CMD_GET_PUBLIC_KEY					0x0302		//获得公钥
//#define CMD_SHOW_PRIVATE_KEY				0x0303		//显示私钥
//
//#define CMD_FINGER_PRINT_INIT				0x0400		//指纹初始化
//#define CMD_FINGER_PRINT_RECORD			0x0401		//录制指纹
//
//#define CMD_SAVE_PRIVATE_PAIR 			0x0500 		//将缓存区的数据保存为私钥对
//#define CMD_CHANGE_NAME_LOGO 				0x0501		// 修改地址的昵称和logo
//
////签名管理
//#define CMD_CHECK_CACHE_DATA 				0x0600 		//签名缓存区的数据
//#define CMD_VERIFY_CACHE_DATA 			0x0601 		//验证缓存区的数据
//
////动态密码管理
//#define CMD_DYNAMIC_PASSWORD_MANAGE 0x0700		//动态密码管理


#endif /* CPP_CMD_H_ */
