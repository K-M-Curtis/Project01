#include"RequestFactory.h"

RequestFactory::RequestFactory()
{

}

RequestFactory::~RequestFactory()
{

}

//���������ı������󷽷�
Codec *RequestFactory::createCodec(void *arg)
{
	Codec *codec = NULL;
	if (NULL == arg)
	{
		//�����������
		codec = new RequestCodec;
	}
	else
	{
		//�����������
		codec = new RequestCodec((RequestMsg*)arg);
	}

	return codec;
}