#pragma once

#include "../Vars.h"
#include "../../Utils/Easings/easing.h" // Don't ask why this is here :wink:

class CMenu
{
private:
	Rect_t m_LastWidget	= {};
	Rect_t m_LastGroupBox = {};

	void Separator();
	/*bool CheckBox(CVar<bool>& Var, const wchar_t* const szToolTip);
	bool ButtonTab(const wchar_t* Label, bool Active = false, int WidthOverride = 0, int HeightOverride = 0);
	bool Button(const wchar_t* Label, bool Active = false, int WidthOverride = 0, int HeightOverride = 0);
	bool ComboBox(CVar<int> &Var, const std::vector<CVar<int>> &List);
	bool InputFloat(CVar<float> &Var, float Min, float Max, float Step = 1.0f, const wchar_t* Fmt = L"%f");
	bool InputInt(CVar<int> &Var, int Min, int Max, int Step = 1);
	void ColorSpectrum(int x, int y, int w, int h);
	bool ColorPicker(Color_t& Var, const wchar_t* Label);
	bool InputColor(Color_t &Var, const wchar_t *Label);
	bool InputString(const wchar_t *Label, std::wstring &output);
	bool InputKey(CVar<int> &output, bool bAllowNone = true);
	int ScaleDPI(int i);
	void GroupBoxStart();
	void GroupBoxEnd(const wchar_t *Label, int Width);
	bool MouseInRegion(const int x, const int y, const int w, const int h);*/
	//void DrawTooltip();

private:
	bool m_bReopened = false;
	std::wstring m_szCurTip = L"";

public:
	float m_flFadeAlpha		= 0.0f;
	float m_flFadeElapsed	= 0.0f;
	float m_flFadeDuration	= 0.1f;
	float flTimeOnChange = 0.0f;
	Vec2 m_vSize, m_vPos;
	Vec2 m_vOldGroupboxPos;
	bool m_bInitialized;
	std::vector<std::string> m_vTabs;
	bool m_bDisableBound = false;
	int m_iColumn = 0;
	int m_iOldGroupboxY = 0;
	bool m_bCloseBoxes = false;
	Vec2 m_vOffset;

	bool m_bOpen = false;
	bool m_bTyping = false;
	void Run();
};

inline CMenu g_Menu;