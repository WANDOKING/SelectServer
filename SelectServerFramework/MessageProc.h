#pragma once

#include "Session.h"

// Message 분기에 따른 직렬화 및 각 메세지 처리 함수 호출
// NetworkRecvProc() 내부에서 호출됨
void SwitchMessage(Session* session, unsigned char messageType);