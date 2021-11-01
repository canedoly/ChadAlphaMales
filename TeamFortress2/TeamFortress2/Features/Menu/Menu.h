#pragma once
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_impl_dx9.h"
#include "ImGui/imgui_stdlib.h"
#include "ImGui/imgui_internal.h"
#include "../../SDK/SDK.h"
#include "../../SDK/DirectX/DirectX.h"
#include "Fonts.h" // Fonts and shit

class CMenu
{
private:
	ImFont* name;
	ImFont* icons;
	ImFont* font;
	ImDrawList* drawList;
public:
	float m_flFadeElapsed = 0.0f;
	float m_flFadeAlpha = 0.0f;
	float flTimeOnChange = 0.0f;
	float m_flFadeDuration = 0.1f;

	bool menuOpen = false;
	bool menuUnload = false;
	void Render(IDirect3DDevice9* pDevice);
};

inline CMenu g_Menu;