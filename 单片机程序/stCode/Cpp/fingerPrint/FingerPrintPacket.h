/*
 * FingerPrintPacket.h
 *
 *  Created on: 2017��9��4��
 *      Author: Administrator
 */

#ifndef CPP_FINGERPRINT_FINGERPRINTPACKET_H_
#define CPP_FINGERPRINT_FINGERPRINTPACKET_H_

#include "../myType.h"

class FingerPrintPacket {
public:
	static const int CMD_VERIFY = 0x13;					//	1 0x13 ��֤����
	static const int CMD_SET = 0x12;					//	2 0x12 ���ÿ���
	static const int CMD_SET_MODULE_ADDRESS = 0x15;		//	3 0x15 ����ģ���ַ
	static const int CMD_SET_SYSTEM_CONFIG = 0x0e;		//	4 0x0e ����ģ��ϵͳ����(�����ʣ��֡��)
	static const int CMD_GET_CONFIG = 0x0f;				//	5 0x0f ��ȡģ��ϵͳ����
	static const int CMD_GET_FINGER_LIST = 0x1f;		//	6 0x1f ��ȡָ�������б�
	static const int CMD_GET_FINGER_COUNT = 0x1d;		//	7 0x1d ��ȡָ������
	static const int CMD_REG_FIGPRINT = 0x01;			//	8 0x01 ע��ָ��
	static const int CMD_UPLOAD_IMAGE = 0x0a;			//	9 0x0a �ϴ�ͼ��
	static const int CMD_DOWNLOAD_IMAGE = 0x0b;			//	10 0x0b ����ͼ��
	static const int CMD_MATCH = 0x04;					//	11 0x04 ƥ��ָ��
	static const int CMD_DELETE = 0x0c;					//	12 0x0c ɾ��ָ��
	static const int CMD_CLEAR = 0x0d;					//	13 0x0d ���ָ�ƿ�
	static const int CMD_INIT_CONFIG = 0x21;			//	14 0x21 �ָ���������
	static const int CMD_INIT_OK = 0x22;				//	15 0x22 ��ʼ���ɹ�
	static const int CMD_HAND_SHAKE = 0x23;				//	16 0x23 ����
	static const int CMD_ENBALE_PRINT = 0x40;			//	17 0x40 ʹ�ܴ�ӡ����
	static const int CMD_PRINT = 0x41;					//	18 0x41 ��ӡ����
	static const int CMD_GET_PARAM = 0x42;				//	19 0x42 ��ȡ����

	//------------------------------ ������� -------------------------------------------------------

	static const int ERR_OK   = 0x00 	;				//ָ��ִ�гɹ�
	static const int ERR_CONMUNICTAE   = 0x01 	;		//ͨ�Ŵ���
	static const int ERR_GET_IMAGE   = 0x03 	;		//��ȡͼ��ʧ��
	static const int ERR_MATCH   = 0x09 	;			//ƥ��ʧ��
	static const int ERR_OUT_OF_BOUNDS   = 0x0b 	;	//ָ�� ID Խ��
	static const int ERR_OVER_MAX_SIZE   = 0x0e 	;	//����֡���ȴ��������
	static const int ERR_UP_CAPTURE   = 0x0f	;		//�ϴ�/�ɼ�ͼ�����
	static const int ERR_ID_FAILED   = 0x10 	;		//ɾ��ʧ�ܣ�ָ�� ID ����
	static const int ERR_CLEAR   = 0x11 	;			//���ָ�ƿ�ʧ��
	static const int ERR_VAERIFY   = 0x13 	;			//��֤����ʧ��
	static const int ERR_REG_CONFIG   = 0x1a 	;		//����ļĴ���/����ϵͳ������Ŵ���
	static const int ERR_REGISTE   = 0x1e 	;			//ע�����
	static const int ERR_ADDRESS   = 0x20 	;			//������ַ����
	static const int ERR_MUST_VERIFY_CMD   = 0x21 	;	//������֤����
	static const int ERR_LIST_AND_SO   = 0x22	;		//��ȡָ�������б��������¼��ʱ��������������ģ��ϵͳ������Ӧ�����ݳ����ϴ�ͼ�����ͳ�������ͼ��ʱ��������ͼ�����ݳ��ȳ������Խ��յķ�Χ
	static const int ERR_ID_HOLD   = 0x24 	;			//ָ�� ID ��ռ��
	static const int ERR_SINGLE_OK   = 0x25 	;		//���βɼ��ɹ�
	static const int ERR_TIME_OUT   = 0x26 	;			//¼��/ƥ�䳬ʱ
	static const int ERR_DOWNLOAD   = 0x27 	;			//����ʧ��
	static const int ERR_CMD   = 0x28 	;				//�������
	static const int ERR_FLAG   = 0x29 	;				//����ʶ����

private:
	static const u32 ADDRESS_DEFAULT = 0xffffffff;
private:
	u8 fsmCRC(u8 *dat, u32 len);
public:
	FingerPrintPacket();
	FingerPrintPacket(u8 packFlag );
};

#endif /* CPP_FINGERPRINT_FINGERPRINTPACKET_H_ */
