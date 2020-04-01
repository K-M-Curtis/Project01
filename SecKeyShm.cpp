#include"SecKeyShm.h"

SecKeyShm::SecKeyShm(int key) :ShareMemory(key)
{
	mMaxNode = 10;
}

SecKeyShm::SecKeyShm(char *pathName) : ShareMemory(pathName)
{
	mMaxNode = 10;
}

SecKeyShm::SecKeyShm(int key, int maxNode) : ShareMemory(key, maxNode * sizeof(NodeShmInfo))
{
	mMaxNode = maxNode;
}

SecKeyShm::SecKeyShm(char *pathName, int shmSize) : ShareMemory(pathName, shmSize)
{
	mMaxNode = shmSize / sizeof(NodeShmInfo);
}

SecKeyShm::~SecKeyShm()
{

}


int SecKeyShm::shmWrite(NodeShmInfo *pNodeInfo)
{
	int i = 0;
	void *addr = NULL;

	NodeShmInfo *tmp = NULL;
	NodeShmInfo nodeInfo;
	//参数检查
	if (NULL == pNodeInfo)
	{
		cout << "SecKeyShm write 非法参数" << endl;
		return -1;
	}

	//关联共享内存
	addr = mapShm();
	if (NULL == addr)
	{
		cout << "attached to the share memory error" << endl;
		return -1;
	}

	//1. 如果共享内存中存在对应的网点密钥信息，就直接覆盖
	for (i = 0; i < mMaxNode; i++)
	{
		tmp = static_cast<NodeShmInfo*>(addr)+1;
		if ((strcmp(tmp->clientId, pNodeInfo->clientId) == 0) && (strcmp(tmp->serverId, pNodeInfo->serverId) == 0))
		{
			cout << "找到网点对应的关联信息， 现在就覆盖" << endl;
			memcpy(tmp, pNodeInfo, sizeof(NodeShmInfo));
			break;
		}
	}

	//2. 如果共享内存中不存在对应网点密钥信息，找一个空白区域写入
	if (i == mMaxNode)
	{
		memset(tmp, 0, sizeof(NodeShmInfo));
		for (i = 0; i < mMaxNode; i++)
		{
			tmp = static_cast<NodeShmInfo*>(addr)+1;
			if (memcmp(tmp, &nodeInfo, sizeof(NodeShmInfo) == 0))
			{
				cout << "找到空白区域，现在将网点密钥信息写入共享内存" << endl;
				memcpy(tmp, pNodeInfo, sizeof(NodeShmInfo));
				break;
			}
			
		}
		if (i == mMaxNode)
		{
			cout << "共享内存已经写满了，请联系管理员" << endl;
		}
	}

	//解除关联
	unMapShm();

	return 0;
}

int SecKeyShm::shmRead(const char *clientId, const char *serverId, NodeShmInfo *pNodeInfo)
{
	int i = 0;
	void *addr = NULL;
	NodeShmInfo *tmp = NULL;
	//参数检查
	if (NULL == clientId || NULL == serverId || NULL == pNodeInfo)
	{
		cout << "shmRead 参数非法" << endl;
		return -1;
	}

	//关联共享内存
	addr = mapShm();
	if (NULL == addr)
	{
		cout << "mapShm failed..." << endl;
		return -1;
	}

	for (i = 0; i < mMaxNode; i++)
	{
		tmp = static_cast<NodeShmInfo*>(addr)+1;
		if ((strcmp(clientId, tmp->clientId) == 0) && (strcmp(serverId, tmp->serverId) == 0))
		{
			cout << "找到对应网点密钥信息" << endl;
			memcpy(pNodeInfo, tmp, sizeof(NodeShmInfo));
			break;
		}

	}
	if (i == mMaxNode)
	{
		cout << "共享内存中不存在对应的网点密钥信息" << endl;
		return -1;
	}

	//解除关联
	unMapShm();
	return 0;
}

