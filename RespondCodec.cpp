#include"RespondCodec.h"

//����������
RespondCodec::RespondCodec()
{
	mRespondMsg = new RespondMsg;

	memset(mRespondMsg, 0, sizeof(RespondMsg));
}

//����������
RespondCodec::RespondCodec(RespondMsg* msg)
{
	mRespondMsg = new RespondMsg;
	memset(mRespondMsg, 0, sizeof(RespondMsg));
	memcpy(mRespondMsg, msg, sizeof(RespondMsg));

}

//��������
RespondCodec::~RespondCodec()
{
	delete mRespondMsg;
}


//���뺯��
int RespondCodec::msgEncode(char **outData, int &len)
{
	int ret = -1;

	if (NULL == outData)
	{
		mLog.Log(__FILE__, __LINE__, ItcastLog::ERROR, -1, "func msgEncode() err");
		return -1;
	}

	ret = writeHeadNode(mRespondMsg->rv);
	if (0 != ret)
	{
		mLog.Log(__FILE__, __LINE__, ItcastLog::ERROR, ret, "func writeHeadNode() err");
		return -1;
	}

	ret = writeNextNode(mRespondMsg->clientId, strlen(mRespondMsg->clientId));
	if (0 != ret)
	{
		mLog.Log(__FILE__, __LINE__, ItcastLog::ERROR, ret, "func writeNextNode() err");
		return -1;
	}

	ret = writeNextNode(mRespondMsg->serverId, strlen(mRespondMsg->serverId));
	if (0 != ret)
	{
		mLog.Log(__FILE__, __LINE__, ItcastLog::ERROR, ret, "func writeNextNode() err");
		return -1;
	}

	ret = writeNextNode(mRespondMsg->r2, strlen(mRespondMsg->r2));
	if (0 != ret)
	{
		mLog.Log(__FILE__, __LINE__, ItcastLog::ERROR, ret, "func writeNextNode() err");
		return -1;
	}

	ret = writeNextNode(mRespondMsg->seckeyId);
	if (0 != ret)
	{
		mLog.Log(__FILE__, __LINE__, ItcastLog::ERROR, ret, "func writeNextNode() err");
		return -1;
	}

	ret = packSequence(outData, len);
	if (0 != ret)
	{
		mLog.Log(__FILE__, __LINE__, ItcastLog::ERROR, ret, "func packSequence() err");
		return -1;
	}

	freeSequence();

	return 0;
}

//���뺯��
void* RespondCodec::msgDecode(char *inData, int inLen)
{
	int ret = -1;

	if (NULL == inData || inLen <= 0)
	{
		mLog.Log(__FILE__, __LINE__, ItcastLog::ERROR, -1, "func msgDecode() err");
		return NULL;
	}

	ret = unpackSequence(inData, inLen);
	if (0 != ret)
	{
		mLog.Log(__FILE__, __LINE__, ItcastLog::ERROR, ret, "func unpackSequence() err");
		return NULL;
	}

	ret = readHeadNode(mRespondMsg->rv);
	if (0 != ret)
	{
		mLog.Log(__FILE__, __LINE__, ItcastLog::ERROR, ret, "func readHeadNode() err");
		return NULL;
	}

	ret = readNextNode(mRespondMsg->clientId);
	if (0 != ret)
	{
		mLog.Log(__FILE__, __LINE__, ItcastLog::ERROR, ret, "func readNextNode() err");
		return NULL;
	}

	ret = readNextNode(mRespondMsg->serverId);
	if (0 != ret)
	{
		mLog.Log(__FILE__, __LINE__, ItcastLog::ERROR, ret, "func readNextNode() err");
		return NULL;
	}

	ret = readNextNode(mRespondMsg->r2);
	if (0 != ret)
	{
		mLog.Log(__FILE__, __LINE__, ItcastLog::ERROR, ret, "func readNextNode() err");
		return NULL;
	}

	ret = readNextNode(mRespondMsg->seckeyId);
	if (0 != ret)
	{
		mLog.Log(__FILE__, __LINE__, ItcastLog::ERROR, ret, "func readNextNode() err");
		return NULL;
	}

	freeSequence();

	return mRespondMsg;
}

//�ڴ��ͷ�
int RespondCodec::msgMemFree(void **point)
{
	if (NULL == point || NULL == *point)
	{
		return -1;
	}

	delete (RespondMsg*)*point;
	*point = NULL;

	return 0;
}

