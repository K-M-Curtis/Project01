#pragma once
#include"TcpSocket.h"
#include"SecKeyShm.h"
#include"TcpServer.h"
#include"RequestFactory.h"
#include"RespondFactory.h"

#include<map>
#include<pthread.h>
#include<openssl/hmac.h>
#include<openssl/sha.h>

//�����������Ϣ
class ServerInfo
{
public:
	char serverId[12];
	unsigned int sPort;
	int maxNode;
	int shmKey;
	char dbUser[24];
	char dbPasswd[24];
	char dbSid[24];
};


class ServerOperation
{
public:
	ServerOperation(ServerInfo *info);
	~ServerOperation();

	// ��������ʼ����
	void startWork();
	// ��ԿЭ��
	int secKeyAgree(RequestMsg *reqMsg, char **outData, int &outLen);
	// ��ԿУ��
	int secKeyCheck(RequestMsg *reqMsg, char **outData, int &outLen);
	// ��Կע��
	int secKeyRevoke(RequestMsg *reqMsg, char **outData, int &outLen);
	// ��Կ�鿴
	int secKeyView(void);
	

	static int flag;
private:
	void getRandString(int len, char *randBuf);

private:
	ServerInfo *mInfo;
	SecKeyShm *mShm;
	TcpServer *mServer;
	TcpSocket *mClient;
};
