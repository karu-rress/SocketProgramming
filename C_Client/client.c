#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <conio.h>
#include <WinSock2.h>
#include <string.h>
#include <time.h>
#include <Windows.h>

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
	SOCKADDR_IN client;
	WSADATA wsaData;

	msg_t msg;
	char buf[BUF_SIZE];

	WSAStartup(MAKEWORD(2, 2), &wsaData);

	if ((s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
		Error("socket");

	client.sin_family = AF_INET;
	client.sin_port = htons(PORT_NO);
	client.sin_addr.s_addr = inet_addr("127.0.0.1");

	if (connect(s, (SOCKADDR *)&client, sizeof(client)) != 0)
		Error("connect");

	while (1) {
		printf("Input message >> ");
		if (gets_s(buf, sizeof buf) == NULL) // ^Z
			break;

		msg.sent = time(NULL);
		msg.length = strlen(buf);

		send(s, (char *)&msg, sizeof msg.sent + sizeof msg.length, 0);
		send(s, buf, msg.length, 0);
	}

	closesocket(s);
	WSACleanup();

	return 0;
}

void Error(char *szMessage)
{
	printf("Error: [%d] %s \n", WSAGetLastError(), szMessage);
	WSACleanup();
	exit(0);
}