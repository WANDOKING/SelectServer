#include "Session.h"
#include "List.h"
#include "Logger.h"
#include "RingBuffer.h"

#define MAX_SESSION_COUNT FD_SETSIZE

using namespace mds;

list<Session*> g_sessionList;
int g_sessionCount = 0;
int g_id = 0;

Session* CreateSession(SOCKET sock, SOCKADDR_IN address)
{
	Session* newSession = new Session(sock, address, g_id);
	g_id = (g_id + 1) % INT_MAX;

	g_sessionList.push_back(newSession);
	g_sessionCount++;

	return newSession;
}

void Disconnect(Session* session)
{
	if (session == nullptr)
	{
		return;
	}

	session->bToDelete = true;
}

void DeleteDisconnectedSessions()
{
	for (list<Session*>::iterator it = g_sessionList.begin(); it != g_sessionList.end(); )
	{
		if ((*it)->bToDelete)
		{
			closesocket((*it)->Socket);
			delete (*it);
			it = g_sessionList.erase(it);
			g_sessionCount--;
		}
		else
		{
			++it;
		}
	}
}

bool SendUnicast(Session* session, char* buffer, int size)
{
	ASSERT_WITH_MESSAGE(session != nullptr, L"SendUnicast() - session is nullptr");

	bool returnValue = false;
	returnValue = session->SendBuffer.Enqueue(buffer, size);
	if (returnValue == false)
	{
		LOG(L"SendBuffer Enqueue Fail");
	}

	return returnValue;
}

void SendBroadcast(int exceptSessionId, char* buffer, int size)
{
	for (list<Session*>::iterator it = g_sessionList.begin(); it != g_sessionList.end(); ++it)
	{
		Session* visit = *it;

		if (visit->Id == exceptSessionId)
		{
			continue;
		}

		if (visit->bToDelete == true)
		{
			continue;
		}

		SendUnicast(visit, buffer, size);
	}
}