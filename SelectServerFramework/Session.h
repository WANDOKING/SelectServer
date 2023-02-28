#pragma once

#include <WS2tcpip.h>

#include "RingBuffer.h"

struct Session
{
	Session(SOCKET sock, SOCKADDR_IN address, int id)
	{
		Socket = sock;
		Address = address;
		Id = id;
		bToDelete = false;
	}

	SOCKET Socket;
	SOCKADDR_IN Address;
	int Id;
	RingBuffer SendBuffer;
	RingBuffer RecvBuffer;
	bool bToDelete;
	// Add User Info
};

// 세션을 만들고, 세션 리스트에 추가됨
Session* CreateSession(SOCKET sock, SOCKADDR_IN address);

//void Disconnect(int sessionId);
void Disconnect(Session* session);
void DeleteDisconnectedSessions();

//bool SendUnicast(int sessionId, char* buffer, int size);
bool SendUnicast(Session* session, char* buffer, int size);
void SendBroadcast(int exceptSessionId, char* buffer, int size);