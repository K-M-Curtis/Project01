#pragma once
#include "BaseASN1.h"
#include "ItcastLog.h"
#include <string>


//���л���
class SequenceASN1 :public BaseASN1
{
public:
	//���캯��
	SequenceASN1();
	//��������
	~SequenceASN1();

	// ���ͷ���
	int writeHeadNode(int iValue);
	int writeHeadNode(char *sValue, int len);

	// ��Ӻ�̽��
	int writeNextNode(int iValue);
	int writeNextNode(char *sValue, int len);

	// ��ͷ�������
	int readHeadNode(int &iValue);
	int readHeadNode(char *sValue);

	// ����̽������
	int readNextNode(int &iValue);
	int readNextNode(char *sValue);

	// �������
	int packSequence(char **outData, int &outLen);

	// �������
	int unpackSequence(char *inData, int inLen);

	// �ͷ�����
	void freeSequence(ITCAST_ANYBUF *node = NULL);

private:

	//ָ������ĵ�һ���ڵ�
	ITCAST_ANYBUF *m_header = NULL;
	//ָ����������һ���ڵ�
	ITCAST_ANYBUF *m_next = NULL;
	//�м���ʱ�ڵ�
	ITCAST_ANYBUF *m_tmp = NULL;

	//��־��ض���
	ItcastLog mLog;
};