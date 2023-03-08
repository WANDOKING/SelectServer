#include "Session.h"
#include "List.h"
#include "Logger.h"
#include "RingBuffer.h"

#define MAX_SESSION_COUNT FD_SETSIZE
#define EXCEPT_NOTHING -1

#define USE_SESSION_CONSOLE_LOG

using namespace mds;

list<Session*> g_sessionList;
int g_id = 0;

Session* CreateSession(SOCKET sock, SOCKADDR_IN address)
{
	Session* newSession = new Session(sock, address, g_id);
	g_id = (g_id + 1) % INT_MAX;

	g_sessionList.push_back(newSession);

#ifdef USE_SESSION_CONSOLE_LOG
	wprintf(L"session created %s:%d, session count = %zd\n", newSession->IpAddress, newSession->Port, g_sessionList.size());
#endif

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
#ifdef USE_SESSION_CONSOLE_LOG
			wprintf(L"session closed %s:%d, session count = %zd\n", (*it)->IpAddress, (*it)->Port, g_sessionList.size() - 1);
#endif

			closesocket((*it)->Socket);
			delete (*it);
			it = g_sessionList.erase(it);
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

	if (session->bToDelete == true)
	{
		return false;
	}

	bool returnValue = false;
	returnValue = session->SendBuffer.Enqueue(buffer, size);

	if (returnValue == false)
	{
		LOGF(L"Session [%s:%d][ID = %d] SendBuffer Enqueue Fail. disconnect", session->IpAddress, session->Port, session->Id);
		Disconnect(session);
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

		SendUnicast(visit, buffer, size);
	}
}