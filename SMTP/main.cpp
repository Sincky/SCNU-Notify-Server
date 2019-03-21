#include <iostream>
#include "server.h"
using namespace std;

int main()
{
    //输出当前工作目录
    char path[1500];
    getcwd(path,sizeof(path));
    cout << path << endl;
    cout << "[Hello my world!]" << endl;
	Server server;
	if (server.init()==0) {
		server.run();
	}
    return 0;
}
