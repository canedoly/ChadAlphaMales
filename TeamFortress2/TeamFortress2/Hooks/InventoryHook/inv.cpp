#include "inv.h"
/*
CTFInventoryManager* CTFInventoryManager::Get() {
	typedef CTFInventoryManager* (__thiscall* FN)();	// define los punteros tipo ctfinvmanager como ???
	//static DWORD dwFn = g_Pattern.Find(L"client.dll", L"E8 ? ? ? ? 8B 4F 38") + 0x1;
	static DWORD dwFn = (reinterpret_cast<DWORD>(GetModuleHandleW(L"client.dll")) + 0x306900ul + 0x1ul); // client.dll+0x00306900
	static DWORD dwEstimate;
	dwEstimate = *(PDWORD)dwFn;
	dwEstimate = dwEstimate + dwFn;
	dwEstimate = dwEstimate + 0x4;
	FN getInventoryManager = (FN)dwEstimate;	// definir getinvmanaer como el casteo de una direccion
	auto wea = getInventoryManager();	//	llamar al procedimiento
	return wea;	// retornar un puntero
}

CTFPlayerInventory* CTFInventoryManager::GetPlayerInventory() {
	return (CTFPlayerInventory*)(this + 0x108);	// castea this+108 a playerinv (???)
}
*/