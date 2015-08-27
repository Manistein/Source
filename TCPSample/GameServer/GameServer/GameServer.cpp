#include "Base.h"
#include "GameServer.h"


GameServer::GameServer()
{
	init();
}


GameServer::~GameServer()
{
	uninit();
}

bool GameServer::init()
{
	bool result = true;

	WSADATA dllData;
	int error = WSAStartup(MAKEWORD(2, 0), &dllData);
	if (error != 0)
	{
		result = false;
		return result;
	}

	m_serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_serverSocket == INVALID_SOCKET)
	{
		result = false;
		return result;
	}

	return result;
}

void GameServer::uninit()
{
	if (m_serverSocket != INVALID_SOCKET)
	{
		closesocket(m_serverSocket);
	}

	WSACleanup();
}

void GameServer::serverBind(int port)
{
	if (m_serverSocket == INVALID_SOCKET)
	{
		return;
	}

	sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(port);
	serverAddress.sin_addr.s_addr = INADDR_ANY;

	int error = bind(m_serverSocket, (sockaddr*)&serverAddress, sizeof(serverAddress));
	if (error != 0)
	{
		return;
	}
}

void GameServer::serverListen(int maxConnectRequireCount)
{
	if (m_serverSocket == INVALID_SOCKET)
	{
		return;
	}

	int error = listen(m_serverSocket, maxConnectRequireCount);
	if (error != 0)
	{
		return;
	}
}

SOCKET GameServer::serverAccept(sockaddr_in* clientInfo)
{
	if (m_serverSocket == INVALID_SOCKET)
	{
		return INVALID_SOCKET;
	}

	int clientInfoSize = sizeof(*clientInfo);
	int clientSocket = accept(m_serverSocket, (sockaddr*)clientInfo, &clientInfoSize);
	if (clientSocket != INVALID_SOCKET)
	{
		return clientSocket;
	}

	return INVALID_SOCKET;
}
