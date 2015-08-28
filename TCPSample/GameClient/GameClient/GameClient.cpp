#include "Base.h"
#include "GameClient.h"


GameClient::GameClient()
{
	init();
}


GameClient::~GameClient()
{
	uninit();
}

bool GameClient::init()
{
	bool result = false;

	WSADATA dllData;
	int error = WSAStartup(MAKEWORD(2, 0), &dllData);
	if (error != 0)
	{
		return false;
	}

	m_clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_clientSocket == INVALID_SOCKET)
	{
		return false;
	}

	return result;
}

void GameClient::uninit()
{
	if (m_clientSocket != INVALID_SOCKET)
	{
		closesocket(m_clientSocket);
	}

	WSACleanup();
}

bool GameClient::connectToServer(const string& ipAddress, int port)
{
	if (m_clientSocket == INVALID_SOCKET)
	{
		return false;
	}

	sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = inet_addr(ipAddress.c_str());
	serverAddress.sin_port = htons(port);

	int connectResult = connect(m_clientSocket, (sockaddr*)&serverAddress, sizeof(serverAddress));
	if (connectResult == SOCKET_ERROR)
	{
		return false;
	}

	return true;
}

ExecuteResult GameClient::receiveFromServer(string& receiveString, int readPackagePerTime)
{
    if (m_clientSocket == INVALID_SOCKET)
    {
        return ExecuteResult::Invalid;
    }

    char receiveBuffer[4096] = {};
    int result = recv(m_clientSocket, receiveBuffer, readPackagePerTime, 0);
    if (result == 0)
    {
        return ExecuteResult::LostConnectServer;
    }
    else if (result < 0) // 数据包传送过程中出错
    {
        return ExecuteResult::Error;
    }

    receiveString.assign(receiveBuffer);

    return ExecuteResult::Success;
}
