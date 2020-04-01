#include"SimFactoryCodec.h"


SimFactoryCodec::SimFactoryCodec()
{

}

SimFactoryCodec::~SimFactoryCodec()
{

}

//工厂类中创建对象方法  解码对象
Codec *SimFactoryCodec::createCodec(int tag)
{
	Codec *codec = NULL;
	switch (tag)
	{
		//创建请求报文解码对象
	case 1:
		codec = new RequestCodec;
		break;
		//创建响应报文解码对象
	case 2:
		break;
	default:
		;
	}

	return codec;
}

//工厂类中创建对象方法  编码对象
Codec *SimFactoryCodec::createCodec(int tag, void* arg)
{
	Codec *codec = NULL;
	switch (tag)
	{
		//创建请求报文编码对象
	case 1:
		codec = new RequestCodec((RequestMsg*)arg);
		break;
		//创建响应报文编码对象
	case 2:
		break;
	default:
		;
	}

	return codec;
}