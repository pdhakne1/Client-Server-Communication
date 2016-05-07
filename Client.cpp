
#include<stdio.h>
#include<winsock2.h>
#include<WS2tcpip.h>
#include <errno.h>


#pragma comment(lib,"ws2_32.lib") //Winsock Library
#define bufSize 4096

int main(int argc, char *argv[])
{
	WSADATA wsa;
	SOCKET s;
	char message[bufSize];
	int i, sd, winDataLen, rc;
	struct addrinfo *result = NULL,
		*ptr = NULL,
		hints;
	int iResult;

	printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Failed. Error Code : %d", WSAGetLastError());
		return 1;
	}

	printf("Initialised.\n");

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	iResult = getaddrinfo(argv[1], argv[2], &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	//Create a socket
	if ((s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		printf("Could not create socket : %d", WSAGetLastError());
	}

	printf("Socket created.\n");

	//Connect to remote server
	if (connect(s, result->ai_addr, result->ai_addrlen) < 0)
	{
		printf("connect error.\n");
		closesocket(s);
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	printf("Connected.\n");
	while (1)
	{
	ClientWrite:

		while (1)
		{
			printf("Client says:");
			i = 0;
			memset(message, 0, bufSize);
			while (1)
			{
				message[i] = getchar();
				if (message[i] == '\n') break;
				i++;
			}
			//Send some data
			sd = send(s, message, sizeof(message), 0);
			if (sd < 0)
			{
				printf("Send failed.\n");
				closesocket(s);
				freeaddrinfo(result);
				WSACleanup();
				return 1;
			}
			else if (sd == 0)
			{
				printf("Server connection closed.\n");
				closesocket(s);
				freeaddrinfo(result);
				WSACleanup();
				return 1;
			}

			winDataLen = strlen(message);
			if (strcmp(message, "end\n") == 0)
				break;


		}

		while (1)
		{
			// receive data
			rc = recv(s, message, sizeof(message), 0);
			if (rc < 0)
			{
				printf("Client reading failed.\n");
				closesocket(s);
				freeaddrinfo(result);
				WSACleanup();
				return 1;

			}
			else if (rc == 0)
			{
				printf("Server connection closed.\n");
				closesocket(s);
				freeaddrinfo(result);
				WSACleanup();
				return 1;
			}
			printf("Server says: %s\n", message);
			winDataLen = strlen(message);

			if (strcmp(message, "end\n") == 0)
			{
				goto ClientWrite;
			}

		}
	}

	closesocket(s);
	freeaddrinfo(result);
	WSACleanup();
	return 0;
}
