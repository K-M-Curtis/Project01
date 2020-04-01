#pragma once
#include"Codec.h"

struct RespondMsg
{
	int rv;				// ����ֵ
	char clientId[12];	// �ͻ��˱��
	char serverId[12];	// ���������
	char r2[64];		// �������������
	int seckeyId;		// �Գ���Կ���    keysn
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

	//����������
	RespondCodec();
	//����������
	RespondCodec(RespondMsg* msg);
	//��������
	~RespondCodec();

	// ��д���ຯ��
	//���뺯��
	int msgEncode(char **outData, int &len);
	//���뺯��
	void* msgDecode(char *inData, int inLen);
	//�ڴ��ͷ�
	int msgMemFree(void **point);

private:
	RespondMsg *mRespondMsg;
	ItcastLog mLog;
};