#include"RespondCodec.h"

//构造解码对象
RespondCodec::RespondCodec()
{
	mRespondMsg = new RespondMsg;

	memset(mRespondMsg, 0, sizeof(RespondMsg));
}

//构造编码对象
RespondCodec::RespondCodec(RespondMsg* msg)
{
	mRespondMsg = new RespondMsg;
	memset(mRespondMsg, 0, sizeof(RespondMsg));
	memcpy(mRespondMsg, msg, sizeof(RespondMsg));

}

//析构函数
RespondCodec::~RespondCodec()
{
	delete mRespondMsg;
}


//编码函数
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

//解码函数
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

//内存释放
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

