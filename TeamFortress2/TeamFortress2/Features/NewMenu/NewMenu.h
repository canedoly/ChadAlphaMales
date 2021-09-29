#pragma once
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_impl_dx9.h"
#include "ImGui/imgui_stdlib.h"
#include "ImGui/imgui_internal.h"
#include "../../SDK/SDK.h"
#include "../../SDK/DirectX/DirectX.h"
#include "Fonts.h" // Fonts and shit

class CNMenu
{
private:
	ImFont* name;
	ImFont* icons;
	ImFont* font;
	ImDrawList* drawList;
public:
	bool menuOpen = false;
	bool menuUnload = false;
	void updateMenuFont(OPENFILENAME* filename);
	void Render(IDirect3DDevice9* pDevice);
};

inline CNMenu g_NewMenu;