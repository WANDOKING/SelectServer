#include "List.h"
#include "Logger.h"
#include "MessageProc.h"
#include "NetworkProc.h"
#include "RingBuffer.h"


// 최대 세션 개수 (최대 동시 접속자 수 제한)
#define MAX_SESSION_COUNT FD_SETSIZE

using namespace mds;

SOCKET g_listenSocket;
extern list<Session*> g_sessionList;

void NetworkAcceptProc()
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

		// 세션 생성
		CreateSession(acceptSocket, clientAddress);
	}
}

void NetworkSendProc(Session* session)
{
	char temp[RingBuffer::DEFAULT_SIZE];
	int retSend;
	int useSize = session->SendBuffer.GetUseSize();

	if (useSize == 0)
	{
		return;
	}

	if (session->SendBuffer.GetDirectDequeueSize() >= useSize)
	{
		retSend = send(session->Socket, session->SendBuffer.GetFrontBufferPtr(), useSize, 0);
	}
	else
	{
		session->SendBuffer.Peek(temp, useSize);
		retSend = send(session->Socket, temp, useSize, 0);
	}

	if (retSend == SOCKET_ERROR)
	{
		int errorCode = WSAGetLastError();

		switch (errorCode)
		{
		case WSAEWOULDBLOCK:
			retSend = 0;
			break;
		case WSAECONNRESET:
			Disconnect(session);
			return;
			break;
		default:
			LOG_WITH_WSAERROR(L"send() error");
			CRASH();
		}
	}

	session->SendBuffer.MoveFront(retSend);
}

void NetworkRecvProc(Session* session)
{
	int retRecv;

	int freeSize = session->RecvBuffer.GetFreeSize();
	char temp[RingBuffer::DEFAULT_SIZE];

	// 1. recv()
	retRecv = recv(session->Socket, temp, sizeof(temp), 0);
	if (retRecv == SOCKET_ERROR)
	{
		int errorCode = WSAGetLastError();
		if (errorCode == WSAECONNRESET)
		{
			Disconnect(session);
			return;
		}
		else
		{
			LOG_WITH_WSAERROR(L"recv() error");
			CRASH();
		}
	}
	else if (retRecv == 0)
	{
		Disconnect(session);
		return;
	}
	else if (retRecv > freeSize)
	{
		LOGF(L"Id:%-3d - RecvBuffer full", session->Id);
		Disconnect(session);
		return;
	}

	// 2. message process
	session->RecvBuffer.Enqueue(temp, retRecv);

	while (session->RecvBuffer.GetUseSize() >= sizeof(P_HEADER))
	{
		if (session->bToDelete == true)
		{
			break;
		}

		P_HEADER packetHeader;
		session->RecvBuffer.Peek((char*)&packetHeader, sizeof(P_HEADER));

		if (packetHeader.Code != P_HEADER::PACKET_CODE)
		{
			wprintf(L"invalid packet header received from %s:%d\n", session->IpAddress, session->Port);
			LOGF(L"invalid packet header received from %s:%d", session->IpAddress, session->Port);
			Disconnect(session);
			break;
		}

		if (session->RecvBuffer.GetUseSize() < sizeof(P_HEADER) + packetHeader.Size)
		{
			break;
		}

		SwitchMessage(session, packetHeader.Type);
	}
}