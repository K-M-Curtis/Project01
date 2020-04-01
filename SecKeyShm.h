#pragma once

#include"ShareMemory.h"
#include<iostream>

using namespace std;

//网点密钥信息结构体
struct NodeShmInfo
{
	int status;
	char clientId[12];
	char serverId[12];
	int secKeyId;
	char secKey[128];
};

//共享内存业务类
class SecKeyShm :public ShareMemory
{
public:
	SecKeyShm(int key);
	SecKeyShm(char *pathName);
	SecKeyShm(int key, int maxNode);
	SecKeyShm(char *pathName, int shmSize);
	~SecKeyShm();

	int shmWrite(NodeShmInfo *pNodeInfo);
	int shmRead(const char *clientId, const char *serverId, NodeShmInfo *pNodeInfo);


private:
	int mMaxNode;
};