// GameClient

#include "Base.h"
#include "GameClient.h"
#include <windows.h>
#include <timeapi.h>

bool isTimeForReconnectAgain()
{
	static float lastTime = ::timeGetTime() / 1000.0f;
	static float currentTime = lastTime;

	currentTime = ::timeGetTime() / 1000.0f;

	if (currentTime - lastTime > 5.0f)
	{
		lastTime = currentTime;

		return true;
	}

	return false;
}

void main()
{
	GameClient client;
	bool connectSuccess = client.connectToServer("192.168.1.105", 8001);

	cout << "Connect to server..." << endl;
	while(true)
	{
		if (!connectSuccess && isTimeForReconnectAgain())
		{
			connectSuccess = client.connectToServer("180.88.45.10", 8001);

			cout << "Try connect server agian..." << endl;
		}
	}
}