
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

#include <iostream>
#include <regex>
#include <string>
#include <set>
#include <fstream>

using namespace std;

int http_read(const string &url, const string &cookie,string &returnStr);


