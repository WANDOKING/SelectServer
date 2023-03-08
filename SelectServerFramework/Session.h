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

	SOCKET Socket;         // 연결된 세션 소켓
	WCHAR IpAddress[16];   // IP 주소
	unsigned short Port;   // PORT 번호
	int Id;                // 세션 ID
	RingBuffer SendBuffer; // 송신 버퍼
	RingBuffer RecvBuffer; // 수신 버퍼

	// 세션의 상태 - 이 값이 true라면 세션은 disconnected 상태
	// Disconnect() 호출 시 이 값이 true로 세팅되며, DeleteDisconnectedSessions() 호출 시 세션리스트에서 제거됨
	bool bToDelete;

	/**************************** Add Additional Session Info *******************************/
};

// 세션을 만들고, 세션 리스트에 추가됨
Session* CreateSession(SOCKET sock, SOCKADDR_IN address);

// 세션 종료. 아직 리스트에서 삭제하진 않고 세션의 상태만을 변경
void Disconnect(Session* session);

// disconnected된 세션들의 소켓을 일괄 파괴하고, 리스트에서 삭제
void DeleteDisconnectedSessions();

// 유니캐스트 - 해당 세션의 sendBuffer에 Enqueue()
// 내부에서 sendBuffer.Enqueue()를 실패할 경우 로그를 남기고 Disconnect()
// 세션이 disconnected 상태이거나 Enqueue를 실패하면 false를 반환
bool SendUnicast(Session* session, char* buffer, int size);

// 브로드캐스트 -  exceptSessionId를 제외한 모든 세션에게 SendUnicast()
void SendBroadcast(int exceptSessionId, char* buffer, int size);