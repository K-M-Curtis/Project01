#include"ClientOperation.h"
#include<openssl/hmac.h>
#include<openssl/sha.h>

ClientOperation::ClientOperation(ClientInfo *info)
{
	if (NULL == info)
	{
		cout << "info 参数非法" << endl;
		return;
	}

	mInfo = new ClientInfo;
	memset(mInfo, 0, sizeof(ClientInfo));
	memcpy(mInfo, info, sizeof(ClientInfo));

	mSocket = new TcpSocket();
	mShm = new SecKeyShm(mInfo->shmKey, mInfo->maxNode);

}

ClientOperation::~ClientOperation()
{
	if (NULL != mInfo)
	{
		delete mInfo;
		mInfo = NULL;
	}
	if (NULL != mSocket)
	{
		delete mSocket;
		mSocket = NULL;
	}
	if (NULL != mShm)
	{
		delete mShm;
		mShm = NULL;
	}

}

// 秘钥协商
int ClientOperation::secKeyAgree(void)
{
	int ret = -1;
	int i = 0;
	FactoryCodec *factorycodec = NULL;
	Codec *codec = NULL;
	RequestMsg requestMsg;

	//1. 组织密钥协商请求报文
	memset(&requestMsg, 0, sizeof(RequestMsg));
	requestMsg.cmdType = RequestCodec::NewOrUpdate;
	strcpy(requestMsg.clientId, mInfo->clientID);
	strcpy(requestMsg.serverId, mInfo->serverID);

	//生成随机序列
	getRandString(sizeof(requestMsg.r1), requestMsg.r1);

	//生成认证码 HMAC
	//1.1 创建CTX
	HMAC_CTX *hmacCtx = NULL;
	hmacCtx = HMAC_CTX_new();
	if (NULL == hmacCtx)
	{
		cout << "HMAC_CTX_new failed..." << endl;
		return 1;
	}

	//1.2 初始化
	char key[32];
	sprintf(key, "@%s+%s@", mInfo->clientID, mInfo->serverID);
	ret = HMAC_Init_ex(hmacCtx, key, strlen(key), EVP_sha256(), NULL);
	if (1 != ret)
	{
		cout << "HMAC_Init_ex failed..." << endl;
		return 1;
	}

	//1.3 添加数据
	ret = HMAC_Update(hmacCtx, (const unsigned char*)requestMsg.r1, strlen(requestMsg.r1));
	if (1 != ret)
	{
		cout << "HMAC_Update failed..." << endl;
		return 1;
	}

	//1.4 计算结果
	unsigned char hmacMd[32];
	unsigned int hmacLen = 0;
	memset(hmacMd, 0, 32);
	ret = HMAC_Final(hmacCtx, hmacMd, &hmacLen);
	if (1 != ret)
	{
		cout << "HMAC_Final failed..." << endl;
		return 1;
	}

	//1.5 转化为字符串
	for (i = 0; i < hmacLen; i++)
	{
		sprintf((char*)&requestMsg.r1[i * 2], "%02X", hmacMd[i]);
	}
	cout << "HMAC: " << requestMsg.r1 << " hmacLen: " << hmacLen << endl;

	//1.6 释放CTX
	HMAC_CTX_free(hmacCtx);

	//2. 创建请求报文编码工厂类对象
	factorycodec = new RequestFactory;

	//3. 创建请求报文编码对象
	codec = factorycodec->createCodec(&requestMsg);

	//4. 编码请求报文
	char *sendBuf = NULL;
	int sendLen = 0;
	codec->msgEncode(&sendBuf, sendLen);

	//5. 连接到服务端
	mSocket->connectToHost(mInfo->serverIP, mInfo->serverPort);

	//6. 发送密钥协商请求报文
	mSocket->sendMsg(sendBuf, sendLen);

	//7. 接收密钥协商响应报文
	char *recvBuf = NULL;
	int recvLen = -1;
	mSocket->recvMsg(&recvBuf, recvLen);

	delete factorycodec;
	factorycodec = NULL;

	delete codec;
	codec = NULL;

	//释放sendBuf
	mSocket->freeMemory(&sendBuf);

	//8. 创建响应报文解码工厂类对象
	factorycodec = new RespondFactory;

	//9. 创建响应报文解码对象
	codec = factorycodec->createCodec();

	//10. 解码响应报文
	RespondMsg *respondMsg = static_cast<RespondMsg*>(codec->msgDecode(recvBuf, recvLen));

	//11. 根据响应结果判断是否响应成功
	if (0 == respondMsg->rv)
	{
		cout << "密钥协商成功..." << endl;
	}
	else
	{
		cout << "密钥协商响应失败..." << endl;
	}

	//12. 生成密钥 (SHA512模拟生成密钥)
	NodeShmInfo nodeShmInfo;
	
	//12.1 初始化
	SHA512_CTX shaCtx;
	ret = SHA512_Init(&shaCtx);
	if (1 != ret)
	{
		cout << "SHA512_Init failed..." << endl;
		return 1;
	}

	//12.2 添加数据
	ret = SHA512_Update(&shaCtx, requestMsg.r1, strlen(requestMsg.r1));
	if (1 != ret)
	{
		cout << "SHA512_Update failed..." << endl;
		return 1;
	}

	ret = SHA512_Update(&shaCtx, respondMsg->r2, strlen(respondMsg->r2));
	if (1 != ret)
	{
		cout << "SHA512_Update failed..." << endl;
		return 1;
	}

	//12.3 计算结果
	unsigned char md[SHA512_DIGEST_LENGTH];
	memset(md, 0, SHA512_DIGEST_LENGTH);
	ret = SHA512_Final(md, &shaCtx);
	if (1 != ret)
	{
		cout << "SHA512_Final failed..." << endl;
		return 1;
	}

	//12.4 转化为字符串
	memset(&nodeShmInfo, 0, sizeof(NodeShmInfo));
	for (i = 0; i < SHA512_DIGEST_LENGTH; i++)
	{
		sprintf(&nodeShmInfo.secKey[i * 2], "%02X", md[i]);
	}
	cout << "Key: " << nodeShmInfo.secKey << endl;

	//13. 写共享内存
	nodeShmInfo.status = 1;
	strcpy(nodeShmInfo.clientId, mInfo->clientID);
	strcpy(nodeShmInfo.serverId, mInfo->serverID);
	nodeShmInfo.secKeyId = 1;

	mShm->shmWrite(&nodeShmInfo);

	//14. 释放内存
	delete factorycodec;
	factorycodec = NULL;

	delete codec;
	codec = NULL;

	//释放recvBuf
	mSocket->freeMemory(&recvBuf);

	return 0;
}

// 秘钥校验
int ClientOperation::secKeyCheck(void)
{
	int ret = -1;
	int i = 0;

	NodeShmInfo nodeShmInfo;
	FactoryCodec *factorycodec = NULL;
	Codec *codec = NULL;
	RequestMsg requestMsg;

	//1. 组织密钥校验请求报文
	memset(&requestMsg, 0, sizeof(RequestMsg));
	requestMsg.cmdType = RequestCodec::Check;
	strcpy(requestMsg.clientId, mInfo->clientID);
	strcpy(requestMsg.serverId, mInfo->serverID);

	//读共享内存，计算哈希值存储在r1中
	memset(&nodeShmInfo, 0, sizeof(nodeShmInfo));
	mShm->shmRead(mInfo->clientID, mInfo->serverID, &nodeShmInfo);

	//1.1 初始化
	SHA256_CTX shaCtx;

	ret = SHA256_Init(&shaCtx);
	if (1 != ret)
	{
		cout << "SHA256_Init failed..." << endl;
		return 1;
	}

	//1.2 添加数据
	ret = SHA256_Update(&shaCtx, nodeShmInfo.secKey, sizeof(nodeShmInfo.secKey));
	if (1 != ret)
	{
		cout << "SHA256_Update failed..." << endl;
		return 1;
	}

	//1.3 计算结果
	unsigned char md[SHA256_DIGEST_LENGTH];
	memset(md, 0, SHA256_DIGEST_LENGTH);
	ret = SHA256_Final(md, &shaCtx);
	if (1 != ret)
	{
		cout << "SHA256_Final failed..." << endl;
		return 1;
	}

	//1.4 转化为字符串
	for (i = 0; i < SHA256_DIGEST_LENGTH; i++)
	{
		sprintf(&requestMsg.r1[i * 2], "%02X", md[i]);
	}
	cout << "Check Hash: " << requestMsg.r1 << endl;

	//生成认证码 HMAC
	//1.1 创建CTX
	HMAC_CTX *hmacCtx = NULL;
	hmacCtx = HMAC_CTX_new();
	if (NULL == hmacCtx)
	{
		cout << "HMAC_CTX_new failed..." << endl;
		return 1;
	}

	//1.2 初始化
	char key[32];
	sprintf(key, "@%s+%s@", mInfo->clientID, mInfo->serverID);
	ret = HMAC_Init_ex(hmacCtx, key, strlen(key), EVP_sha256(), NULL);
	if (1 != ret)
	{
		cout << "HMAC_Init_ex failed..." << endl;
		return 1;
	}

	//1.3 添加数据
	ret = HMAC_Update(hmacCtx, (const unsigned char*)requestMsg.r1, sizeof(requestMsg.r1));
	if (1 != ret)
	{
		cout << "HMAC_Update failed..." << endl;
		return 1;
	}

	//1.4 计算结果
	unsigned char hmacMd[32];
	unsigned int hmacLen = 0;
	memset(hmacMd, 0, 32);
	ret = HMAC_Final(hmacCtx, hmacMd, &hmacLen);
	if (1 != ret)
	{
		cout << "HMAC_Final failed..." << endl;
		return 1;
	}

	//1.5 转化为字符串
	for (i = 0; i < hmacLen; i++)
	{
		sprintf((char*)&requestMsg.authCode[i * 2], "%02X", hmacMd[i]);

	}
	cout << "HMAC: " << requestMsg.authCode << " hmacLen: " << hmacLen << endl;

	//1.6 释放CTX
	HMAC_CTX_free(hmacCtx);


	//2. 创建请求报文编码工厂类对象
	factorycodec = new RequestFactory;

	//3. 创建请求报文编码对象
	codec = factorycodec->createCodec(&requestMsg);

	//4. 编码请求报文
	char *sendBuf = NULL;
	int sendLen = 0;
	codec->msgEncode(&sendBuf, sendLen);

	//5. 连接到服务端
	mSocket->connectToHost(mInfo->serverIP, mInfo->serverPort);
	cout << "连接到服务器OK.." << endl;

	//6. 发送密钥校验请求报文
	mSocket->sendMsg(sendBuf, sendLen);

	//7. 接收密钥校验响应报文
	char *recvBuf = NULL;
	int recvLen = -1;
	mSocket->recvMsg(&recvBuf, recvLen);

	delete factorycodec;
	factorycodec = NULL;

	delete codec;
	codec = NULL;

	//释放sendBuf
	mSocket->freeMemory(&sendBuf);

	//8. 创建响应报文解码工厂类对象
	factorycodec = new RespondFactory;

	//9. 创建响应报文解码对象
	codec = factorycodec->createCodec();

	//10. 解码响应报文
	RespondMsg *respondMsg = static_cast<RespondMsg*>(codec->msgDecode(recvBuf, recvLen));

	//11. 根据响应结果判断是否响应成功
	if (0 == respondMsg->rv)
	{
		cout << "密钥校验成功..." << endl;
	}
	else
	{
		cout << "密钥校验响应失败..." << endl;
	}

	//12. 释放内存
	delete factorycodec;
	factorycodec = NULL;

	delete codec;
	codec = NULL;

	//释放recvBuf
	mSocket->freeMemory(&recvBuf);

	return 0;
}

// 秘钥注销
int ClientOperation::secKeyRevoke(void)
{
	int ret = -1;
	int i = 0;
	FactoryCodec *factorycodec = NULL;
	Codec *codec = NULL;
	RequestMsg requestMsg;

	//1. 组织密钥注销请求报文
	memset(&requestMsg, 0, sizeof(RequestMsg));
	requestMsg.cmdType = RequestCodec::Revoke;
	strcpy(requestMsg.clientId, mInfo->clientID);
	strcpy(requestMsg.serverId, mInfo->serverID);

	//生成随机序列
	getRandString(sizeof(requestMsg.r1), requestMsg.r1);

	//生成认证码 HMAC
	//1.1 创建CTX
	HMAC_CTX *hmacCtx = NULL;
	hmacCtx = HMAC_CTX_new();
	if (NULL == hmacCtx)
	{
		cout << "HMAC_CTX_new failed..." << endl;
		return 1;
	}

	//1.2 初始化
	char key[32];
	sprintf(key, "@%s+%s@", mInfo->clientID, mInfo->serverID);
	ret = HMAC_Init_ex(hmacCtx, key, strlen(key), EVP_sha256(), NULL);
	if (1 != ret)
	{
		cout << "HMAC_Init_ex failed..." << endl;
		return 1;
	}

	//1.3 添加数据
	ret = HMAC_Update(hmacCtx, (const unsigned char*)requestMsg.r1, sizeof(requestMsg.r1));
	if (1 != ret)
	{
		cout << "HMAC_Update failed..." << endl;
		return 1;
	}

	//1.4 计算结果
	unsigned char hmacMd[32];
	unsigned int hmacLen = 0;
	memset(hmacMd, 0, 32);
	ret = HMAC_Final(hmacCtx, hmacMd, &hmacLen);
	if (1 != ret)
	{
		cout << "HMAC_Final failed..." << endl;
		return 1;
	}

	//1.5 转化为字符串
	for (i = 0; i < hmacLen; i++)
	{
		sprintf((char*)&requestMsg.authCode[i * 2], "%02X", hmacMd[i]);

	}
	cout << "HMAC: " << requestMsg.authCode << " hmacLen: " << hmacLen << endl;

	//1.6 释放CTX
	HMAC_CTX_free(hmacCtx);


	//2. 创建请求报文编码工厂类对象
	factorycodec = new RequestFactory;

	//3. 创建请求报文编码对象
	codec = factorycodec->createCodec(&requestMsg);

	//4. 编码请求报文
	char *sendBuf = NULL;
	int sendLen = 0;
	codec->msgEncode(&sendBuf, sendLen);

	//5. 连接到服务端
	mSocket->connectToHost(mInfo->serverIP, mInfo->serverPort);
	cout << "连接到服务端OK.." << endl;

	//6. 发送密钥注销请求报文
	mSocket->sendMsg(sendBuf, sendLen);

	//7. 接收密钥注销响应报文
	char *recvBuf = NULL;
	int recvLen = -1;
	mSocket->recvMsg(&recvBuf, recvLen);

	delete factorycodec;
	factorycodec = NULL;

	delete codec;
	codec = NULL;

	//释放sendBuf
	mSocket->freeMemory(&sendBuf);

	//8. 创建响应报文解码工厂类对象
	factorycodec = new RespondFactory;

	//9. 创建响应报文解码对象
	codec = factorycodec->createCodec();

	//10. 解码响应报文
	RespondMsg *respondMsg = static_cast<RespondMsg*>(codec->msgDecode(recvBuf, recvLen));

	//11. 根据响应结果判断是否响应成功
	if (0 == respondMsg->rv)
	{
		cout << "密钥注销成功..." << endl;
	}
	else
	{
		cout << "密钥注销响应失败..." << endl;
		return -1;
	}

	//13. 写共享内存
	NodeShmInfo nodeShmInfo;

	//读共享内存
	memset(&nodeShmInfo, 0, sizeof(NodeShmInfo));
	mShm->shmRead(requestMsg.clientId, requestMsg.serverId, &nodeShmInfo);

	//将密钥的状态信息改为无效
	nodeShmInfo.status = 0;

	mShm->shmWrite(&nodeShmInfo);
	//14. 释放内存
	delete factorycodec;
	factorycodec = NULL;

	delete codec;
	codec = NULL;
	//释放recvBuf
	mSocket->freeMemory(&recvBuf);

	return 0;
}

// 秘钥查看
int ClientOperation::secKeyView(void)
{
	int ret = -1;
	NodeShmInfo nodeShmInfo;

	memset(&nodeShmInfo, 0, sizeof(nodeShmInfo));
	ret = mShm->shmRead(mInfo->clientID, mInfo->serverID, &nodeShmInfo);
	if (-1 == ret)
	{
		cout << "shmRead failed..." << endl;
		return -1;
	}

	cout << "status: " << nodeShmInfo.status << endl;
	cout << "clientId: " << nodeShmInfo.clientId << endl;
	cout << "serverId: " << nodeShmInfo.serverId << endl;
	cout << "secKeyId: " << nodeShmInfo.secKeyId << endl;
	cout << "secKey: " << nodeShmInfo.secKey << endl;

	return 0;
}


//获取随机字符序列
void ClientOperation::getRandString(int len, char *randBuf)
{
	//随机序列四种字符
	//1. 大写字母 A-Z
	//2. 小写字母 a-z
	//3. 数字 0-9
	//4. 特殊字符 ~!@#$%^&*()-+=
	int tag;
	int i = 0;

	//1. 参数检查
	if (len <= 0 || NULL == randBuf)
	{
		cout << "getRandString 参数非法" << endl;
		return;
	}

	//2. 生成随机序列
	for (i = 0; i < len; i++)
	{
		//随机字符种类
		tag = random() % 4;
		switch (tag)
		{
		case 0:
			randBuf[i] = random() % 26 + 'A';
			break;
		case 1:
			randBuf[i] = random() % 26 + 'a';
			break;
		case 2:
			randBuf[i] = random() % 10 + '0';
			break;
		case 3:
			randBuf[i] = "~!@#$%^&*()-+="[random() % 14];
			break;
		}
	}

}

