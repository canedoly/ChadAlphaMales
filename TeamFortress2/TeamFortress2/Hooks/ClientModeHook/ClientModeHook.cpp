#include "ClientModeHook.h"

#include "../../Features/Prediction/Prediction.h"
#include "../../Features/Aimbot/Aimbot.h"
#include "../../Features/Auto/Auto.h"
#include "../../Features/Misc/Misc.h"
#include "../../Features/Visuals/Visuals.h"
#include "../../Features/AntiHack/AntiAim.h"
#include "../../Features/KatzeB0t/AntiCat.h"
#include "../../SDK/Timer.h"
#include "../../Features/Cache/Cache.h"
#include "../../Features/ESP/ESP.h"

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

		// what is this for?
		// i'm guessing fake angle chams but idk
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
inline Vector ComputeMove(CUserCmd* pCmd, CBaseEntity* pLocal, Vec3& a, Vec3& b)
{
	Vec3 diff = (b - a);
	if (diff.Lenght() == 0.0f)
		return Vec3(0.0f, 0.0f, 0.0f);
	const float x = diff.x;
	const float y = diff.y;
	Vec3 vsilent(x, y, 0);
	Vec3 ang;
	Math::VectorAngles(vsilent, ang);
	float yaw = DEG2RAD(ang.y - pCmd->viewangles.y);
	float pitch = DEG2RAD(ang.x - pCmd->viewangles.x);
	Vec3 move = { cos(yaw) * 450.0f, -sin(yaw) * 450.0f, -cos(pitch) * 450.0f };

	// Only apply upmove in water
	if (!(g_Interfaces.EngineTrace->GetPointContents(pLocal->GetEyePosition()) & CONTENTS_WATER))
		move.z = pCmd->upmove;
	return move;
}

// Function for when you want to goto a vector
inline void WalkTo(CUserCmd* pCmd, CBaseEntity* pLocal, Vec3& a, Vec3& b, float scale)
{
	// Calculate how to get to a vector
	auto result = ComputeMove(pCmd, pLocal, a, b);
	// Push our move to usercmd
	pCmd->forwardmove = result.x * scale;
	pCmd->sidemove = result.y * scale;
	pCmd->upmove = result.z * scale;
}

void FastStop(CBaseEntity* pLocal, CUserCmd* pCmd) {
	static Vec3 vStartOrigin = {};
	static Vec3 vStartVel = {};
	static int nShiftTick = 0;
	if (pLocal && pLocal->IsAlive()) {
		if (DT.shouldStop)
		{
			if (vStartOrigin.IsZero()) {
				vStartOrigin = pLocal->GetVecOrigin();
			}

			if (vStartVel.IsZero()) {
				vStartVel = pLocal->GetVecVelocity();
			}

			Vec3 vPredicted = vStartOrigin + (vStartVel * TICKS_TO_TIME(24 - nShiftTick));
			Vec3 vPredictedMax = vStartOrigin + (vStartVel * TICKS_TO_TIME(24));

			float flScale = Math::RemapValClamped(vPredicted.DistTo(vStartOrigin), 0.0f, vPredictedMax.DistTo(vStartOrigin) * 1.27f, 1.0f, 0.0f);
			float flScaleScale = Math::RemapValClamped(vStartVel.Lenght2D(), 0.0f, 520.f, 0.f, 1.f);
			WalkTo(pCmd, pLocal, vPredictedMax, vStartOrigin, flScale * flScaleScale);

			nShiftTick++;
		}
		else {
			vStartOrigin = Vec3(0,0,0);
			vStartVel = Vec3(0,0,0);
			nShiftTick = 0;
		}
	}
}


bool __stdcall ClientModeHook::CreateMove::Hook(float input_sample_frametime, CUserCmd* pCmd)
{
	g_GlobalInfo.m_bSilentTime = false;
	g_GlobalInfo.m_bAttacking = false;

	fn OriginalFn = Table.Original<fn>(index);

	if (!pCmd || !pCmd->command_number)
		return OriginalFn(g_Interfaces.ClientMode, input_sample_frametime, pCmd);

	if (OriginalFn(g_Interfaces.ClientMode, input_sample_frametime, pCmd))
		g_Interfaces.Prediction->SetLocalViewAngles(pCmd->viewangles);

	if (DT.currentTicks == 0 && DT.barAlpha > 0) {
		if (!DT.barAlpha - 3 < 0) {
			DT.barAlpha -= 3;
		}
	}

	if (g_PlayerArrows.upAlpha) {
		g_PlayerArrows.alpha = std::min(255, (g_PlayerArrows.alpha + 5));
		if (g_PlayerArrows.alpha == 255) {
			g_PlayerArrows.upAlpha = false;
		}
	}

	if (!g_PlayerArrows.upAlpha) {
		g_PlayerArrows.alpha = std::max(0, (g_PlayerArrows.alpha - 5));
		if (g_PlayerArrows.alpha == 0) {
			g_PlayerArrows.upAlpha = true;
		}
	}


	uintptr_t _bp; __asm mov _bp, ebp;
	bool* pSendPacket = (bool*)(***(uintptr_t***)_bp - 0x1);

	if (pCmd->command_number)
	{
		g_GlobalInfo.lastChlTick = pCmd->tick_count;
	}

	if (g_GlobalInfo.ForceSendPacket)
	{
		*pSendPacket = true;
		g_GlobalInfo.ForceSendPacket = false;
	} // if we are trying to force update do this lol

	CBaseEntity* you;
	if (!g_Interfaces.EntityList->GetClientEntity(g_Interfaces.Engine->GetLocalPlayer())->ToPlayer(you) || you->GetLifeState() != LIFE_ALIVE)
		return OriginalFn(g_Interfaces.ClientMode, input_sample_frametime, pCmd);

	if (pCmd->command_number)
	{
		g_Cache.Update(you, pCmd);
	}

	int nOldFlags = 0;
	Vec3 vOldAngles = pCmd->viewangles;
	float fOldSide = pCmd->sidemove;
	float fOldForward = pCmd->forwardmove;

	// planned
	// - add glow thickness (not yet)
	// - clean up the code (no yet)
	// - add keyhelper from fedoraware (not yet)
	// - add 2 methods of antiwarp - fed/cam

	g_Visuals.FreecamCM(pCmd);

	if (const auto& pLocal = g_EntityCache.m_pLocal)
	{
		if (DT.isShifting && DT.shouldStop) {
			FastStop(pLocal, pCmd);
		}
		Ray_t trace;
		g_GlobalInfo.m_Latency = g_Interfaces.ClientState->m_NetChannel->GetLatency(0);
		nOldFlags = pLocal->GetFlags();

		if (const auto& pWeapon = g_EntityCache.m_pLocalWeapon)
		{
			const int nItemDefIndex = pWeapon->GetItemDefIndex();

			if (g_GlobalInfo.m_nCurItemDefIndex != nItemDefIndex || !pWeapon->GetClip1())
				DT.WaitTicks = DT_WAIT_CALLS;

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

	if ((Vars::Misc::CL_Move::Fakelag.m_Var && !Vars::Misc::CL_Move::FakelagOnKey.m_Var) || (Vars::Misc::CL_Move::Fakelag.m_Var && GetAsyncKeyState(Vars::Misc::CL_Move::FakelagKey.m_Var))) {
		if (const auto& pLocal = g_EntityCache.m_pLocal) {
			if (const auto& pWeapon = g_EntityCache.m_pLocalWeapon) {
				if (!g_GlobalInfo.m_bAttacking &&
					!DT.ShouldShift &&
					pLocal->IsAlive()) {
					*pSendPacket = (g_GlobalInfo.fakelagTicks >= Vars::Misc::CL_Move::FakelagValue.m_Var);
					if (*pSendPacket) {
						g_Visuals.DrawHitboxMatrix(pLocal, Colors::bonecolor, TICKS_TO_TIME(Vars::Misc::CL_Move::FakelagValue.m_Var + 1));
					}
					g_GlobalInfo.fakelagTicks++;
					DT.currentTicks = std::max(DT.currentTicks - g_GlobalInfo.fakelagTicks, 0);
					if (g_GlobalInfo.fakelagTicks > Vars::Misc::CL_Move::FakelagValue.m_Var) {
						g_GlobalInfo.fakelagTicks = 0;
					}
				}
			}
		}
	}
	// static int chockedPackets = 0;
	// if ((Vars::Misc::CL_Move::Fakelag.m_Var && !Vars::Misc::CL_Move::FakelagOnKey.m_Var) || (Vars::Misc::CL_Move::Fakelag.m_Var && GetAsyncKeyState(Vars::Misc::CL_Move::FakelagKey.m_Var))) {
	// 	if (const auto& pLocal = g_EntityCache.m_pLocal) {
	// 		if (const auto& pWeapon = g_EntityCache.m_pLocalWeapon) {
	// 			if (!g_GlobalInfo.m_bAttacking &&
	// 				!DT.ShouldShift &&
	// 				pLocal->IsAlive()) {
	// 				*pSendPacket = (chockedPackets >= Vars::Misc::CL_Move::FakelagValue.m_Var);
	// 				chockedPackets++;
	// 				DT.currentTicks = std::max(DT.currentTicks - chockedPackets, 0);
	// 				if (chockedPackets > Vars::Misc::CL_Move::FakelagValue.m_Var) {
	// 					chockedPackets = 0;
	// 				}
	// 			}
	// 		}
	// 	}
	// }

	if (Vars::Misc::TauntSlide.m_Var)
	{
		if (const auto& pLocal = g_EntityCache.m_pLocal)
		{
			if (pLocal->IsTaunting())
			{
				if (Vars::Misc::TauntControl.m_Var)
				{
					if (Vars::Misc::TauntControlMouse.m_Var)
					{
						Vec3 vAngle = g_Interfaces.Engine->GetViewAngles();
						pCmd->viewangles.y = vAngle.y;

						return false;
					}
					else
					{
						pCmd->viewangles.x = (pCmd->buttons & IN_BACK) ? 91.0f : (pCmd->buttons & IN_FORWARD) ? 0.0f : 90.0f;
					}
				}
				return false;
			}
		}
	}

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