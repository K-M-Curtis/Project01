#include"SecKeyShm.h"


int WriteShm(void)
{
	NodeShmInfo node;
	SecKeyShm s(0x55, 10);

	memset(&node, 0, sizeof(node));
	node.status = 1;
	node.secKeyId = 1;
	strcpy(node.clientId, "0001");
	strcpy(node.serverId, "1111");
	strcpy(node.secKey, "The world is wonderful");

	s.shmWrite(&node);

	memset(&node, 0, sizeof(node));
	node.status = 1;
	node.secKeyId = 2;
	strcpy(node.clientId, "0002");
	strcpy(node.serverId, "1111");
	strcpy(node.secKey, "Today is sunny day");

	s.shmWrite(&node);

	return 0;
}

int ReadShm(void)
{
	NodeShmInfo node;
	SecKeyShm s(0x55);

	memset(&node, 0, sizeof(node));
	s.shmRead("0001", "1111", &node);
	cout << "status: " << node.status << endl;
	cout << "secKeyId: " << node.secKeyId << endl;
	cout << "clientId: " << node.clientId << endl;
	cout << "serverId: " << node.serverId << endl;
	cout << "secKey: " << node.secKey << endl;

	memset(&node, 0, sizeof(node));
	s.shmRead("0002", "1111", &node);
	cout << "status: " << node.status << endl;
	cout << "secKeyId: " << node.secKeyId << endl;
	cout << "clientId: " << node.clientId << endl;
	cout << "serverId: " << node.serverId << endl;
	cout << "secKey: " << node.secKey << endl;

	return 0;
}


int main(void)
{
	int choice;

	while (1)
	{
		cout << "0. Quit" << endl;
		cout << "1. WriteShm" << endl;
		cout << "2. ReadShm" << endl;

		cin >> choice;
		if (0 == choice)
		{
			break;
		}

		switch (choice)
		{
		case 1:
			WriteShm();
			break;
		case 2:
			ReadShm();
			break;
		default:
			cout << "Invalid choice!" << endl;

		}
	}

	return 0;
}