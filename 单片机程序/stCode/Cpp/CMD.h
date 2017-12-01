/*
 * CMD.h
 *
 *  Created on: 2017��7��30��
 *      Author: Administrator
 */

#ifndef CPP_CMD_H_
#define CPP_CMD_H_


#define CMD_CRC_FAILED							0x00e0		//CRCУ�����

#define	CMD_NOTIFY_DATA							0x0101		//
#define	CMD_PULL_DATA								0x0110		//

#define	CMD_PULL_DATA_REQ						0x01a2		//
#define	CMD_PULL_DATA_REQ_OVER			0x01a3		//

#define CMD_ERR_DATA_TOO_BIG				0x01e0		//
#define CMD_ERR_HASH								0x0112		//

#define CMD_ACK_HASH_OK							0x0111		//

#define CMD_DEL_ADDRESS							0x0203		
#define CMD_DEL_ADDRESS_OK					0x02c1
#define CMD_DEL_ADDRESS_FAILED			0x02c2

#define CMD_ADD_ADDRESS							0x0204		//
#define CMD_ADDR_ADD_OK							0x02b1		//
#define CMD_ADDR_ADD_FAILED					0x02b2		//

#define CMD_GET_PRIKEY							0x0206
#define CMD_RETURN_MESSAGE					0x02a4



#define CMD_QUERY_ADDRESS						0x0201		//��ѯ�豸��ַ���
#define CMD_RETURN_ADDRESS					0x02a0		//����һ����ַ
#define CMD_RETURN_ADDRESS_OK				0x02a1		//���ص�ַ����

//add by hkh
#define CMD_SIGN_DATA								0x020a
#define CMD_SIGN_OK									0x02a8
#define CMD_SIGN_FAILED							0x02e0



//#define CMD_CLEAR_ALL_CACHE					0x0200		//������л�������
//#define CMD_TRANSMIT_BLOCK_DATA			0x0201		//����һ������
//#define CMD_VERIFY_BLOCK_DATA				0x0202		//��֤һ������
//#define CMD_GET_HARDWARE_INFO				0x0300		//��ѯӲ����Ϣ
//#define CMD_GET_ADDR_LIST 					0x0301		//��ѯ��ַ�б�
//#define CMD_GET_PUBLIC_KEY					0x0302		//��ù�Կ
//#define CMD_SHOW_PRIVATE_KEY				0x0303		//��ʾ˽Կ
//
//#define CMD_FINGER_PRINT_INIT				0x0400		//ָ�Ƴ�ʼ��
//#define CMD_FINGER_PRINT_RECORD			0x0401		//¼��ָ��
//
//#define CMD_SAVE_PRIVATE_PAIR 			0x0500 		//�������������ݱ���Ϊ˽Կ��
//#define CMD_CHANGE_NAME_LOGO 				0x0501		// �޸ĵ�ַ���ǳƺ�logo
//
////ǩ������
//#define CMD_CHECK_CACHE_DATA 				0x0600 		//ǩ��������������
//#define CMD_VERIFY_CACHE_DATA 			0x0601 		//��֤������������
//
////��̬�������
//#define CMD_DYNAMIC_PASSWORD_MANAGE 0x0700		//��̬�������


#endif /* CPP_CMD_H_ */
