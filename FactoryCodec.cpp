#include"FactoryCodec.h"

FactoryCodec::FactoryCodec()
{

}

FactoryCodec::~FactoryCodec()
{

}

//负责创建对应类的对象
Codec *FactoryCodec::createCodec(void *arg)
{
	return nullptr;
}