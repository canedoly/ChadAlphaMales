#include "EngineVGuiHook.h"

#include "../../SDK/Includes/icons.h"
#include "../../Features/Menu/Menu.h"
#include "../../Features/SpectatorList/SpectatorList.h"
#include "../../Features/SpyWarning/SpyWarning.h"
#include "../../Features/ESP/ESP.h"
#include "../../Features/Misc/Misc.h"
#include "../../Features/Radar/Radar.h"
#include "../../Features/Visuals/Visuals.h"
#include "../../Features/Playerlist/Playerlist.h"

int ticksChoked = 0;
std::time_t CurzTime = std::time(nullptr);

std::string comp_name() {

	char buff[MAX_PATH];
	GetEnvironmentVariableA("USERNAME", buff, MAX_PATH);

	return std::string(buff);
}

void __stdcall EngineVGuiHook::Paint::Hook(int mode)
{
	static auto StartDrawing = reinterpret_cast<void(__thiscall*)(void*)>(g_Pattern.Find(_(L"vguimatsurface.dll"), _(L"55 8B EC 64 A1 ? ? ? ? 6A FF 68 ? ? ? ? 50 64 89 25 ? ? ? ? 83 EC 14")));
	static auto FinishDrawing = reinterpret_cast<void(__thiscall*)(void*)>(g_Pattern.Find(_(L"vguimatsurface.dll"), _(L"55 8B EC 6A FF 68 ? ? ? ? 64 A1 ? ? ? ? 50 64 89 25 ? ? ? ? 51 56 6A 00")));

	if (!g_ScreenSize.w || !g_ScreenSize.h)
		g_ScreenSize.Update();

	//HACK: for some reason we need to do this
	{
		static bool bInitIcons = false;

		if (!bInitIcons)
		{
			for (int nIndex = 0; nIndex < ICONS::TEXTURE_AMOUNT; nIndex++)
			{
				ICONS::ID[nIndex] = -1;
				g_Draw.Texture(-200, 0, 18, 18, Colors::White, nIndex);
			}

			bInitIcons = true;
		}
	}

	Table.Original<fn>(index)(g_Interfaces.EngineVGui, mode);

	if (mode & PAINT_UIPANELS)
	{
		//Update W2S
		{
			CViewSetup ViewSetup = {};

			if (g_Interfaces.Client->GetPlayerView(ViewSetup)) {
				VMatrix WorldToView = {}, ViewToProjection = {}, WorldToPixels = {};
				g_Interfaces.RenderView->GetMatricesForView(ViewSetup, &WorldToView, &ViewToProjection, &g_GlobalInfo.m_WorldToProjection, &WorldToPixels);
			}
		}

		StartDrawing(g_Interfaces.Surface);
		{
			g_ESP.Run();

			auto OtherDraws = [&]() -> void
			{
				if (g_Interfaces.EngineVGui->IsGameUIVisible())
					return;

				//Tickbase info
				if (Vars::Misc::CL_Move::Enabled)
				{
					const auto& pLocal = g_EntityCache.m_pLocal;
					const auto& pWeapon = g_EntityCache.m_pLocalWeapon;

					if (pLocal && pWeapon)
					{
						if (pLocal->GetLifeState() == LIFE_ALIVE)
						{

							const int nY = (g_ScreenSize.h / 2) + 20;

							int ticks;

							if (pLocal->GetClassNum() == CLASS_HEAVY) {

								for (int i = MAX_NEW_COMMANDS_HEAVY; i >= 0; i--) {
									//printf("i: %d\n", i);
									for (int j = MAX_NEW_COMMANDS_HEAVY - g_GlobalInfo.m_nShifted; j <= MAX_NEW_COMMANDS_HEAVY; j++) {
										//printf("j: %d\n", j);
										ticksChoked = j;
										break;
									}
								}
								ticks = MAX_NEW_COMMANDS_HEAVY;
							}
							else {
								for (int i = g_GlobalInfo.MaxNewCommands; i >= 0; i--) {
									//printf("i: %d\n", i);
									for (int j = g_GlobalInfo.MaxNewCommands - g_GlobalInfo.m_nShifted; j <= g_GlobalInfo.MaxNewCommands; j++) {
										//printf("j: %d\n", j);
										ticksChoked = j;
										break;
									}
								}
								ticks = g_GlobalInfo.MaxNewCommands;
							}
							int tickWidth = 5;
							int barWidth = (tickWidth * ticks) + 2;

							g_Draw.Rect(g_ScreenSize.c - (barWidth / 2), nY + 50, barWidth, 6, { 40,40,40,255 });
							g_Draw.OutlinedRect(g_ScreenSize.c - (barWidth / 2),nY + 50, barWidth,6,{ 0, 0, 0, 255 });
							g_Draw.GradientRect(g_ScreenSize.c - (barWidth / 2) + 1, nY + 51, (g_ScreenSize.c - (barWidth / 2) + 1) + tickWidth * ticksChoked, nY + 51 + 4, { 0,0,0,255 }, Vars::Menu::Colors::WidgetActive, true);
						}
					}
				}
				//Current Active Aimbot FOV
				if (Vars::Visuals::AimFOVAlpha && g_GlobalInfo.m_flCurAimFOV)
				{
					if (const auto &pLocal = g_EntityCache.m_pLocal)
					{
						float flFOV = static_cast<float>(Vars::Visuals::FieldOfView);
						float flR = tanf(DEG2RAD(g_GlobalInfo.m_flCurAimFOV) / 2.0f)
							/ tanf(DEG2RAD((pLocal->IsScoped() && !Vars::Visuals::RemoveZoom) ? 30.0f : flFOV) / 2.0f) * g_ScreenSize.w;
						Color_t clr = Colors::FOVCircle;
						clr.a = static_cast<byte>(Vars::Visuals::AimFOVAlpha);
						g_Draw.OutlinedCircle(g_ScreenSize.w / 2, g_ScreenSize.h / 2, flR, 68, clr);
					}
				}
			};
			OtherDraws();

			g_Visuals.RunEventLogs();
			g_Misc.BypassPure();
			g_SpyWarning.Run();
			g_SpectatorList.Run();
			//g_Playerlist.DrawPlayers();
			g_Radar.Run();
			//g_Menu.Run(); // Goodbye old menu :wave:

			int width = g_ScreenSize.w;
			int	height = g_ScreenSize.h;
			static wchar_t buff[512];
			int label_w, label_h;
			static int nignog = 0;
			auto fps = 0;

			if (nignog == 100) { // I tried to make it slower, I failed so hard. How unfortunate :(
				fps = static_cast<int>(1.f / g_Interfaces.GlobalVars->frametime);
			}
			else {
				nignog++;
			}
			int ms = std::max(0, (int)std::round(g_GlobalInfo.m_Latency * 1000.f));

			_snwprintf(buff, sizeof(buff), _(L"CAM [v1.1] | fps: %i | delay: %ims"), fps, ms);

			g_Interfaces.Surface->GetTextSize(g_Draw.m_vecFonts[FONT_MENU].dwFont, buff, label_w, label_h);

			int boxw = label_w + 10;

			if (g_Interfaces.Engine->IsInGame())
			{
				auto nci = g_Interfaces.Engine->GetNetChannelInfo();

				if (nci)
				{
					//ms = 1.f / nci->GetLatency(FLOW_INCOMING);
					//ms = std::max(0, (int)std::round(g_GlobalInfo.m_Latency * 1000.f));

					g_Draw.Rect(width - 10 - boxw, 11, boxw, 18, { 0, 0, 0, 200 });
					g_Draw.Rect(width - 10 - boxw, 10, boxw, 2, Vars::Menu::Colors::WidgetActive);
					g_Draw.String(FONT_MENU, width - 10 - boxw + 5, 20, { 255,255,255,255 }, ALIGN_CENTERVERTICAL, buff);
				}
			}
		}	
		FinishDrawing(g_Interfaces.Surface);
	}
}