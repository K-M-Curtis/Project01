#include"Codec.h"

Codec::Codec()
{

}

Codec::~Codec()
{

}

// ���ݱ���
int Codec::msgEncode(char **outData, int &len)
{
	return 0;
}
// ���ݽ���
void* Codec::msgDecode(char *inData, int inLen)
{
	return nullptr;
}
//�ڴ��ͷ�
int Codec::msgMemFree(void **data)
{
	return 0;
}