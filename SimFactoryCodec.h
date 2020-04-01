#pragma once
#include"RequestCodec.h"

//简单工厂模式： 负责创建很多类对象
class SimFactoryCodec
{
public:
	SimFactoryCodec();
	~SimFactoryCodec();

	//工厂类中创建对象方法  解码对象
	Codec *createCodec(int tag);
	//工厂类中创建对象方法  编码对象
	Codec *createCodec(int tag, void* arg);

};