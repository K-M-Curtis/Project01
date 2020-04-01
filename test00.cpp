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
	cout << "=================1. ��ԿЭ��==================" << endl;
	cout << "=================2. ��ԿУ��==================" << endl;
	cout << "=================3. ��Կע��==================" << endl;
	cout << "=================4. ��Կ�鿴==================" << endl;
	cout << "=================5. �˳�ϵͳ==================" << endl;
	cout << "==============================================" << endl;
	cout << "������ѡ�� ";
	cin >> choice;

	while ('\n' != getchar());

	return choice;
}


int main(void)
{
	int ret = -1;
	ClientInfo info;

	//�������ļ��ж�ȡ��ԿЭ�̿ͻ���������Ϣ JSON
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
		case 1:		//��ԿЭ��
			ret = client.secKeyAgree();
			cout << "��ԿЭ��" << endl;
			break;

		case 2:		//��ԿУ��
			ret = client.secKeyCheck();
			cout << "��ԿУ��" << endl;
			break;

		case 3:		//��Կע��
			ret = client.secKeyRevoke();
			cout << "��Կע��" << endl;
			break;

		case 4:		//��Կ�鿴
			ret = client.secKeyView();
			cout << "��Կ�鿴" << endl;
			break;

		default:
			ret = -1;
			cout << "invalid choice" << endl;
			break;
		}
		//���ݷ���ֵ�жϽ��
		if (-1 == ret)
		{
			cout << "����Ӧ��ҵ��ʧ��" << endl;
		}
		else
		{
			cout << "����Ӧ��ҵ��ɹ�" << endl;
		}

		cout << "press enter to continue..." << endl;
		getchar();

	}

	return 0;
}


#endif