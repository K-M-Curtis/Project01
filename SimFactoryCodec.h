#pragma once
#include"RequestCodec.h"

//�򵥹���ģʽ�� ���𴴽��ܶ������
class SimFactoryCodec
{
public:
	SimFactoryCodec();
	~SimFactoryCodec();

	//�������д������󷽷�  �������
	Codec *createCodec(int tag);
	//�������д������󷽷�  �������
	Codec *createCodec(int tag, void* arg);

};