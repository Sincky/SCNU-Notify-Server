#include "smtp.h"

Smtp::Smtp()
{
	wVersionRequested = MAKEWORD(1, 1);
	memset(rBuff, 0, sizeof(rBuff));
}


Smtp::~Smtp()
{
	this->destrution();
}

void Smtp::destrution()
{
	if (err == 0)//待优化
	{
		WSACleanup();
		closesocket(sClient);
	}
}


//SMTP初始化
int Smtp::Init()
{
	wVersionRequested = MAKEWORD(1, 1);
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0)
	{
		return -1;
	}

	if (LOBYTE(wsaData.wVersion) != 1 || HIBYTE(wsaData.wVersion) != 1)
	{
		WSACleanup();
		return -2;
	}
	return 0;
}


int Smtp::Connect(string address, unsigned int port,string account,string password)
{
	Account = account;
	Password = password;
	//建立通讯
	sClient = socket(AF_INET, SOCK_STREAM, 0);
	host = gethostbyname(address.c_str());
	addrSrv.sin_addr.S_un.S_addr = inet_addr(inet_ntoa(*(in_addr*)host->h_addr));
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(port);

	//连接服务器
	if (connect(sClient, (LPSOCKADDR)&addrSrv, sizeof(addrSrv)) == SOCKET_ERROR)
	{
		
		closesocket(sClient);
		cout << "连接发信服务器失败!（SOCKET_ERROR:" << WSAGetLastError() <<")" << endl;
		cout << address << " " << port << endl;
		return -1;
	}
	cout << "connect: " << endl;
	recv(sClient, rBuff, 1024, 0);
	cout << rBuff << endl;

	//打招呼
	cout << "helo hihi: " << endl;
	sBuff = "helo hihi\r\n";
	send(sClient, sBuff.c_str(), sBuff.size(), 0);
	recv(sClient, rBuff, 1024, 0);
	cout << rBuff << endl;

	//开始登陆
	cout << "auth login: " << endl;
	sBuff = "auth login\r\n";
	send(sClient, sBuff.c_str(), sBuff.size(), 0);
	recv(sClient, rBuff, 1024, 0);
	cout << rBuff << endl;

	//发送账号
	cout << "send userN: " << endl;
	sBuff = base64_encode(account.c_str(), account.size()) + "\r\n";
	cout << sBuff<<endl;
	send(sClient, sBuff.c_str(), sBuff.size(), 0);
	recv(sClient, rBuff, 1024, 0);
	cout << rBuff << endl;

	//发送密码
	cout << "send passW: " << endl;
	sBuff = base64_encode(password.c_str(), password.size()) + "\r\n";
	cout << sBuff<<endl;
	send(sClient, sBuff.c_str(), sBuff.size(), 0);
	recv(sClient, rBuff, 1024, 0);
	cout << rBuff << endl;

	//发送发件人
	cout << "send FROM: " << endl;
	sBuff = "MAIL FROM:<" + account + ">\r\n";
	send(sClient, sBuff.c_str(), sBuff.size(), 0);
	recv(sClient, rBuff, 1024, 0);
	cout << rBuff  << endl;

	return 0;
}

// 添加收件人
int Smtp::RCPT(string recipient)
{
	cout << "send recipients: " << endl;
	sBuff = "RCPT TO:<" + recipient + ">\r\n";
	send(sClient, sBuff.c_str(), sBuff.size(), 0) ;
	recv(sClient, rBuff, 1024, 0);
	cout << rBuff <<" - "<<recipient<<endl;

	return 0;
}

// 添加数据
int Smtp::Date(string YourName,string TaName,string Title,string text)
{
	cout << "send Data: " << endl;
	sBuff = "Data\r\n";
	send(sClient, sBuff.c_str(), sBuff.size(), 0);
	recv(sClient, rBuff, 1024, 0);
	cout << rBuff << endl;

	cout << "send Title: " << endl;
	sBuff = "From:" + YourName + "<" + Account + ">" + "\r\nTo:" + TaName + "\r\nSubject:" + Title + "\r\n\r\n" + text + "\r\n"+"."+"\r\n";
	send(sClient, sBuff.c_str(), sBuff.size(), 0);
	recv(sClient, rBuff, 1024, 0);
	cout << rBuff << endl;

	//离开
	cout << "sed quit: " << endl;
	sBuff = "quit\r\n";
	send(sClient, sBuff.c_str(), sBuff.size(), 0);
	recv(sClient, rBuff, 1024, 0);
	cout << rBuff << endl;

	return 0;
}