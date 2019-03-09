#pragma once
#pragma comment(lib,"ws2_32.lib")
#include <winsock2.h>
#include <iostream>
#include <regex>
#include <string>
#include <set>
#include <fstream>

using namespace std;

int http_read(const string &url, const string &cookie,string &returnStr);

void UTF8toANSI(string &strUTF8);