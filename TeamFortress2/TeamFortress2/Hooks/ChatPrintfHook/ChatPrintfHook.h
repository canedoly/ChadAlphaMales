#pragma once
#include "../../SDK/SDK.h"
#include "../Hooks.h"

namespace ChatPrintfHook {
	inline SEOHook::Func Func;
	using fn = void(__fastcall*)(void* , void* , int, int, const char*, ...);
	void __fastcall Hook(void* ecx, void* edx, int nIndex, int nFilter, const char* fmt, ...);
	void Init();
}