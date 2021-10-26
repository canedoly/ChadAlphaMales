#include "Menu.h"

#include "InputHelper/InputHelper.h"
#include "ConfigManager/ConfigManager.h"
#include "../AttributeChanger/AttributeChanger.h"

bool CMenu::MouseInRegion(const int x, const int y, const int w, const int h)
{
	return g_InputHelper.m_nMouseX > x && g_InputHelper.m_nMouseY > y && g_InputHelper.m_nMouseX < w + x && g_InputHelper.m_nMouseY < h + y;
}
/*
void CMenu::DrawTooltip()
{
	if (m_szCurTip.empty() || !Vars::Visuals::ToolTips.m_Var)
		return;

	int w, h;
	g_Interfaces.Surface->GetTextSize(g_Draw.m_vecFonts[FONT_MENU].dwFont, m_szCurTip.data(), w, h);

	g_Draw.OutlinedRect(g_InputHelper.m_nMouseX + 13, g_InputHelper.m_nMouseY + 13, w + 4, h + 4, { 0, 0, 0, 255 });
	g_Draw.Rect(g_InputHelper.m_nMouseX + 14, g_InputHelper.m_nMouseY + 14, w + 2, h + 2, Vars::Menu::Colors::WindowBackground);
	g_Draw.String(FONT_MENU, g_InputHelper.m_nMouseX + 15, g_InputHelper.m_nMouseY + 15, Vars::Menu::Colors::Text, ALIGN_DEFAULT, m_szCurTip.data());
}
*/
void CMenu::Separator()
{
	int x = m_LastWidget.x + m_LastWidget.w + Vars::Menu::SpacingX;
	int y = Vars::Menu::Position.y;
	int w = 0;
	int h = 0;

	//g_Draw.Line(x, y, x, (y + Vars::Menu::Position.h - 1), Vars::Menu::Colors::OutlineMenu);

	m_LastWidget.x = x + Vars::Menu::SpacingX;
	m_LastWidget.y = y;
	m_LastWidget.w = w;
	m_LastWidget.h = h;
}


void CMenu::GroupBoxStart()
{
	m_LastGroupBox.x = m_LastWidget.x;
	m_LastGroupBox.y = m_LastWidget.y + m_LastWidget.h + Vars::Menu::SpacingY;

	m_LastWidget.x += Vars::Menu::SpacingX;
	m_LastWidget.y += Vars::Menu::SpacingY * 2;
}

void CMenu::GroupBoxEnd(const wchar_t* Label, int Width)
{
	int h = m_LastWidget.y - m_LastGroupBox.y + m_LastWidget.h + Vars::Menu::SpacingY;

	int label_w, label_h;
	g_Interfaces.Surface->GetTextSize(g_Draw.m_vecFonts[FONT_MENU].dwFont, Label, label_w, label_h);

	int label_x = m_LastGroupBox.x + Vars::Menu::SpacingText;
	int label_y = m_LastGroupBox.y - (label_h / 2);

	g_Draw.OutlinedRect(m_LastGroupBox.x - 1, m_LastGroupBox.y - 1, Width + 4, h + 4, { 0,0,0,255 });
	g_Draw.OutlinedRect(m_LastGroupBox.x, m_LastGroupBox.y, Width + 2, h + 2, { 50,50,50,255 });

	g_Draw.Rect(label_x - 2, label_y, label_w + 5, label_h, { 20, 20, 20, 255 });
	g_Draw.String(FONT_MENU, label_x, label_y, Vars::Menu::Colors::Text, ALIGN_DEFAULT, Label);

	m_LastWidget.x -= Vars::Menu::SpacingX;
	m_LastWidget.y += Vars::Menu::SpacingY * 2;
	m_LastGroupBox.h = h;
}

bool CMenu::CheckBox(CVar<bool>& Var, const wchar_t* const szToolTip)
{
	bool callback = false;

	int x = m_LastWidget.x;
	int y = m_LastWidget.y + m_LastWidget.h + Vars::Menu::SpacingY;
	int w = Vars::Menu::CheckBoxW;
	int h = Vars::Menu::CheckBoxH;
	int wl, wh;
	g_Interfaces.Surface->GetTextSize(FONT_MENU, Var.m_szDisplayName, wl, wh);

	g_Draw.GradientRect(x, y, x + w, y + h, { 0,0,0,255 }, { 65, 65, 65, 255 }, false);

	if (g_InputHelper.m_nMouseX > x && g_InputHelper.m_nMouseX < x + w && g_InputHelper.m_nMouseY > y && g_InputHelper.m_nMouseY < y + h)
	{
		if (g_InputHelper.IsPressed(VK_LBUTTON)) {
			callback = true;
			Var.m_Var = !Var.m_Var;
		}
		g_Draw.GradientRect(x, y, x + w, y + h, { 45, 45, 45, 255 }, { 65, 65, 65, 255 }, false);
		//g_Draw.Rect(x, y, w, h, Vars::Menu::Colors::WidgetActive);
		m_szCurTip += szToolTip;
	}

	if (Var.m_Var)
	{
		g_Draw.GradientRect(x, y, x + w, y + h, { 0, 0, 0, 255 }, Vars::Menu::Colors::WidgetActive, false);
	}
	//else {
		//g_Draw.String(FONT_MENU, x + w + Vars::Menu::SpacingText, y + (h / 2), Vars::Menu::Colors::Text, ALIGN_CENTERVERTICAL, Var.m_szDisplayName);
	//}
	g_Draw.String(FONT_MENU, x + w + Vars::Menu::SpacingText, y + (h / 2), Vars::Menu::Colors::Text, ALIGN_CENTERVERTICAL, Var.m_szDisplayName);
	g_Draw.OutlinedRect(x, y, w, h, { 30,30,30,255 });

	m_LastWidget.x = x;
	m_LastWidget.y = y;
	m_LastWidget.w = w;
	m_LastWidget.h = h;

	return callback;
}

bool CMenu::ButtonTab(const wchar_t* Label, bool Active, int WidthOverride, int HeightOverride)
{
	bool callback = false;

	int x = m_LastWidget.x;
	int y = m_LastWidget.y + m_LastWidget.h + Vars::Menu::SpacingY;
	int w = WidthOverride ? WidthOverride : Vars::Menu::ButtonTabW;
	int h = HeightOverride ? HeightOverride : Vars::Menu::ButtonTabH;

	g_Draw.Rect(x, y, w, h, { 25,25,25,255 });
	g_Draw.OutlinedRect(x + 1, y + 1, w - 2, h - 2, { 50,50,50,255 });
	g_Draw.OutlinedRect(x, y, w, h, { 0,0,0,255 });

	if (g_InputHelper.m_nMouseX > x && g_InputHelper.m_nMouseX < x + w && g_InputHelper.m_nMouseY > y && g_InputHelper.m_nMouseY < y + h)
	{
		if (g_InputHelper.IsPressed(VK_LBUTTON))
			callback = true;
		g_Draw.String(FONT_MENU, x + (w / 2), y + (h / 2), { 150,150,150,255 }, ALIGN_CENTER, Label);
	}

	if (Active) {
		g_Draw.String(FONT_MENU, x + (w / 2), y + (h / 2), { 255,255,255,255 }, ALIGN_CENTER, Label);
		g_Draw.Rect(x + 1, y + h - 2, w - 2, 1, Vars::Menu::Colors::WidgetActive);
	}
	else {
		g_Draw.String(FONT_MENU, x + (w / 2), y + (h / 2), { 80,80,80,255 }, ALIGN_CENTER, Label);
	}

	m_LastWidget.x = x;
	m_LastWidget.y = y;
	m_LastWidget.w = w;
	m_LastWidget.h = h;

	if (callback)
		m_bReopened = true;

	return callback;
}

bool CMenu::Button(const wchar_t* Label, bool Active, int WidthOverride, int HeightOverride)
{
	bool callback = false;

	int x = m_LastWidget.x;
	int y = m_LastWidget.y + m_LastWidget.h + Vars::Menu::SpacingY;
	int w = WidthOverride ? WidthOverride : Vars::Menu::ButtonW;
	int h = HeightOverride ? HeightOverride : Vars::Menu::ButtonH;

	if (g_InputHelper.m_nMouseX > x && g_InputHelper.m_nMouseX < x + w && g_InputHelper.m_nMouseY > y && g_InputHelper.m_nMouseY < y + h) // Hovered
	{
		if (g_InputHelper.IsPressed(VK_LBUTTON))
			callback = true;

		g_Draw.Rect(x, y, w, h, Vars::Menu::Colors::WidgetActive);
	}

	if (Active)
		g_Draw.GradientRect(x, y, x + w, y + h, { 30,30,30,255 }, Vars::Menu::Colors::WidgetActive, false); // Active

	g_Draw.OutlinedRect(x, y, w, h, { 50,50,50,255 });
	g_Draw.OutlinedRect(x - 1, y - 1, w + 2, h + 2, { 0,0,0,255 });
	g_Draw.String(FONT_MENU, x + (w / 2), y + (h / 2), Vars::Menu::Colors::Text, ALIGN_CENTER, Label);

	m_LastWidget.x = x;
	m_LastWidget.y = y;
	m_LastWidget.w = w;
	m_LastWidget.h = h;

	if (callback)
		m_bReopened = true;

	return callback;
}

bool CMenu::ComboBox(CVar<int>& Var, const std::vector<CVar<int>>& List)
{
	auto FindCurItemName = [&]() -> const wchar_t*
	{
		for (const auto& Item : List)
		{
			if (Item.m_Var == Var.m_Var)
				return Item.m_szDisplayName;
		}

		return _(L"UNKNOWN_ITEM");
	};

	auto FindCurItemIndex = [&]() -> int
	{
		for (size_t n = 0; n < List.size(); n++)
		{
			if (List[n].m_Var == Var.m_Var)
				return n;
		}

		return 0;
	};

	bool callback = false;

	int x = m_LastWidget.x;
	int y = m_LastWidget.y + m_LastWidget.h + Vars::Menu::SpacingY + 10;
	int w = Vars::Menu::ComboBoxW;
	int h = Vars::Menu::ComboBoxH;

	static std::map<CVar<int>*, int> indexes = {};
	static std::map<CVar<int>*, bool> inits = {};

	if (!inits[&Var] || m_bReopened) {
		indexes[&Var] = FindCurItemIndex();
		inits[&Var] = true;
	}

	if (g_InputHelper.m_nMouseX > x && g_InputHelper.m_nMouseX < x + (w / 2) && g_InputHelper.m_nMouseY > y && g_InputHelper.m_nMouseY < y + h)
	{
		if (indexes[&Var] > 0)
		{
			if (g_InputHelper.IsPressed(VK_LBUTTON)) {
				Var.m_Var = List[--indexes[&Var]].m_Var;
				callback = true;
			}

			g_Draw.GradientRect(x, y, x + (w / 2), y + h, Vars::Menu::Colors::WidgetActive, Vars::Menu::Colors::Widget, true);
		}
	}

	else if (g_InputHelper.m_nMouseX > x + (w / 2) && g_InputHelper.m_nMouseX < x + w && g_InputHelper.m_nMouseY > y && g_InputHelper.m_nMouseY < y + h)
	{
		if (indexes[&Var] < int(List.size() - 1))
		{
			if (g_InputHelper.IsPressed(VK_LBUTTON)) {
				Var.m_Var = List[++indexes[&Var]].m_Var;
				callback = true;
			}

			g_Draw.GradientRect(x + (w / 2), y, x + w, y + h, Vars::Menu::Colors::Widget, Vars::Menu::Colors::WidgetActive, true);
		}
	}

	g_Draw.OutlinedRect(x, y, w, h, Vars::Menu::Colors::OutlineMenu);
	g_Draw.String(FONT_MENU, x + (w / 2), y + (h / 2), Vars::Menu::Colors::Text, ALIGN_CENTER, FindCurItemName());
	g_Draw.String(FONT_MENU, x, y - Vars::Menu::SpacingText * 2, Vars::Menu::Colors::Text, ALIGN_CENTERVERTICAL, Var.m_szDisplayName);

	m_LastWidget.x = x;
	m_LastWidget.y = y;
	m_LastWidget.w = w;
	m_LastWidget.h = h;

	return callback;
}

bool CMenu::InputFloat(CVar<float>& Var, float Min, float Max, float Step, const wchar_t* Fmt)
{
	bool callback = false;

	int x = m_LastWidget.x;
	int y = m_LastWidget.y + m_LastWidget.h + Vars::Menu::SpacingY;
	int w = Vars::Menu::InputBoxW;
	int h = Vars::Menu::InputBoxH;

	if (Var.m_Var != Min)
	{
		if (g_InputHelper.m_nMouseX > x && g_InputHelper.m_nMouseX < x + (w / 2) && g_InputHelper.m_nMouseY > y && g_InputHelper.m_nMouseY < y + h)
		{
			if (g_InputHelper.IsPressedAndHeld(VK_LBUTTON)) {
				Var.m_Var -= Step;
				Var.m_Var = std::clamp(Var.m_Var, Min, Max);
				callback = true;
			}

			g_Draw.GradientRect(x, y, x + (w / 2), y + h, Vars::Menu::Colors::WidgetActive, Vars::Menu::Colors::Widget, true);
		}
	}

	if (Var.m_Var != Max)
	{
		if (g_InputHelper.m_nMouseX > x + (w / 2) && g_InputHelper.m_nMouseX < x + w && g_InputHelper.m_nMouseY > y && g_InputHelper.m_nMouseY < y + h)
		{
			if (g_InputHelper.IsPressedAndHeld(VK_LBUTTON)) {
				Var.m_Var += Step;
				Var.m_Var = std::clamp(Var.m_Var, Min, Max);
				callback = true;
			}

			g_Draw.GradientRect(x + (w / 2), y, x + w, y + h, Vars::Menu::Colors::Widget, Vars::Menu::Colors::WidgetActive, true);
		}
	}

	g_Draw.OutlinedRect(x, y, w, h, Vars::Menu::Colors::OutlineMenu);
	g_Draw.String(FONT_MENU, x + (w / 2), y + (h / 2), Vars::Menu::Colors::Text, ALIGN_CENTER, Fmt, Var.m_Var);
	g_Draw.String(FONT_MENU, x + w + Vars::Menu::SpacingText, y + (h / 2), Vars::Menu::Colors::Text, ALIGN_CENTERVERTICAL, Var.m_szDisplayName);

	m_LastWidget.x = x;
	m_LastWidget.y = y;
	m_LastWidget.w = w;
	m_LastWidget.h = h;

	return callback;
}

bool CMenu::InputInt(CVar<int>& Var, int Min, int Max, int Step)
{
	bool callback = false;

	int x = m_LastWidget.x;
	int y = m_LastWidget.y + m_LastWidget.h + Vars::Menu::SpacingY;
	int w = Vars::Menu::InputBoxW;
	int h = Vars::Menu::InputBoxH;

	if (Var.m_Var != Min)
	{
		if (g_InputHelper.m_nMouseX > x && g_InputHelper.m_nMouseX < x + (w / 2) && g_InputHelper.m_nMouseY > y && g_InputHelper.m_nMouseY < y + h)
		{
			if (g_InputHelper.IsPressedAndHeld(VK_LBUTTON)) {
				Var.m_Var -= Step;
				Var.m_Var = std::clamp(Var.m_Var, Min, Max);
				callback = true;
			}

			g_Draw.GradientRect(x, y, x + (w / 2), y + h, Vars::Menu::Colors::WidgetActive, Vars::Menu::Colors::Widget, true);
		}
	}

	if (Var.m_Var != Max)
	{
		if (g_InputHelper.m_nMouseX > x + (w / 2) && g_InputHelper.m_nMouseX < x + w && g_InputHelper.m_nMouseY > y && g_InputHelper.m_nMouseY < y + h)
		{
			if (g_InputHelper.IsPressedAndHeld(VK_LBUTTON)) {
				Var.m_Var += Step;
				Var.m_Var = std::clamp(Var.m_Var, Min, Max);
				callback = true;
			}

			g_Draw.GradientRect(x + (w / 2), y, x + w, y + h, Vars::Menu::Colors::Widget, Vars::Menu::Colors::WidgetActive, true);
		}
	}

	g_Draw.OutlinedRect(x, y, w, h, Vars::Menu::Colors::OutlineMenu);
	g_Draw.String(FONT_MENU, x + (w / 2), y + (h / 2), Vars::Menu::Colors::Text, ALIGN_CENTER, "%d", Var.m_Var);
	g_Draw.String(FONT_MENU, x + w + Vars::Menu::SpacingText, y + (h / 2), Vars::Menu::Colors::Text, ALIGN_CENTERVERTICAL, Var.m_szDisplayName);

	m_LastWidget.x = x;
	m_LastWidget.y = y;
	m_LastWidget.w = w;
	m_LastWidget.h = h;

	return callback;
}

std::unique_ptr<Color_t[]> pGradient = nullptr;

void CMenu::ColorSpectrum(int x, int y, int w, int h)
{
	if (!pGradient)
	{
		pGradient = std::make_unique<Color_t[]>(w * h);

		float hue = 0.0f, sat = 0.99f, lum = 1.0f;
		for (int i = 0; i < w; i++)
		{
			for (int j = 0; j < h; j++)
			{
				*(Color_t*)(pGradient.get() + j + i * h) = Color::HSLToRGB(hue, sat, lum);

				hue += (1.f / w);
			}

			lum -= (1.f / h);
			hue = 0.f;
		}
	}
}

Color_t GetOfficialColor(int x, int y)
{
	return *(Color_t*)(pGradient.get() + x + y * 150);
}


bool CMenu::ColorPicker(Color_t& Var, const wchar_t* Label) {
	if (g_Menu.m_bOpen) {
		const Vec2 vecPosition = g_Menu.m_vOffset;
		const Vec2 vecRelative = { g_InputHelper.m_nMouseX - vecPosition.x, g_InputHelper.m_nMouseY - vecPosition.y };
		static Vec2 vecSize = { 150, 150 };

		g_Draw.Rect(vecPosition.x - 4, vecPosition.y - 4, true ? 173 : 158, 158, { 40, 40, 40, 255 });
		g_Draw.OutlinedRect(vecPosition.x - 4, vecPosition.y - 4, true ? 173 : 158, 158, { 0, 0, 0, 255 });
		g_Draw.OutlinedRect(vecPosition.x - 3, vecPosition.y - 3, true ? 171 : 156, 156, { 60, 60, 60, 255 });

		ColorSpectrum(vecPosition.x, vecPosition.y, vecSize.x, vecSize.y);
	}
	return true;
}

bool CMenu::InputColor(Color_t& Var, const wchar_t* Label)
{
	bool callback = false;
	static bool drawcp = false;
	int x = m_LastWidget.x;
	int y = m_LastWidget.y + m_LastWidget.h + Vars::Menu::SpacingY;
	int w = Vars::Menu::InputColorBoxW;
	int h = Vars::Menu::InputBoxH;
	int _x = x;

	int oldspacing = Vars::Menu::SpacingX;
	Vars::Menu::SpacingX /= 2;

	
	//red
	{
		if (Var.r != 0)
		{
			if (g_InputHelper.m_nMouseX > x && g_InputHelper.m_nMouseX < x + (w / 2) && g_InputHelper.m_nMouseY > y && g_InputHelper.m_nMouseY < y + h)
			{
				if (g_InputHelper.IsPressedAndHeld(VK_LBUTTON)) {
					Var.r -= 1;
					Var.r = std::clamp(Var.r, static_cast<byte>(0), static_cast<byte>(255));
					callback = true;
				}

				g_Draw.GradientRect(x, y, x + (w / 2), y + h, Vars::Menu::Colors::WidgetActive, Vars::Menu::Colors::Widget, true);
			}
		}

		if (Var.r != 255)
		{
			if (g_InputHelper.m_nMouseX > x + (w / 2) && g_InputHelper.m_nMouseX < x + w && g_InputHelper.m_nMouseY > y && g_InputHelper.m_nMouseY < y + h)
			{
				if (g_InputHelper.IsPressedAndHeld(VK_LBUTTON)) {
					Var.r += 1;
					Var.r = std::clamp(Var.r, static_cast<byte>(0), static_cast<byte>(255));
					callback = true;
				}

				g_Draw.GradientRect(x + (w / 2), y, x + w, y + h, Vars::Menu::Colors::Widget, Vars::Menu::Colors::WidgetActive, true);
			}
		}

		g_Draw.OutlinedRect(_x, y, w, h, Vars::Menu::Colors::OutlineMenu);
		g_Draw.String(FONT_MENU, _x + (w / 2), y + (h / 2), Vars::Menu::Colors::Text, ALIGN_CENTER, "%d", Var.r);
		_x += w + Vars::Menu::SpacingX;
	}

	//green
	{
		if (Var.g != 0)
		{
			if (g_InputHelper.m_nMouseX > _x && g_InputHelper.m_nMouseX < _x + (w / 2) && g_InputHelper.m_nMouseY > y && g_InputHelper.m_nMouseY < y + h)
			{
				if (g_InputHelper.IsPressedAndHeld(VK_LBUTTON)) {
					Var.g -= 1;
					Var.g = std::clamp(Var.g, static_cast<byte>(0), static_cast<byte>(255));
					callback = true;
				}

				g_Draw.GradientRect(_x, y, _x + (w / 2), y + h, Vars::Menu::Colors::WidgetActive, Vars::Menu::Colors::Widget, true);
			}
		}

		if (Var.g != 255)
		{
			if (g_InputHelper.m_nMouseX > _x + (w / 2) && g_InputHelper.m_nMouseX < _x + w && g_InputHelper.m_nMouseY > y && g_InputHelper.m_nMouseY < y + h)
			{
				if (g_InputHelper.IsPressedAndHeld(VK_LBUTTON)) {
					Var.g += 1;
					Var.g = std::clamp(Var.g, static_cast<byte>(0), static_cast<byte>(255));
					callback = true;
				}

				g_Draw.GradientRect(_x + (w / 2), y, _x + w, y + h, Vars::Menu::Colors::Widget, Vars::Menu::Colors::WidgetActive, true);
			}
		}

		g_Draw.OutlinedRect(_x, y, w, h, Vars::Menu::Colors::OutlineMenu);
		g_Draw.String(FONT_MENU, _x + (w / 2), y + (h / 2), Vars::Menu::Colors::Text, ALIGN_CENTER, "%d", Var.g);
		_x += w + Vars::Menu::SpacingX;
	}

	//blue
	{
		if (Var.b != 0)
		{
			if (g_InputHelper.m_nMouseX > _x && g_InputHelper.m_nMouseX < _x + (w / 2) && g_InputHelper.m_nMouseY > y && g_InputHelper.m_nMouseY < y + h)
			{
				if (g_InputHelper.IsPressedAndHeld(VK_LBUTTON)) {
					Var.b -= 1;
					Var.b = std::clamp(Var.b, static_cast<byte>(0), static_cast<byte>(255));
					callback = true;
				}

				g_Draw.GradientRect(_x, y, _x + (w / 2), y + h, Vars::Menu::Colors::WidgetActive, Vars::Menu::Colors::Widget, true);
			}
		}

		if (Var.b != 255)
		{
			if (g_InputHelper.m_nMouseX > _x + (w / 2) && g_InputHelper.m_nMouseX < _x + w && g_InputHelper.m_nMouseY > y && g_InputHelper.m_nMouseY < y + h)
			{
				if (g_InputHelper.IsPressedAndHeld(VK_LBUTTON)) {
					Var.b += 1;
					Var.b = std::clamp(Var.b, static_cast<byte>(0), static_cast<byte>(255));
					callback = true;
				}

				g_Draw.GradientRect(_x + (w / 2), y, _x + w, y + h, Vars::Menu::Colors::Widget, Vars::Menu::Colors::WidgetActive, true);
			}
		}

		g_Draw.OutlinedRect(_x, y, w, h, Vars::Menu::Colors::OutlineMenu);
		g_Draw.String(FONT_MENU, _x + (w / 2), y + (h / 2), Vars::Menu::Colors::Text, ALIGN_CENTER, "%d", Var.b);
		_x += w + Vars::Menu::SpacingX;
	}

	//alpha
	{
		if (Var.a != 0)
		{
			if (g_InputHelper.m_nMouseX > _x && g_InputHelper.m_nMouseX < _x + (w / 2) && g_InputHelper.m_nMouseY > y && g_InputHelper.m_nMouseY < y + h)
			{
				if (g_InputHelper.IsPressedAndHeld(VK_LBUTTON)) {
					Var.a -= 1;
					Var.a = std::clamp(Var.a, static_cast<byte>(0), static_cast<byte>(255));
					callback = true;
				}

				g_Draw.GradientRect(_x, y, _x + (w / 2), y + h, Vars::Menu::Colors::WidgetActive, Vars::Menu::Colors::Widget, true);
			}
		}

		if (Var.a != 255)
		{
			if (g_InputHelper.m_nMouseX > _x + (w / 2) && g_InputHelper.m_nMouseX < _x + w && g_InputHelper.m_nMouseY > y && g_InputHelper.m_nMouseY < y + h)
			{
				if (g_InputHelper.IsPressedAndHeld(VK_LBUTTON)) {
					Var.a += 1;
					Var.a = std::clamp(Var.a, static_cast<byte>(0), static_cast<byte>(255));
					callback = true;
				}

				g_Draw.GradientRect(_x + (w / 2), y, _x + w, y + h, Vars::Menu::Colors::Widget, Vars::Menu::Colors::WidgetActive, true);
			}
		}

		g_Draw.OutlinedRect(_x, y, w, h, Vars::Menu::Colors::OutlineMenu);
		g_Draw.String(FONT_MENU, _x + (w / 2), y + (h / 2), Vars::Menu::Colors::Text, ALIGN_CENTER, "%d", Var.a);
	}
	
	if (g_InputHelper.m_nMouseX > x && g_InputHelper.m_nMouseX < x + (h) && g_InputHelper.m_nMouseY > y && g_InputHelper.m_nMouseY < y + h)
	{
		if (g_InputHelper.IsPressed(VK_LBUTTON)) {
			//Var.r -= 1;
			//Var.r = std::clamp(Var.r, static_cast<byte>(0), static_cast<byte>(255));
			callback = true;
		}
	}
	g_Draw.GradientRect(_x + w + Vars::Menu::SpacingText, y, _x + w + Vars::Menu::SpacingText + h, y + h, { 0,0,0,255 }, { Var.r,Var.g,Var.b,Var.a }, false);
	//g_Draw.OutlinedRect(_x + w + Vars::Menu::SpacingText, y, _x + w, h, { 0,0,0,255 });
	//g_Draw.GradientRect(x, y, x + h, y + h, { 0,0,0,255 }, { Var.r,Var.g,Var.b,Var.a }, false);
	//g_Draw.OutlinedRect(x, y, h, h, { 0,0,0,255 });
	//g_Draw.String(FONT_MENU, x + h + Vars::Menu::SpacingText, y + (h / 2), Vars::Menu::Colors::Text, ALIGN_CENTERVERTICAL, Label);
	g_Draw.String(FONT_MENU, _x + w + Vars::Menu::SpacingText + h + 5, y + (h / 2), Vars::Menu::Colors::Text, ALIGN_CENTERVERTICAL, Label);

	Vars::Menu::SpacingX = oldspacing;

	m_LastWidget.x = x;
	m_LastWidget.y = y;
	m_LastWidget.w = w;
	m_LastWidget.h = h;

	return callback;
}

bool CMenu::InputString(const wchar_t* Label, std::wstring& output)
{
	bool callback = false;

	int x = m_LastWidget.x;
	int y = m_LastWidget.y + m_LastWidget.h + Vars::Menu::SpacingY;
	int w = Vars::Menu::InputBoxW * 2;
	int h = Vars::Menu::ButtonH;

	static bool active = false;
	static std::wstring active_str = {};

	if (g_InputHelper.IsPressed(VK_LBUTTON))
	{
		if (g_InputHelper.m_nMouseX > x && g_InputHelper.m_nMouseX < x + w && g_InputHelper.m_nMouseY > y && g_InputHelper.m_nMouseY < y + h)
			active = !active;

		else active = false;
	}

	if (active)
	{
		m_bTyping = true;

		if (g_InputHelper.IsPressed(VK_INSERT)) {
			active = false;
			return false;
		}

		if (active_str.length() < 21)
		{
			if (g_InputHelper.IsPressed(VK_SPACE))
				active_str += char(VK_SPACE);

			for (int16_t key = L'A'; key < L'Z' + 1; key++)
			{
				if (g_InputHelper.IsPressed(key))
					active_str += wchar_t(key);
			}
		}

		if (g_InputHelper.IsPressedAndHeld(VK_BACK) && !active_str.empty())
			active_str.erase(active_str.end() - 1);

		if (g_InputHelper.IsPressed(VK_RETURN))
		{
			active = false;

			if (!active_str.empty())
			{
				output = active_str;
				callback = true;
			}
		}

		g_Draw.String(FONT_MENU, x + (w / 2), y + (h / 2), Vars::Menu::Colors::Text, ALIGN_CENTER, "%ws", active_str.empty() ? _(L"Enter a Name") : active_str.c_str());
	}

	else
	{
		active_str = {};
		g_Draw.String(FONT_MENU, x + (w / 2), y + (h / 2), Vars::Menu::Colors::Text, ALIGN_CENTER, "%ws", Label);
	}

	g_Draw.OutlinedRect(x, y, w, h, Vars::Menu::Colors::OutlineMenu);

	m_LastWidget.x = x;
	m_LastWidget.y = y;
	m_LastWidget.w = w;
	m_LastWidget.h = h;

	return callback;
}


bool CMenu::InputKey(CVar<int>& output, bool bAllowNone)
{
	auto VK2STR = [&](const short key) -> std::wstring {
		switch (key) {
		case VK_LBUTTON: return _(L"LButton");
		case VK_RBUTTON: return _(L"RButton");
		case VK_MBUTTON: return _(L"MButton");
		case VK_XBUTTON1: return _(L"XButton1");
		case VK_XBUTTON2: return _(L"XButton2");
		case VK_SPACE: return _(L"Space");
		case 0x0: return _(L"None");
		default: break;
		}

		WCHAR output[16] = { L"\0" };

		if (const int result = GetKeyNameTextW(MapVirtualKeyW(key, MAPVK_VK_TO_VSC) << 16, output, 16))
			return output;

		return _(L"VK2STR_FAILED");
	};

	bool callback = false;

	int x = m_LastWidget.x;
	int y = m_LastWidget.y + m_LastWidget.h + Vars::Menu::SpacingY;
	int w = Vars::Menu::InputBoxW;
	int h = Vars::Menu::InputBoxH;

	static float time = g_Interfaces.Engine->Time();
	float elapsed = g_Interfaces.Engine->Time() - time;
	static CVar<int>* curr = nullptr, * prevv = curr;
	if (curr != prevv) {
		time = g_Interfaces.Engine->Time();
		prevv = curr;
	}

	if (curr == nullptr && elapsed > 0.1f && g_InputHelper.IsPressed(VK_LBUTTON))
	{
		if (g_InputHelper.m_nMouseX > x && g_InputHelper.m_nMouseX < x + w && g_InputHelper.m_nMouseY > y && g_InputHelper.m_nMouseY < y + h) {
			curr = &output;
			g_InputHelper.NullKey(VK_LBUTTON);
		}
	}

	static float time_notactive = 0.0f;

	if (curr == &output)
	{
		m_bTyping = true;

		if (g_Interfaces.Engine->Time() - time_notactive > 0.1f) {
			for (short n = 0; n < 256; n++) {
				if ((n > 0x0 && n < 0x7) || (n > L'A' - 1 && n < L'Z' + 1) || (n > L'0' - 1 && n < L'9' + 1) || n == VK_LSHIFT || n == VK_RSHIFT || n == VK_SHIFT || n == VK_ESCAPE || n == VK_HOME) {
					if (g_InputHelper.IsPressed(n))
					{
						if (n == VK_HOME) {
							curr = nullptr;
							break;
						}

						if (n == VK_ESCAPE && bAllowNone) {
							output.m_Var = 0x0;
							curr = nullptr;
							break;
						}

						output.m_Var = n;
						curr = nullptr;
						break;
					}
				} //loop
			}
			g_Draw.String(FONT_MENU, x + (w / 2), y + (h / 2), Vars::Menu::Colors::Text, ALIGN_CENTER, "%ws", _(L"Press a Key"));
		}
	}

	else
	{
		if (curr == nullptr)
			time_notactive = g_Interfaces.Engine->Time();
		g_Draw.String(FONT_MENU, x + (w / 2), y + (h / 2), Vars::Menu::Colors::Text, ALIGN_CENTER, "%ws", VK2STR(output.m_Var).c_str());
	}

	g_Draw.String(FONT_MENU, x + w + Vars::Menu::SpacingText, y + (h / 2), Vars::Menu::Colors::Text, ALIGN_CENTERVERTICAL, output.m_szDisplayName);
	g_Draw.OutlinedRect(x, y, w, h, { 50,50,50,255 });
	g_Draw.OutlinedRect(x - 1, y - 1, w + 2, h + 2, { 0,0,0,255 });

	m_LastWidget.x = x;
	m_LastWidget.y = y;
	m_LastWidget.w = w;
	m_LastWidget.h = h;

	return callback;
}
/*
bool CMenu::InputKey(CVar<int>& output, bool bAllowNone)
{
	auto VK2STR = [&](const short key) -> std::wstring
	{
		switch (key) {
		case VK_LBUTTON: return _(L"LButton");
		case VK_RBUTTON: return _(L"RButton");
		case VK_MBUTTON: return _(L"MButton");
		case VK_XBUTTON1: return _(L"XButton1");
		case VK_XBUTTON2: return _(L"XButton2");
		case 0x0: return _(L"None");
		default: break;
		}

		WCHAR output[16] = { L"\0" };

		if (const int result = GetKeyNameTextW(MapVirtualKeyW(key, MAPVK_VK_TO_VSC) << 16, output, 16))
			return output;

		return _(L"VK2STR_FAILED");
	};

	bool callback = false;

	int x = m_LastWidget.x;
	int y = m_LastWidget.y + m_LastWidget.h + Vars::Menu::SpacingY;
	int w = Vars::Menu::InputBoxW;
	int h = Vars::Menu::InputBoxH;

	static bool active = false, old_active = active;
	static float time = g_Interfaces.Engine->Time();
	float elapsed = g_Interfaces.Engine->Time() - time;

	if (old_active != active) {
		time = g_Interfaces.Engine->Time();
		old_active = active;
	}

	if (!active && elapsed > 0.1f && g_InputHelper.IsPressed(VK_LBUTTON))
	{
		if (g_InputHelper.m_nMouseX > x && g_InputHelper.m_nMouseX < x + w && g_InputHelper.m_nMouseY > y && g_InputHelper.m_nMouseY < y + h) {
			active = true;
			g_InputHelper.NullKey(VK_LBUTTON);
		}
	}

	static float time_notactive = 0.0f;

	if (active)
	{
		m_bTyping = true;

		if (g_Interfaces.Engine->Time() - time_notactive > 0.1f)
		{
			for (short n = 0; n < 256; n++)
			{
				if ((n > 0x0 && n < 0x7)
					|| (n > L'A' - 1 && n < L'Z' + 1)
					|| n == VK_LSHIFT || n == VK_RSHIFT || n == VK_SHIFT
					|| n == VK_ESCAPE || n == VK_INSERT)
				{
					if (g_InputHelper.IsPressed(n))
					{
						if (n == VK_INSERT) {
							active = false;
							break;
						}

						if (n == VK_ESCAPE && bAllowNone) {
							output.m_Var = 0x0;
							active = false;
							break;
						}

						output.m_Var = n;
						active = false;
						break;
					}
				}
			}
		}
		g_Draw.String(FONT_MENU, x + (w / 2), y + (h / 2), Vars::Menu::Colors::Text, ALIGN_CENTER, "%ws", _(L"..."));
		//g_Draw.String(FONT_MENU, x + (w / 2), y + (h / 2), Vars::Menu::Colors::Text, ALIGN_CENTER, "%ws", _(L"Press a Key"));
	}
	//

	else
	{
		time_notactive = g_Interfaces.Engine->Time();
		g_Draw.String(FONT_MENU, x + (w / 2), y + (h / 2), Vars::Menu::Colors::Text, ALIGN_CENTER, "%ws", VK2STR(output.m_Var).c_str());
	}

	g_Draw.String(FONT_MENU, x + w + Vars::Menu::SpacingText, y + (h / 2), Vars::Menu::Colors::Text, ALIGN_CENTERVERTICAL, output.m_szDisplayName);
	g_Draw.OutlinedRect(x, y, w, h, { 50,50,50,255 });
	g_Draw.OutlinedRect(x - 1, y - 1, w + 2, h + 2, { 0,0,0,255 });
	//g_Draw.OutlinedRect(x, y, w, h, {30,30,30,255});

	m_LastWidget.x = x;
	m_LastWidget.y = y;
	m_LastWidget.w = w;
	m_LastWidget.h = h;

	return callback;
}
*/
inline int CMenu::ScaleDPI(int i)
{
	return (i * (1.0f + (0.25f * 0)));
}
int Width = 168, Height = 179;
int GradientTexture = 0;
int GradientTextureB = 0;
static std::unique_ptr<Color_t[]> Gradient = nullptr;
static std::unique_ptr<Color_t[]> GradientB = nullptr;

void InitColorPicker() {
	//static int GradientTexture = 0;
	if (!Gradient)
	{
		g_Interfaces.CVars->ConsoleColorPrintf({ 255, 100, 0, 255 }, _("Gradient texture doesn't exist... Making one\n"));
		Gradient = std::make_unique<Color_t[]>(Width * Height);

		for (int i = 0; i < Width; i++)
		{
			int div = Width / 6;
			int phase = i / div;
			float t = (i % div) / (float)div;
			int r, g, b;

			switch (phase)
			{
			case(0):
				r = 255;
				g = 255 * t;
				b = 0;
				break;
			case(1):
				r = 255 * (1.f - t);
				g = 255;
				b = 0;
				break;
			case(2):
				r = 0;
				g = 255;
				b = 255 * t;
				break;
			case(3):
				r = 0;
				g = 255 * (1.f - t);
				b = 255;
				break;
			case(4):
				r = 255 * t;
				g = 0;
				b = 255;
				break;
			case(5):
				r = 255;
				g = 0;
				b = 255 * (1.f - t);
				break;
			}

			for (int k = 0; k < Height; k++)
			{
				float sat = k / (float)Height;
				int _r = r + sat * (0 - r);
				int _g = g + sat * (0 - g);
				int _b = b + sat * (0 - b);

				*reinterpret_cast<Color_t*>(Gradient.get() + i + k * Width) = Color_t(_r, _g, _b,255);
			}
		}
		g_Interfaces.CVars->ConsoleColorPrintf({ 255, 100, 0, 255 }, _("Creating New texture\n"));
		GradientTexture = g_Interfaces.Surface->CreateNewTextureID(true);

		g_Interfaces.CVars->ConsoleColorPrintf({ 255, 100, 0, 255 }, _("Setting texture RGBA\n"));
		//g_Interfaces.Surface->DrawGetTextureSize(GradientTexture, Width, Height);
		g_Interfaces.Surface->DrawSetTextureRGBAEx(GradientTexture, (byte*)Gradient.get(), Width, Height, IMAGE_FORMAT_RGBA8888);
	}

	if (!GradientB)
	{
		g_Interfaces.CVars->ConsoleColorPrintf({ 255, 100, 0, 255 }, _("Gradient texture doesn't exist... Making one\n"));
		GradientB = std::make_unique<Color_t[]>(10 * Height);

		for (int i = 0; i < Height; i++)
		{
			int div = Height / 6;
			int phase = i / div;
			float t = (i % div) / (float)div;
			int r, g, b;

			switch (phase)
			{
			case(0):
				r = 255;
				g = 255 * t;
				b = 0;
				break;
			case(1):
				r = 255 * (1.f - t);
				g = 255;
				b = 0;
				break;
			case(2):
				r = 0;
				g = 255;
				b = 255 * t;
				break;
			case(3):
				r = 0;
				g = 255 * (1.f - t);
				b = 255;
				break;
			case(4):
				r = 255 * t;
				g = 0;
				b = 255;
				break;
			case(5):
				r = 255;
				g = 0;
				b = 255 * (1.f - t);
				break;
			}

			for (int k = 0; k < Height; k++)
			{
				float sat = k / (float)Height;
				int _r = r + sat * (0 - r);
				int _g = g + sat * (0 - g);
				int _b = b + sat * (0 - b);

				*reinterpret_cast<Color_t*>(Gradient.get() + i + k * 10) = Color_t(_r, _g, _b, 255);
			}
		}
		g_Interfaces.CVars->ConsoleColorPrintf({ 255, 100, 0, 255 }, _("Creating New texture\n"));
		GradientTextureB = g_Interfaces.Surface->CreateNewTextureID(true);

		g_Interfaces.CVars->ConsoleColorPrintf({ 255, 100, 0, 255 }, _("Setting texture RGBA\n"));
		//g_Interfaces.Surface->DrawGetTextureSize(GradientTexture, Width, Height);
		g_Interfaces.Surface->DrawSetTextureRGBAEx(GradientTextureB, (byte*)Gradient.get(), 10, Height, IMAGE_FORMAT_RGBA8888);
	}

}

void CMenu::Run()
{
	m_bReopened = false;
	m_bTyping = false;
	static bool bOldOpen = m_bOpen;

	if (bOldOpen != m_bOpen)
	{
		bOldOpen = m_bOpen;

		if (m_bOpen)
			m_bReopened = true;
	}

	if (g_Interfaces.Engine->IsDrawingLoadingImage()) {
		m_bOpen = false;
		return;
	}

	static float flTimeOnChange = 0.0f;

	if (Utils::IsGameWindowInFocus() && (GetAsyncKeyState(VK_HOME) & 1)) {
		g_Interfaces.Surface->SetCursorAlwaysVisible(m_bOpen = !m_bOpen);
		flTimeOnChange = g_Interfaces.Engine->Time();
	}

	m_flFadeElapsed = g_Interfaces.Engine->Time() - flTimeOnChange;

	if (m_flFadeElapsed < m_flFadeDuration) {
		m_flFadeAlpha = Math::RemapValClamped(m_flFadeElapsed, 0.0f, m_flFadeDuration, !m_bOpen ? 1.0f : 0.0f, m_bOpen ? 1.0f : 0.0f);
		g_Interfaces.Surface->DrawSetAlphaMultiplier(m_flFadeAlpha);
	}

	if (m_bOpen || m_flFadeElapsed < m_flFadeDuration)
	{
		m_szCurTip = L"";
		g_InputHelper.Update();
		/*
		Color_t Colorz = GetColorFromPenPosition(Vec2(g_InputHelper.m_nMouseX, g_InputHelper.m_nMouseY));
		InitColorPicker(); // Init this shit lol 
		g_Interfaces.Surface->SetDrawColor(255, 255, 255, 255); // Set the RGBA color 
		g_Interfaces.Surface->DrawSetTexture(GradientTexture); // Set the texture
		g_Interfaces.Surface->DrawTexturedRect(0, 0, Width, Height); // Finally draw the gay ass texture

		//g_Draw.GradientRect(Width + 10, 0, Width + 20, Height, Colorz, { 0,0,0,255 }, false);
		g_Draw.Rect(Width + 50, 30, 50, 50, Colorz);
		//Vars::Menu::Colors::WidgetActive = Colorz;
		*/
		if (Vars::Visuals::Snow.m_Var)
		{
			struct SnowFlake_t {
				float x, y, fall, drift;
				int size;
			};

			static std::vector<SnowFlake_t> SnowFlakes;
			static const int Count = 500;
			static bool Init = false;

			if (!Init)
			{
				for (int n = 0; n < Count; n++)
				{
					SnowFlake_t Flake = {};
					Flake.x = static_cast<float>(Utils::RandIntSimple(0, g_ScreenSize.w));
					Flake.y = -static_cast<float>(Utils::RandIntSimple(50, 500));
					Flake.fall = static_cast<float>(Utils::RandIntSimple(50, 100));
					Flake.drift = static_cast<float>(Utils::RandIntSimple(5, 20));
					Flake.size = Utils::RandIntSimple(1, 4) == 4 ? 2 : 1;
					SnowFlakes.push_back(Flake);
				}

				Init = true;
			}

			for (auto& Flake : SnowFlakes)
			{
				Flake.x += ((sinf(g_Interfaces.GlobalVars->curtime) * Flake.drift) * g_Interfaces.GlobalVars->interval_per_tick);
				Flake.y += (Flake.fall * g_Interfaces.GlobalVars->interval_per_tick);

				float Alpha = Math::MapFloat(Flake.y, 0.0f, static_cast<float>(g_ScreenSize.h - 200), 1.0f, 0.0f);

				if (Alpha <= 0.0f)
				{
					Flake.x = static_cast<float>(Utils::RandIntSimple(0, g_ScreenSize.w));
					Flake.y = -static_cast<float>(Utils::RandIntSimple(50, 500));
					Flake.fall = static_cast<float>(Utils::RandIntSimple(50, 100));
					Flake.drift = static_cast<float>(Utils::RandIntSimple(5, 20));
					Flake.size = Utils::RandIntSimple(1, 4) == 4 ? 2 : 1;
				}

				Color_t Color = { 255, 255, 255, static_cast<byte>(Alpha * 255.0f) };
				//g_Draw.String(FONT_MENU, static_cast<int>(Flake.x), static_cast<int>(Flake.y), Color, ALIGN_DEFAULT, _("*")); // Was only for testing, Looks cool though 
				g_Draw.Rect(static_cast<int>(Flake.x), static_cast<int>(Flake.y), Flake.size, Flake.size, Color);
			}
		}

		// Removed because how annoying it can actually be
		/*
		if (Vars::Menu::Position.x < 5)
			Vars::Menu::Position.x = 5;

		if (Vars::Menu::Position.y < 22)
			Vars::Menu::Position.y = 22;

		if ((Vars::Menu::Position.x + Vars::Menu::Position.w) > g_ScreenSize.w - 5)
			Vars::Menu::Position.x = g_ScreenSize.w - 5 - Vars::Menu::Position.w;

		if ((Vars::Menu::Position.y + Vars::Menu::Position.h) > g_ScreenSize.h + 10)
			Vars::Menu::Position.y = g_ScreenSize.h + 10 - Vars::Menu::Position.h;
		*/
		//Do the Window
		{
			g_InputHelper.Drag(
				g_InputHelper.m_nMouseX,
				g_InputHelper.m_nMouseY,
				Vars::Menu::Position.x,
				Vars::Menu::Position.y,
				Vars::Menu::Position.w,
				Vars::Menu::TitleBarH,
				Vars::Menu::TitleBarH);

			g_Draw.GradientRect(Vars::Menu::Position.x, Vars::Menu::Position.y - Vars::Menu::TitleBarH, Vars::Menu::Position.x + Vars::Menu::Position.w, Vars::Menu::Position.y + Vars::Menu::Position.h - 15, { 0, 0, 0, 240 }, { 0, 0, 0, 240 }, false);
			//g_Draw.Rect(Vars::Menu::Position.x, Vars::Menu::Position.y - Vars::Menu::TitleBarH, Vars::Menu::Position.w, Vars::Menu::Position.h, { 30, 30, 30, 255 });
			g_Draw.OutlinedRect(Vars::Menu::Position.x - 1, Vars::Menu::Position.y - Vars::Menu::TitleBarH - 1, Vars::Menu::Position.w + 2, Vars::Menu::Position.h + 2, { 0, 0, 0, 255 });
			g_Draw.OutlinedRect(Vars::Menu::Position.x, Vars::Menu::Position.y - Vars::Menu::TitleBarH, Vars::Menu::Position.w, Vars::Menu::Position.h, { 60, 60, 60, 255 });
			g_Draw.OutlinedRect(Vars::Menu::Position.x + (4), Vars::Menu::Position.y - Vars::Menu::TitleBarH + ScaleDPI(25), Vars::Menu::Position.w - (9), Vars::Menu::Position.h - (29), { 60, 60, 60, 255 });
			g_Draw.Rect(Vars::Menu::Position.x + (5), Vars::Menu::Position.y - Vars::Menu::TitleBarH + ScaleDPI(26), Vars::Menu::Position.w - (11), Vars::Menu::Position.h - (31), { 20, 20, 20, 255 });

			//Left
			//g_Draw.String(FONT_MENU, Vars::Menu::Position.x + 6, Vars::Menu::Position.y - Vars::Menu::TitleBarH + ScaleDPI(12) - 5, { 208,208,208,255 }, ALIGN_DEFAULT, _("ChadAlphaMales"));
			//g_Draw.String(FONT_MENU, Vars::Menu::Position.x + 92, Vars::Menu::Position.y - Vars::Menu::TitleBarH + ScaleDPI(12) - 5, Vars::Menu::Colors::WidgetActive, ALIGN_DEFAULT, _(".club"));

			//Middle
			g_Draw.String(FONT_MENU, Vars::Menu::Position.x + (Vars::Menu::Position.w / 2) - 16, Vars::Menu::Position.y - Vars::Menu::TitleBarH + ScaleDPI(12) - 5, { 208,208,208,255 }, ALIGN_CENTERHORIZONTAL, _("ChadAlphaMales"));
			g_Draw.String(FONT_MENU, Vars::Menu::Position.x + (Vars::Menu::Position.w / 2) + 39, Vars::Menu::Position.y - Vars::Menu::TitleBarH + ScaleDPI(12) - 5, Vars::Menu::Colors::WidgetActive, ALIGN_CENTERHORIZONTAL, _(".club"));
		}

		{
			enum struct EMainTabs { TAB_AIM, TAB_VISUALS, TAB_MISC, TAB_CONFIGS };
			enum struct EAimTabs { TAB_AIMBOT, TAB_TRIGGERBOT, TAB_OTHER };
			enum struct EVisualsTabs { TAB_ESP, TAB_RADAR, TAB_CHAMS, TAB_GLOW, TAB_OTHER, TAB_SKINS, TAB_COLORS };
			enum struct EMiscTabs { TAB_HVH, TAB_MAIN };

			m_LastWidget = { Vars::Menu::Position.x + Vars::Menu::SpacingX - 3, Vars::Menu::Position.y, 0, 6 };

			static EMainTabs Tab = EMainTabs::TAB_AIM;
			{
				if (ButtonTab(_(L"Aimbot"), Tab == EMainTabs::TAB_AIM))
					Tab = EMainTabs::TAB_AIM;

				m_LastWidget = { Vars::Menu::Position.x + (Vars::Menu::ButtonW * 2 + (Vars::Menu::SpacingX * 2)) + 6, Vars::Menu::Position.y, 0, 6 };
				if (ButtonTab(_(L"Visuals"), Tab == EMainTabs::TAB_VISUALS))
					Tab = EMainTabs::TAB_VISUALS;

				m_LastWidget = { Vars::Menu::Position.x + (Vars::Menu::ButtonW * 4 + (Vars::Menu::SpacingX * 4)) + 7, Vars::Menu::Position.y, 0, 6 };
				if (ButtonTab(_(L"Misc"), Tab == EMainTabs::TAB_MISC))
					Tab = EMainTabs::TAB_MISC;

				m_LastWidget = { Vars::Menu::Position.x + (Vars::Menu::ButtonW * 6 + (Vars::Menu::SpacingX * 6)) + 8, Vars::Menu::Position.y, 0, 6 };
				if (ButtonTab(_(L"Config"), Tab == EMainTabs::TAB_CONFIGS))
					Tab = EMainTabs::TAB_CONFIGS;
				m_LastWidget = {
				Vars::Menu::Position.x + Vars::Menu::SpacingX,
				Vars::Menu::Position.y + Vars::Menu::Position.h - (Vars::Menu::ButtonH + (Vars::Menu::SpacingY * 2)),
				0, 0 };
			}

			Separator();
			m_LastWidget = { Vars::Menu::Position.x + Vars::Menu::SpacingX - 3, Vars::Menu::Position.y, 0, 30 };
			switch (Tab)
			{
			case EMainTabs::TAB_AIM:
			{
				static EAimTabs Tab = EAimTabs::TAB_AIMBOT;
				{
					Rect_t checkpoint_line = m_LastWidget;
					checkpoint_line.x -= Vars::Menu::SpacingX;
					checkpoint_line.y += Vars::Menu::ButtonHSmall + (Vars::Menu::SpacingY * 2) + 10;
					Rect_t checkpoint_move = m_LastWidget;

					if (ButtonTab(_(L"Main"), Tab == EAimTabs::TAB_AIMBOT, 264))
						Tab = EAimTabs::TAB_AIMBOT;

					checkpoint_move.x += Vars::Menu::ButtonWSmall + Vars::Menu::SpacingX;
					//m_LastWidget = checkpoint_move;
					m_LastWidget = { Vars::Menu::Position.x + (Vars::Menu::ButtonW * 2 + (Vars::Menu::SpacingX * 2)) + 72, Vars::Menu::Position.y, 0, 30 };
					if (ButtonTab(_(L"Triggerbot"), Tab == EAimTabs::TAB_TRIGGERBOT, 264))
						Tab = EAimTabs::TAB_TRIGGERBOT;

					checkpoint_move.x += Vars::Menu::ButtonWSmall + Vars::Menu::SpacingX;
					m_LastWidget = { Vars::Menu::Position.x + (Vars::Menu::ButtonW * 4 + (Vars::Menu::SpacingX * 4)) + 139, Vars::Menu::Position.y, 0, 30 };
					if (ButtonTab(_(L"Other"), Tab == EAimTabs::TAB_OTHER, 263))
						Tab = EAimTabs::TAB_OTHER;

					m_LastWidget = checkpoint_line;
					//g_Draw.Line(checkpoint_line.x, checkpoint_line.y, Vars::Menu::Position.x + Vars::Menu::Position.w - 1, checkpoint_line.y, Vars::Menu::Colors::OutlineMenu);
					checkpoint_line.x += Vars::Menu::SpacingX + 11;
					checkpoint_line.y += Vars::Menu::SpacingY;
					m_LastWidget = checkpoint_line;
				}

				switch (Tab)
				{
				case EAimTabs::TAB_AIMBOT:
				{
					Rect_t checkpoint = m_LastWidget;

					GroupBoxStart();
					{
						CheckBox(Vars::Aimbot::Global::Active, _(L"Aimbot master switch"));
						InputKey(Vars::Aimbot::Global::AimKey);
						CheckBox(Vars::Aimbot::Global::AutoShoot, _(L"Automatically shoot when target is found"));
						CheckBox(Vars::Aimbot::Global::AimPlayers, _(L"Aim at players"));
						CheckBox(Vars::Aimbot::Global::AimBuildings, _(L"Aim at buildings"));
						CheckBox(Vars::Aimbot::Global::IgnoreInvlunerable, _(L"Ignore players who can't be damaged"));
						CheckBox(Vars::Aimbot::Global::IgnoreCloaked, _(L"Ignore cloaked spies"));
						CheckBox(Vars::Aimbot::Global::IgnoreFriends, _(L"Ignore Steam friends"));
						CheckBox(Vars::Aimbot::Global::IgnoreTaunting, _(L"Ignore taunting players"));
					}
					GroupBoxEnd(_(L"Global"), 160);

					checkpoint.x += 160 + Vars::Menu::SpacingX;
					m_LastWidget = checkpoint;

					GroupBoxStart();
					{
						CheckBox(Vars::Aimbot::Hitscan::Active, _(L"Hitscan master switch"));
						ComboBox(Vars::Aimbot::Hitscan::SortMethod, { { 0, _(L"FOV") }, { 1, _(L"Distance") } });
						ComboBox(Vars::Aimbot::Hitscan::AimMethod, { { 0, _(L"Normal") }, { 1, _(L"Smooth") }, { 2, _(L"Silent") } });
						ComboBox(Vars::Aimbot::Hitscan::AimHitbox, { { 0, _(L"Head") }, { 1, _(L"Body") }, { 2, _(L"Auto") } });
						InputFloat(Vars::Aimbot::Hitscan::AimFOV, 1.0f, 180.0f, 1.0f, L"%.0f");
						InputFloat(Vars::Aimbot::Hitscan::SmoothingAmount, 1.0f, 10.0f, 1.0f, L"%.0f");
						ComboBox(Vars::Aimbot::Hitscan::TapFire, { { 0, _(L"Off") } , { 1, _(L"Distance") }, { 2, _(L"Always") } });
						CheckBox(Vars::Aimbot::Hitscan::AutoRev, _(L"Automatically rev minigun when aimbot active"));
						CheckBox(Vars::Aimbot::Hitscan::ScanHitboxes, _(L"Scan body edges"));
						CheckBox(Vars::Aimbot::Hitscan::ScanHead, _(L"Scan head edges"));
						CheckBox(Vars::Aimbot::Hitscan::ScanBuildings, _(L"Scan building edges"));
						CheckBox(Vars::Aimbot::Hitscan::WaitForHeadshot, _(L"Wait untill weapon can headshot"));
						CheckBox(Vars::Aimbot::Hitscan::WaitForCharge, _(L"Wait untill sniper can one shot target"));
						CheckBox(Vars::Aimbot::Hitscan::SpectatedSmooth, _(L"Toggle smooth aim when spectated"));
						CheckBox(Vars::Aimbot::Hitscan::ScopedOnly, _(L"Sniper only aims when scoped"));
						CheckBox(Vars::Aimbot::Hitscan::AutoScope, _(L"Sniper automatically scopes when target found"));
						CheckBox(Vars::Misc::CL_Move::Doubletap, _(L"Shift tickbase while aimbotting when it's possible. Check the misc tab for more."));
					}
					GroupBoxEnd(_(L"Hitscan"), 215);

					checkpoint.x += 215 + Vars::Menu::SpacingX;
					m_LastWidget = checkpoint;

					GroupBoxStart();
					{
						CheckBox(Vars::Aimbot::Projectile::Active, _(L"Projectile master switch"));
						//CheckBox(Vars::Aimbot::Projectile::PerformanceMode, _(L"Only target enemy closest to crosshair"));
						//ComboBox(Vars::Aimbot::Projectile::SortMethod, { { 0, _(L"FOV") }, { 1, _(L"Distance") } });
						ComboBox(Vars::Aimbot::Projectile::AimMethod, { { 0, _(L"Normal") }, { 1, _(L"Silent") } });
						ComboBox(Vars::Aimbot::Projectile::AimPosition, { { 0, _(L"Body") }, { 1, _(L"Feet") }, { 2, _(L"Auto") } });
						InputFloat(Vars::Aimbot::Projectile::AimFOV, 1.0f, 180.0f, 1.0f, L"%.0f");
					}
					GroupBoxEnd(_(L"Projectile"), 215);

					GroupBoxStart();
					{
						CheckBox(Vars::Aimbot::Melee::Active, _(L"Melee master switch"));
						ComboBox(Vars::Aimbot::Melee::SortMethod, { { 0, _(L"FOV") }, { 1, _(L"Distance") } });
						ComboBox(Vars::Aimbot::Melee::AimMethod, { { 0, _(L"Normal") }, { 1, _(L"Smooth") }, { 2, _(L"Silent") } });
						//InputFloat(Vars::Aimbot::Melee::AimFOV, 1.0f, 180.0f, 1.0f, L"%.0f");
						InputFloat(Vars::Aimbot::Melee::SmoothingAmount, 1.0f, 10.0f, 0.5f, L"%.0f");
						CheckBox(Vars::Aimbot::Melee::RangeCheck, _(L"Only aim when target in range"));
						CheckBox(Vars::Aimbot::Melee::PredictSwing, _(L"Predict melee attack"));
						CheckBox(Vars::Aimbot::Melee::WhipTeam, _(L"Hit teammates when holding the Disciplinary action"));
					}
					GroupBoxEnd(_(L"Melee"), 215);

					break;
				}

				case EAimTabs::TAB_TRIGGERBOT:
				{
					Rect_t checkpoint = m_LastWidget;
					Rect_t airblast = {}, autouber = {};

					GroupBoxStart();
					{
						CheckBox(Vars::Triggerbot::Global::Active, _(L"Triggerbot master switch"));
						InputKey(Vars::Triggerbot::Global::TriggerKey);
						CheckBox(Vars::Triggerbot::Global::IgnoreInvlunerable, _(L"Ignore players who can't be damaged"));
						CheckBox(Vars::Triggerbot::Global::IgnoreCloaked, _(L"Ignore cloaked spies"));
						CheckBox(Vars::Triggerbot::Global::IgnoreFriends, _(L"Ignore Steam friends"));
					}
					GroupBoxEnd(_(L"Global"), 170);

					airblast = m_LastWidget;

					checkpoint.x += 170 + Vars::Menu::SpacingX;
					m_LastWidget = checkpoint;

					GroupBoxStart();
					{
						CheckBox(Vars::Triggerbot::Shoot::Active, _(L"Shoot master switch"));
						CheckBox(Vars::Triggerbot::Shoot::TriggerPlayers, _(L"Shoot players"));
						CheckBox(Vars::Triggerbot::Shoot::TriggerBuildings, _(L"Shoot buildings"));
						CheckBox(Vars::Triggerbot::Shoot::HeadOnly, _(L"Only shoot when aiming at head"));
						CheckBox(Vars::Triggerbot::Shoot::WaitForHeadshot, _(L"Sniper waits for enough charge"));
						InputFloat(Vars::Triggerbot::Shoot::HeadScale, 0.5f, 1.0f, 0.1f, L"%.1f");
					}
					GroupBoxEnd(_(L"AutoShoot"), 170);

					autouber = m_LastWidget;

					checkpoint.x += 170 + Vars::Menu::SpacingX;
					m_LastWidget = checkpoint;

					GroupBoxStart();
					{
						CheckBox(Vars::Triggerbot::Stab::Active, _(L"Stab master switch"));
						CheckBox(Vars::Triggerbot::Stab::RageMode, _(L"Stabs always when possible, even behind"));
						CheckBox(Vars::Triggerbot::Stab::Silent, _(L"Aim changes made by rage aren't visible"));
						CheckBox(Vars::Triggerbot::Stab::Disguise, _(L"Apply last disguise after stab"));
						CheckBox(Vars::Triggerbot::Stab::IgnRazor, _(L"Ignore snipers with Razorback"));
						InputFloat(Vars::Triggerbot::Stab::Range, 0.5f, 1.0f, 0.1f, L"%.1f");
					}
					GroupBoxEnd(_(L"AutoStab"), 140);

					checkpoint.x += 140 + Vars::Menu::SpacingX;
					m_LastWidget = checkpoint;

					GroupBoxStart();
					{
						CheckBox(Vars::Triggerbot::Detonate::Active, _(L"Detonate master switch"));
						CheckBox(Vars::Triggerbot::Detonate::Stickies, _(L"Detonate demoman stickies"));
						CheckBox(Vars::Triggerbot::Detonate::Flares, _(L"Detonate pyro flares"));
						InputFloat(Vars::Triggerbot::Detonate::RadiusScale, 0.5f, 1.0f, 0.1f, L"%.1f");
					}
					GroupBoxEnd(_(L"AutoDetonate"), 175);

					m_LastWidget = airblast;

					GroupBoxStart();
					{
						CheckBox(Vars::Triggerbot::Blast::Active, _(L"Airblast master switch"));
						CheckBox(Vars::Triggerbot::Blast::Rage, _(L"Blast always when possible, even behind"));
						CheckBox(Vars::Triggerbot::Blast::Silent, _(L"Aim changes made by rage aren't visible"));
						InputInt(Vars::Triggerbot::Blast::Fov, 1, 60, 1);
					}
					GroupBoxEnd(L"AutoAirblast", 170);

					m_LastWidget = autouber;

					GroupBoxStart();
					{
						CheckBox(Vars::Triggerbot::Uber::Active, _(L"Uber master switch"));
						CheckBox(Vars::Triggerbot::Uber::OnlyFriends, _(L"Only pop uber on Steam friends"));
						CheckBox(Vars::Triggerbot::Uber::PopLocal, _(L"Pop uber when local below selected health percentage"));
						InputFloat(Vars::Triggerbot::Uber::HealthLeft, 1.0f, 99.0f, 1.0f, L"%.0f%%");
					}
					GroupBoxEnd(L"AutoUber", 170);

					break;
				}

				case EAimTabs::TAB_OTHER:
				{
					GroupBoxStart();
					{
						CheckBox(Vars::Misc::DisableInterpolation, _(L"Disable interpolation"));
						CheckBox(Vars::Visuals::RemoveDisguises, _(L"Remove disguises from spies"));
						CheckBox(Vars::Visuals::RemoveTaunts, _(L"Remove taunts from enemy players"));
					}
					GroupBoxEnd(_(L"Accuracy"), 160);
					break;
				}
				}

				break;
			}

			case EMainTabs::TAB_VISUALS:
			{
				static EVisualsTabs Tab = EVisualsTabs::TAB_ESP;
				{
					Rect_t checkpoint_line = m_LastWidget;
					checkpoint_line.x -= Vars::Menu::SpacingX;
					checkpoint_line.y += Vars::Menu::ButtonHSmall + (Vars::Menu::SpacingY * 2);
					Rect_t checkpoint_move = m_LastWidget;

					if (Button(_(L"ESP"), Tab == EVisualsTabs::TAB_ESP, Vars::Menu::ButtonWSmall, Vars::Menu::ButtonHSmall))
						Tab = EVisualsTabs::TAB_ESP;

					checkpoint_move.x += Vars::Menu::ButtonWSmall + Vars::Menu::SpacingX;
					m_LastWidget = checkpoint_move;

					if (Button(_(L"Radar"), Tab == EVisualsTabs::TAB_RADAR, Vars::Menu::ButtonWSmall, Vars::Menu::ButtonHSmall))
						Tab = EVisualsTabs::TAB_RADAR;

					checkpoint_move.x += Vars::Menu::ButtonWSmall + Vars::Menu::SpacingX;
					m_LastWidget = checkpoint_move;

					if (Button(_(L"Chams"), Tab == EVisualsTabs::TAB_CHAMS, Vars::Menu::ButtonWSmall, Vars::Menu::ButtonHSmall))
						Tab = EVisualsTabs::TAB_CHAMS;

					checkpoint_move.x += Vars::Menu::ButtonWSmall + Vars::Menu::SpacingX;
					m_LastWidget = checkpoint_move;

					if (Button(_(L"Glow"), Tab == EVisualsTabs::TAB_GLOW, Vars::Menu::ButtonWSmall, Vars::Menu::ButtonHSmall))
						Tab = EVisualsTabs::TAB_GLOW;

					checkpoint_move.x += Vars::Menu::ButtonWSmall + Vars::Menu::SpacingX;
					m_LastWidget = checkpoint_move;

					if (Button(_(L"Other"), Tab == EVisualsTabs::TAB_OTHER, Vars::Menu::ButtonWSmall, Vars::Menu::ButtonHSmall))
						Tab = EVisualsTabs::TAB_OTHER;

					checkpoint_move.x += Vars::Menu::ButtonWSmall + Vars::Menu::SpacingX;
					m_LastWidget = checkpoint_move;

					if (Button(_(L"Colors"), Tab == EVisualsTabs::TAB_COLORS, Vars::Menu::ButtonWSmall, Vars::Menu::ButtonHSmall))
						Tab = EVisualsTabs::TAB_COLORS;

					m_LastWidget = checkpoint_line;
					//g_Draw.Line(checkpoint_line.x, checkpoint_line.y, Vars::Menu::Position.x + Vars::Menu::Position.w - 1, checkpoint_line.y, Vars::Menu::Colors::OutlineMenu);
					checkpoint_line.x += Vars::Menu::SpacingX;
					checkpoint_line.y += Vars::Menu::SpacingY;
					m_LastWidget = checkpoint_line;
				}

				switch (Tab)
				{
				case EVisualsTabs::TAB_ESP:
				{
					Rect_t checkpoint = m_LastWidget;
					/*
					GroupBoxStart();
					{
						CheckBox(Vars::ESP::Main::Active, _(L"ESP master switch"));
						ComboBox(Vars::ESP::Main::Outline, { { 0, _(L"Off") }, { 1, _(L"Text Only") }, { 2, _(L"All") } });
					}
					GroupBoxEnd(_(L"Main"), 210);
					*/
					GroupBoxStart();
					{
						CheckBox(Vars::ESP::Players::Active, _(L"Player master switch"));
						CheckBox(Vars::ESP::Players::ShowLocal, _(L"Draw ESP on local player"));
						ComboBox(Vars::ESP::Players::IgnoreTeammates, { { 0, _(L"Off") }, { 1, _(L"All") }, { 2, _(L"Keep Friends") } });
						ComboBox(Vars::ESP::Players::IgnoreCloaked, { { 0, _(L"Off") }, { 1, _(L"All") }, { 2, _(L"Enemies Only") } });
						CheckBox(Vars::ESP::Players::Name, _(L"Draw player's name"));
						ComboBox(Vars::ESP::Players::Class, { { 0, _(L"Off") }, { 1, _(L"Icon") }, { 2, _(L"Text") }, { 3, _(L"Both") } });
						//CheckBox(Vars::ESP::Players::Health, _(L"Draw player's health"));
						CheckBox(Vars::ESP::Players::Cond, _(L"Draw player's condition(s)"));
						CheckBox(Vars::ESP::Players::Healthbar::Enabled, _(L"Draw player's health with bar"));
						ComboBox(Vars::ESP::Players::Uber, { { 0, _(L"Off") }, { 1, _(L"Text") }, { 2, _(L"Bar") } });
						ComboBox(Vars::ESP::Players::Box, { { 0, _(L"Off") }, { 1, _(L"Simple") }, { 2, _(L"Corners") }, { 3, _(L"3D") } });
						ComboBox(Vars::ESP::Players::Bones, { {0, _(L"Off")}, {1, _(L"Custom")}, {2, _(L"Health")} }); // Off / Custom / Health
						CheckBox(Vars::ESP::Players::GUID, _(L"Draw player's GUID"));
						CheckBox(Vars::ESP::Players::Lines, _(L"Draw line to player"));
						CheckBox(Vars::ESP::Players::Dlights, _(L"Attach a light source to the player"));
						InputFloat(Vars::ESP::Players::DlightRadius, 5.0f, 400.0f, 5.0f, L"%.f");
						InputFloat(Vars::ESP::Players::Alpha, 0.05f, 1.0f, 0.05f, L"%.2f");
					}
					GroupBoxEnd(_(L"Players"), 210);

					checkpoint.x += 210 + Vars::Menu::SpacingX;
					m_LastWidget = checkpoint;

					GroupBoxStart();
					{
						CheckBox(Vars::ESP::Buildings::Active, _(L"Building master switch"));
						CheckBox(Vars::ESP::Buildings::IgnoreTeammates, _(L"Ignore team's buildings"));
						CheckBox(Vars::ESP::Buildings::Name, _(L"Draw building's name"));
						CheckBox(Vars::ESP::Buildings::Health, _(L"Draw building's health"));
						CheckBox(Vars::ESP::Buildings::Level, _(L"Draw building's level"));
						CheckBox(Vars::ESP::Buildings::Cond, _(L"Draw building's condition(s)"));
						CheckBox(Vars::ESP::Buildings::HealthBar, _(L"Draw building's health with bar"));
						CheckBox(Vars::ESP::Buildings::Owner, _(L"Draw building's owner's name"));
						ComboBox(Vars::ESP::Buildings::Box, { { 0, _(L"Off") }, { 1, _(L"Simple") }, { 2, _(L"Corners") }, { 3, _(L"3D") } });
						CheckBox(Vars::ESP::Buildings::Lines, _(L"Draw line to building"));
						CheckBox(Vars::ESP::Buildings::Dlights, _(L"Attach a light source to the building"));
						InputFloat(Vars::ESP::Buildings::DlightRadius, 5.0f, 400.0f, 5.0f, L"%.f");
						InputFloat(Vars::ESP::Buildings::Alpha, 0.05f, 1.0f, 0.05f, L"%.f");
					}
					GroupBoxEnd(_(L"Buildings"), 175);

					checkpoint.x += 175 + Vars::Menu::SpacingX;
					m_LastWidget = checkpoint;

					GroupBoxStart();
					{
						CheckBox(Vars::ESP::World::Active, _(L"World ESP master switch"));
						CheckBox(Vars::ESP::World::HealthText, _(L"Draw text on healthpacks"));
						CheckBox(Vars::ESP::World::AmmoText, _(L"Draw text on ammopacks"));
						InputFloat(Vars::ESP::World::Alpha, 0.05f, 1.0f, 0.05f, L"%.2f");
					}
					GroupBoxEnd(_(L"World"), 175);

					break;
				}

				case EVisualsTabs::TAB_RADAR:
				{
					Rect_t checkpoint = m_LastWidget;

					GroupBoxStart();
					{
						CheckBox(Vars::Radar::Main::Active, _(L"Radar master switch"));
						InputInt(Vars::Radar::Main::Size, 20, 200, 1);
						InputInt(Vars::Radar::Main::Range, 50, 3000, 50);
						InputInt(Vars::Radar::Main::BackAlpha, 0, 255);
					}
					GroupBoxEnd(_(L"Main"), 210);

					GroupBoxStart();
					{
						CheckBox(Vars::Radar::Players::Active, _(L"Player master switch"));
						ComboBox(Vars::Radar::Players::IconType, { { 0, _(L"Scoreboard") }, { 1,_(L"Portraits") }, { 2, _(L"Avatar") } });
						//ComboBox(Vars::Radar::Players::BackGroundType, { { 0, _(L"Off") }, { 1, _(L"Rect") }, { 2, _(L"Texture") } });
						//CheckBox(Vars::Radar::Players::Outline, _(L"Draw outline"));
						ComboBox(Vars::Radar::Players::IgnoreTeam, { { 0, _(L"Off") }, { 1, _(L"All") }, { 2, _(L"Keep Friends") } });
						//ComboBox(Vars::Radar::Players::IgnoreCloaked, { { 0, _(L"Off") }, { 1, _(L"All") }, { 2, _(L"Enemies Only") } });
						CheckBox(Vars::Radar::Players::Health, _(L"Draw player's healt with bar"));
						InputInt(Vars::Radar::Players::IconSize, 12, 30, 1);
					}
					GroupBoxEnd(_(L"Players"), 210);

					GroupBoxStart();
					{
						CheckBox(Vars::Radar::Buildings::Active, _(L"Building master switch"));
						//CheckBox(Vars::Radar::Buildings::Outline, _(L"Draw outline"));
						CheckBox(Vars::Radar::Buildings::IgnoreTeam, _(L"Ignore team's buildings"));
						CheckBox(Vars::Radar::Buildings::Health, _(L"Draw building's health with bar"));
						InputInt(Vars::Radar::Buildings::IconSize, 12, 30, 1);
					}
					GroupBoxEnd(_(L"Buildings"), 210);

					checkpoint.x += 210 + Vars::Menu::SpacingX;
					m_LastWidget = checkpoint;

					GroupBoxStart();
					{
						//CheckBox(Vars::Radar::World::Active, _(L"Radar world master switch"));
						CheckBox(Vars::Radar::World::Health, _(L"Draw health icons on healthpacks"));
						CheckBox(Vars::Radar::World::Ammo, _(L"Draw ammo icons on ammopacks"));
						InputInt(Vars::Radar::World::IconSize, 12, 30, 1);
					}
					GroupBoxEnd(_(L"World"), 100);

					break;
				}

				case EVisualsTabs::TAB_CHAMS:
				{
					Rect_t checkpoint = m_LastWidget;
					/*
					GroupBoxStart();
					{
						CheckBox(Vars::Chams::Main::Active, _(L"Chams master switch"));
					}
					GroupBoxEnd(_(L"Main"), 200);
					*/
					GroupBoxStart();
					{
						CheckBox(Vars::Chams::Players::Active, _(L"Player master switch"));
						CheckBox(Vars::Chams::Players::ShowLocal, _(L"Draw chams on local player"));
						ComboBox(Vars::Chams::Players::IgnoreTeammates, { { 0, _(L"Off") }, { 1,_(L"All") }, { 2, _(L"Keep Friends") } });
						CheckBox(Vars::Chams::Players::Wearables, _(L"Draw chams on hats etc."));
						CheckBox(Vars::Chams::Players::Weapons, _(L"Draw chams on weapons"));
						ComboBox(Vars::Chams::Players::Material, { { 0, _(L"None") }, { 1, _(L"Shaded") }, { 2, _(L"Shiny") }, { 3, _(L"Flat") },{ 4, _(L"Glowy") } });
						CheckBox(Vars::Chams::Players::IgnoreZ, _(L"Chams visible trough walls"));
						InputFloat(Vars::Chams::Players::Alpha, 0.0f, 1.0f, 0.05f, L"%.2f");
					}
					GroupBoxEnd(_(L"Players"), 200);

					GroupBoxStart();
					{
						CheckBox(Vars::Chams::Buildings::Active, _(L"Building master switch"));
						CheckBox(Vars::Chams::Buildings::IgnoreTeammates, _(L"Ignore team's buildings"));
						ComboBox(Vars::Chams::Buildings::Material, { { 0, _(L"None") }, { 1, _(L"Shaded") }, { 2, _(L"Shiny") }, { 3, _(L"Flat") }, { 4, _(L"Glowy") } });
						CheckBox(Vars::Chams::Buildings::IgnoreZ, _(L"Chams visible trough walls"));
						InputFloat(Vars::Chams::Buildings::Alpha, 0.0f, 1.0f, 0.05f, L"%.2f");
					}
					GroupBoxEnd(_(L"Buildings"), 200);

					checkpoint.x += 200 + Vars::Menu::SpacingX;
					m_LastWidget = checkpoint;

					GroupBoxStart();
					{
						CheckBox(Vars::Chams::World::Active, _(L"World master switch"));
						CheckBox(Vars::Chams::World::Health, _(L"Draw chams on healthpacks"));
						CheckBox(Vars::Chams::World::Ammo, _(L"Draw chams on ammopacks"));
						ComboBox(Vars::Chams::World::Projectiles, { { 0, _(L"Off") }, { 1, _(L"All") }, { 2, _(L"Enemy Only") } });
						ComboBox(Vars::Chams::World::Material, { { 0, _(L"None") }, { 1, _(L"Shaded") }, { 2, _(L"Shiny") }, { 3, _(L"Flat") }, { 4, _(L"Glowy") } });
						CheckBox(Vars::Chams::World::IgnoreZ, _(L"Chams visible trough walls"));
						InputFloat(Vars::Chams::World::Alpha, 0.0f, 1.0f, 0.05f, L"%.2f");
					}
					GroupBoxEnd(_(L"World"), 200);

					GroupBoxStart();
					{
						CheckBox(Vars::Chams::DME::Active, _(L"DME chams masterswitch"));
						CheckBox(Vars::Chams::DME::SeeThru, _(L"See through"));
						ComboBox(Vars::Chams::DME::Hands, {
							{ 0, _(L"Original") },
							{ 1, _(L"Shaded") },
							{ 2, _(L"Shiny") },
							{ 3, _(L"Flat") },
							{ 4, _(L"WF-Shaded") },
							{ 5, _(L"WF-Shiny") },
							{ 6, _(L"WF-Flat") },
							{ 7, _(L"Glowy") }, });
						InputFloat(Vars::Chams::DME::HandsAlpha, 0.0f, 1.0f, 0.05f, L"%.2f");
						ComboBox(Vars::Chams::DME::Weapon, {
							{ 0, _(L"Original") },
							{ 1, _(L"Shaded") },
							{ 2, _(L"Shiny") },
							{ 3, _(L"Flat") },
							{ 4, _(L"WF-Shaded") },
							{ 5, _(L"WF-Shiny") },
							{ 6, _(L"WF-Flat") },
							{ 7, _(L"Glowy") }, });
						InputFloat(Vars::Chams::DME::WeaponAlpha, 0.0f, 1.0f, 0.05f, L"%.2f");
					}
					GroupBoxEnd(_(L"DME"), 200);

					break;
				}

				case EVisualsTabs::TAB_GLOW:
				{
					GroupBoxStart();
					{
						//CheckBox(Vars::Glow::Main::Active, _(L"Glow master switch"));
						InputInt(Vars::Glow::Main::Scale, 1, 10);
					}
					GroupBoxEnd(_(L"Main"), 200);

					GroupBoxStart();
					{
						CheckBox(Vars::Glow::Players::Active, _(L"Player master switch"));
						CheckBox(Vars::Glow::Players::ShowLocal, _(L"Draw glow on local player"));
						ComboBox(Vars::Glow::Players::IgnoreTeammates, { { 0, _(L"Off") }, { 1, _(L"All") }, { 2, _(L"Keep Friends") } });
						CheckBox(Vars::Glow::Players::Wearables, _(L"Draw glow around hats etc."));
						CheckBox(Vars::Glow::Players::Weapons, _(L"Draw glow around weapons"));
						InputFloat(Vars::Glow::Players::Alpha, 0.1f, 1.0f, 0.05f, L"%.2f");
						ComboBox(Vars::Glow::Players::Color, { { 0, _(L"Default") }, { 1, _(L"Health") } });
					}
					GroupBoxEnd(_(L"Players"), 200);

					GroupBoxStart();
					{
						CheckBox(Vars::Glow::Buildings::Active, _(L"Building master switch"));
						CheckBox(Vars::Glow::Buildings::IgnoreTeammates, _(L"Ignore team's buildings"));
						InputFloat(Vars::Glow::Buildings::Alpha, 0.1f, 1.0f, 0.05f, L"%.2f");
						ComboBox(Vars::Glow::Buildings::Color, { { 0, _(L"Default") }, { 1, _(L"Health") } });
					}
					GroupBoxEnd(_(L"Buildings"), 200);

					GroupBoxStart();
					{
						CheckBox(Vars::Glow::World::Active, _(L"World master switch"));
						CheckBox(Vars::Glow::World::Health, _(L"Draw glow on healthpacks"));
						CheckBox(Vars::Glow::World::Ammo, _(L"Draw glow on ammopacks"));
						ComboBox(Vars::Glow::World::Projectiles, { { 0, _(L"Off") }, { 1, _(L"All") }, { 2, _(L"Enemy Only") } });
						InputFloat(Vars::Glow::World::Alpha, 0.1f, 1.0f, 0.05f, L"%.2f");
					}
					GroupBoxEnd(_(L"World"), 200);

					break;
				}

				case EVisualsTabs::TAB_OTHER:
				{
					Rect_t checkpoint = m_LastWidget;
					GroupBoxStart();
					{
						InputInt(Vars::Visuals::FieldOfView, 70, 140);
						InputFloat(Vars::Visuals::AspectRatioValue, 0.f, 200.f, 1.f, L"%.0f");
						ComboBox(Vars::Visuals::Fullbright, { { 0, _(L"Off") }, { 1, _(L"Classic") }, { 2, _(L"NoShader?") } });
						InputInt(Vars::Visuals::AimFOVAlpha, 0, 255);
						InputInt(Vars::Visuals::ViewModelX, -25, 25, 1);
						InputInt(Vars::Visuals::ViewModelY, -25, 25, 1);
						InputInt(Vars::Visuals::ViewModelZ, -25, 25, 1);
						CheckBox(Vars::Visuals::RemoveScope, _(L"Remove sniper's scope overlay"));
						CheckBox(Vars::Visuals::RemoveZoom, _(L"Remove sniper's scope zoom"));
						CheckBox(Vars::Visuals::RemovePunch, _(L"Remove visual punch/recoil"));
						CheckBox(Vars::Visuals::CrosshairAimPos, _(L"Place crosshair at aimbot's aim position"));
						CheckBox(Vars::Visuals::ChatInfo, _(L"Display enemy class changes on chat"));
					}
					GroupBoxEnd(_(L"Local"), 190);

					GroupBoxStart();
					{
						CheckBox(Vars::Visuals::ThirdPerson, _(L"Thirdperson master switch"));
						InputKey(Vars::Visuals::ThirdPersonKey);
						CheckBox(Vars::Visuals::ThirdPersonSilentAngles, _(L"Show silent angles on thirdperson"));
						CheckBox(Vars::Visuals::ThirdPersonInstantYaw, _(L"Set yaw instantly on thirdperson"));
					}
					GroupBoxEnd(_(L"ThirdPerson"), 190);

					GroupBoxStart();
					{
						CheckBox(Vars::Visuals::SpyWarning, _(L"Spywarning master switch"));
						CheckBox(Vars::Visuals::SpyWarningAnnounce, _(L"Use voice command"));
						CheckBox(Vars::Visuals::SpyWarningVisibleOnly, _(L"Ignore spies that are not visible"));
						CheckBox(Vars::Visuals::SpyWarningIgnoreFriends, _(L"Ignore Steam friend spies"));
						ComboBox(Vars::Visuals::SpyWarningStyle, { { 0, _(L"Indicator") }, { 1, _(L"Flash") } });
					}
					GroupBoxEnd(_(L"SpyWarning"), 190);

					checkpoint.x += 190 + Vars::Menu::SpacingX;
					m_LastWidget = checkpoint;

					GroupBoxStart();
					{
						//ComboBox(Vars::Visuals::SpectatorList, { { 0, _(L"Off") }, { 1, _(L"Default") }, { 2, _(L"Classic") }, { 3, _(L"Classic Avatars") } });
						CheckBox(Vars::Visuals::Snow, _(L"Enable / Disable snowrain on menu"));
						CheckBox(Vars::Visuals::ToolTips, _(L"This is an example tooltip"));
						CheckBox(Vars::Visuals::WorldModulation, _(L"Modulate world materials with custom color"));
					}
					GroupBoxEnd(_(L"Other"), 200);

#ifdef DEVELOPER_BUILD
					GroupBoxStart();
					{
						CheckBox(Vars::Visuals::Skins::Enabled);
						ComboBox(Vars::Visuals::Skins::Effect, {
							{ 0  , L"None" },
							{ 701, L"Hot" },
							{ 702, L"Isotope" },
							{ 703, L"Cool" },
							{ 704, L"Energy Orb" } });
						ComboBox(Vars::Visuals::Skins::Particle, {
							{ 0  , L"None" },
							{ 701, L"Hot" },
							{ 702, L"Isotope" },
							{ 703, L"Cool" },
							{ 704, L"Energy Orb" } });
						ComboBox(Vars::Visuals::Skins::Sheen, {
							{ 0, L"None" },
							{ 1, L"Team Shine" },
							{ 2, L"Deadly Daffodil" },
							{ 3, L"MannDarin" },
							{ 4, L"Mean Green" },
							{ 5, L"Agonizing Emerald" },
							{ 6, L"Villainous Violet" },
							{ 7, L"Hot Rod" } });
						CheckBox(Vars::Visuals::Skins::Acient);
						CheckBox(Vars::Visuals::Skins::Override);

						if (Button(L"Set Current"))
							g_AttributeChanger.m_bSet = true;

						if (Button(L"Save All"))
							g_AttributeChanger.m_bSave = true;

						if (Button(L"Load All"))
							g_AttributeChanger.m_bLoad = true;

					}
					GroupBoxEnd(_(L"Weapon Attributes"), 150);
#endif
					break;
				}

				case EVisualsTabs::TAB_COLORS:
				{
					Rect_t checkpoint = m_LastWidget;

					GroupBoxStart();
					{
						//InputColor(Vars::Menu::Colors::WindowBackground, _(L"Window Background"));
						//InputColor(Vars::Menu::Colors::TitleBar, _(L"Title Bar"));
						//InputColor(Vars::Menu::Colors::Text, _(L"Text"));
						//InputColor(Vars::Menu::Colors::Widget, _(L"Widget"));
						InputColor(Vars::Menu::Colors::WidgetActive, _(L"Widget Active"));
						InputColor(Vars::Menu::Colors::OutlineMenu, _(L"Outline Menu"));
					}
					GroupBoxEnd(_(L"Menu"), 250);

					checkpoint.x += 250 + Vars::Menu::SpacingX;
					m_LastWidget = checkpoint;

					GroupBoxStart();
					{
						InputColor(Colors::OutlineESP, _(L"Outline ESP"));
						InputColor(Colors::Cond, _(L"Cond"));
						InputColor(Colors::Target, _(L"Target"));
						InputColor(Colors::Invuln, _(L"Invuln"));
						InputColor(Colors::Cloak, _(L"Cloak"));
						InputColor(Colors::Friend, _(L"Friend"));
						InputColor(Colors::Overheal, _(L"Overheal"));
						InputColor(Colors::Health, _(L"Health"));
						InputColor(Colors::Ammo, _(L"Ammo"));
						InputColor(Colors::UberColor, _(L"Uber"));
						InputColor(Colors::TeamRed, _(L"Team RED"));
						InputColor(Colors::TeamBlu, _(L"Team BLU"));
						InputColor(Colors::Hands, _(L"Hands"));
						InputColor(Colors::Weapon, _(L"Weapon"));
						InputColor(Colors::FresnelBase, _(L"Fresnel Base"));
						InputColor(Colors::WorldModulation, _(L"World Color"));
						InputColor(Colors::StaticPropModulation, _(L"Prop Color"));
						InputColor(Colors::FOVCircle, _(L"FOV Circle"));
						InputColor(Colors::Bones, _(L"Bone color"));
					}
					GroupBoxEnd(_(L"Others"), 250);

					break;
				}
				}

				break;
			}

			case EMainTabs::TAB_MISC:
			{
				static EMiscTabs Tab = EMiscTabs::TAB_MAIN;
				{
					Rect_t checkpoint_line = m_LastWidget;
					checkpoint_line.x -= Vars::Menu::SpacingX;
					checkpoint_line.y += Vars::Menu::ButtonHSmall + (Vars::Menu::SpacingY * 2);
					Rect_t checkpoint_move = m_LastWidget;

					if (Button(_(L"Main"), Tab == EMiscTabs::TAB_MAIN, Vars::Menu::ButtonWSmall, Vars::Menu::ButtonHSmall))
						Tab = EMiscTabs::TAB_MAIN;

					checkpoint_move.x += Vars::Menu::ButtonWSmall + Vars::Menu::SpacingX;
					m_LastWidget = checkpoint_move;

					if (Button(_(L"HvH"), Tab == EMiscTabs::TAB_HVH, Vars::Menu::ButtonWSmall, Vars::Menu::ButtonHSmall))
						Tab = EMiscTabs::TAB_HVH;

					m_LastWidget = checkpoint_line;
					//g_Draw.Line(checkpoint_line.x, checkpoint_line.y, Vars::Menu::Position.x + Vars::Menu::Position.w - 1, checkpoint_line.y, Vars::Menu::Colors::OutlineMenu);
					checkpoint_line.x += Vars::Menu::SpacingX;
					checkpoint_line.y += Vars::Menu::SpacingY;
					m_LastWidget = checkpoint_line;
				}

				switch (Tab)
				{
				case EMiscTabs::TAB_MAIN:
				{
					GroupBoxStart();
					{
						CheckBox(Vars::Misc::AutoJump, _(L"Automatically bunnyhop"));
						CheckBox(Vars::Misc::AutoStrafe, _(L"Automatically strafe"));
						CheckBox(Vars::Misc::EdgeJump, _(L"Jump before falling from an edge"));
						CheckBox(Vars::Misc::TauntSlide, _(L"Allow user input during taunts"));
						CheckBox(Vars::Misc::TauntControl, _(L"Gives better control with taunt slide"));
						CheckBox(Vars::Misc::BypassPure, _(L"Bypass sv_pure"));
						CheckBox(Vars::Misc::MedalFlip, _(L"Make menu medal flip always when clicked"));
						CheckBox(Vars::Misc::NoisemakerSpam, _(L"Spam noisemaker"));
						CheckBox(Vars::Misc::AutoRocketJump, _(L"Automatically rocketjump when mouse2 held"));
						CheckBox(Vars::Misc::ChatSpam, _(L"Advertise a free cheat, will ya? x)"));
						CheckBox(Vars::Misc::NoPush, _(L"Prevent players from pushing you"));
					}
					GroupBoxEnd(_(L"Main"), 210);

					GroupBoxStart();
					{
						InputKey(Vars::Misc::CL_Move::RechargeKey, false);
						InputKey(Vars::Misc::CL_Move::DoubletapKey, false);
						//InputKey(Vars::Misc::CL_Move::TeleportKey, false);
						CheckBox(Vars::Misc::CL_Move::WaitForDT, _(L"Wait for DT."));
						CheckBox(Vars::Misc::CL_Move::NotInAir, _(L"Doesn't DT in air."));
					}
					GroupBoxEnd(_(L"Tickbase Exploits"), 210);

					break;
				}

				case EMiscTabs::TAB_HVH:
				{
					GroupBoxStart();
					{
						CheckBox(Vars::AntiHack::AntiAim::Active, _(L"Anti-Aim master switch"));
						ComboBox(Vars::AntiHack::AntiAim::Pitch, { { 0, _(L"None") }, { 1, _(L"Up") }, { 2, _(L"Down") }, { 3, _(L"Fake Up") }, { 4, _(L"Fake Down") } });
						ComboBox(Vars::AntiHack::AntiAim::YawReal, { { 0, _(L"None") }, { 1, _(L"Left") }, { 2, _(L"Right") }, { 3, _(L"Backwards") } });
						ComboBox(Vars::AntiHack::AntiAim::YawFake, { { 0, _(L"None") }, { 1, _(L"Left") }, { 2, _(L"Right") }, { 3, _(L"Backwards") } });
					}
					GroupBoxEnd(_(L"AntiAim"), 180);
					break;
				}
				}

				break;
			}

			case EMainTabs::TAB_CONFIGS:
			{
				Rect_t checkpoint = m_LastWidget;
				static std::wstring selected = {};
				int config_count = 0;

				for (const auto& entry : std::filesystem::directory_iterator(g_CFG.m_sConfigPath))
				{
					if (std::string(std::filesystem::path(entry).filename().string()).find(_(".CAM")) == std::string_view::npos)
						continue;

					config_count++;
				}

				GroupBoxStart();
				{
					if (config_count < 15)
					{
						std::wstring output = {};

						if (InputString(_(L"Add"), output))
						{
							if (!std::filesystem::exists(g_CFG.m_sConfigPath + L"\\" + output))
								g_CFG.Save(output.c_str());
						}
					}

					if (!selected.empty())
					{
						if (Button(_(L"Save"))) {
							g_CFG.Save(selected.c_str());
							selected.clear();
						}

						if (Button(_(L"Load"))) {
							g_CFG.Load(selected.c_str());
							selected.clear();
						}

						if (Button(_(L"Remove"))) {
							g_CFG.Remove(selected.c_str());
							selected.clear();
						}
					}
				}
				GroupBoxEnd(selected.empty() ? _(L"Manage(not selected)")
					: std::wstring(_(L"Manage(") + selected + _(L")")).c_str(), (Vars::Menu::InputBoxW * 2) + (Vars::Menu::SpacingX * 2));

				m_LastWidget.y += Vars::Menu::SpacingY;
				checkpoint.x += (Vars::Menu::InputBoxW * 2) + (Vars::Menu::SpacingX * 3);
				m_LastWidget = checkpoint;

				GroupBoxStart();
				{
					for (const auto& entry : std::filesystem::directory_iterator(g_CFG.m_sConfigPath))
					{
						if (std::string(std::filesystem::path(entry).filename().string()).find(_(".CAM")) == std::string_view::npos)
							continue;

						std::wstring s = entry.path().filename().wstring();
						s.erase(s.end() - 4, s.end());

						if (Button(s.c_str(), s == selected, (Vars::Menu::InputBoxW * 2)))
							selected = s;
					}
				}
				GroupBoxEnd(std::wstring(_(L"Configs(") + std::to_wstring(config_count) + _(L")")).c_str(), (Vars::Menu::InputBoxW * 2) + (Vars::Menu::SpacingX * 2));

				break;
			}

			default: break;
			}

		}
		/*
		g_Draw.Rect(g_InputHelper.m_nMouseX + 1, g_InputHelper.m_nMouseY, 1, 17, {0, 0, 0, 255});
		for (int i = 0; i < 11; i++) {
			g_Draw.Rect(g_InputHelper.m_nMouseX + 2 + i, g_InputHelper.m_nMouseY + 1 + i, 1, 1, {0, 0, 0, 255});
		}
		g_Draw.Rect(g_InputHelper.m_nMouseX + 8, g_InputHelper.m_nMouseY + 12, 5, 1, {0, 0, 0, 255});
		g_Draw.Rect(g_InputHelper.m_nMouseX + 8, g_InputHelper.m_nMouseY + 13, 1, 1, {0, 0, 0, 255});
		g_Draw.Rect(g_InputHelper.m_nMouseX + 9, g_InputHelper.m_nMouseY + 14, 1, 2, {0, 0, 0, 255});
		g_Draw.Rect(g_InputHelper.m_nMouseX + 10, g_InputHelper.m_nMouseY + 16, 1, 2, {0, 0, 0, 255});
		g_Draw.Rect(g_InputHelper.m_nMouseX + 8, g_InputHelper.m_nMouseY + 18, 2, 1, {0, 0, 0, 255});
		g_Draw.Rect(g_InputHelper.m_nMouseX + 7, g_InputHelper.m_nMouseY + 16, 1, 2, {0, 0, 0, 255});
		g_Draw.Rect(g_InputHelper.m_nMouseX + 6, g_InputHelper.m_nMouseY + 14, 1, 2, {0, 0, 0, 255});
		g_Draw.Rect(g_InputHelper.m_nMouseX + 5, g_InputHelper.m_nMouseY + 13, 1, 1, {0, 0, 0, 255});
		g_Draw.Rect(g_InputHelper.m_nMouseX + 4, g_InputHelper.m_nMouseY + 14, 1, 1, {0, 0, 0, 255});
		g_Draw.Rect(g_InputHelper.m_nMouseX + 3, g_InputHelper.m_nMouseY + 15, 1, 1, {0, 0, 0, 255});
		g_Draw.Rect(g_InputHelper.m_nMouseX + 2, g_InputHelper.m_nMouseY + 16, 1, 1, {0, 0, 0, 255});
		for (int i = 0; i < 4; i++) {
			g_Draw.Rect(g_InputHelper.m_nMouseX + 2 + i, g_InputHelper.m_nMouseY + 2 + i, 1, 14 - (i * 2), Vars::Menu::Colors::WidgetActive);
		}
		g_Draw.Rect(g_InputHelper.m_nMouseX + 6, g_InputHelper.m_nMouseY + 6, 1, 8, Vars::Menu::Colors::WidgetActive);
		g_Draw.Rect(g_InputHelper.m_nMouseX + 7, g_InputHelper.m_nMouseY + 7, 1, 9, Vars::Menu::Colors::WidgetActive);
		for (int i = 0; i < 4; i++) {
			g_Draw.Rect(g_InputHelper.m_nMouseX + 8 + i, g_InputHelper.m_nMouseY + 8 + i, 1, 4 - i, Vars::Menu::Colors::WidgetActive);
		}
		g_Draw.Rect(g_InputHelper.m_nMouseX + 8, g_InputHelper.m_nMouseY + 14, 1, 4, Vars::Menu::Colors::WidgetActive);
		g_Draw.Rect(g_InputHelper.m_nMouseX + 9, g_InputHelper.m_nMouseY + 16, 1, 2, Vars::Menu::Colors::WidgetActive);
		*/
	}

	g_Interfaces.Surface->DrawSetAlphaMultiplier(1.0f);
}