
// main_new_tcp_client_test.cpp
// 2017 Jul 06

#include <common_sockettcp.hpp>
#include <stdio.h>
#ifdef WIN32
#include <conio.h>
#else
#define _getch(...) do{}while(0)
#endif

int main()
{
	common::SocketTCP aSocket;
	common::SocketTCP* pNewSocket;
	int nReturn(0), nReceived;
	char vcStringFromRemote[128];

	common::SocketBase::Initialize();

	if(aSocket.connectC("znnomos41",9030))
	{
		fprintf(stderr,"Unable to connect!\n");
		nReturn = 1;
		goto returnPoint;
	}

	nReceived = aSocket.readC(vcStringFromRemote, 127, 10000);
	printf("nReceived=%d\n", nReceived);

	if(nReceived>0){
		vcStringFromRemote[nReceived] = 0;
		printf("remote_str: %s\n", vcStringFromRemote);
	}

	pNewSocket = new common::SocketTCP(aSocket);

	
returnPoint:
	aSocket.closeC();
	delete pNewSocket;
	common::SocketBase::Cleanup();
	printf("Press any key to exit!\n"); fflush(stdout);
	_getch();
	return nReturn;
}
