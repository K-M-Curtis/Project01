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

//服务端配置信息
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

	// 服务器开始工作
	void startWork();
	// 秘钥协商
	int secKeyAgree(RequestMsg *reqMsg, char **outData, int &outLen);
	// 秘钥校验
	int secKeyCheck(RequestMsg *reqMsg, char **outData, int &outLen);
	// 秘钥注销
	int secKeyRevoke(RequestMsg *reqMsg, char **outData, int &outLen);
	// 秘钥查看
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
