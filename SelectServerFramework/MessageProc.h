#pragma once

#include "Session.h"

// Message �б⿡ ���� ����ȭ �� �� �޼��� ó�� �Լ� ȣ��
// NetworkRecvProc() ���ο��� ȣ���
void SwitchMessage(Session* session, unsigned char messageType);