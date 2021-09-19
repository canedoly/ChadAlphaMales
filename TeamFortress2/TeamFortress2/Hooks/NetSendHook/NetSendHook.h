#pragma once

#include "../../SDK/SDK.h"
#include "../Hooks.h"
#include "../../Utils/MinHook/hook.h"
#include "../../Features/NewMenu/NewMenu.h"

namespace NetSendHook
{
	inline CHook Hook;

	using fn = void(__cdecl*)(bool, SOCKET, int, int, int, int);
	void __cdecl Hook(bool verbose, SOCKET* s, int buf, int len, int to, int tolen);
}