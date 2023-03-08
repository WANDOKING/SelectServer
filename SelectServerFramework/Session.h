#pragma once

#include <WS2tcpip.h>

#include "RingBuffer.h"

struct Session
{
	Session(SOCKET sock, SOCKADDR_IN address, int id)
		: Socket(sock)
		, Id(id)
		, bToDelete(false)
	{
		memset(IpAddress, 0, sizeof(IpAddress));
		InetNtopW(AF_INET, &address.sin_addr.s_addr, IpAddress, 16);
		Port = ntohs(address.sin_port);
	}

	SOCKET Socket;         // ����� ���� ����
	WCHAR IpAddress[16];   // IP �ּ�
	unsigned short Port;   // PORT ��ȣ
	int Id;                // ���� ID
	RingBuffer SendBuffer; // �۽� ����
	RingBuffer RecvBuffer; // ���� ����

	// ������ ���� - �� ���� true��� ������ disconnected ����
	// Disconnect() ȣ�� �� �� ���� true�� ���õǸ�, DeleteDisconnectedSessions() ȣ�� �� ���Ǹ���Ʈ���� ���ŵ�
	bool bToDelete;

	/**************************** Add Additional Session Info *******************************/
};

// ������ �����, ���� ����Ʈ�� �߰���
Session* CreateSession(SOCKET sock, SOCKADDR_IN address);

// ���� ����. ���� ����Ʈ���� �������� �ʰ� ������ ���¸��� ����
void Disconnect(Session* session);

// disconnected�� ���ǵ��� ������ �ϰ� �ı��ϰ�, ����Ʈ���� ����
void DeleteDisconnectedSessions();

// ����ĳ��Ʈ - �ش� ������ sendBuffer�� Enqueue()
// ���ο��� sendBuffer.Enqueue()�� ������ ��� �α׸� ����� Disconnect()
// ������ disconnected �����̰ų� Enqueue�� �����ϸ� false�� ��ȯ
bool SendUnicast(Session* session, char* buffer, int size);

// ��ε�ĳ��Ʈ -  exceptSessionId�� ������ ��� ���ǿ��� SendUnicast()
void SendBroadcast(int exceptSessionId, char* buffer, int size);