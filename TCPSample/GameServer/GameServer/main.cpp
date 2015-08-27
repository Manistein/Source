// GameServer

#include "Base.h"
#include "GameServer.h"

void main()
{
	GameServer gameServer;
	gameServer.serverBind(8001);
	gameServer.serverListen(30);

	cout << "Listening..." << endl;
	while (true)
	{
		sockaddr_in clientInfo;
		SOCKET clientSocket = gameServer.serverAccept(&clientInfo);
		if (clientSocket != INVALID_SOCKET)
		{
			cout << "Accept client, ip = " << inet_ntoa(clientInfo.sin_addr) << endl;
		}
	}
}