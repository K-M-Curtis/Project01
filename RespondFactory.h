#pragma once 
#include"FactoryCodec.h"
#include"RespondCodec.h"

class RespondFactory :public FactoryCodec
{
public:
	RespondFactory();
	~RespondFactory();

	Codec *createCodec(void *arg = NULL);

};