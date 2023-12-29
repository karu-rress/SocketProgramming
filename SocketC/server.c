#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <conio.h>
#include <WinSock2.h>
#include <time.h>
#include <stdlib.h>

#pragma comment(lib, "Ws2_32.lib")

#define BUF_SIZE 4096
#define PORT_NO 10000

typedef struct {
	time_t sent;
	int length;
	char *message;
} msg_t;

void Error(const char *szMessage);

int main(void) {
	SOCKET s; // server
	SOCKET cs; // client
	SOCKADDR_IN server; // socket struct
	SOCKADDR_IN client;
	WSADATA wsaData;
	HANDLE thread;
	DWORD threadID;

	int size;
	int num;
	time_t sent;
	struct tm tm_sent;

	msg_t message;

	WSAStartup(MAKEWORD(2, 2), &wsaData);

	if ((s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
		Error("socket");

	server.sin_family = AF_INET;
	server.sin_port = htons(PORT_NO);
	server.sin_addr.s_addr = htonl(ADDR_ANY);

	if (bind(s, (SOCKADDR *)&server, sizeof(server)) == SOCKET_ERROR)
		Error("bind");

	if (listen(s, SOMAXCONN) != 0)
		Error("listen");

	puts("Waiting for client...");

	size = sizeof(client);
	if ((cs = accept(s, (SOCKADDR *)&client, &size)) == INVALID_SOCKET)
		Error("accept");

	while (1) {
		num = recv(cs, (char *)&message, sizeof(message.sent) + sizeof(message.length), 0);
		if (num == 0 || num == SOCKET_ERROR)
			break;
	
		message.message = (char *)malloc(message.length + 1);
		num = recv(cs, message.message, message.length, 0);
		if (num == 0 || num == SOCKET_ERROR)
			break;
		message.message[message.length] = '\0';

		tm_sent = *localtime(&message.sent);
		printf("[%02d:%02d] %s\n", tm_sent.tm_hour, tm_sent.tm_min, message.message);
		free(message.message);
	}

	closesocket(s);
	closesocket(cs);
	WSACleanup();

	return 0;
}

void Error(char *szMessage)
{
	printf("Error: [%d] %s \n", WSAGetLastError(), szMessage);
	WSACleanup();
	exit(0);
}