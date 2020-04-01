#define _CRT_SECURE_NO_WARNINGS
#include "ServerOperation.h"

#include <signal.h>

//信号处理函数
void handler(int signo, siginfo_t *info, void *context)
{
	ServerOperation::flag = 1;

	cout << "捕捉到信号：" << signo << endl;
}


//成为守护进程
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

	//1. 从配置文件中获取服务端的配置信息
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
	//注册信号
	sigaction(SIGUSR1, &act, NULL);
	//让当前进程成为守护进程
	createDaemon();

	//2. 创建密钥协商服务端对象
	ServerOperation server(&info);
	//启动服务端
	server.startWork();

	cout << "密钥协商服务端退出了" << endl;

	return 0;
}
