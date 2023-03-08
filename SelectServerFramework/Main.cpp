﻿#pragma comment(lib, "ws2_32")

#include <iostream>
#include <wchar.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

#include "List.h"
#include "Session.h"
#include "Logger.h"
#include "NetworkProc.h"

#pragma region WSA_STARTUP

#define WSA_STARTUP()                          \
do                                             \
{                                              \
	WSADATA wsa;                               \
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) \
	{                                          \
		return 1;                              \
	}                                          \
} while(false)                                 \

#define WSA_CLEANUP() WSACleanup()

#pragma endregion

#define SERVER_PORT 5000 // server port number

using namespace mds;

extern SOCKET g_listenSocket;
extern list<Session*> g_sessionList; // session list

int main(void)
{
	WSA_STARTUP();

	SOCKADDR_IN serverAddress;

	int retIoctlsocket;
	int retBind;
	int retListen;
	int retSelect;

	// socket()
	g_listenSocket = socket(AF_INET, SOCK_STREAM, 0);
	ASSERT_WITH_MESSAGE(g_listenSocket != INVALID_SOCKET, L"listenSocket socket() Error");

	// ioctlsocket()
	u_long on = 1;
	retIoctlsocket = ioctlsocket(g_listenSocket, FIONBIO, &on);
	ASSERT_WITH_MESSAGE(retIoctlsocket != SOCKET_ERROR, L"listenSocket ioctlsocket() Error");

	// g_serverAddress initialize
	ZeroMemory(&serverAddress, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddress.sin_port = htons(SERVER_PORT);

	// bind()
	retBind = bind(g_listenSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress));
	ASSERT_WITH_MESSAGE(retBind != SOCKET_ERROR, L"listenSocket bind() Error");

	// listen()
	retListen = listen(g_listenSocket, SOMAXCONN);
	ASSERT_WITH_MESSAGE(retListen != SOCKET_ERROR, L"listenSocket listen() Error");

	/*********************************** loop start ***********************************/
	wprintf(L"Server Start\n");

	FD_SET readSet;
	FD_SET writeSet;

	for (;;)
	{
		// FD_ZERO()
		FD_ZERO(&readSet);
		FD_ZERO(&writeSet);

		// FD_SET()
		FD_SET(g_listenSocket, &readSet);

		for (list<Session*>::iterator it = g_sessionList.begin(); it != g_sessionList.end(); ++it)
		{
			Session* visit = *it;

			FD_SET(visit->Socket, &readSet);

			if (visit->SendBuffer.GetUseSize() > 0)
			{
				FD_SET(visit->Socket, &writeSet);
			}
		}

		// select()
		TIMEVAL timeout;
		timeout.tv_sec = 0;
		timeout.tv_usec = 0;
		retSelect = select(0, &readSet, &writeSet, nullptr, &timeout);
		ASSERT_WITH_MESSAGE(retSelect != SOCKET_ERROR, L"select() Error");

		if (retSelect > 0)
		{
			if (FD_ISSET(g_listenSocket, &readSet))
			{
				AcceptProc();
			}

			for (list<Session*>::iterator it = g_sessionList.begin(); it != g_sessionList.end(); ++it)
			{
				Session* visit = *it;

				if (FD_ISSET(visit->Socket, &readSet))
				{
					RecvProc(visit);
				}

				if (FD_ISSET(visit->Socket, &writeSet))
				{
					SendProc(visit);
				}
			}
		}

		// DeleteDisconnectedSessions()
		DeleteDisconnectedSessions();
	}

	WSA_CLEANUP();
}