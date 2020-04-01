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
	// 秘钥协商
	int secKeyAgree(void);
	// 秘钥校验
	int secKeyCheck(void);
	// 秘钥注销
	int secKeyRevoke(void);
	// 秘钥查看
	int secKeyView(void);

private:
	//获取随机字符序列
	void getRandString(int len, char *randBuf);

private:
	//客户端配置信息
	ClientInfo *mInfo;
	SecKeyShm *mShm;
	TcpSocket *mSocket;
};