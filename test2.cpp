#define _CRT_SECURE_NO_WARNINGS
#include "ServerOperation.h"

int writeFile(const char *out, int len)
{
	FILE *fp = NULL;
	fp = fopen("E://Compilation Project//Project01//test.ber", "w");
	if (NULL == fp)
	{
		cout << "fopen failed..." << endl;
		return -1;
	}

	fwrite(out, sizeof(char), len, fp);

	fclose(fp);

	return 0;
}


int main(void)
{
	int ret = -1;

	ServerInfo info;

	//1. 从配置文件中获取服务端的配置信息
	memset(&info, 0, sizeof(info));
	strcpy(info.serverId, "1111");
	strcpy(info.dbUser, "SECMNG");
	strcpy(info.dbPasswd, "SECMNG");
	strcpy(info.dbSid, "orcl");
	info.sPort = 10086;
	info.maxNode = 10;
	info.shmKey = 0x11;

	//2. 创建密钥协商服务端对象
	ServerOperation server(&info);
	//启动服务端
	server.startWork();

	cout << "密钥协商服务端退出了" << endl;

	return 0;
}
