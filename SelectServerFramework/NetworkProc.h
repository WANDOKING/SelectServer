#pragma once

#include "Session.h"

void AcceptProc();
void SendProc(Session* session);
void RecvProc(Session* session);