#include "server.h"
#include <fstream>
#include <sstream>
Server::Server()
{
	
}

Server::~Server()
{
	smtp.~Smtp();
}

int Server::init(string & url)
{
	this->url = url;
	
	if (smtp.Init()) {
		cerr << "Failed init smtp!" << endl;
		return -1;
	}

	return 0;
}


void Server::run()
{
	//���ж�ȡ��ҳ����	
	string *sourceStr = new string;
	bool readTrue = false;

	while (!readTrue)
	{
		cout << "[��ʼ��]Reading web source..." << endl;

		//��Ϣ�ı���ʼ������
		int status = InfoInit();

		switch (status)
		{
		case 0:
			cout << "Successfully reading of text!" << endl;
			readTrue = true;
			break;
		case -3:
		case -4:
			cout << "��������ʧ�ܣ�" << endl;
			cout << "Waiting for 1h to retry..." << endl;
			Sleep(1000 * 60 * 60);
			break;
		default:
			cout << "Other fault: " << status << endl;
			cout << "Waiting for 1h to retry..." << endl;
			Sleep(1000 * 60 * 60); //һСʱ
			break;
		}

	}
	
	//ɾ���Ѷ�ȡ����Դ
	delete sourceStr;

	cout << "[����]�����ʼ���ɹ�����ʼ�����С���" << endl;

	//��ʼѭ����ȡ
	readTrue = false;
	while (1)
	{
		Sleep(1000 * 60 * 60 * 6);
		//Sleep(1000*30);
		int status = InfoDetect();
		
		switch (status)
		{
		case 0:
			cout << "Successfully loading!" << endl;
			break;
		case -3:
		case -4:
			cout << "��������ʧ�ܣ�" << endl;
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
	//��ȡ

	email.clear();
	ifstream file("./conf.ini");
	if (!file.is_open()) {
		cout << "���ö�ȡʧ��1";
		return -1;
	}

	int pos;
	string line;
	while (getline(file, line)) {
		if (smtp_url.empty() && (pos = line.find("SMTP=")) != string::npos) {

			smtp_url = line.substr(pos + 5);
		}
		if (sender_userN.empty() && (pos = line.find("USER=")) != string::npos) {
			sender_userN = line.substr(pos + 5);
		}
		if (sender_passW.empty() && (pos = line.find("PASSW=")) != string::npos) {
			sender_passW = line.substr(pos + 6);
		}
		if (line.find("[Recipient]")!=string::npos) {
			while (getline(file, line)) {
				if (line.find('#') == string::npos) {
					email.push_back(line);
				}
			}
		}
	}
	

	if (smtp_url.empty() || sender_userN.empty() || sender_passW.empty() || email.empty()) {
		cout << "���ö�ȡʧ��2";
		return -1;
	}
	else {
		cout << ">��ǰ������Ϣ<" << endl;
		cout <<"SMTP_URL:" << smtp_url << endl <<"SENDER:"<<sender_userN << endl;
		for (auto i : email) {
			cout <<"Recipient: "<< i << endl;
		}
	}
	return 0;
}




int Server::readWebSource(string &str)
{
	int status = http_read(this->url, str);
	return status;
}




int Server::InfoInit()
{
	string sourceStr;

	//��ȡ�Ͼ���������ҳ����
	int status = readWebSource(sourceStr);
	if (status != 0)
	{
		return status;
	}

	// ��ȡ��Ϣ
	extractInfo(sourceStr);

	// ����Ϣ�������в鿴��Ϣ
	if (units.empty())
	{
		cout << "��Ϣ��ʼ��ʧ�ܣ�" << endl;
		return -7;
	}

	units_it = units.begin();

	for (unsigned int i = 0; units_it!=units.end(); i++)
	{
		unit unit_t = *(units_it++);
		//��ַ���ϲ���
		if (address.insert(unit_t.address).second)
		{
			cout << "[True:" << i << "]" << endl << "Time: " << unit_t.time << endl << "Title: " << unit_t.title << endl << "Address: " << unit_t.address << endl;
		}
		else
		{
			cout << "[False:" << i << "]" << endl << "Time: " << unit_t.time << endl << "Title: " << unit_t.title << endl << "Address: " << unit_t.address << endl;
		}
	}

	//ɾ����Ϣ������,�����Ԫ�أ�����Ϣ���ռ��С���ڣ�
	units.clear();
	
	return 0;
}

int Server::InfoDetect()
{
	string sourceStr, out;
	//��ȡίԱ��֪ͨ����Դ��
	int stauts = readWebSource(sourceStr);
	if (stauts != 0)
	{
		return stauts;
	}

	// ��ȡ��Ϣ
	extractInfo(sourceStr);

	units_it = units.begin();

	for (unsigned int i = 0; units_it!=units.end(); i++)
	{
		unit unit_t = *(units_it++);
		//��ַ���ϲ���
		if (address.insert(unit_t.address).second)
		{
			units_send.push_back(unit_t);
			cout << "[New:" << i << "]" << endl << "Time: " << unit_t.time << endl << "Title: " << unit_t.title << endl << "Address: " << unit_t.address << endl;
		}
		
	}

	//ɾ����Ϣ������,�����Ԫ�أ�����Ϣ���ռ��С���ڣ�
	units.clear();

	//������Ϣ��
	int info_nums = 0;
	info_nums = units_send.size();
	if (info_nums)
	{
		char temp[10] = { '\0' };
		string nums;
		sprintf(temp, "%d", info_nums);
		nums = temp;
		nums = "������" + nums + "���¹���~\n";
		
		//ѭ����ȡ��������Ϣ������

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

		// ������Ϣ
		if (InfoSend(nums + out) == 0)
		{
			//���ͳɹ�
			//������ͻ���
			units_send.clear();
		}
		else
		{
			//����ʧ��
			//��������ͻ��棬���´η���
			cout << "��Ϣ����ʧ�ܣ�" << endl;
		}
	}
	else cout << "δ���¹��档����" << endl;
	return 0;
}

int Server::InfoSend(const string & info)
{
	//�����շ�����Ϣ
	if (updateConf() != 0) {
		return -1;
	}
	//���ӷ��ŷ�����
	int status = smtp.Connect(smtp_url, 25, sender_userN, sender_passW);
	if (status != 0)
	{
		return status;
	}
	//����������
	vector<string>::iterator it;
	for (it = email.begin(); it != email.end(); it++)
	{
		smtp.RCPT(*it);
	}
	//��������
	smtp.Date("Ժ��ί�Ƽ���(C++)", "�����յĸ�λ��", "ѧУ֪ͨ�����и���Ӵ~", info);

	return 0;
}

//�ָ���Ϣ
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
	//��λ
	int sFirst = str.find(divClass);
	int sEnd = str.find(divClassEnd);

	//��λ�ָ�
	str = str.substr(sFirst, sEnd - sFirst);

	// string str[10][3];
	int times = 0;

	for (int i = 0; i<10; i++)
	{
		string tempStr;
		string strStr, unitStr, unitStr2, unitStr3;
		int tempF, tempE;
		//��ָ�
		sFirst = str.find(divLi);
		sEnd = str.find(divLiEnd) + divLiEnd.length();
		//ϸ�ڷָ�
		tempStr = str.substr(sFirst, sEnd - sFirst);

		//��ַ��ȡ
		tempF = tempStr.find(divHref) + divHref.length();
		tempE = tempStr.find(divHrefEnd, tempF);;
		unitStr = tempStr.substr(tempF, tempE - tempF);

		//�����ȡ
		tempF = tempStr.find(divTitle, tempE) + divTitle.length();
		tempE = tempStr.find(divTitleEnd);
		unitStr3 = tempStr.substr(tempF, tempE - tempF);

		//ʱ���ȡ
		tempF = tempStr.find(divTime) + divTime.length();
		tempE = tempStr.find(divTimeEnd);
		unitStr2 = tempStr.substr(tempF, tempE - tempF);

		// ������Ϣ������
		units.push_back(unit(unitStr2, unitStr3, unitStr));
		
		//�����ָ�?
		str = str.substr(sEnd, str.size());
	}

	return 0;
}