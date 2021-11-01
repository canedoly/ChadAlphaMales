#include "ClientModeHook.h"

#include "../../Features/Prediction/Prediction.h"
#include "../../Features/Aimbot/Aimbot.h"
#include "../../Features/Auto/Auto.h"
#include "../../Features/Misc/Misc.h"
#include "../../Features/Visuals/Visuals.h"
#include "../../Features/AntiHack/AntiAim.h"
#include "../../Features/KatzeB0t/AntiCat.h"
#include "../../SDK/Timer.h"

//#include "../../Features/KatzeB0t/AntiCat.h"

void __stdcall ClientModeHook::OverrideView::Hook(CViewSetup* pView)
{
	Table.Original<fn>(index)(g_Interfaces.ClientMode, pView);
	g_Visuals.FOV(pView);
	g_Visuals.OffsetCamera(pView);
	g_Visuals.Freecam(pView);
}

bool __stdcall ClientModeHook::ShouldDrawViewModel::Hook()
{
	if (const auto& pLocal = g_EntityCache.m_pLocal)
	{
		if (pLocal->IsScoped() && Vars::Visuals::RemoveScope.m_Var && Vars::Visuals::RemoveZoom.m_Var && !g_Interfaces.Input->CAM_IsThirdPerson())
			return true;

		/*if (g_GlobalInfo.m_bAAActive) {
			CTFPlayerAnimState* pAnimState = pLocal->GetAnimState();

			static CTFPlayerAnimState* AnimStateBackup();
			memcpy(&AnimStateBackup, pAnimState, sizeof(CTFPlayerAnimState));
			std::array<float, 24> PoseParamBackup = pLocal->GetPoseParam();

			pAnimState->Update(g_GlobalInfo.m_vFakeViewAngles.y, g_GlobalInfo.m_vFakeViewAngles.x);
			matrix3x4 cumcumcucmucumcum[128];
			pLocal->SetupBones(cumcumcucmucumcum, 128, BONE_USED_BY_ANYTHING, 0.0f);

			memcpy(pAnimState, &AnimStateBackup, sizeof(CTFPlayerAnimState));
			pLocal->GetPoseParam() = PoseParamBackup;
		}*/
	}

	return Table.Original<fn>(index)(g_Interfaces.ClientMode);
}

Timer AntiAfkTimer{  };
int last_buttons{ 0 };

static void updateAntiAfk(CUserCmd* pCmd)
{
	if (pCmd->buttons != last_buttons) {
		AntiAfkTimer.update();
		last_buttons = pCmd->buttons;
	}
	else {
		if (g_ConVars.afkTimer->GetInt() != 0 && AntiAfkTimer.check(g_ConVars.afkTimer->GetInt() * 60 * 1000 - 10000)) {
			bool flip = false;
			pCmd->buttons |= flip ? IN_FORWARD : IN_BACK;
			flip = !flip;
			g_Visuals.AddToEventLog(_("Attempting to prevent AFK kick..."));
			g_Interfaces.CVars->ConsoleColorPrintf({ 150, 255, 0, 255 }, _("Attempting to prevent AFK kick...\n"));
			g_Interfaces.ClientMode->m_pChatElement->ChatPrintf(0, "\x4[CAM]\x1 Attempting to prevent AFK kick...");
			if (AntiAfkTimer.check(g_ConVars.afkTimer->GetInt() * 60 * 1000 + 1000))
			{
				AntiAfkTimer.update();
			}
		}
		last_buttons = pCmd->buttons;
	}
}

int badcode = 0;
bool __stdcall ClientModeHook::CreateMove::Hook(float input_sample_frametime, CUserCmd* pCmd)
{
	if (gMenu.menuOpen) {
		g_Interfaces.InputSystem->ResetInputState();
		g_Interfaces.InputSystem->EnableInput(false);
	}
	g_GlobalInfo.m_bSilentTime = false;
	g_GlobalInfo.m_bAttacking = false;

	fn OriginalFn = Table.Original<fn>(index);

	if (!pCmd || !pCmd->command_number)
		return OriginalFn(g_Interfaces.ClientMode, input_sample_frametime, pCmd);

	if (OriginalFn(g_Interfaces.ClientMode, input_sample_frametime, pCmd))
		g_Interfaces.Prediction->SetLocalViewAngles(pCmd->viewangles);

	if (dt.Charged == 0 && dt.barAlpha > 0) {
		if (!dt.barAlpha - 3 < 0) {
			dt.barAlpha -= 3;
		}
	}

	uintptr_t _bp; __asm mov _bp, ebp;
	bool* pSendPacket = (bool*)(***(uintptr_t***)_bp - 0x1);

	int nOldFlags = 0;
	Vec3 vOldAngles = pCmd->viewangles;
	float fOldSide = pCmd->sidemove;
	float fOldForward = pCmd->forwardmove;

	auto AntiWarp = [](CUserCmd* cmd) -> void
	{
		int shiftcheck = dt.ChargedReverse;
		auto pLocal = GLOCAL;

		if (shiftcheck < 19)
		{
			if (shiftcheck < 6)
			{
				// should be -1 btw
				cmd->forwardmove *= -1;
				cmd->sidemove *= -1;
			}
			else
			{
				cmd->forwardmove = 0;
				cmd->sidemove = 0;
			}
		}
		else {
			dt.FastStop = false;
		}
	};

	if (dt.FastStop) {
		AntiWarp(pCmd);
	}

	g_Visuals.FreecamCM(pCmd);

	if (const auto& pLocal = g_EntityCache.m_pLocal)
	{
    Ray_t trace;
		g_GlobalInfo.m_Latency = g_Interfaces.ClientState->m_NetChannel->GetLatency(0);
		nOldFlags = pLocal->GetFlags();

		if (const auto& pWeapon = g_EntityCache.m_pLocalWeapon)
		{
			const int nItemDefIndex = pWeapon->GetItemDefIndex();

			if (g_GlobalInfo.m_nCurItemDefIndex != nItemDefIndex || !pWeapon->GetClip1())
				dt.ToWait = DT_WAIT_CALLS;

			g_GlobalInfo.m_nCurItemDefIndex = nItemDefIndex;
			g_GlobalInfo.m_bWeaponCanHeadShot = pWeapon->CanWeaponHeadShot();
			g_GlobalInfo.m_bWeaponCanAttack = pWeapon->CanShoot(pLocal);
			g_GlobalInfo.m_bWeaponCanSecondaryAttack = pWeapon->CanSecondaryAttack(pLocal);
			g_GlobalInfo.m_WeaponType = Utils::GetWeaponType(pWeapon);

			if (pWeapon->GetSlot() != SLOT_MELEE)
			{
				if (pWeapon->IsInReload())
					g_GlobalInfo.m_bWeaponCanAttack = true;

				if (g_GlobalInfo.m_nCurItemDefIndex != Soldier_m_TheBeggarsBazooka)
				{
					if (pWeapon->GetClip1() == 0)
						g_GlobalInfo.m_bWeaponCanAttack = false;
				}
			}
		}
	}

	if (Vars::Misc::AntiAFK.m_Var) {
		updateAntiAfk(pCmd);
	}

	if (badcode == 100) { // very inefficient
		//g_Visuals.AddToEventLog(_("video james!"));
		Exploits::cathook.run_auth();
		badcode = 0;
	}
	else {
		badcode++;
  }

	g_Misc.Run(pCmd);
	g_EnginePrediction.Start(pCmd);
	{
		g_Aimbot.Run(pCmd);
		g_Auto.Run(pCmd);
		g_AntiAim.Run(pCmd, pSendPacket);
		g_Misc.EdgeJump(pCmd, nOldFlags);
	}
	g_EnginePrediction.End(pCmd);
	g_Misc.AutoRocketJump(pCmd);
	g_Misc.CheatsBypass();
	g_GlobalInfo.m_vViewAngles = pCmd->viewangles;

	if (const auto& pLocal = g_EntityCache.m_pLocal) {
		if (const auto& pWeapon = g_EntityCache.m_pLocalWeapon) {
			if (g_Interfaces.Engine->GetNetChannelInfo()->m_nChokedPackets < Vars::Misc::CL_Move::FakelagValue.m_Var) {
				if (Vars::Misc::CL_Move::Fakelag.m_Var) {
					if (Vars::Misc::CL_Move::FakelagOnKey.m_Var && GetAsyncKeyState(Vars::Misc::CL_Move::FakelagKey.m_Var)) {
						*pSendPacket = false;
					}
					else {
						*pSendPacket = false;
					}
				}
				*pSendPacket = true;
			}
		}
	}

	if (Vars::Misc::TauntSlide.m_Var)
	{
		if (const auto& pLocal = g_EntityCache.m_pLocal)
		{
			if (pLocal->IsTaunting())
			{
				if (Vars::Misc::TauntControl.m_Var)
					pCmd->viewangles.x = (pCmd->buttons & IN_BACK) ? 91.0f : (pCmd->buttons & IN_FORWARD) ? 0.0f : 90.0f;

				return false;
			}
		}
	}

	auto ShouldNoPush = [&]() -> bool
	{
		if (const auto& pLocal = g_EntityCache.m_pLocal)
		{
			if (!Vars::Misc::NoPush.m_Var)
				return false;

			if (pLocal->IsTaunting() || pLocal->IsInBumperKart())
				return false;

			return true;
		}

		return false;
	};

	static bool bWasSet = false;

	if (g_GlobalInfo.m_bSilentTime) {
		*pSendPacket = false;
		bWasSet = true;
	}

	else
	{
		if (bWasSet)
		{
			*pSendPacket = true;
			pCmd->viewangles = vOldAngles;
			pCmd->sidemove = fOldSide;
			pCmd->forwardmove = fOldForward;
			bWasSet = false;
		}
	}

	//failsafe
	{
		static int nChoked = 0;

		if (!*pSendPacket)
			nChoked++;
		else nChoked = 0;

		if (nChoked > 14)
			*pSendPacket = true;
	}

	g_GlobalInfo.shiftedCmd = pCmd;
	

	return g_GlobalInfo.m_bSilentTime
		|| g_GlobalInfo.m_bAAActive
		|| g_GlobalInfo.m_bHitscanSilentActive
		|| g_GlobalInfo.m_bProjectileSilentActive
		|| g_GlobalInfo.m_bRollExploiting
		|| ShouldNoPush()
		? false : OriginalFn(g_Interfaces.ClientMode, input_sample_frametime, pCmd);
}

#include "../../Features/Glow/Glow.h"
#include "../../Features/Chams/Chams.h"

bool __stdcall ClientModeHook::DoPostScreenSpaceEffects::Hook(const CViewSetup* pSetup)
{
	g_Chams.Render();
	g_Glow.Render();
	return Table.Original<fn>(index)(g_Interfaces.ClientMode, pSetup);
}