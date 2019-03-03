#include "stdafx.h"



int main()
{
    cout<<"[Hello my world!]"<<endl;
	string url = "http://youth.scnu.edu.cn/qy_tongzhigonggao/index.html";

	Server server;
	server.init(url);
	server.updateConf();
	server.run();

    return 0;
}
