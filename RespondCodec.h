#pragma once
#include"Codec.h"

struct RespondMsg
{
	int rv;				// 返回值
	char clientId[12];	// 客户端编号
	char serverId[12];	// 服务器编号
	char r2[64];		// 服务器端随机数
	int seckeyId;		// 对称密钥编号    keysn
	//RespondMsg(){}
	//RespondMsg(char *clientId, char *serverId, char *r2, int rv, int seckeyId)
	//{
	//	this->rv = rv;
	//	this->seckeyId = seckeyId;
	//	strcpy(this->clientId, clientId);
	//	strcpy(this->serverId, serverId);
	//	strcpy(this->r2, r2);
	//}
};

class RespondCodec :public Codec
{
public:

	//构造解码对象
	RespondCodec();
	//构造编码对象
	RespondCodec(RespondMsg* msg);
	//析构函数
	~RespondCodec();

	// 重写父类函数
	//编码函数
	int msgEncode(char **outData, int &len);
	//解码函数
	void* msgDecode(char *inData, int inLen);
	//内存释放
	int msgMemFree(void **point);

private:
	RespondMsg *mRespondMsg;
	ItcastLog mLog;
};