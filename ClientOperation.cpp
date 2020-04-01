#include"ClientOperation.h"
#include<openssl/hmac.h>
#include<openssl/sha.h>

ClientOperation::ClientOperation(ClientInfo *info)
{
	if (NULL == info)
	{
		cout << "info �����Ƿ�" << endl;
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

// ��ԿЭ��
int ClientOperation::secKeyAgree(void)
{
	int ret = -1;
	int i = 0;
	FactoryCodec *factorycodec = NULL;
	Codec *codec = NULL;
	RequestMsg requestMsg;

	//1. ��֯��ԿЭ��������
	memset(&requestMsg, 0, sizeof(RequestMsg));
	requestMsg.cmdType = RequestCodec::NewOrUpdate;
	strcpy(requestMsg.clientId, mInfo->clientID);
	strcpy(requestMsg.serverId, mInfo->serverID);

	//�����������
	getRandString(sizeof(requestMsg.r1), requestMsg.r1);

	//������֤�� HMAC
	//1.1 ����CTX
	HMAC_CTX *hmacCtx = NULL;
	hmacCtx = HMAC_CTX_new();
	if (NULL == hmacCtx)
	{
		cout << "HMAC_CTX_new failed..." << endl;
		return 1;
	}

	//1.2 ��ʼ��
	char key[32];
	sprintf(key, "@%s+%s@", mInfo->clientID, mInfo->serverID);
	ret = HMAC_Init_ex(hmacCtx, key, strlen(key), EVP_sha256(), NULL);
	if (1 != ret)
	{
		cout << "HMAC_Init_ex failed..." << endl;
		return 1;
	}

	//1.3 �������
	ret = HMAC_Update(hmacCtx, (const unsigned char*)requestMsg.r1, strlen(requestMsg.r1));
	if (1 != ret)
	{
		cout << "HMAC_Update failed..." << endl;
		return 1;
	}

	//1.4 ������
	unsigned char hmacMd[32];
	unsigned int hmacLen = 0;
	memset(hmacMd, 0, 32);
	ret = HMAC_Final(hmacCtx, hmacMd, &hmacLen);
	if (1 != ret)
	{
		cout << "HMAC_Final failed..." << endl;
		return 1;
	}

	//1.5 ת��Ϊ�ַ���
	for (i = 0; i < hmacLen; i++)
	{
		sprintf((char*)&requestMsg.r1[i * 2], "%02X", hmacMd[i]);
	}
	cout << "HMAC: " << requestMsg.r1 << " hmacLen: " << hmacLen << endl;

	//1.6 �ͷ�CTX
	HMAC_CTX_free(hmacCtx);

	//2. ���������ı��빤�������
	factorycodec = new RequestFactory;

	//3. ���������ı������
	codec = factorycodec->createCodec(&requestMsg);

	//4. ����������
	char *sendBuf = NULL;
	int sendLen = 0;
	codec->msgEncode(&sendBuf, sendLen);

	//5. ���ӵ������
	mSocket->connectToHost(mInfo->serverIP, mInfo->serverPort);

	//6. ������ԿЭ��������
	mSocket->sendMsg(sendBuf, sendLen);

	//7. ������ԿЭ����Ӧ����
	char *recvBuf = NULL;
	int recvLen = -1;
	mSocket->recvMsg(&recvBuf, recvLen);

	delete factorycodec;
	factorycodec = NULL;

	delete codec;
	codec = NULL;

	//�ͷ�sendBuf
	mSocket->freeMemory(&sendBuf);

	//8. ������Ӧ���Ľ��빤�������
	factorycodec = new RespondFactory;

	//9. ������Ӧ���Ľ������
	codec = factorycodec->createCodec();

	//10. ������Ӧ����
	RespondMsg *respondMsg = static_cast<RespondMsg*>(codec->msgDecode(recvBuf, recvLen));

	//11. ������Ӧ����ж��Ƿ���Ӧ�ɹ�
	if (0 == respondMsg->rv)
	{
		cout << "��ԿЭ�̳ɹ�..." << endl;
	}
	else
	{
		cout << "��ԿЭ����Ӧʧ��..." << endl;
	}

	//12. ������Կ (SHA512ģ��������Կ)
	NodeShmInfo nodeShmInfo;
	
	//12.1 ��ʼ��
	SHA512_CTX shaCtx;
	ret = SHA512_Init(&shaCtx);
	if (1 != ret)
	{
		cout << "SHA512_Init failed..." << endl;
		return 1;
	}

	//12.2 �������
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

	//12.3 ������
	unsigned char md[SHA512_DIGEST_LENGTH];
	memset(md, 0, SHA512_DIGEST_LENGTH);
	ret = SHA512_Final(md, &shaCtx);
	if (1 != ret)
	{
		cout << "SHA512_Final failed..." << endl;
		return 1;
	}

	//12.4 ת��Ϊ�ַ���
	memset(&nodeShmInfo, 0, sizeof(NodeShmInfo));
	for (i = 0; i < SHA512_DIGEST_LENGTH; i++)
	{
		sprintf(&nodeShmInfo.secKey[i * 2], "%02X", md[i]);
	}
	cout << "Key: " << nodeShmInfo.secKey << endl;

	//13. д�����ڴ�
	nodeShmInfo.status = 1;
	strcpy(nodeShmInfo.clientId, mInfo->clientID);
	strcpy(nodeShmInfo.serverId, mInfo->serverID);
	nodeShmInfo.secKeyId = 1;

	mShm->shmWrite(&nodeShmInfo);

	//14. �ͷ��ڴ�
	delete factorycodec;
	factorycodec = NULL;

	delete codec;
	codec = NULL;

	//�ͷ�recvBuf
	mSocket->freeMemory(&recvBuf);

	return 0;
}

// ��ԿУ��
int ClientOperation::secKeyCheck(void)
{
	int ret = -1;
	int i = 0;

	NodeShmInfo nodeShmInfo;
	FactoryCodec *factorycodec = NULL;
	Codec *codec = NULL;
	RequestMsg requestMsg;

	//1. ��֯��ԿУ��������
	memset(&requestMsg, 0, sizeof(RequestMsg));
	requestMsg.cmdType = RequestCodec::Check;
	strcpy(requestMsg.clientId, mInfo->clientID);
	strcpy(requestMsg.serverId, mInfo->serverID);

	//�������ڴ棬�����ϣֵ�洢��r1��
	memset(&nodeShmInfo, 0, sizeof(nodeShmInfo));
	mShm->shmRead(mInfo->clientID, mInfo->serverID, &nodeShmInfo);

	//1.1 ��ʼ��
	SHA256_CTX shaCtx;

	ret = SHA256_Init(&shaCtx);
	if (1 != ret)
	{
		cout << "SHA256_Init failed..." << endl;
		return 1;
	}

	//1.2 �������
	ret = SHA256_Update(&shaCtx, nodeShmInfo.secKey, sizeof(nodeShmInfo.secKey));
	if (1 != ret)
	{
		cout << "SHA256_Update failed..." << endl;
		return 1;
	}

	//1.3 ������
	unsigned char md[SHA256_DIGEST_LENGTH];
	memset(md, 0, SHA256_DIGEST_LENGTH);
	ret = SHA256_Final(md, &shaCtx);
	if (1 != ret)
	{
		cout << "SHA256_Final failed..." << endl;
		return 1;
	}

	//1.4 ת��Ϊ�ַ���
	for (i = 0; i < SHA256_DIGEST_LENGTH; i++)
	{
		sprintf(&requestMsg.r1[i * 2], "%02X", md[i]);
	}
	cout << "Check Hash: " << requestMsg.r1 << endl;

	//������֤�� HMAC
	//1.1 ����CTX
	HMAC_CTX *hmacCtx = NULL;
	hmacCtx = HMAC_CTX_new();
	if (NULL == hmacCtx)
	{
		cout << "HMAC_CTX_new failed..." << endl;
		return 1;
	}

	//1.2 ��ʼ��
	char key[32];
	sprintf(key, "@%s+%s@", mInfo->clientID, mInfo->serverID);
	ret = HMAC_Init_ex(hmacCtx, key, strlen(key), EVP_sha256(), NULL);
	if (1 != ret)
	{
		cout << "HMAC_Init_ex failed..." << endl;
		return 1;
	}

	//1.3 �������
	ret = HMAC_Update(hmacCtx, (const unsigned char*)requestMsg.r1, sizeof(requestMsg.r1));
	if (1 != ret)
	{
		cout << "HMAC_Update failed..." << endl;
		return 1;
	}

	//1.4 ������
	unsigned char hmacMd[32];
	unsigned int hmacLen = 0;
	memset(hmacMd, 0, 32);
	ret = HMAC_Final(hmacCtx, hmacMd, &hmacLen);
	if (1 != ret)
	{
		cout << "HMAC_Final failed..." << endl;
		return 1;
	}

	//1.5 ת��Ϊ�ַ���
	for (i = 0; i < hmacLen; i++)
	{
		sprintf((char*)&requestMsg.authCode[i * 2], "%02X", hmacMd[i]);

	}
	cout << "HMAC: " << requestMsg.authCode << " hmacLen: " << hmacLen << endl;

	//1.6 �ͷ�CTX
	HMAC_CTX_free(hmacCtx);


	//2. ���������ı��빤�������
	factorycodec = new RequestFactory;

	//3. ���������ı������
	codec = factorycodec->createCodec(&requestMsg);

	//4. ����������
	char *sendBuf = NULL;
	int sendLen = 0;
	codec->msgEncode(&sendBuf, sendLen);

	//5. ���ӵ������
	mSocket->connectToHost(mInfo->serverIP, mInfo->serverPort);
	cout << "���ӵ�������OK.." << endl;

	//6. ������ԿУ��������
	mSocket->sendMsg(sendBuf, sendLen);

	//7. ������ԿУ����Ӧ����
	char *recvBuf = NULL;
	int recvLen = -1;
	mSocket->recvMsg(&recvBuf, recvLen);

	delete factorycodec;
	factorycodec = NULL;

	delete codec;
	codec = NULL;

	//�ͷ�sendBuf
	mSocket->freeMemory(&sendBuf);

	//8. ������Ӧ���Ľ��빤�������
	factorycodec = new RespondFactory;

	//9. ������Ӧ���Ľ������
	codec = factorycodec->createCodec();

	//10. ������Ӧ����
	RespondMsg *respondMsg = static_cast<RespondMsg*>(codec->msgDecode(recvBuf, recvLen));

	//11. ������Ӧ����ж��Ƿ���Ӧ�ɹ�
	if (0 == respondMsg->rv)
	{
		cout << "��ԿУ��ɹ�..." << endl;
	}
	else
	{
		cout << "��ԿУ����Ӧʧ��..." << endl;
	}

	//12. �ͷ��ڴ�
	delete factorycodec;
	factorycodec = NULL;

	delete codec;
	codec = NULL;

	//�ͷ�recvBuf
	mSocket->freeMemory(&recvBuf);

	return 0;
}

// ��Կע��
int ClientOperation::secKeyRevoke(void)
{
	int ret = -1;
	int i = 0;
	FactoryCodec *factorycodec = NULL;
	Codec *codec = NULL;
	RequestMsg requestMsg;

	//1. ��֯��Կע��������
	memset(&requestMsg, 0, sizeof(RequestMsg));
	requestMsg.cmdType = RequestCodec::Revoke;
	strcpy(requestMsg.clientId, mInfo->clientID);
	strcpy(requestMsg.serverId, mInfo->serverID);

	//�����������
	getRandString(sizeof(requestMsg.r1), requestMsg.r1);

	//������֤�� HMAC
	//1.1 ����CTX
	HMAC_CTX *hmacCtx = NULL;
	hmacCtx = HMAC_CTX_new();
	if (NULL == hmacCtx)
	{
		cout << "HMAC_CTX_new failed..." << endl;
		return 1;
	}

	//1.2 ��ʼ��
	char key[32];
	sprintf(key, "@%s+%s@", mInfo->clientID, mInfo->serverID);
	ret = HMAC_Init_ex(hmacCtx, key, strlen(key), EVP_sha256(), NULL);
	if (1 != ret)
	{
		cout << "HMAC_Init_ex failed..." << endl;
		return 1;
	}

	//1.3 �������
	ret = HMAC_Update(hmacCtx, (const unsigned char*)requestMsg.r1, sizeof(requestMsg.r1));
	if (1 != ret)
	{
		cout << "HMAC_Update failed..." << endl;
		return 1;
	}

	//1.4 ������
	unsigned char hmacMd[32];
	unsigned int hmacLen = 0;
	memset(hmacMd, 0, 32);
	ret = HMAC_Final(hmacCtx, hmacMd, &hmacLen);
	if (1 != ret)
	{
		cout << "HMAC_Final failed..." << endl;
		return 1;
	}

	//1.5 ת��Ϊ�ַ���
	for (i = 0; i < hmacLen; i++)
	{
		sprintf((char*)&requestMsg.authCode[i * 2], "%02X", hmacMd[i]);

	}
	cout << "HMAC: " << requestMsg.authCode << " hmacLen: " << hmacLen << endl;

	//1.6 �ͷ�CTX
	HMAC_CTX_free(hmacCtx);


	//2. ���������ı��빤�������
	factorycodec = new RequestFactory;

	//3. ���������ı������
	codec = factorycodec->createCodec(&requestMsg);

	//4. ����������
	char *sendBuf = NULL;
	int sendLen = 0;
	codec->msgEncode(&sendBuf, sendLen);

	//5. ���ӵ������
	mSocket->connectToHost(mInfo->serverIP, mInfo->serverPort);
	cout << "���ӵ������OK.." << endl;

	//6. ������Կע��������
	mSocket->sendMsg(sendBuf, sendLen);

	//7. ������Կע����Ӧ����
	char *recvBuf = NULL;
	int recvLen = -1;
	mSocket->recvMsg(&recvBuf, recvLen);

	delete factorycodec;
	factorycodec = NULL;

	delete codec;
	codec = NULL;

	//�ͷ�sendBuf
	mSocket->freeMemory(&sendBuf);

	//8. ������Ӧ���Ľ��빤�������
	factorycodec = new RespondFactory;

	//9. ������Ӧ���Ľ������
	codec = factorycodec->createCodec();

	//10. ������Ӧ����
	RespondMsg *respondMsg = static_cast<RespondMsg*>(codec->msgDecode(recvBuf, recvLen));

	//11. ������Ӧ����ж��Ƿ���Ӧ�ɹ�
	if (0 == respondMsg->rv)
	{
		cout << "��Կע���ɹ�..." << endl;
	}
	else
	{
		cout << "��Կע����Ӧʧ��..." << endl;
		return -1;
	}

	//13. д�����ڴ�
	NodeShmInfo nodeShmInfo;

	//�������ڴ�
	memset(&nodeShmInfo, 0, sizeof(NodeShmInfo));
	mShm->shmRead(requestMsg.clientId, requestMsg.serverId, &nodeShmInfo);

	//����Կ��״̬��Ϣ��Ϊ��Ч
	nodeShmInfo.status = 0;

	mShm->shmWrite(&nodeShmInfo);
	//14. �ͷ��ڴ�
	delete factorycodec;
	factorycodec = NULL;

	delete codec;
	codec = NULL;
	//�ͷ�recvBuf
	mSocket->freeMemory(&recvBuf);

	return 0;
}

// ��Կ�鿴
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


//��ȡ����ַ�����
void ClientOperation::getRandString(int len, char *randBuf)
{
	//������������ַ�
	//1. ��д��ĸ A-Z
	//2. Сд��ĸ a-z
	//3. ���� 0-9
	//4. �����ַ� ~!@#$%^&*()-+=
	int tag;
	int i = 0;

	//1. �������
	if (len <= 0 || NULL == randBuf)
	{
		cout << "getRandString �����Ƿ�" << endl;
		return;
	}

	//2. �����������
	for (i = 0; i < len; i++)
	{
		//����ַ�����
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

