#include "EngineVGuiHook.h"

#include "../../SDK/Includes/icons.h"
#include "../../Features/Menu/Menu.h"
#include "../../Features/SpectatorList/SpectatorList.h"
#include "../../Features/SpyWarning/SpyWarning.h"
#include "../../Features/ESP/ESP.h"
#include "../../Features/Misc/Misc.h"
#include "../../Features/Radar/Radar.h"
#include "../../Features/Aimbot/AimbotMelee/AimbotMelee.h"
#include "../../Features/Visuals/Visuals.h"

int ticksChoked = 0;

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

				//Projectile Aim's Predicted Position
				if (!g_GlobalInfo.m_vPredictedPos.IsZero())
				{
					if (Vars::Visuals::CrosshairAimPos.m_Var ? g_GlobalInfo.m_vAimPos.IsZero() : true)
					{
						static const int size = 10;
						Vec3 vecScreen = Vec3();

						if (Utils::W2S(g_GlobalInfo.m_vPredictedPos, vecScreen))
						{
							g_Draw.OutlinedCircle(static_cast<int>(vecScreen.x - (size / 2)) - 1, static_cast<int>(vecScreen.y - (size / 2)) - 1, 4, 15, {0,0,0,255});
							g_Draw.OutlinedCircle(static_cast<int>(vecScreen.x - (size / 2)) - 1, static_cast<int>(vecScreen.y - (size / 2)) - 1, 3, 15, Colors::Target);
							g_Draw.OutlinedCircle(static_cast<int>(vecScreen.x - (size / 2)) - 1, static_cast<int>(vecScreen.y - (size / 2)) - 1, 2, 15, { 0,0,0,255 });
							/*
							g_Draw.OutlinedRect(
								static_cast<int>(vecScreen.x - (size / 2)) - 1,
								static_cast<int>(vecScreen.y - (size / 2)) - 1,
								size + 2, size + 2,
								Colors::OutlineESP);
								*/
						}
					}
				}

				//Tickbase info
				/*
				if (Vars::Misc::CL_Move::Enabled.m_Var)
				{
					const auto& pLocal  = g_EntityCache.m_pLocal;
					const auto& pWeapon = g_EntityCache.m_pLocalWeapon;

					if (pLocal && pWeapon)
					{
						if (pLocal->GetLifeState() == LIFE_ALIVE)
						{
							const int nY = (g_ScreenSize.h / 2) + 20;

							if (g_GlobalInfo.m_nShifted)
								g_Draw.String(FONT_ESP_NAME_OUTLINED, g_ScreenSize.c, nY, { 255, 64, 64, 255 }, ALIGN_CENTERHORIZONTAL, _(L"Recharge! (%i / %i)"), g_GlobalInfo.m_nShifted, MAX_NEW_COMMANDS);
							else if (!g_GlobalInfo.m_nShifted && g_GlobalInfo.m_nWaitForShift)
								g_Draw.String(FONT_ESP_NAME_OUTLINED, g_ScreenSize.c, nY, { 255, 178, 0, 255 }, ALIGN_CENTERHORIZONTAL, _(L"Wait! (%i / %i)"), g_GlobalInfo.m_nWaitForShift, DT_WAIT_CALLS);
							else if (!g_GlobalInfo.m_nShifted && !g_GlobalInfo.m_nWaitForShift && pLocal->GetClassNum() == CLASS_HEAVY && pWeapon->GetSlot() == SLOT_PRIMARY && !pLocal->GetVecVelocity().IsZero())
								g_Draw.String(FONT_ESP_NAME_OUTLINED, g_ScreenSize.c, nY, { 255, 178, 0, 255 }, ALIGN_CENTERHORIZONTAL, _(L"Stop!"));
							else
								g_Draw.String(FONT_ESP_NAME_OUTLINED, g_ScreenSize.c, nY, { 153, 255, 153, 255 }, ALIGN_CENTERHORIZONTAL, _(L"Shift ready!"));
						}
					}
				}
				*/

				//Tickbase info
				if (Vars::Misc::CL_Move::Enabled.m_Var)
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
								for (int i = MAX_NEW_COMMANDS; i >= 0; i--) {
									//printf("i: %d\n", i);
									for (int j = MAX_NEW_COMMANDS - g_GlobalInfo.m_nShifted; j <= MAX_NEW_COMMANDS; j++) {
										//printf("j: %d\n", j);
										ticksChoked = j;
										break;
									}
								}
								ticks = MAX_NEW_COMMANDS;
							}
							
							//g_AimbotProjectile.DrawTrace(Trace);
							/*
							if (g_GlobalInfo.m_nShifted)
								g_Draw.String(FONT_MENU, g_ScreenSize.c, nY + 60, { 255, 64, 64, 255 }, ALIGN_CENTERHORIZONTAL, _(L"Recharging (%i / %i)"), g_GlobalInfo.m_nShifted, MAX_NEW_COMMANDS);
							else if (!g_GlobalInfo.m_nShifted && g_GlobalInfo.m_nWaitForShift)
								g_Draw.String(FONT_MENU, g_ScreenSize.c, nY + 60, { 255, 178, 0, 255 }, ALIGN_CENTERHORIZONTAL, _(L"Not ready (%i / %i)"), g_GlobalInfo.m_nWaitForShift, DT_WAIT_CALLS);
							else
								g_Draw.String(FONT_MENU, g_ScreenSize.c, nY + 60, { 153, 255, 153, 255 }, ALIGN_CENTERHORIZONTAL, _(L"Ready"));
							*/
							//g_Draw.String(FONT_MENU, g_ScreenSize.c, nY - 60, { 255, 64, 64, 255 }, ALIGN_CENTERHORIZONTAL, _(L"Ticks Choked: %i "), ticksChoked);
							//g_Draw.String(FONT_MENU, g_ScreenSize.c, nY - 100, { 255, 64, 64, 255 }, ALIGN_CENTERHORIZONTAL, Vars::Skybox::SkyboxName.c_str());
							int tickWidth = 5;
							int barWidth = (tickWidth * ticks) + 2;

							g_Draw.Rect(g_ScreenSize.c - (barWidth / 2), nY + 50, barWidth, 6, { 40,40,40,255 });
							g_Draw.OutlinedRect(g_ScreenSize.c - (barWidth / 2),nY + 50, barWidth,6,{ 0, 0, 0, 255 });
							g_Draw.GradientRect(g_ScreenSize.c - (barWidth / 2) + 1, nY + 51, (g_ScreenSize.c - (barWidth / 2) + 1) + tickWidth * ticksChoked, nY + 51 + 4, { 0,0,0,255 }, Vars::Menu::Colors::WidgetActive, true);
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
			g_Radar.Run();
			g_Menu.Run();
			//int label_w, label_h;
			int ms = std::max(0, (int)std::round(g_GlobalInfo.m_Latency * 1000.f));
			//g_Interfaces.Surface->GetTextSize(g_Draw.m_vecFonts[FONT_MENU].dwFont, _(L"CAM.club"), label_w, label_h);
			/*
			if (g_EntityCache.m_pLocal) {
			//g_Draw.GradientRect(g_ScreenSize.w - label_w - 10, 0, g_ScreenSize.w, label_h + 10, { 0,0,0,0 }, { 0,0,0,255 }, true);
				g_Draw.String(FONT_MENU, g_ScreenSize.w - 120, 5, { 255, 255, 255, 255 }, ALIGN_DEFAULT, _("CAM.club | %ims"), ms);
			//g_Draw.String(FONT_MENU, g_ScreenSize.w - label_w - 10, 5, Vars::Menu::Colors::WidgetActive, ALIGN_DEFAULT, ".club");
			}
			else {
				g_Draw.String(FONT_MENU, g_ScreenSize.w - 80, 5, { 255, 255, 255, 255 }, ALIGN_DEFAULT, _("CAM.club"));
			}
			*/
		}	
		FinishDrawing(g_Interfaces.Surface);
	}
}