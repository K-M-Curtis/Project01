#pragma once

#include"Codec.h"

//���������
class FactoryCodec
{
public:
	FactoryCodec();
	virtual ~FactoryCodec();
	//���𴴽���Ӧ��Ķ���
	virtual Codec *createCodec(void *arg = NULL);
};