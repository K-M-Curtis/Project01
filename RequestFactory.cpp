#include"RequestFactory.h"

RequestFactory::RequestFactory()
{

}

RequestFactory::~RequestFactory()
{

}

//创建请求报文编解码对象方法
Codec *RequestFactory::createCodec(void *arg)
{
	Codec *codec = NULL;
	if (NULL == arg)
	{
		//创建解码对象
		codec = new RequestCodec;
	}
	else
	{
		//创建编码对象
		codec = new RequestCodec((RequestMsg*)arg);
	}

	return codec;
}