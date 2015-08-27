#pragma once
class GameClient
{
public:
	GameClient();
	~GameClient();

	bool connectToServer(const string& ipAddress, int port);

private:
	bool init();
	void uninit();

	SOCKET m_clientSocket = INVALID_SOCKET;
};

