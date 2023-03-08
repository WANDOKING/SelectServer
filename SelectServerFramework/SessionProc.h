#pragma once
#include "Session.h"

// 연결이 성공하고 세션이 생성될 때 호출됨
void CreateProc(Session* session);

// 세션이 disconnect 될 때 호출 됨
void CloseProc(Session* session);