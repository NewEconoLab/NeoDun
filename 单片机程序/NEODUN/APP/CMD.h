/*
 * CMD.h
 *
 *  Created on: 2017年7月30日
 *      Author: Administrator
 */

#ifndef CPP_CMD_H_
#define CPP_CMD_H_

//数据传输
#define CMD_CRC_FAILED							0x00e0		//CRC校验出错
#define	CMD_NOTIFY_DATA							0x0101		//
#define	CMD_PULL_DATA								0x0110		//
#define	CMD_PULL_DATA_REQ						0x01a2		//
#define	CMD_PULL_DATA_REQ_OVER			0x01a3		//
#define CMD_ERR_DATA_TOO_BIG				0x01e0		//
#define CMD_ERR_HASH								0x0112		//
#define CMD_ACK_HASH_OK							0x0111		//

//地址签名
#define CMD_QUERY_ADDRESS						0x0201		//查询设备地址情况
#define CMD_RETURN_ADDRESS					0x02a0		//返回一条地址
#define CMD_RETURN_ADDRESS_OK				0x02a1		//返回地址结束
#define CMD_SET_ADDRESS_NAME				0x0202		//设置地址名称
#define CMD_SET_ADDRESS_OK					0x02a2
#define CMD_SET_ADDRESS_FAILED			0x02e2
#define CMD_DEL_ADDRESS							0x0203		//删除地址
#define CMD_DEL_ADDRESS_OK					0x02a3
#define CMD_DEL_ADDRESS_FAILED			0x02e3
#define CMD_ADD_ADDRESS							0x0204		//增加地址
#define CMD_ADDR_ADD_OK							0x02a4		
#define CMD_ADDR_ADD_FAILED					0x02e4		
#define CMD_SIGN_DATA								0x0205		//数据签名
#define CMD_SIGN_OK									0x02a5
#define CMD_SIGN_FAILED							0x02e5

#define CMD_SET_INFO								0x021a		//设置配置信息
#define CMD_SET_INFO_OK							0x02d3
#define CMD_SET_INFO_FAILED					0x02d4
#define CMD_GET_INFO								0x021b		//获取配置信息
#define CMD_GET_INFO_OK							0x02d1
#define CMD_GET_INFO_FAILED					0x02d2

//插件管理
#define CMD_INSTALL_PACK						0x0301		//安装插件
#define CMD_INSTALL_REQUEST					0x0302		//请求安装	
#define CMD_UNINSTALL_PACK					0x0303		//卸载插件
#define CMD_QUERY_PACK_INFO					0x0304		//查询固件信息

//特殊操作
#define CMD_SECU_PIPE								0x0401		//请求安全通道公钥
#define CMD_SECU_PIPE_REP						0x04a0		//回复安全通道公钥

#endif /* CPP_CMD_H_ */
