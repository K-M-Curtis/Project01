#include"ClientOperation.h"
#include<iostream>

#if 1
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

int showMenu(void)
{
	int choice;

	cout << "==============================================" << endl;
	cout << "=================1. 密钥协商==================" << endl;
	cout << "=================2. 密钥校验==================" << endl;
	cout << "=================3. 密钥注销==================" << endl;
	cout << "=================4. 密钥查看==================" << endl;
	cout << "=================5. 退出系统==================" << endl;
	cout << "==============================================" << endl;
	cout << "请做出选择： ";
	cin >> choice;

	while ('\n' != getchar());

	return choice;
}


int main(void)
{
	int ret = -1;
	ClientInfo info;

	//从配置文件中读取密钥协商客户端配置信息 JSON
	memset(&info, 0, sizeof(info));
	strcpy(info.clientID, "0001");
	strcpy(info.serverID, "1111");
	strcpy(info.authCode, "1111");
	strcpy(info.serverIP, "192.168.19.50");
	info.serverPort = 10086;
	info.maxNode = 1;
	info.shmKey = 0x11;

	ClientOperation client(&info);

	srandom(time(NULL));

	while (1)
	{
		system("clear");

		ret = showMenu();

		if (5 == ret)
		{
			break;
		}

		switch (ret)
		{
		case 1:		//密钥协商
			ret = client.secKeyAgree();
			cout << "密钥协商" << endl;
			break;

		case 2:		//密钥校验
			ret = client.secKeyCheck();
			cout << "密钥校验" << endl;
			break;

		case 3:		//密钥注销
			ret = client.secKeyRevoke();
			cout << "密钥注销" << endl;
			break;

		case 4:		//密钥查看
			ret = client.secKeyView();
			cout << "密钥查看" << endl;
			break;

		default:
			ret = -1;
			cout << "invalid choice" << endl;
			break;
		}
		//根据返回值判断结果
		if (-1 == ret)
		{
			cout << "做对应的业务失败" << endl;
		}
		else
		{
			cout << "做对应的业务成功" << endl;
		}

		cout << "press enter to continue..." << endl;
		getchar();

	}

	return 0;
}


#endif