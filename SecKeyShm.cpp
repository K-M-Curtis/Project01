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
	//�������
	if (NULL == pNodeInfo)
	{
		cout << "SecKeyShm write �Ƿ�����" << endl;
		return -1;
	}

	//���������ڴ�
	addr = mapShm();
	if (NULL == addr)
	{
		cout << "attached to the share memory error" << endl;
		return -1;
	}

	//1. ��������ڴ��д��ڶ�Ӧ��������Կ��Ϣ����ֱ�Ӹ���
	for (i = 0; i < mMaxNode; i++)
	{
		tmp = static_cast<NodeShmInfo*>(addr)+1;
		if ((strcmp(tmp->clientId, pNodeInfo->clientId) == 0) && (strcmp(tmp->serverId, pNodeInfo->serverId) == 0))
		{
			cout << "�ҵ������Ӧ�Ĺ�����Ϣ�� ���ھ͸���" << endl;
			memcpy(tmp, pNodeInfo, sizeof(NodeShmInfo));
			break;
		}
	}

	//2. ��������ڴ��в����ڶ�Ӧ������Կ��Ϣ����һ���հ�����д��
	if (i == mMaxNode)
	{
		memset(tmp, 0, sizeof(NodeShmInfo));
		for (i = 0; i < mMaxNode; i++)
		{
			tmp = static_cast<NodeShmInfo*>(addr)+1;
			if (memcmp(tmp, &nodeInfo, sizeof(NodeShmInfo) == 0))
			{
				cout << "�ҵ��հ��������ڽ�������Կ��Ϣд�빲���ڴ�" << endl;
				memcpy(tmp, pNodeInfo, sizeof(NodeShmInfo));
				break;
			}
			
		}
		if (i == mMaxNode)
		{
			cout << "�����ڴ��Ѿ�д���ˣ�����ϵ����Ա" << endl;
		}
	}

	//�������
	unMapShm();

	return 0;
}

int SecKeyShm::shmRead(const char *clientId, const char *serverId, NodeShmInfo *pNodeInfo)
{
	int i = 0;
	void *addr = NULL;
	NodeShmInfo *tmp = NULL;
	//�������
	if (NULL == clientId || NULL == serverId || NULL == pNodeInfo)
	{
		cout << "shmRead �����Ƿ�" << endl;
		return -1;
	}

	//���������ڴ�
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
			cout << "�ҵ���Ӧ������Կ��Ϣ" << endl;
			memcpy(pNodeInfo, tmp, sizeof(NodeShmInfo));
			break;
		}

	}
	if (i == mMaxNode)
	{
		cout << "�����ڴ��в����ڶ�Ӧ��������Կ��Ϣ" << endl;
		return -1;
	}

	//�������
	unMapShm();
	return 0;
}

