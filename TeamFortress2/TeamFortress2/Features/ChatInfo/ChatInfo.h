#pragma once

#include "../../SDK/SDK.h"
#include "../../Utils/Hash/FNV1A.h"

class CChatInfo
{
public:
	void Event(CGameEvent* pEvent, const FNV1A_t uNameHash);

	std::map<int, bool> m_known_bots;
};

inline CChatInfo g_ChatInfo;