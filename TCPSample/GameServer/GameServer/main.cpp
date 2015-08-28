// GameServer

#include "Base.h"
#include "GameServer.h"

void printResult(SendResult result);
void main()
{
	GameServer gameServer;
	gameServer.serverBind(8001);
	gameServer.serverListen(30);

    vector<SOCKET> clientSocketList;

	cout << "Listening..." << endl;
	while (true)
	{
		sockaddr_in clientInfo;
		SOCKET newClientSocket = gameServer.serverAccept(&clientInfo);
		if (newClientSocket != INVALID_SOCKET)
		{
            auto acceptedClientAddress = inet_ntoa(clientInfo.sin_addr);
            cout << "Accept client, ip = " << acceptedClientAddress << endl;

            string serverSendData(acceptedClientAddress);
            SendResult sendResult = gameServer.sendData(newClientSocket, serverSendData + " Welcome!");
            printResult(sendResult);

            clientSocketList.push_back(newClientSocket);
		}

        string receiveString;
        gameServer.receiveData(receiveString, 255);
        if (!receiveString.empty())
        {
            cout << "Receive data : " << receiveString << endl;
        }

        string sendString;
        cin >> sendString;
        for (auto clientSocket : clientSocketList)
        {
            SendResult sendResult = gameServer.sendData(clientSocket, sendString);
            printResult(sendResult);
        }
	}
}

void printResult(SendResult result)
{
    switch (result)
    {
    case SendResult::Invalid:
        cout << "Server socket invalid" << endl;
        break;
    case SendResult::LostConnect:
        cout << "Lost to connect " << endl;
        break;
    case SendResult::SendDataError:
        cout << "Get error " << WSAGetLastError() << endl;
        break;
    case SendResult::Success:
        cout << "Success" << endl;
    default:   break;
    }
}
