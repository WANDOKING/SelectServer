#include <iostream>

#include "MessageProc.h"
#include "Logger.h"

void SwitchMessage(Session* session, unsigned char messageType)
{
	switch (messageType)
	{
	case 0:
		break;
	default:
		// invalid packet received - disconnect
		wprintf(L"invalid packet type received(%d) from %s:%d\n", messageType, session->IpAddress, session->Port);
		LOGF(L"invalid packet type received(%d) from %s:%d", messageType, session->IpAddress, session->Port);
		Disconnect(session);
		break;
	}
}