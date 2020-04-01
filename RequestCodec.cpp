#include"RequestCodec.h"

//构造解码对象
RequestCodec::RequestCodec()
{
	mRequestMsg = new RequestMsg;

	memset(mRequestMsg, 0, sizeof(RequestMsg));
}

//构造编码对象
RequestCodec::RequestCodec(RequestMsg* msg)
{
	mRequestMsg = new RequestMsg;
	memset(mRequestMsg, 0, sizeof(RequestMsg));
	memcpy(mRequestMsg, msg, sizeof(RequestMsg));
}

//析构函数
RequestCodec::~RequestCodec()
{
	delete mRequestMsg;
}

// 重写父类函数
//编码函数
int RequestCodec::msgEncode(char **outData, int &len)
{
	int ret = -1;

	//1. 参数检查
	if (NULL == outData)
	{
		mLog.Log(__FILE__, __LINE__, ItcastLog::ERROR, -1, "func msgEncode() err");
		return -1;
	}

	//2. 编码cmdType
	ret = writeHeadNode(mRequestMsg->cmdType);
	if (0 != ret)
	{
		mLog.Log(__FILE__, __LINE__, ItcastLog::ERROR, ret, "func writeHeadNode() err");
		return -1;
	}

	//3. 编码clientId
	ret = writeNextNode(mRequestMsg->clientId, strlen(mRequestMsg->clientId));
	if (0 != ret)
	{
		mLog.Log(__FILE__, __LINE__, ItcastLog::ERROR, ret, "func writeNextNode() err");
		return -1;
	}

	//4. 编码authCode
	ret = writeNextNode(mRequestMsg->authCode, strlen(mRequestMsg->authCode));
	if (0 != ret)
	{
		mLog.Log(__FILE__, __LINE__, ItcastLog::ERROR, ret, "func writeNextNode() err");
		return -1;
	}

	//5. 编码serverId
	ret = writeNextNode(mRequestMsg->serverId, strlen(mRequestMsg->serverId));
	if (0 != ret)
	{
		mLog.Log(__FILE__, __LINE__, ItcastLog::ERROR, ret, "func writeNextNode() err");
		return -1;
	}

	//6. 编码r1
	ret = writeNextNode(mRequestMsg->r1, strlen(mRequestMsg->r1));
	if (0 != ret)
	{
		mLog.Log(__FILE__, __LINE__, ItcastLog::ERROR, ret, "func writeNextNode() err");
		return -1;
	}

	//7. 编码结构体
	ret = packSequence(outData, len);
	if (0 != ret)
	{
		mLog.Log(__FILE__, __LINE__, ItcastLog::ERROR, ret, "func packSequence() err");
		return -1;
	}

	//8. 传出

	//9. 释放内存 链表的内存
	freeSequence();

	return 0;
}

//解码函数
void* RequestCodec::msgDecode(char *inData, int inLen)
{
	int ret = -1;
	//1. 参数检查
	if (inData == NULL || inLen <= 0)
	{
		mLog.Log(__FILE__, __LINE__, ItcastLog::ERROR, -1, "func msgDecode() err");
		return NULL;
	}

	//2. 解码结构体
	ret = unpackSequence(inData, inLen);
	if (0 != ret)
	{
		mLog.Log(__FILE__, __LINE__, ItcastLog::ERROR, ret, "func unpackSequence() err");
		return NULL;
	}

	//3. 解码cmdType
	ret = readHeadNode(mRequestMsg->cmdType);
	if (0 != ret)
	{
		mLog.Log(__FILE__, __LINE__, ItcastLog::ERROR, ret, "func readHeadNode() err");
		return NULL;
	}

	//4. 解码clientId
	ret = readNextNode(mRequestMsg->clientId);
	if (0 != ret)
	{
		mLog.Log(__FILE__, __LINE__, ItcastLog::ERROR, ret, "func readNextNode() err");
		return NULL;
	}

	//5. 解码authCode
	ret = readNextNode(mRequestMsg->authCode);
	if (0 != ret)
	{
		mLog.Log(__FILE__, __LINE__, ItcastLog::ERROR, ret, "func readNextNode() err");
		return NULL;
	}

	//6. 解码serverId
	ret = readNextNode(mRequestMsg->serverId);
	if (0 != ret)
	{
		mLog.Log(__FILE__, __LINE__, ItcastLog::ERROR, ret, "func readNextNode() err");
		return NULL;
	}

	//7. 解码r1
	ret = readNextNode(mRequestMsg->r1);
	if (0 != ret)
	{
		mLog.Log(__FILE__, __LINE__, ItcastLog::ERROR, ret, "func readNextNode() err");
		return NULL;
	}

	//8. 传出

	//9. 释放内存 链表的内存
	freeSequence();
	return mRequestMsg;
}

//内存释放
int RequestCodec::msgMemFree(void **point)
{
	//1. 参数检查
	if (NULL == point || NULL == *point)
	{
		return -1;
	}
	//2. 释放内存
	delete (RequestMsg*)*point;
	return 0;
}
