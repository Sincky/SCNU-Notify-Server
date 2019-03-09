#include "stdafx.h"



int main()
{
    cout<<"[Hello my world!]"<<endl;
	Server server;
	if (server.init()==0) {
		server.run();
	}
    return 0;
}
