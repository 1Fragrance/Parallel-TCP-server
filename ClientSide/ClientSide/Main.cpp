#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <winsock2.h>
#include <iostream>
#include <conio.h>

#pragma comment(lib,"Ws2_32.lib")

using namespace std;

// NOTE: Error msgs
const char WSASTARTUP_ERROR_MSG[255] = "WSAStartup failed with error: %d\n";
const char SOCKET_CREATING_ERROR_MSG[255] = "socket failed with error: %ld\n";
const char SOCKET_CONNECTION_ERROR_MSG[255] = "Error while connecting socket\n";
const char INCORRECT_INPUT_ERROR_MSG[255] = "** Incorrect input! **\n";
const char SERVER_ERROR_MSG[255] = "Something goes wrong, try to reconnect to the server\n";

// NOTE: Menu
const char WELCOME_MSG[255] = "Welcome to the players managing system\n1. View players\n2. Create new player\n3. Edit player\n4. Delete player\n5. Show youngest player\n6. Exit\n";
const char CONNECT_SUCCESS_MSG[255] = "You was successfully connected to the server\n";


#define LOCALHOST_ADDRESS "127.0.0.1"
#define LOCALHOST_PORT 8000

#define VIEW_PLAYERS_ACTION 49
#define CREATE_PLAYER_ACTION 50
#define EDIT_PLAYER_ACTION 51
#define DELETE_PLAYER_ACTION 52
#define YOUNGEST_PLAYER_ACTION 53
#define EXIT_ACTION 54

#define RESPONSE_BUFFER_LENGTH 2048
#define INPUT_BUFFER_LENGTH 10

bool IsRequestValid(int status, SOCKET personalSocket)
{
	if (status == SOCKET_ERROR)
	{
		printf(SERVER_ERROR_MSG);

		closesocket(personalSocket);
		return false;
	}

	return true;
}

bool IsResponseValid(int status, SOCKET personalSocket)
{
	if (status < 0)
	{
		printf(SERVER_ERROR_MSG);

		closesocket(personalSocket);
		return false;
	}

	return true;
}

void main()
{
	WSADATA wsaData;

	// NOTE: Try to initialize WinSock library
	int initializeOutput = WSAStartup(MAKEWORD(2, 2), &wsaData);

	// NOTE: Intialize errors handling
	if (initializeOutput != 0) {
		printf(WSASTARTUP_ERROR_MSG, initializeOutput);
		return;
	}

	// NOTE: Define socket with TCP protocol
	SOCKET connectSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (connectSocket == INVALID_SOCKET) {
		printf(SOCKET_CREATING_ERROR_MSG, WSAGetLastError());
		WSACleanup();
		return;
	}

	// NOTE: Socket params
	struct sockaddr_in ad;
	ad.sin_family = AF_INET;
	ad.sin_port = htons(LOCALHOST_PORT);
	ad.sin_addr.s_addr = inet_addr(LOCALHOST_ADDRESS);
	int adSize = sizeof(ad);


	// NOTE: Try to connect to the server
	int connectionResult = connect(connectSocket, (struct sockaddr*)&ad, adSize);
	if (connectionResult == SOCKET_ERROR) {
		closesocket(connectSocket);
		cout << SOCKET_CONNECTION_ERROR_MSG;
		return;
	}

	cout << CONNECT_SUCCESS_MSG;

	int input = 0;
	while (input != 54) {

		fflush(stdout);

		// NOTE: UI
		cout << WELCOME_MSG;
		input = _getche();

		// NOTE: From 1 to 8
		if (input < 49 || input > 54) {
			cout << INCORRECT_INPUT_ERROR_MSG << endl;
			continue;
		}

		// NOTE: Exit
		if (input == EXIT_ACTION) {
			break;
		}

		char inputBuffer[INPUT_BUFFER_LENGTH];
		char responseBuffer[RESPONSE_BUFFER_LENGTH];
		sprintf_s(inputBuffer, "%d", input);
		int response;

		switch(input) 
		{
			case VIEW_PLAYERS_ACTION:
				send(connectSocket, inputBuffer, strlen(inputBuffer), 0);

				response = recv(connectSocket, responseBuffer, sizeof(responseBuffer), 0);
				if (!IsResponseValid)
				{
					break;
				}

				responseBuffer[response] = '\0';

				cout << endl << endl << responseBuffer << endl;
				memset(responseBuffer, 0, RESPONSE_BUFFER_LENGTH);

				break;

			case CREATE_PLAYER_ACTION:
				send(connectSocket, inputBuffer, strlen(inputBuffer), 0);


				break;
			case EDIT_PLAYER_ACTION:
				send(connectSocket, inputBuffer, strlen(inputBuffer), 0);
				break;
			case DELETE_PLAYER_ACTION:
				send(connectSocket, inputBuffer, strlen(inputBuffer), 0);
				break;
			case YOUNGEST_PLAYER_ACTION:
				send(connectSocket, inputBuffer, strlen(inputBuffer), 0);

				response = recv(connectSocket, responseBuffer, sizeof(responseBuffer), 0);
				responseBuffer[response] = '\0';
				if (!IsResponseValid)
				{
					break;
				}

				cout << endl << endl << responseBuffer << endl;
				memset(responseBuffer, 0, RESPONSE_BUFFER_LENGTH);

				break;
			default:
				cout << INCORRECT_INPUT_ERROR_MSG << endl;
				break;
		}
	}

	// NOTE: Close socket
	closesocket(connectSocket);

	// NOTE: Dispose WinSock lib
	WSACleanup();
}