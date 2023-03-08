#pragma once

#include "Session.h"

struct P_HEADER
{
	enum
	{
		PACKET_CODE = 0x89,
	};

	unsigned char Code = PACKET_CODE;
	unsigned char Size = 0x00;
	unsigned char Type = 0x00;
};

static_assert(sizeof(P_HEADER) == 3, "P_HEADER SIZE");

void NetworkAcceptProc();

void NetworkSendProc(Session* session);
void NetworkRecvProc(Session* session);