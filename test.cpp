
#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
//#include"RequestCodec.h"
//#include"SimFactoryCodec.h"
#include"RequestFactory.h"
#include"RespondFactory.h"

using namespace std;

int writeFile(const char *outData, int len)
{
	FILE *fp = NULL;
	fp = fopen("E://Compilation Project//Project01//test.ber", "w");
	if (NULL == fp)
	{
		cout << "fopen failed..." << endl;
		return -1;
	}
	
	fwrite(outData, sizeof(char), len, fp);

	fclose(fp);

	return 0;
}

#if 0
int main()
{
	struct RequestMsg msg;
	Codec *codec = NULL;

	char *outData = NULL;
	int outDataLen = 0;
	//初始化
	memset(&msg, 0, sizeof(msg));

	msg.cmdType = 0x01;
	strcpy(msg.clientId, "0001");
	strcpy(msg.authCode, "1111");
	strcpy(msg.serverId, "1111");
	strcpy(msg.r1, "hello itcast");

	////1. 编码 new一个对象
	//codec = new RequestCodec(&msg);
	//codec->msgEncode(&outData, outDataLen);

	//0. 创建简单工厂类对象
	//SimFactoryCodec sfactoryCodec;
	FactoryCodec *factoryCodec = new RequestFactory;

	//1. 编码 用工厂类对象的方法来创建对象
	//codec = sfactoryCodec.createCodec(1, &msg);
	codec = factoryCodec->createCodec(&msg);
	codec->msgEncode(&outData, outDataLen);

	//写文件测试
	writeFile(outData, outDataLen);
	cout << "data:" << outData << "outDataLen" << outDataLen << endl;
	cout << "编码OK" << endl;

	delete codec;

	//2. 解码
	//codec = new RequestCodec();
	//codec = sfactoryCodec.createCodec(1);
	codec = factoryCodec->createCodec();

	RequestMsg *pRequestMsg = static_cast<RequestMsg*>(codec->msgDecode(outData, outDataLen));

	cout << "cmdType:" << pRequestMsg->cmdType << endl;
	cout << "clientId:" << pRequestMsg->clientId << endl;
	cout << "authCode:" << pRequestMsg->authCode << endl;
	cout << "serverId:" << pRequestMsg->serverId << endl;
	cout << "r1:" << pRequestMsg->r1 << endl;
	cout << "解码OK" << endl;

	//内存释放
	delete codec;
	delete outData;
	
	delete factoryCodec;
	
	printf("\n");
	system("pause");
	return 0;
}

#else

int main()
{

	struct RespondMsg msg;
	Codec *codec = NULL;

	char *outData = NULL;
	int outDataLen = 0;
	memset(&msg, 0, sizeof(msg));

	msg.rv = 1;
	strcpy(msg.clientId, "0001");
	strcpy(msg.serverId, "1111");
	strcpy(msg.r2, "changetheworld");
	msg.seckeyId = 01;

	//创建工厂类对象
	FactoryCodec *factorycodec = new RespondFactory;
	codec = factorycodec->createCodec(&msg);
	//编码
	codec->msgEncode(&outData, outDataLen);

	writeFile(outData, outDataLen);
	cout << "outData: " << outData << "outDataLen: " << outDataLen << endl;
	cout << "msgEncode OK" << endl;

	delete codec;

	//创建工厂类对象
	codec = factorycodec->createCodec();
	//解码
	RespondMsg *pRespondMsg = static_cast<RespondMsg*>(codec->msgDecode(outData, outDataLen));

	cout << "rv: " << pRespondMsg->rv << endl;
	cout << "clientId: " << pRespondMsg->clientId << endl;
	cout << "serverId: " << pRespondMsg->serverId << endl;
	cout << "r2: " << pRespondMsg->r2 << endl;
	cout << "seckeyId: " << pRespondMsg->seckeyId << endl;
	cout << "解码OK" << endl;

	delete codec;
	delete outData;
	delete factorycodec;

	printf("\n");
	system("pause");
	return 0;
}

#endif
