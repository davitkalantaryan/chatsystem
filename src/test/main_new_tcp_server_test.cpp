
// main_new_tcp_server_test.cpp
// 2017 Jul 06

#include <common_servertcp.hpp>
#include <common_socketbase.hpp>
#include <stdio.h>
#ifdef WIN32
#include <conio.h>
#else
#define _getch(...) do{}while(0)
#endif

class NewServerTcp : public common::ServerTCP
{
	void AddClient(common::SocketTCP& clientSocket, const sockaddr_in* remAddress);
};


int main()
{
	NewServerTcp aServer;
	int nReturn(0);

	common::SocketBase::Initialize();

	aServer.StartServer(9030,1000);

	common::SocketBase::Cleanup();
	
	printf("Press any key to exit!\n"); fflush(stdout);
	_getch();
	return nReturn;
}


void NewServerTcp::AddClient(common::SocketTCP& a_clientSocket, const sockaddr_in* a_remAddress)
{
	char vcBuf[128];

	printf("Connection from the host: %s\n", common::SocketBase::GetIPAddress(a_remAddress, vcBuf, 127));
	a_clientSocket.writeC("hallo", 5);
}
