#pragma once
class GameServer
{
public:
	GameServer();
	~GameServer();

	void serverBind(int port);
	void serverListen(int maxConnectRequireCount);
	SOCKET serverAccept(sockaddr_in* clientInfo);
private:
	bool init();
	void uninit();

	SOCKET m_serverSocket = INVALID_SOCKET;
};

