

#include <iostream>
#include "http.h"
#include "smtp.h"

using namespace std;

// 信息结构体
struct unit
{
	string time;
	string title;
	string address;
	unit(string time, string title, string address)
		:time(time), title(title), address(address)
	{};
};

class Server {
public:
	Server();
	~Server();
	// 初始化
	int init();
	// 运行
	void run();
	// 更新发送者、收件人信息
	int updateConf();

private:
	Smtp smtp; //发信服务器
	string url; //目标地址
	string cookie;
	string smtp_url;
	string sender_userN;
	string sender_passW;
	set<string> address; //存储信息
	vector<string> email; //存储收件人email地址
	vector<unit> units;	//信息集缓存
	vector<unit> units_send; //待发送信息集缓存
	vector<unit>::iterator units_it; //信息集通用迭代器

	// 读取网页源码
	int readWebSource(string &Str);

	// 信息初始化
	int InfoInit();

	// 信息检测
	int InfoDetect();

	// 发送信息
	int InfoSend(const string &info);

	// 提取信息
	int extractInfo(string str);

	// 首位去空
	string& Trim(string& str);

};
