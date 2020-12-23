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
const char INCORRECT_INPUT_ERROR_MSG[255] = "** Incorrect input! **\n** Please, try again **\n";
const char SERVER_ERROR_MSG[255] = "Something goes wrong, try to reconnect to the server\n";

// NOTE: Menu
const char WELCOME_MSG[255] = "\nWelcome to the players managing system\n1. View players\n2. Create new player\n3. Edit player\n4. Delete player\n5. Show youngest player\n6. Exit\n";
const char CONNECT_SUCCESS_MSG[255] = "You was successfully connected to the server\n";
const char DELETE_PLAYER_SUBMENU[255] = "Please, enter id of player to delete or type '*' to return back\n";
const char EDIT_PLAYER_SUBMENU[255] = "Please, enter id of player to edit or type '*' to return back\n";

const char CREATE_PLAYER_FULLNAME_MSG[255] = "\nEnter fullname\n";
const char CREATE_PLAYER_AGE_MSG[255] = "\nEnter age\n";
const char CREATE_PLAYER_HEIGHT_MSG[255] = "\nEnter height\n";
const char CREATE_PLAYER_WEIGHT_MSG[255] = "\nEnter weight\n";


// NOTE: Server constants
#define LOCALHOST_ADDRESS "127.0.0.1"
#define LOCALHOST_PORT 8000

// NOTE: Action constants
#define VIEW_PLAYERS_ACTION 49
#define CREATE_PLAYER_ACTION 50
#define EDIT_PLAYER_ACTION 51
#define DELETE_PLAYER_ACTION 52
#define YOUNGEST_PLAYER_ACTION 53
#define EXIT_ACTION 54

// NOTE: Misc constants
#define RESPONSE_BUFFER_LENGTH 2048
#define INPUT_BUFFER_LENGTH 255

#define INPUT_MAX_BUTTON 54
#define INPUT_MIN_BUTTON 49
#define INPUT_EXIT_CHARACTER "*"

// NOTE: Validate request was successfully sent
bool IsRequestValid(int status, SOCKET& personalSocket)
{
	if (status == SOCKET_ERROR)
	{
		printf(SERVER_ERROR_MSG);

		return false;
	}

	return true;
}

// NOTE: Validate response was successfully received
bool IsResponseValid(int status, SOCKET& personalSocket)
{
	if (status < 0)
	{
		printf(SERVER_ERROR_MSG);

		return false;
	}

	return true;
}

// NOTE: Validate is string contain only digits
bool IsDigits(const string& str)
{
	return str.find_first_not_of("0123456789") == std::string::npos;
}

// NOTE: Try to enter only numbers
void InputNumber(string& strInput)
{
	strInput.clear();
	while (true)
	{
		cin >> strInput;

		if (!IsDigits(strInput))
		{
			printf(INCORRECT_INPUT_ERROR_MSG);
			continue;
		};

		break;
	}
}

// NOTE: Try to enter only number with exit possibility
bool InputNumberWithExit(string& strInput)
{
	while (true)
	{
		cin >> strInput;

		if (strInput.compare(INPUT_EXIT_CHARACTER) == 0)
		{
			return true;
		}

		if (!IsDigits(strInput))
		{
			printf(INCORRECT_INPUT_ERROR_MSG);
			continue;
		};

		break;
	}

	return false;
}

// NOTE: View all users action handler
void ShowUsers(SOCKET& connectSocket)
{
	// NOTE: Get players from server
	char responseBuffer[RESPONSE_BUFFER_LENGTH];
	int response = recv(connectSocket, responseBuffer, sizeof(responseBuffer), 0);

	if (!IsResponseValid(response, connectSocket))
	{
		return;
	}

	responseBuffer[response] = '\0';
	cout << endl << endl << responseBuffer << endl;
}

// NOTE: Fill player create form action handler
void FillCreatePlayerForm(string& strInput, SOCKET& connectSocket)
{
	char inputBuffer[INPUT_BUFFER_LENGTH];
	char responseBuffer[RESPONSE_BUFFER_LENGTH];

	// NOTE: Send player fullname
	printf(CREATE_PLAYER_FULLNAME_MSG);
	cin >> strInput;
	strcpy_s(inputBuffer, strInput.c_str());
	send(connectSocket, inputBuffer, strlen(inputBuffer), 0);

	// NOTE: Send player age
	printf(CREATE_PLAYER_AGE_MSG);
	InputNumber(strInput);
	strcpy_s(inputBuffer, strInput.c_str());
	send(connectSocket, inputBuffer, strlen(inputBuffer), 0);

	// NOTE: send player height
	printf(CREATE_PLAYER_HEIGHT_MSG);
	InputNumber(strInput);
	strcpy_s(inputBuffer, strInput.c_str());
	send(connectSocket, inputBuffer, strlen(inputBuffer), 0);

	// NOTE: send player weight
	printf(CREATE_PLAYER_WEIGHT_MSG);
	InputNumber(strInput);
	strcpy_s(inputBuffer, strInput.c_str());
	send(connectSocket, inputBuffer, strlen(inputBuffer), 0);

	// NOTE: Waiting for server ok result
	int response = recv(connectSocket, responseBuffer, sizeof(responseBuffer), 0);
	responseBuffer[response] = '\0';
	cout << endl << endl << responseBuffer << endl;
}

void main()
{
	WSADATA wsaData;

	// NOTE: Try to initialize WinSock library
	int initializeOutput = WSAStartup(MAKEWORD(2, 2), &wsaData);

	// NOTE: Intialize errors handling
	if (initializeOutput != 0) 
	{
		printf(WSASTARTUP_ERROR_MSG, initializeOutput);
		return;
	}

	// NOTE: Define socket with TCP protocol
	SOCKET connectSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (connectSocket == INVALID_SOCKET) 
	{
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
	if (connectionResult == SOCKET_ERROR) 
	{
		closesocket(connectSocket);
		cout << SOCKET_CONNECTION_ERROR_MSG;
		return;
	}

	cout << CONNECT_SUCCESS_MSG;

	int intInput = 0;
	string strInput;
	while (intInput != INPUT_MAX_BUTTON) 
	{
		fflush(stdout);

		// NOTE: UI
		cout << WELCOME_MSG;
		intInput = _getche();

		// NOTE: From 1 to 8
		if (intInput < INPUT_MIN_BUTTON || intInput > INPUT_MAX_BUTTON) 
		{
			cout << INCORRECT_INPUT_ERROR_MSG << endl;
			continue;
		}

		// NOTE: Exit
		if (intInput == EXIT_ACTION) 
		{
			break;
		}

		char inputBuffer[INPUT_BUFFER_LENGTH];
		
		sprintf_s(inputBuffer, "%d", intInput);
		bool isExit = false;

		// NOTE: Send choice to backend
		send(connectSocket, inputBuffer, strlen(inputBuffer), 0);
		switch(intInput)
		{
			case VIEW_PLAYERS_ACTION:
			case YOUNGEST_PLAYER_ACTION:
				ShowUsers(connectSocket);
				break;

			case CREATE_PLAYER_ACTION:
				FillCreatePlayerForm(strInput, connectSocket);

				break;
			case EDIT_PLAYER_ACTION:
				ShowUsers(connectSocket);
				cout << endl << EDIT_PLAYER_SUBMENU;

				isExit = InputNumberWithExit(strInput);

				// NOTE: Send selected id to the server
				strcpy_s(inputBuffer, strInput.c_str());
				send(connectSocket, inputBuffer, strlen(inputBuffer), 0);

				if (!isExit)
				{
					// NOTE: Fill new data
					FillCreatePlayerForm(strInput, connectSocket);
				}

				break;
			case DELETE_PLAYER_ACTION:
				ShowUsers(connectSocket);
				cout << endl << DELETE_PLAYER_SUBMENU;
				
				isExit = InputNumberWithExit(strInput);
				
				// NOTE: Send selected id to the server
				strcpy_s(inputBuffer, strInput.c_str());
				send(connectSocket, inputBuffer, strlen(inputBuffer), 0);

				if (!isExit)
				{
					// NOTE: Waiting for backend ok result
					char responseBuffer[RESPONSE_BUFFER_LENGTH];
					int response = recv(connectSocket, responseBuffer, sizeof(responseBuffer), 0);
					responseBuffer[response] = '\0';
					cout << endl << endl << responseBuffer << endl;
				}

				break;

			default:
				cout << INCORRECT_INPUT_ERROR_MSG << endl;
				break;
		}

		strInput = "";
		intInput = 0;
	}

	// NOTE: Close socket
	closesocket(connectSocket);

	// NOTE: Dispose WinSock lib
	WSACleanup();
}