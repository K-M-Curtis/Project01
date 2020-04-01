#include"RequestCodec.h"

//����������
RequestCodec::RequestCodec()
{
	mRequestMsg = new RequestMsg;

	memset(mRequestMsg, 0, sizeof(RequestMsg));
}

//����������
RequestCodec::RequestCodec(RequestMsg* msg)
{
	mRequestMsg = new RequestMsg;
	memset(mRequestMsg, 0, sizeof(RequestMsg));
	memcpy(mRequestMsg, msg, sizeof(RequestMsg));
}

//��������
RequestCodec::~RequestCodec()
{
	delete mRequestMsg;
}

// ��д���ຯ��
//���뺯��
int RequestCodec::msgEncode(char **outData, int &len)
{
	int ret = -1;

	//1. �������
	if (NULL == outData)
	{
		mLog.Log(__FILE__, __LINE__, ItcastLog::ERROR, -1, "func msgEncode() err");
		return -1;
	}

	//2. ����cmdType
	ret = writeHeadNode(mRequestMsg->cmdType);
	if (0 != ret)
	{
		mLog.Log(__FILE__, __LINE__, ItcastLog::ERROR, ret, "func writeHeadNode() err");
		return -1;
	}

	//3. ����clientId
	ret = writeNextNode(mRequestMsg->clientId, strlen(mRequestMsg->clientId));
	if (0 != ret)
	{
		mLog.Log(__FILE__, __LINE__, ItcastLog::ERROR, ret, "func writeNextNode() err");
		return -1;
	}

	//4. ����authCode
	ret = writeNextNode(mRequestMsg->authCode, strlen(mRequestMsg->authCode));
	if (0 != ret)
	{
		mLog.Log(__FILE__, __LINE__, ItcastLog::ERROR, ret, "func writeNextNode() err");
		return -1;
	}

	//5. ����serverId
	ret = writeNextNode(mRequestMsg->serverId, strlen(mRequestMsg->serverId));
	if (0 != ret)
	{
		mLog.Log(__FILE__, __LINE__, ItcastLog::ERROR, ret, "func writeNextNode() err");
		return -1;
	}

	//6. ����r1
	ret = writeNextNode(mRequestMsg->r1, strlen(mRequestMsg->r1));
	if (0 != ret)
	{
		mLog.Log(__FILE__, __LINE__, ItcastLog::ERROR, ret, "func writeNextNode() err");
		return -1;
	}

	//7. ����ṹ��
	ret = packSequence(outData, len);
	if (0 != ret)
	{
		mLog.Log(__FILE__, __LINE__, ItcastLog::ERROR, ret, "func packSequence() err");
		return -1;
	}

	//8. ����

	//9. �ͷ��ڴ� ������ڴ�
	freeSequence();

	return 0;
}

//���뺯��
void* RequestCodec::msgDecode(char *inData, int inLen)
{
	int ret = -1;
	//1. �������
	if (inData == NULL || inLen <= 0)
	{
		mLog.Log(__FILE__, __LINE__, ItcastLog::ERROR, -1, "func msgDecode() err");
		return NULL;
	}

	//2. ����ṹ��
	ret = unpackSequence(inData, inLen);
	if (0 != ret)
	{
		mLog.Log(__FILE__, __LINE__, ItcastLog::ERROR, ret, "func unpackSequence() err");
		return NULL;
	}

	//3. ����cmdType
	ret = readHeadNode(mRequestMsg->cmdType);
	if (0 != ret)
	{
		mLog.Log(__FILE__, __LINE__, ItcastLog::ERROR, ret, "func readHeadNode() err");
		return NULL;
	}

	//4. ����clientId
	ret = readNextNode(mRequestMsg->clientId);
	if (0 != ret)
	{
		mLog.Log(__FILE__, __LINE__, ItcastLog::ERROR, ret, "func readNextNode() err");
		return NULL;
	}

	//5. ����authCode
	ret = readNextNode(mRequestMsg->authCode);
	if (0 != ret)
	{
		mLog.Log(__FILE__, __LINE__, ItcastLog::ERROR, ret, "func readNextNode() err");
		return NULL;
	}

	//6. ����serverId
	ret = readNextNode(mRequestMsg->serverId);
	if (0 != ret)
	{
		mLog.Log(__FILE__, __LINE__, ItcastLog::ERROR, ret, "func readNextNode() err");
		return NULL;
	}

	//7. ����r1
	ret = readNextNode(mRequestMsg->r1);
	if (0 != ret)
	{
		mLog.Log(__FILE__, __LINE__, ItcastLog::ERROR, ret, "func readNextNode() err");
		return NULL;
	}

	//8. ����

	//9. �ͷ��ڴ� ������ڴ�
	freeSequence();
	return mRequestMsg;
}

//�ڴ��ͷ�
int RequestCodec::msgMemFree(void **point)
{
	//1. �������
	if (NULL == point || NULL == *point)
	{
		return -1;
	}
	//2. �ͷ��ڴ�
	delete (RequestMsg*)*point;
	return 0;
}
