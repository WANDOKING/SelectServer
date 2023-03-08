#include "Session.h"
#include "List.h"
#include "Logger.h"
#include "RingBuffer.h"

#define MAX_SESSION_COUNT FD_SETSIZE

using namespace mds;

SOCKET g_listenSocket;
extern list<Session*> g_sessionList;

void AcceptProc()
{
	LINGER optval;
	int retSetsockopt;

	SOCKADDR_IN clientAddress;
	int addressLength = sizeof(clientAddress);

	for (;;)
	{
		SOCKET acceptSocket = accept(g_listenSocket, (SOCKADDR*)&clientAddress, &addressLength);
		if (acceptSocket == INVALID_SOCKET)
		{
			int WSAErrorCode = WSAGetLastError();
			if (WSAErrorCode == WSAEWOULDBLOCK)
			{
				break;
			}
			else
			{
				LOG_WITH_WSAERROR(L"listenSocket accept() Error");
				CRASH();
			}
		}

		// SO_LINGER
		optval.l_onoff = 1;
		optval.l_linger = 0;
		retSetsockopt = setsockopt(acceptSocket, SOL_SOCKET, SO_LINGER, (const char*)&optval, sizeof(optval));
		if (retSetsockopt == SOCKET_ERROR)
		{
			LOG_WITH_WSAERROR(L"accepted socket setsockopt() error");
			CRASH();
		}

		// MAX_SESSION_COUNT check
		if (g_sessionList.size() >= MAX_SESSION_COUNT)
		{
			WCHAR acceptedClientIpAddress[16];
			InetNtopW(AF_INET, &clientAddress, acceptedClientIpAddress, 16);
			LOGF(L"%s:%d client disconnected : g_sessionCount >= MAX_SESSION_COUNT", acceptedClientIpAddress, ntohs(clientAddress.sin_port));
			closesocket(acceptSocket);
			break;
		}

		// 0. 리스트에 세션 정보 추가
		Session* createdSession = CreateSession(acceptSocket, clientAddress);
	}
}

void SendProc(Session* session)
{
	char temp[RingBuffer::DEFAULT_SIZE];
	int retSend;
	int useSize = session->SendBuffer.GetUseSize();

	if (useSize == 0)
	{
		return;
	}

	session->SendBuffer.Dequeue(temp, useSize);

	retSend = send(session->Socket, temp, useSize, 0);
	if (retSend == SOCKET_ERROR)
	{
		int errorCode = WSAGetLastError();

		if (errorCode == WSAECONNRESET)
		{
			Disconnect(session);
		}
		else
		{
			LOG_WITH_WSAERROR(L"send() error");
			CRASH();
		}
	}
}

void RecvProc(Session* session)
{
	const int MESSAGE_SIZE = 16;
	const int MESSAGE_HEADER_SIZE = 4;

	int retRecv;
	char messageBuffer[MESSAGE_SIZE];
	 
	int freeSize = session->RecvBuffer.GetFreeSize();
	char temp[RingBuffer::DEFAULT_SIZE];

	// 1. recv()
	bool bToDisconnect = false;
	retRecv = recv(session->Socket, temp, sizeof(temp), 0);
	if (retRecv == SOCKET_ERROR)
	{
		int errorCode = WSAGetLastError();
		if (errorCode == WSAECONNRESET)
		{
			bToDisconnect = true;
		}
		else
		{
			LOG_WITH_WSAERROR(L"recv() error");
			CRASH();
		}
	}
	else if (retRecv == 0)
	{
		bToDisconnect = true;
	}
	else if (retRecv > freeSize)
	{
		LOG(L"RecvBuffer 부족");
		bToDisconnect = true;
	}

	if (bToDisconnect)
	{
		Disconnect(session);
		return;
	}

	// 2. message process
	session->RecvBuffer.Enqueue(temp, retRecv);

	while (session->RecvBuffer.GetUseSize() >= MESSAGE_SIZE)
	{
		session->RecvBuffer.Peek(messageBuffer, MESSAGE_HEADER_SIZE);
	}
}