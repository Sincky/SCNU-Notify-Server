#include "http.h"

int http_read(const string &url,const string &cookie ,string &returnStr)
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

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

	//建立通讯
	SOCKET sClient = socket(AF_INET, SOCK_STREAM, 0);
	SOCKADDR_IN addrSrv;
	hostent *host;

	//正则取web、host
	string web;
	string hosts;
	string cookies = cookie;
	regex reg("//([^/]+)(.*)");
	smatch match;
	regex_search(url, match, reg);
	hosts = match.str(1);
	web = match.str(2).c_str();
	host = gethostbyname(hosts.c_str());
	cout << "[SOCKET]" << endl
		 << "HOSTS:" << hosts << endl
		 << "WEB:" << web << endl;
	if (host == NULL)
	{
		return -4;
	}
	addrSrv.sin_addr.s_addr = inet_addr(inet_ntoa(*(in_addr*)host->h_addr));
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(80);

	//连接服务器
	if (connect(sClient, (LPSOCKADDR)&addrSrv, sizeof(addrSrv)) == SOCKET_ERROR)
	{
		WSACleanup();
		return -3;
	}

	string sBuff;
	char rBuff[1024] = { '\0' };

	//设置send 和 recv超时
	int TimeOut = 10000;
	setsockopt(sClient, SOL_SOCKET, SO_RCVTIMEO, (char *)&TimeOut, sizeof(int));
	setsockopt(sClient, SOL_SOCKET, SO_SNDTIMEO, (char *)&TimeOut, sizeof(int));

	//
	sBuff = "GET "+ web + " HTTP/1.1\r\n" +
			"HOST: " + hosts + "\r\n" +
			"Connection: Close\r\n"+
			"Cookie:"+ cookies +"\r\n"+
			"\r\n";
	if (!send(sClient, sBuff.c_str(), sBuff.size(), 0))
	{
		cout << "Send error!" << endl;
		return -5;
	}

	sBuff = "";

	int status = 0;
	while (status = recv(sClient, rBuff, 512, 0))
	{
		if (status > 0)
		{
			sBuff += rBuff;
			memset(rBuff, 0, 512);
		}
		else
		{
			cout << "Recv error!" << endl;
			return -6;
		}
	}

	//网页编码UTF8转为ANSI
	UTF8toANSI(sBuff);

	//清除socket
	WSACleanup();
	closesocket(sClient);

	returnStr = sBuff;

	//输出网页信息
	fstream file;
	file.open("test.html", ios::out);
	//file << *p;
	file.write(returnStr.c_str(), returnStr.length());
	file.close();

	return 0;
}


void UTF8toANSI(string &strUTF8)
{
	//获取转换为多字节后需要的缓冲区大小，创建多字节缓冲区
	UINT nLen = MultiByteToWideChar(CP_UTF8, NULL, strUTF8.c_str(), -1, NULL, NULL);
	WCHAR *wszBuffer = new WCHAR[nLen + 1];
	nLen = MultiByteToWideChar(CP_UTF8, NULL, strUTF8.c_str(), -1, wszBuffer, nLen);
	wszBuffer[nLen] = 0;

	nLen = WideCharToMultiByte(936, NULL, wszBuffer, -1, NULL, NULL, NULL, NULL);
	CHAR *szBuffer = new CHAR[nLen + 1];
	nLen = WideCharToMultiByte(936, NULL, wszBuffer, -1, szBuffer, nLen, NULL, NULL);
	szBuffer[nLen] = 0;

	strUTF8 = szBuffer;
	//清理内存
	delete[]szBuffer;
	delete[]wszBuffer;
}