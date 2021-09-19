#include "NetSendHook.h"

void NetSendHook::Init()
{
	fn FN = reinterpret_cast<fn>(Utils::GetVFuncPtr(reinterpret_cast<void**>(g_dwDirectXDevice), 42));
	Hook.Create(reinterpret_cast<void*>(FN), reinterpret_cast<void*>(Func));
}

void __stdcall NetSendHook::Hook(bool verbose, SOCKET* s, int buf, int len, int to, int tolen)
{
	Table.Original<fn>(index)(g_Interfaces.ClientMode, pView);
}
