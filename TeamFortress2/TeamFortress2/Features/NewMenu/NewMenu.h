#pragma once
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_impl_dx9.h"
#include "../../SDK/SDK.h"
#include "../../SDK/DirectX/DirectX.h"
#include "lolwhat.h" // Fonts and shit
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
	//void TextCenter(std::string text);
	void Render(IDirect3DDevice9* pDevice);
};

inline CNMenu g_NewMenu;