#pragma once

enum class SendResult
{
    Invalid = 0,

    SendDataError,
    LostConnect,
    Success

};


class GameServer
{
public:
	GameServer();
	~GameServer();

	void serverBind(int port);
	void serverListen(int maxConnectRequireCount);
	SOCKET serverAccept(sockaddr_in* clientInfo);
    SendResult sendData(SOCKET clientSocket, const string& data);
    int receiveData(string& receiveData, int readPackageAmountPerTime);
private:
	bool init();
	void uninit();

	SOCKET m_serverSocket = INVALID_SOCKET;
};

