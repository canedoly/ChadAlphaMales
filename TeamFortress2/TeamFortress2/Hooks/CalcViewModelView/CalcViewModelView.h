#pragma once
#include "../../SDK/SDK.h"
#include "../Hooks.h"

namespace CalcViewModelView {
	inline SEOHook::Func Func;
	using fn = void(__fastcall*)(void*, void*, CBaseEntity*, Vec3&, Vec3&);
	void __fastcall Hook(void* ecx, void* edx, CBaseEntity* owner, Vec3& eyePosition, Vec3& eyeAngles);
	void Init();
}