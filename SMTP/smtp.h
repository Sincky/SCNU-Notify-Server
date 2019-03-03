#pragma once
#pragma comment(lib,"ws2_32.lib")

#include <winsock2.h>
#include <string>
#include <iostream>
#include "base64.h"

using namespace std;

class Smtp
{
public:
	Smtp();
	~Smtp();
	// SMTP��ʼ��
	int Init();
	// ���ӷ��ŷ�����
	int Connect(string address,unsigned int port,string account,string password);
	// ����ռ��ˣ����ظ�����
	int RCPT(string recipient);
	// �������
	int Date(string YourName, string TaName, string Title, string text);
	//
private:
	//
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	//
	SOCKET sClient;
	SOCKADDR_IN addrSrv;
	hostent *host;
	//
	string Account;
	string Password;
	//
	string sBuff;
	char rBuff[1025] = { '\0' };
	//
	void destrution();
};