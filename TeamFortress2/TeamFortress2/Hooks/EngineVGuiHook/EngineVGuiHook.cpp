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
				if (Vars::Misc::CL_Move::Doubletap.m_Var)
				{
					const auto& pLocal = g_EntityCache.m_pLocal;
					const auto& pWeapon = g_EntityCache.m_pLocalWeapon;

					if (pLocal && pWeapon)
					{
						if (pLocal->GetLifeState() == LIFE_ALIVE)
						{
							const int nY = (g_ScreenSize.h / 2) + 20;
							/*auto vel = pLocal->GetVelocity();
							vel.z = 0;
							auto speed = vel.Lenght2D();
							auto speed2 = vel.Lenght();
							g_Draw.String(FONT_MENU, g_ScreenSize.c, g_ScreenSize.h / 2, { 255,255,255,255 }, ALIGN_CENTERHORIZONTAL, std::string("Length2D: " + std::to_string(speed)).c_str());
							g_Draw.String(FONT_MENU, g_ScreenSize.c, (g_ScreenSize.h / 2) + 20, { 255,255,255,255 }, ALIGN_CENTERHORIZONTAL, std::string("Length: " + std::to_string(speed2)).c_str());
							
							g_Draw.String(FONT_MENU, 0, nY + 00, { 255,255,255,255 }, ALIGN_DEFAULT, std::string("Charged: " + std::to_string(dt.Charged)).c_str());
							g_Draw.String(FONT_MENU, 0, nY + 20, { 255,255,255,255 }, ALIGN_DEFAULT, std::string("Fast stop: " + std::to_string(dt.FastStop)).c_str());
							g_Draw.String(FONT_MENU, 0, nY + 40, { 255,255,255,255 }, ALIGN_DEFAULT, std::string("Recharging: " + std::to_string(dt.Recharging)).c_str());
							g_Draw.String(FONT_MENU, 0, nY + 60, { 255,255,255,255 }, ALIGN_DEFAULT, std::string("Shifting: " + std::to_string(dt.Shifting)).c_str());
							g_Draw.String(FONT_MENU, 0, nY + 80, { 255,255,255,255 }, ALIGN_DEFAULT, std::string("Ticks to shift: " + std::to_string(dt.ToShift)).c_str());
							g_Draw.String(FONT_MENU, 0, nY + 99, { 255,255,255,255 }, ALIGN_DEFAULT, std::string("Wait ticks: " + std::to_string(dt.ToWait)).c_str());*/
							int ticks = dt.Charged;
							int tickWidth = 5;
							int barWidth = (tickWidth * 24) + 2;
							g_Draw.Rect(g_ScreenSize.c - (barWidth / 2), nY + 50, barWidth, 6, { 40,40,40,255 });
							g_Draw.OutlinedRect(g_ScreenSize.c - (barWidth / 2),nY + 50, barWidth,6,{ 0, 0, 0, 255 });
							g_Draw.GradientRect(g_ScreenSize.c - (barWidth / 2) + 1, nY + 51, (g_ScreenSize.c - (barWidth / 2) + 1) + tickWidth * dt.Charged, nY + 51 + 4, { 0,0,0,255 }, Vars::Menu::Colors::WidgetActive, true);
						}
					}
				}
				//Current Active Aimbot FOV
				if (Vars::Visuals::AimFOVAlpha.m_Var && g_GlobalInfo.m_flCurAimFOV)
				{
					if (const auto &pLocal = g_EntityCache.m_pLocal)
					{
						float flFOV = static_cast<float>(Vars::Visuals::FieldOfView.m_Var);
						float flR = tanf(DEG2RAD(g_GlobalInfo.m_flCurAimFOV) / 2.0f)
							/ tanf(DEG2RAD((pLocal->IsScoped() && !Vars::Visuals::RemoveZoom.m_Var) ? 30.0f : flFOV) / 2.0f) * g_ScreenSize.w;
						Color_t clr = Colors::FOVCircle;
						clr.a = static_cast<byte>(Vars::Visuals::AimFOVAlpha.m_Var);
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
			static auto fps = 0;

			static float flPressedTime = g_Interfaces.Engine->Time();
			float flElapsed = g_Interfaces.Engine->Time() - flPressedTime;

			if (flElapsed > 0.4f) {
				fps = static_cast<int>(1.f / g_Interfaces.GlobalVars->frametime);
				flPressedTime = g_Interfaces.Engine->Time();
			}

			int ms = std::max(0, (int)std::round(g_GlobalInfo.m_Latency * 1000.f));
			// Change admin to something else if you really need to, it was for the protection thing
			_snwprintf(buff, sizeof(buff), _(L"CAM [v1.2b] | admin | fps: %i | delay: %ims") ,fps, ms);

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