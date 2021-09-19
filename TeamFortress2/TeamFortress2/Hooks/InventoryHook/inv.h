#pragma once
#include "..\..\SDK\SDK.h"

/*
class CTFPlayerInventory {
public:
	//CTFPlayerInventory() = delete;
};

class CTFInventoryManager {
public:
	//CTFInventoryManager() = delete;
	static CTFInventoryManager* Get();
	CTFPlayerInventory* GetPlayerInventory();
};
*/
namespace inv {
	inline SEOHook::Func func;
	inline int __fastcall hookwea(void* ecx, void* edx) {
		return 3000;
	}
}