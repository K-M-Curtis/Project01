#define _CRT_SECURE_NO_WARNINGS
#include "ServerOperation.h"

#include <signal.h>

//�źŴ�����
void handler(int signo, siginfo_t *info, void *context)
{
	ServerOperation::flag = 1;

	cout << "��׽���źţ�" << signo << endl;
}


//��Ϊ�ػ�����
int createDaemon(void)
{
	int ret = -1;
	pid_t pid = -1;

	pid = fork();
	if (-1 == ret)
	{
		perror("fork");
		return -1;
	}

	if (pid > 0)
	{
		exit(0);
	}

	ret = setsid();
	if (-1 == ret)
	{
		perror("setsid");
		return -1;
	}



	return 0;
}

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

	struct sigaction act;

	ServerInfo info;

	//1. �������ļ��л�ȡ����˵�������Ϣ
	memset(&info, 0, sizeof(info));
	strcpy(info.serverId, "1111");
	strcpy(info.dbUser, "SECMNG");
	strcpy(info.dbPasswd, "SECMNG");
	strcpy(info.dbSid, "orcl");
	info.sPort = 10086;
	info.maxNode = 10;
	info.shmKey = 0x11;

	memset(&act, 0, sizeof(act));
	act.sa_sigaction = handler;
	act.sa_flags = SA_SIGINFO;
	//ע���ź�
	sigaction(SIGUSR1, &act, NULL);
	//�õ�ǰ���̳�Ϊ�ػ�����
	createDaemon();

	//2. ������ԿЭ�̷���˶���
	ServerOperation server(&info);
	//���������
	server.startWork();

	cout << "��ԿЭ�̷�����˳���" << endl;

	return 0;
}
