#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <winsock2.h>
#include <iostream>
#include <string>

#pragma comment(lib,"Ws2_32.lib")
using namespace std;

// NOTE: Error msgs
const char WSASTARTUP_ERROR_MSG[255] = "WSAStartup failed with error: %d\n";
const char SOCKET_CREATING_ERROR_MSG[255] = "socket failed with error: %ld\n";
const char SOCKET_BINDING_ERROR_MSG[255] = "socket binding failed with error: %ld\n";
const char REQUEST_PROCESSING_ERROR_MSG[255] = "Error in handling request: action - %d, status: %ld, client - %s:%d\n";
const char RESPONSE_PROCESSING_ERROR_MSG[255] = "Error in handling response: action - %d, status: %ld, client - %s:%d\n";

// NOTE: Info msgs
const char NEW_CLIENT_MSG[255] = "%d clients connected\n";
const char SERVER_LISTENING_MSG[255] = "Server is listening for new connections\n";
const char SERVER_GETLIST_INFO_MSG[255] = "Server sended players list to %s:%d\n";
const char SERVER_GETYOUNGEST_INFO_MSG[255] = "Server sended youngest player to %s:%d\n";

// NOTE: Logic msgs
const char PLAYERS_LIST_HEADER[255] = "Players list: \n";
const char YOUNGEST_PLAYER_HEADER[255] = "Youngest player: \n";


#define LOCALHOST_PORT 8000

#define GET_MENU_ACTION 0
#define VIEW_PLAYERS_ACTION 49
#define CREATE_PLAYER_ACTION 50
#define EDIT_PLAYER_ACTION 51
#define DELETE_PLAYER_ACTION 52
#define YOUNGEST_PLAYER_ACTION 53
#define EXIT_ACTION 54


int playersCount = 0;
struct Player
{
	int Id;
	char Fullname[255];
	int Age;
	int Height;
	int Weight;
} players[255];

struct ConnectionInfo
{
	SOCKET socket;
	sockaddr_in addr;
};

void SeedDatabase()
{
	players[playersCount].Id = playersCount;
	strcpy(players[playersCount].Fullname, "Trevon Park");
	players[playersCount].Age = 20;
	players[playersCount].Height = 188;
	players[playersCount].Weight = 75;
	playersCount++;

	players[playersCount].Id = playersCount;
	strcpy(players[playersCount].Fullname, "Alex Chan");
	players[playersCount].Age = 22;
	players[playersCount].Height = 184;
	players[playersCount].Weight = 72;
	playersCount++;

	players[playersCount].Id = playersCount;
	strcpy(players[playersCount].Fullname, "Albert Mixon");
	players[playersCount].Age = 26;
	players[playersCount].Height = 189;
	players[playersCount].Weight = 80;
	playersCount++;

	players[playersCount].Id = playersCount;
	strcpy(players[playersCount].Fullname, "Donald Hobbs");
	players[playersCount].Age = 23;
	players[playersCount].Height = 180;
	players[playersCount].Weight = 76;
	playersCount++;

	players[playersCount].Id = playersCount;
	strcpy(players[playersCount].Fullname, "Patrick Sorrels");
	players[playersCount].Age = 21;
	players[playersCount].Height = 182;
	players[playersCount].Weight = 79;
	playersCount++;
}

bool IsRequestValid(int status, int action, sockaddr_in addr)
{
	if (status == SOCKET_ERROR)
	{
		int iError = WSAGetLastError();
		printf(REQUEST_PROCESSING_ERROR_MSG, action, iError, inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));

		return false;
	}

	return true;
}

bool IsResponseValid(int status, int action, sockaddr_in addr)
{
	if (status < 0)
	{
		int iError = WSAGetLastError();
		printf(RESPONSE_PROCESSING_ERROR_MSG, action, iError, inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));

		return false;
	}

	return true;
}

void HandleViewPlayersAction(SOCKET socket, sockaddr_in addr)
{
	string output(PLAYERS_LIST_HEADER);

	char responseBuffer[2048];

	for (int i = 0; i < playersCount; i++)
	{
		output.append("Id: ").append(to_string(players[i].Id)).append("\n");
		output.append("Fullname: ").append(players[i].Fullname).append("\n");
		output.append("Age: ").append(to_string(players[i].Age)).append("\n");
		output.append("Height: ").append(to_string(players[i].Height)).append("\n");
		output.append("Weight: ").append(to_string(players[i].Weight)).append("\n");
	}

	strcpy_s(responseBuffer, output.c_str());

	int responseSendingStatus = send(socket, responseBuffer, strlen(responseBuffer), 0);
	if (IsRequestValid(responseSendingStatus, VIEW_PLAYERS_ACTION, addr))
	{
		printf(SERVER_GETLIST_INFO_MSG, inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
	}
}

void HandleCreatePlayerAction(SOCKET socket, sockaddr_in addr)
{
	return;
}

void HandleEditPlayerAction(SOCKET socket, sockaddr_in addr)
{
	return;
}

void HandleDeletePlayerAction(SOCKET socket, sockaddr_in addr)
{
	return;
}

void HandleYoungestPlayerAction(SOCKET socket, sockaddr_in addr)
{
	string output(YOUNGEST_PLAYER_HEADER);
	char responseBuffer[2048];
	int youngestPlayerIndex = 0;

	for (int i = 0; i < playersCount; i++)
	{
		if (players[youngestPlayerIndex].Age > players[i].Age)
		{
			youngestPlayerIndex = i;
		}
	}

	output.append("Id: ").append(to_string(players[youngestPlayerIndex].Id)).append("\n");
	output.append("Fullname: ").append(players[youngestPlayerIndex].Fullname).append("\n");
	output.append("Age: ").append(to_string(players[youngestPlayerIndex].Age)).append("\n");
	output.append("Height: ").append(to_string(players[youngestPlayerIndex].Height)).append("\n");
	output.append("Weight: ").append(to_string(players[youngestPlayerIndex].Weight)).append("\n");

	strcpy_s(responseBuffer, output.c_str());

	int responseSendingStatus = send(socket, responseBuffer, strlen(responseBuffer), 0);
	if (IsRequestValid(responseSendingStatus, YOUNGEST_PLAYER_ACTION, addr))
	{
		printf(SERVER_GETYOUNGEST_INFO_MSG, inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
	}
}

DWORD WINAPI ThreadFunc(LPVOID connectionInfo)
{
	ConnectionInfo connection = ((ConnectionInfo*)connectionInfo)[0];

	SOCKET personalSocket = connection.socket;
	sockaddr_in addr = connection.addr;

	char responseBuffer[1024];
	char requestBuffer[1024];

	while (true)
	{
		int responseResult = recv(personalSocket, responseBuffer, sizeof(responseBuffer), 0);
		if (!IsResponseValid(responseResult, GET_MENU_ACTION, addr))
		{
			closesocket(personalSocket);
			return 0;
		}
		
		int choice = atoi(responseBuffer);

		if (choice == EXIT_ACTION)
		{
			break;
		}

		switch (choice)
		{
			case VIEW_PLAYERS_ACTION:
				HandleViewPlayersAction(personalSocket, addr);
				break;
			case CREATE_PLAYER_ACTION:
				HandleCreatePlayerAction(personalSocket, addr);
				break;
			case EDIT_PLAYER_ACTION:
				HandleEditPlayerAction(personalSocket, addr);
				break;
			case DELETE_PLAYER_ACTION:
				HandleDeletePlayerAction(personalSocket, addr);
				break;
			case YOUNGEST_PLAYER_ACTION:
				HandleYoungestPlayerAction(personalSocket, addr);
				break;
			default:
				break;
		}
	}
	
	closesocket(personalSocket);
	return 0;
}

void main()
{
	// NOTE: Seed initial array with players
	SeedDatabase();

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
	ad.sin_addr.s_addr = 0;
	int adSize = sizeof(ad);

	// NOTE: Bind params to socket
	int bindingResult = bind(connectSocket, (struct sockaddr*)&ad, sizeof(ad));
	if (bindingResult == SOCKET_ERROR)
	{
		printf(SOCKET_BINDING_ERROR_MSG, WSAGetLastError());
		WSACleanup();
		return;
	}


	// NOTE: Start to listen requests
	listen(connectSocket, 5);
	printf(SERVER_LISTENING_MSG);

	// NOTE: Socket for client
	SOCKET client_socket; 
	sockaddr_in client_addr; 
	int client_addr_size = sizeof(client_addr);
	
	int connectedClients = 0;

	// NOTE: Receive new clients
	while ((client_socket = accept(connectSocket, (sockaddr*)&client_addr, &client_addr_size))) 
	{
		connectedClients++;
		printf(NEW_CLIENT_MSG, connectedClients);

		// NOTE: Handle client in new thread
		DWORD thID;
		
		ConnectionInfo connectionInfo;
		connectionInfo.socket = client_socket;
		connectionInfo.addr = client_addr;

		CreateThread(NULL, NULL, ThreadFunc, &connectionInfo, NULL, &thID);
	}
}

