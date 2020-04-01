#include"ServerOperation.h"

//线程传递参数类型
struct pthread_info_t{
	ServerOperation *pServer;
	TcpSocket *pSocket;
};

int ServerOperation::flag = 0;
void *myroute(void* arg)
{
	int ret = -1;
	int i = 0;
	char *recvData = NULL;
	int recvLen = 0;

	FactoryCodec *factorycodec = NULL;
	Codec *codec = NULL;

	RequestMsg *pRequestMsg = NULL;

	char authCode[65];

	char *sendData;
	int sendLen = -1;

	pthread_info_t *pInfo = static_cast<pthread_info_t*>(arg);

	do
	{
		//1. 接收请求报文
		ret = pInfo->pSocket->recvMsg(&recvData, recvLen);
		if (ret < 0)
		{
			cout << "myroute recv failed..." << endl;
			break;
		}

		//2. 创建请求报文工厂类对象
		factorycodec = new RequestFactory;

		//3. 创建请求报文解码对象 解码
		codec = factorycodec->createCodec();

		pRequestMsg = static_cast<RequestMsg*>(codec->msgDecode(recvData, recvLen));

		//4. 验证消息认证码 HMAC
		//12.1 初始化
		//1.1 创建CTX
		HMAC_CTX *hmacCtx = NULL;
		hmacCtx = HMAC_CTX_new();
		if (NULL == hmacCtx)
		{
			cout << "HMAC_CTX_new failed..." << endl;
			break;
		}

		//1.2 初始化
		char key[32];
		sprintf(key, "@%s+%s@", pRequestMsg->clientId, pRequestMsg->serverId);
		ret = HMAC_Init_ex(hmacCtx, key, strlen(key), EVP_sha256(), NULL);
		if (1 != ret)
		{
			cout << "HMAC_Init_ex failed..." << endl;
			break;
		}

		//1.3 添加数据
		ret = HMAC_Update(hmacCtx, (const unsigned char*)pRequestMsg->r1, strlen(pRequestMsg->r1));
		if (1 != ret)
		{
			cout << "HMAC_Update failed..." << endl;
			break;
		}

		//1.4 计算结果
		unsigned char hmacMd[32];
		unsigned int hmacLen = 0;
		memset(hmacMd, 0, 32);
		ret = HMAC_Final(hmacCtx, hmacMd, &hmacLen);
		if (1 != ret)
		{
			cout << "HMAC_Final failed..." << endl;
			break;
		}

		//1.5 转化为字符串
		memset(authCode, 0, sizeof(authCode));
		for (i = 0; i < hmacLen; i++)
		{
			sprintf((char*)&authCode[i * 2], "%02X", hmacMd[i]);
		}
		cout << "HMAC: " << authCode << " hmacLen: " << hmacLen << endl;

		//1.6 释放CTX
		HMAC_CTX_free(hmacCtx);

		//1.7 比较消息认证码
		if (memcmp(authCode, pRequestMsg->authCode, sizeof(authCode)) == 0)
		{
			cout << "消息认证码验证一致" << endl;
		}
		else
		{
			cout << "消息认证码不一致" << endl;
			break;
		}

		//5. 根据请求报文类型做对应响应
		switch (pRequestMsg->cmdType)
		{
		case RequestCodec::NewOrUpdate:
			ret = pInfo->pServer->secKeyAgree(pRequestMsg, &sendData, sendLen);
			break;
		case RequestCodec::Check:
			ret = pInfo->pServer->secKeyCheck(pRequestMsg, &sendData, sendLen);
			break;
		case RequestCodec::Revoke:
			ret = pInfo->pServer->secKeyRevoke(pRequestMsg, &sendData, sendLen);
			break;
	
		}

		//6. 判断做业务是否成功
		if (0 != ret)
		{
			cout << "做对应的业务失败" << endl;
			break;
		}
		else
		{
			cout << "做对应的业务成功" << endl;
		}

		//7. 发送响应报文
		pInfo->pSocket->sendMsg(sendData, sendLen);

	} while (0);
	
	//8. 内存释放s
	if (NULL != pInfo)
	{
		delete pInfo;
		pInfo = NULL;
	}
	if (NULL != factorycodec)
	{
		delete factorycodec;
		factorycodec = NULL;
	}
	if (NULL != codec)
	{
		delete codec;
		codec = NULL;
	}
	if (NULL != recvData)
	{
		delete recvData;
		recvData = NULL;
	}
	if (NULL != sendData)
	{
		delete sendData;
		sendData = NULL;
	}

	pthread_exit(NULL);
}

ServerOperation::ServerOperation(ServerInfo *Info)
{
	if (NULL == Info)
	{
		cout << "ServerOperation invalid parameter..." << endl;
		return;
	}

	mInfo = new ServerInfo;
	memset(mInfo, 0, sizeof(ServerInfo));
	memcpy(mInfo, Info, sizeof(ServerInfo));

	mShm = new SecKeyShm(mInfo->shmKey, mInfo->maxNode);

	mServer = new TcpServer;
}

ServerOperation::~ServerOperation()
{
	if (NULL != mInfo)
	{
		delete mInfo;
		mInfo = NULL;
	}
	if (NULL != mShm)
	{
		delete mShm;
		mShm = NULL;
	}
	if (NULL != mServer)
	{
		delete mServer;
		mServer = NULL;
	}
}

// 服务器开始工作
void ServerOperation::startWork()
{
	int ret = -1;
	pthread_t tid = -1;
	pthread_info_t *pInfo = NULL;

	//1. 设置监听
	ret = mServer->setListen(mInfo->sPort);
	if (ret < 0)
	{
		cout << "setListen failed..." << endl;
		return;
	}
	cout << "mServer setListen successful..." << endl;

	//2. 循环接受客户端连接，创建线程
	while (1)
	{
        if (1 == ServerOperation::flag)
        {
            cout << "密钥协商服务端友好退出" << endl;
            break; 
        }

		mClient = mServer->acceptConn(3);
		if (NULL == mClient)
		{
			cout << "acceptConn failed..." << endl;
			continue;
		}

		pInfo = new pthread_info_t;
		if (NULL == pInfo)
		{
			cout << "分配空间失败" << endl;
			break;
		}
		memset(pInfo, 0, sizeof(pthread_info_t));

		pInfo->pServer = this;
		pInfo->pSocket = mClient;

		pthread_create(&tid, NULL, myroute, pInfo);

		pthread_detach(tid);
	}

	//关闭套接字
	mServer->closefd();
}

// 秘钥协商
int ServerOperation::secKeyAgree(RequestMsg *reqMsg, char **outData, int &outLen)
{
	int i = 0;
	int ret = -1;
	RespondMsg respondMsg;
	FactoryCodec *factorycodec = NULL;
	Codec *codec = NULL;

	NodeShmInfo nodeShmInfo;

	//0. 参数检查
	if (NULL == reqMsg || NULL == outData)
	{
		cout << "secKeyAgree invalid parameter.." << endl;
		return -1;
	}

	//1. 组织密钥协商响应报文
	memset(&respondMsg, 0, sizeof(respondMsg));
	respondMsg.rv = 0;
	strcpy(respondMsg.clientId, reqMsg->clientId);
	strcpy(respondMsg.serverId, reqMsg->serverId);

	getRandString(sizeof(respondMsg.r2), respondMsg.r2);
	respondMsg.seckeyId = 1;

	//2. 创建密钥协商响应报文工厂类对象
	factorycodec = new RespondFactory;

	//3. 创建响应报文编码对象
	codec = factorycodec->createCodec(&respondMsg);

	//4. 编码响应报文
	codec->msgEncode(outData, outLen);

	//5. 生成密钥 SHA512
	memset(&nodeShmInfo, 0, sizeof(NodeShmInfo));
	SHA512_CTX shaCtx;

	ret = SHA512_Init(&shaCtx);
	if (1 != ret)
	{
		cout << "SHA512_Init failed..." << endl;
		return 1;
	}

	//1.2 添加数据
	ret = SHA512_Update(&shaCtx, reqMsg->r1, strlen(reqMsg->r1));
	if (1 != ret)
	{
		cout << "SHA512_UpDate failed..." << endl;
		return 1;
	}

	ret = SHA512_Update(&shaCtx, respondMsg.r2, strlen(respondMsg.r2));
	if (1 != ret)
	{
		cout << "SHA512_Update failed..." << endl;
		return 1;
	}

	//1.3 计算结果
	unsigned char md[SHA512_DIGEST_LENGTH];
	memset(md, 0, SHA512_DIGEST_LENGTH);
	ret = SHA512_Final(md, &shaCtx);
	if (1 != ret)
	{
		cout << "SHA512_Final failed..." << endl;
		return 1;
	}

	//1.4 转化为字符串
	memset(&nodeShmInfo, 0, sizeof(NodeShmInfo));
	for (i = 0; i < SHA512_DIGEST_LENGTH; i++)
	{
		sprintf(&nodeShmInfo.secKey[i * 2], "%02X", md[i]);
	}
	cout << "Key: " << nodeShmInfo.secKey << endl;


	//6. 写共享内存
	nodeShmInfo.status = 0;
	strcpy(nodeShmInfo.clientId, reqMsg->clientId);
	strcpy(nodeShmInfo.serverId, reqMsg->serverId);
	nodeShmInfo.secKeyId = 1;

	mShm->shmWrite(&nodeShmInfo);
	//7. 写数据库

	//8. 释放内存
	if (NULL != factorycodec)
	{
		delete factorycodec;
		factorycodec = NULL;
	}
	if (NULL != codec)
	{
		delete codec;
		codec = NULL;
	}

	return 0;
}

// 秘钥校验
int ServerOperation::secKeyCheck(RequestMsg *reqMsg, char **outData, int &outLen)
{
	int i = 0;
	int ret = -1;
	RespondMsg respondMsg;
	FactoryCodec *factorycodec = NULL;
	Codec *codec = NULL;

	NodeShmInfo nodeShmInfo;

	//0. 参数检查
	if (NULL == reqMsg || NULL == outData)
	{
		cout << "secKeyCheck invalid parameter.." << endl;
		return -1;
	}

	//1. 组织密钥协商响应报文
	memset(&respondMsg, 0, sizeof(respondMsg));
	//respondMsg.rv = 0;
	strcpy(respondMsg.clientId, reqMsg->clientId);
	strcpy(respondMsg.serverId, reqMsg->serverId);

	getRandString(sizeof(respondMsg.r2), respondMsg.r2);
	respondMsg.seckeyId = 1;

	//2. 从共享内存中读取网点密钥信息
	memset(&nodeShmInfo, 0, sizeof(NodeShmInfo));
	mShm->shmRead(reqMsg->clientId, reqMsg->serverId, &nodeShmInfo);

	//5. 生成密钥 SHA256
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
	char authCode[SHA256_DIGEST_LENGTH * 2 + 1];
	memset(md, 0, SHA256_DIGEST_LENGTH);
	ret = SHA256_Final(md, &shaCtx);
	if (1 != ret)
	{
		cout << "SHA256_Final failed..." << endl;
		return 1;
	}

	//1.4 转化为字符串
	memset(authCode, 0, SHA256_DIGEST_LENGTH * 2 + 1);
	for (i = 0; i < SHA256_DIGEST_LENGTH; i++)
	{
		sprintf(&authCode[i * 2], "%02X", md[i]);
	}
	cout << "Check Key: " << authCode << endl;

	if (memcmp(authCode, reqMsg->r1, SHA256_DIGEST_LENGTH) == 0)
	{
		cout << "服务端密钥校验成功" << endl;
		respondMsg.rv = 0;
	}
	else
	{
		cout << "服务端密钥校验失败" << endl;
		respondMsg.rv = 1;
	}

	//4. 创建密钥协商响应报文工厂类对象
	factorycodec = new RespondFactory;

	//5. 创建响应报文编码对象
	codec = factorycodec->createCodec(&respondMsg);

	//6. 编码响应报文
	codec->msgEncode(outData, outLen);

	//7. 释放内存
	if (NULL != factorycodec)
	{
		delete factorycodec;
		factorycodec = NULL;
	}
	if (NULL != codec)
	{
		delete codec;
		codec = NULL;
	}

	return 0;
}

// 秘钥注销
int ServerOperation::secKeyRevoke(RequestMsg *reqMsg, char **outData, int &outLen)
{
	int ret = -1;
	RespondMsg respondMsg;
	FactoryCodec *factorycodec = NULL;
	Codec *codec = NULL;

	NodeShmInfo nodeShmInfo;

	//0. 参数检查
	if (NULL == reqMsg || NULL == outData)
	{
		cout << "secKeyRevoke invalid parameter.." << endl;
		return -1;
	}

	//1. 组织密钥注销响应报文
	memset(&respondMsg, 0, sizeof(respondMsg));
	respondMsg.rv = 0;
	strcpy(respondMsg.clientId, reqMsg->clientId);
	strcpy(respondMsg.serverId, reqMsg->serverId);

	getRandString(sizeof(respondMsg.r2), respondMsg.r2);
	respondMsg.seckeyId = 1;

	//2. 创建密钥协商响应报文工厂类对象
	factorycodec = new RespondFactory;

	//3. 创建响应报文编码对象
	codec = factorycodec->createCodec(&respondMsg);

	//4. 编码响应报文
	codec->msgEncode(outData, outLen);

	//6. 写共享内存
	memset(&nodeShmInfo, 0, sizeof(NodeShmInfo));
	mShm->shmRead(reqMsg->clientId, reqMsg->serverId, &nodeShmInfo);
	//密钥的状态1--> 0
	nodeShmInfo.status = 0;
	
	mShm->shmWrite(&nodeShmInfo);
	cout << "密钥协商服务端密钥注销成功" << endl;

	//7. 释放内存
	if (NULL != factorycodec)
	{
		delete factorycodec;
		factorycodec = NULL;
	}
	if (NULL != codec)
	{
		delete codec;
		codec = NULL;
	}

	return 0;
}

// 秘钥查看
int ServerOperation::secKeyView(void)
{

	return 0;
}


void ServerOperation::getRandString(int len, char *randBuf)
{
	int i = 0;
	int tag;

	//参数检查
	if (NULL == randBuf || len <= 0)
	{
		cout << "getRandString invalid parameter..." << endl;
		return ;
	}

	memset(randBuf, 0, len);
	for (i = 0; i < len; i++)
	{
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
			randBuf[i] = "~!@#$%^&*()_+="[random() % 14];
			break;

		}
	}
}


