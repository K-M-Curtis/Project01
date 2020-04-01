#include"RespondFactory.h"

RespondFactory::RespondFactory()
{

}

RespondFactory::~RespondFactory()
{

}

Codec *RespondFactory::createCodec(void *arg)
{
	Codec *codec = NULL;
	if (NULL == arg)
	{
		codec = new RespondCodec;
	}
	else
	{
		codec = new RespondCodec(static_cast<RespondMsg*>(arg));
	}

	return codec;
}

