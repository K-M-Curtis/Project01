#pragma once

#include"FactoryCodec.h"
#include"RequestCodec.h"

//�����Ĺ�����  ��Ҫ���������ı��������߽�����󴴽�
class RequestFactory :public FactoryCodec
{
public:
	RequestFactory();
	~RequestFactory();
	Codec *createCodec(void *arg = NULL);

};
