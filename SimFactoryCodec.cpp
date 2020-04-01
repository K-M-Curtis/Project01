#include"SimFactoryCodec.h"


SimFactoryCodec::SimFactoryCodec()
{

}

SimFactoryCodec::~SimFactoryCodec()
{

}

//�������д������󷽷�  �������
Codec *SimFactoryCodec::createCodec(int tag)
{
	Codec *codec = NULL;
	switch (tag)
	{
		//���������Ľ������
	case 1:
		codec = new RequestCodec;
		break;
		//������Ӧ���Ľ������
	case 2:
		break;
	default:
		;
	}

	return codec;
}

//�������д������󷽷�  �������
Codec *SimFactoryCodec::createCodec(int tag, void* arg)
{
	Codec *codec = NULL;
	switch (tag)
	{
		//���������ı������
	case 1:
		codec = new RequestCodec((RequestMsg*)arg);
		break;
		//������Ӧ���ı������
	case 2:
		break;
	default:
		;
	}

	return codec;
}