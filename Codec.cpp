#include"Codec.h"

Codec::Codec()
{

}

Codec::~Codec()
{

}

// 数据编码
int Codec::msgEncode(char **outData, int &len)
{
	return 0;
}
// 数据解码
void* Codec::msgDecode(char *inData, int inLen)
{
	return nullptr;
}
//内存释放
int Codec::msgMemFree(void **data)
{
	return 0;
}