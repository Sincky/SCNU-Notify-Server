#include "server.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
Server::Server()
{

}

Server::~Server()
{
	smtp.~Smtp();
}

int Server::init()
{

	if (smtp.Init()) {
		cerr << "Failed init smtp!" << endl;
		return -1;
	}
	if (updateConf()==-1) {
		return -1;
	}

	return 0;
}


void Server::run()
{
	//运行读取网页数据
	string *sourceStr = new string;
	bool readTrue = false;

	while (!readTrue)
	{
		cout << "[初始化]Reading web source..." << endl;

		//信息文本初始化处理
		int status = InfoInit();

		switch (status)
		{
		case 0:
			cout << "Successfully reading of text!" << endl;
			readTrue = true;
			break;
		case -3:
		case -4:
			cout << "网络连接失败！" << endl;
			cout << "Waiting for 1h to retry..." << endl;

			sleep(60 * 60);//1小时
			break;
		default:
			cout << "Other fault: " << status << endl;
			cout << "Waiting for 6h to retry..." << endl;
			sleep(60 * 60 * 6); //6小时
			break;
		}

	}


	//删除已读取的资源
	delete sourceStr;

	cout << "[运行]程序初始化成功，开始运行中。。" << endl;

	//开始循环读取
	readTrue = false;
	while (1)
	{
		sleep(60*60*6);
		//Sleep(1000*30);
		int status = InfoDetect();

		switch (status)
		{
		case 0:
			cout << "Successfully loading!" << endl;
			break;
		case -3:
		case -4:
			cout << "网络连接失败！" << endl;
			cout << "Waiting for next time(6h) to retry..." << endl;
			break;
		default:
			cout << "Other fault: " << status << endl;
			cout << "Waiting for next time(6h) to retry..." << endl;
			break;
		}
	}
}



int Server::updateConf()
{
	//读取

	email.clear();
	ifstream file("./conf.ini");
	if (!file.is_open()) {
		cout << "配置文件打开失败！请检查配置文件~";
		return -1;
	}

	int pos;
	string line;
	while (getline(file, line)) {
		if (url.empty() && (pos = line.find("URL=")) == 0) {
			url = line.substr(pos + 4);
			Trim(url);
		}
		if (cookie.empty() && (pos = line.find("COOKIE=")) == 0) {
			cookie = line.substr(pos + 7);
			Trim(cookie);
		}
		if (smtp_url.empty() && (pos = line.find("SMTP=")) == 0) {
			smtp_url = line.substr(pos + 5);
			Trim(smtp_url);
		}
		if (sender_userN.empty() && (pos = line.find("USER=")) == 0) {
			sender_userN = line.substr(pos + 5);
			Trim(sender_userN);
		}
		if (sender_passW.empty() && (pos = line.find("PASSW=")) == 0) {
			sender_passW = line.substr(pos + 6);
			Trim(sender_passW);


		}
		if (line.find("[Recipient]")!=string::npos) {
			while (getline(file, line)) {
				if (line.find('#') == string::npos) {
					email.push_back(Trim(line));
				}
			}
		}
	}


	if (url.empty() || smtp_url.empty() || sender_userN.empty() || sender_passW.empty() || email.empty()) {
		cout << "配置读取失败，请检查配置格式是否正确~" << endl;
		return -1;
	}
	else {
		cout << ">当前配置信息<" << endl;
		cout << "URL:" << url << endl
			 << "COOKIE:" << cookie << endl
			 <<"SMTP_URL:" << smtp_url << endl
			 <<"SENDER:"<<sender_userN << endl
			 << "PASSW:" <<sender_passW<< endl;
		for (auto i : email) {
			cout <<"Recipient: "<< i << endl;
		}
	}
	return 0;
}




int Server::readWebSource(string &str)
{
	int status = http_read(this->url,this->cookie, str);
	return status;
}




int Server::InfoInit()
{
	string sourceStr;

	//读取紫荆网公告网页数据
	int status = readWebSource(sourceStr);
	if (status != 0)
	{
		cout << "读取Web错误" << endl;
		return status;
	}

	// 提取信息
	status = extractInfo(sourceStr);
	if (status != 0)
	{
		cout << "提取信息错误" << endl;
		return status;
	}


	// 从信息集缓存中查看信息
	if (units.empty())
	{
		cout << "信息初始化失败！" << endl;
		return -7;
	}

	units_it = units.begin();

	for (unsigned int i = 0; units_it!=units.end(); i++)
	{
		unit unit_t = *(units_it++);
		//地址集合插入
		if (address.insert(unit_t.address).second)
		{
			cout << "[True:" << i << "]" << endl << "Time: " << unit_t.time << endl << "Title: " << unit_t.title << endl << "Address: " << unit_t.address << endl;
		}
		else
		{
			cout << "[False:" << i << "]" << endl << "Time: " << unit_t.time << endl << "Title: " << unit_t.title << endl << "Address: " << unit_t.address << endl;
		}
	}

	//删除信息集缓存,（清除元素，但信息集空间大小还在）
	units.clear();

	return 0;
}

int Server::InfoDetect()
{
	string sourceStr, out;
	//读取委员会通知公告源码
	int stauts = readWebSource(sourceStr);
	if (stauts != 0)
	{
		return stauts;
	}

	// 提取信息
	stauts = extractInfo(sourceStr);
	if (stauts != 0)
	{
		return stauts;
	}

	units_it = units.begin();

	for (unsigned int i = 0; units_it!=units.end(); i++)
	{
		unit unit_t = *(units_it++);
		//地址集合插入
		if (address.insert(unit_t.address).second)
		{
			units_send.push_back(unit_t);
			cout << "[New:" << i << "]" << endl << "Time: " << unit_t.time << endl << "Title: " << unit_t.title << endl << "Address: " << unit_t.address << endl;
		}

	}

	//删除信息集缓存,（清除元素，但信息集空间大小还在）
	units.clear();

	//发送信息集
	int info_nums = 0;
	info_nums = units_send.size();
	if (info_nums)
	{
		char temp[10] = { '\0' };
		string nums;
		sprintf(temp, "%d", info_nums);
		nums = temp;
		nums = "本次有" + nums + "个新公告~\n";

		//循环读取待发送信息集缓存

		for (units_it = units_send.begin(); units_it != units_send.end(); units_it++)
		{
			unit unit_t = *units_it;
			out += "Times: " + unit_t.time + "\r\n"
				+ "Title: " + unit_t.title + "\r\n"
				+ "Address: " + unit_t.address + "\r\n";
		}
		cout << endl;
		cout << nums;
		cout << out;

		// 发送信息
		if (InfoSend(nums + out) == 0)
		{
			//发送成功
			//清除发送缓存
			units_send.clear();
		}
		else
		{
			//发送失败
			//不清除发送缓存，留下次发送
			cout << "信息发送失败！" << endl;
		}
	}
	else cout << "未有新公告。。。" << endl;
	return 0;
}

int Server::InfoSend(const string & info)
{
	//更新收发人信息
	if (updateConf() != 0) {
		return -1;
	}
	//连接发信服务器
	int status = smtp.Connect(smtp_url, 25, sender_userN, sender_passW);
	if (status != 0)
	{
		return status;
	}
	//发送收信人
	vector<string>::iterator it;
	for (it = email.begin(); it != email.end(); it++)
	{
		smtp.RCPT(*it);
	}
	//发送内容
	smtp.Date("院团委科技部(C++)", "萌哒哒的各位亲", "学校通知公告有更新哟~", info);

	return 0;
}

//分割信息
const string divClass = "<div class=\"category-list\">";
const string divClassEnd = "<div class=\"pages\">";
const string divLi = "<li>";
const string divLiEnd = "</li>";
const string divTime = "<span class=\"time\">";
const string divTimeEnd = "</span>";
const string divHref = "<a href=\"";
const string divHrefEnd = "\">";
const string divTitle = "\">";
const string divTitleEnd = "</a>";

int Server::extractInfo(string str)
{
	int sFirst = 0;
	int sEnd = 0;
	try {
		//定位
		sFirst = str.find(divClass);
		sEnd = str.find(divClassEnd);

		//定位分割
		str = str.substr(sFirst, sEnd - sFirst);

		// string str[10][3];

		for (int i = 0; i < 10; i++)
		{
			string tempStr;
			string strStr, unitStr, unitStr2, unitStr3;
			int tempF, tempE;
			//大分割
			sFirst = str.find(divLi);
			sEnd = str.find(divLiEnd) + divLiEnd.length();
			//细节分割
			tempStr = str.substr(sFirst, sEnd - sFirst);

			//网址获取
			tempF = tempStr.find(divHref) + divHref.length();
			tempE = tempStr.find(divHrefEnd, tempF);;
			unitStr = tempStr.substr(tempF, tempE - tempF);

			//标题获取
			tempF = tempStr.find(divTitle, tempE) + divTitle.length();
			tempE = tempStr.find(divTitleEnd);
			unitStr3 = tempStr.substr(tempF, tempE - tempF);

			//时间获取
			tempF = tempStr.find(divTime) + divTime.length();
			tempE = tempStr.find(divTimeEnd);
			unitStr2 = tempStr.substr(tempF, tempE - tempF);

			// 加入信息集缓存
			units.push_back(unit(unitStr2, unitStr3, unitStr));

			//迭代分割?
			str = str.substr(sEnd, str.size());
		}
	}
	catch (std::exception e) {
		cout << "*信息分割异常*" << endl;
		cout << e.what() << endl;
		cout << "First:" << sFirst << " " << "End:" << sEnd << endl;
		return -8;
	}


	return 0;
}


std::string& TrimL(std::string& str)
{
	str.erase(0, str.find_first_not_of(" \t\n\r"));
	return str;
}

std::string& TrimR(std::string& str)
{
	str.erase(str.find_last_not_of(" \t\n\r") + 1);
	return str;
}

std::string& Server::Trim(std::string& str)
{
	return TrimR(TrimL(str));
}

