
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

#include <string>
#include <iostream>
#include "base64.h"

using namespace std;

class Smtp
{
public:
	Smtp();
	~Smtp();
	// SMTP初始化
	int Init();
	// 连接发信服务器
	int Connect(string address,unsigned int port,string account,string password);
	// 添加收件人，可重复调用
	int RCPT(string recipient);
	// 添加数据
	int Date(string YourName, string TaName, string Title, string text);
	//
private:
	//
	int err;
	//
	int sClient;
	sockaddr_in addrSrv;
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
