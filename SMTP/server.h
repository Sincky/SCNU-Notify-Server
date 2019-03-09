#pragma once

#include <iostream>
#include "http.h"
#include "smtp.h"

using namespace std;

// ��Ϣ�ṹ��
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
	// ��ʼ��
	int init();
	// ����
	void run();
	// ���·����ߡ��ռ�����Ϣ
	int updateConf();

private:
	Smtp smtp; //���ŷ�����
	string url; //Ŀ���ַ
	string cookie;
	string smtp_url;
	string sender_userN;
	string sender_passW;
	set<string> address; //�洢��Ϣ
	vector<string> email; //�洢�ռ���email��ַ
	vector<unit> units;	//��Ϣ������
	vector<unit> units_send; //��������Ϣ������
	vector<unit>::iterator units_it; //��Ϣ��ͨ�õ�����
	
	// ��ȡ��ҳԴ��
	int readWebSource(string &Str);

	// ��Ϣ��ʼ��
	int InfoInit();

	// ��Ϣ���
	int InfoDetect();
	
	// ������Ϣ
	int InfoSend(const string &info);

	// ��ȡ��Ϣ 
	int extractInfo(string str);


};