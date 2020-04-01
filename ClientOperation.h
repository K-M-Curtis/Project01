#pragma once
#include"RequestFactory.h"
#include"RespondFactory.h"
#include"TcpSocket.h"
#include"SecKeyShm.h"

class ClientInfo
{
public:
	char clientID[12];
	char serverID[12];
	char authCode[65];
	char serverIP[32];
	unsigned int serverPort;
	int maxNode;
	int shmKey;
};

class ClientOperation
{
public:
	ClientOperation(ClientInfo *info);
	~ClientOperation();
	// ��ԿЭ��
	int secKeyAgree(void);
	// ��ԿУ��
	int secKeyCheck(void);
	// ��Կע��
	int secKeyRevoke(void);
	// ��Կ�鿴
	int secKeyView(void);

private:
	//��ȡ����ַ�����
	void getRandString(int len, char *randBuf);

private:
	//�ͻ���������Ϣ
	ClientInfo *mInfo;
	SecKeyShm *mShm;
	TcpSocket *mSocket;
};