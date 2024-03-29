#pragma once
#include "../../SDK/SDK.h"
#include "../Hooks.h"
#include "../../SDK/DirectX/DirectX.h"
#include "../../Features/Menu/Menu.h"

namespace ResetHook {
	using fn = HRESULT(__stdcall*)(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);
	HRESULT __stdcall Func(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentParams);
	void Init();
}