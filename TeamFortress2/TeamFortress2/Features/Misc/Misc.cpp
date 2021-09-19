#include "Misc.h"

#include "../Vars.h"
#include "../Visuals/Visuals.h"

#define TICK_INTERVAL			(g_Interfaces.GlobalVars->interval_per_tick)
#define TIME_TO_TICKS( dt )		( (int)( 0.5f + (float)(dt) / TICK_INTERVAL ) )
#define MAX_CACHE TIME_TO_TICKS(1)

static bool push = true;

void CMisc::Run(CUserCmd* pCmd)
{
	AutoJump(pCmd);
	AutoStrafe(pCmd);
	//StoreData(pCmd);
	//FixMove(pCmd, g_Misc.m_strafe_angles);
	StopFast(pCmd);
	NoiseMakerSpam();
	ChatSpam();
	//CatIgnore();
	nopush();
}

void VectorAngles(Vector& forward, Vector& angles)
{
	float tmp, yaw, pitch;

	if (forward[1] == 0 && forward[0] == 0)
	{
		yaw = 0;
		if (forward[2] > 0)
			pitch = 270;
		else
			pitch = 90;
	}
	else
	{
		yaw = (atan2(forward[1], forward[0]) * 180 / PI);
		if (yaw < 0)
			yaw += 360;

		tmp = sqrt((forward[0] * forward[0] + forward[1] * forward[1]));
		pitch = (atan2(-forward[2], tmp) * 180 / PI);
		if (pitch < 0)
			pitch += 360;
	}

	angles[0] = pitch;
	angles[1] = yaw;
	angles[2] = 0;
}

void AngleVectors2(const QAngle& angles, Vector* forward)
{
	float sp, sy, cp, cy;

	Math::SinCos(DEG2RAD(angles.x), &sy, &cy);
	Math::SinCos(DEG2RAD(angles.y), &sp, &cp);

	forward->x = cp * cy;
	forward->y = cp * sy;
	forward->z = -sp;
}

QAngle VectorToQAngle(Vector in)
{
	return *(QAngle*)&in;
}

void FastStop(CUserCmd* pCmd) {
	Vector vel;
	if (const auto& pLocal = g_EntityCache.m_pLocal) {
		vel = pLocal->GetVecVelocity();

		static auto sv_friction = g_Interfaces.CVars->FindVar("sv_friction");
		static auto sv_stopspeed = g_Interfaces.CVars->FindVar("sv_stopspeed");

		auto speed = vel.Lenght2D();
		auto friction = sv_friction->GetFloat() * (DWORD)pLocal + 0x12b8;
		auto control = (speed < sv_stopspeed->GetFloat()) ? sv_stopspeed->GetFloat() : speed;
		auto drop = control * friction * g_Interfaces.GlobalVars->interval_per_tick;

		if (speed > drop - 1.0f)
		{
			Vector velocity = vel;
			Vector direction;
			VectorAngles(vel, direction);
			float speed = velocity.Lenght();

			direction.y = pCmd->viewangles.y - direction.y;

			Vector forward;
			AngleVectors2(VectorToQAngle(direction), &forward);
			Vector negated_direction = forward * -speed;

			pCmd->forwardmove = negated_direction.x;
			pCmd->sidemove = negated_direction.y;
		}
		else {
			pCmd->forwardmove = pCmd->sidemove = 0.0f;
		}
	}
}

/*
Vector CMisc::predPosAt(float flTime, CBaseEntity* target)
{
	if (!flTime)
		return target->GetAbsOrigin();

	static ConVar* sv_gravity = g_Interfaces.CVars->FindVar(_("sv_gravity"));

	Vector startPos = target->GetAbsOrigin(), velocity = target->GetVecVelocity();
	float zdrop;
	if (target->GetFlags() & FL_ONGROUND)
		zdrop = velocity.z * flTime;
	else
		zdrop = 0.5 * -sv_gravity->GetInt() * pow(flTime, 2) + velocity.z * flTime;

	Vector result(
		startPos.x + (velocity.x * flTime),
		startPos.y + (velocity.y * flTime),
		startPos.z + zdrop);

	//if (paths.value == 2)
	//{
		float angleY = 0;
		CBaseEntity* cache = g_Interfaces.EntityList->GetClientEntity(target);
		if (cache && cache->Full()) // Determine player strafe by averaging cache data
		{
			// A = Last, B = Mid, C = Current (startPos)
			int last = (MAX_CACHE - 1) / 3, mid = (MAX_CACHE - 1) / 6;
			Vector vLast, vMid;
			if (HitboxData* data = cache->FindTick(g_Interfaces.GlobalVars->tickcount - last))
			{
				vLast = data->vCenter;
				if (data = cache->FindTick(g_Interfaces.GlobalVars->tickcount - mid))
					vMid = data->vCenter;
			}
			// Approximate angle
			Vector forward, strafe;
			Math::VectorAngles(vMid - vLast, forward);
			Math::VectorAngles(startPos - vMid, strafe);
			// Divide our angle to measure by distance
			angleY = (strafe.y - forward.y) / ((vMid - vLast).Lenght2D() + (startPos - vMid).Lenght2D());
		}
		gEsp.rotate_vec2(*(Vec2*)&result, *(Vec2*)&startPos, DEG2RAD(angleY * (result - startPos).Lenght2D()));
	//}

	return result;
}
*/
void CMisc::EdgeJump(CUserCmd* pCmd, const int nOldFlags)
{
	if ((nOldFlags & FL_ONGROUND) && Vars::Misc::EdgeJump.m_Var)
	{
		if (const auto& pLocal = g_EntityCache.m_pLocal)
		{
			if (pLocal->IsAlive() && !pLocal->IsOnGround() && !pLocal->IsSwimming())
				pCmd->buttons |= IN_JUMP;
		}
	}
}

void CMisc::AutoJump(CUserCmd *pCmd)
{
	if (const auto &pLocal = g_EntityCache.m_pLocal)
	{
		if (!Vars::Misc::AutoJump.m_Var
			|| !pLocal->IsAlive()
			|| pLocal->IsSwimming()
			|| pLocal->IsInBumperKart()
			|| pLocal->IsAGhost())
			return;

		static bool bJumpState = false;

		if (pCmd->buttons & IN_JUMP)
		{
			if (!bJumpState && !pLocal->IsOnGround())
				pCmd->buttons &= ~IN_JUMP;

			else if (bJumpState)
				bJumpState = false;
		}

		else if (!bJumpState)
			bJumpState = true;
	}
}
bool AutoStrafeFlip = false;
/*
void CMisc::AutoStrafe(CUserCmd* pCmd)
{
	if (Vars::Misc::AutoStrafe.m_Var)
	{
		if (const auto& pLocal = g_EntityCache.m_pLocal)
		{
			
			if (pLocal->IsAlive() && !pLocal->IsSwimming() && !pLocal->IsOnGround() && (pCmd->mousedx > 1 || pCmd->mousedx < -1)) {
				pCmd->sidemove = pCmd->mousedx > 1 ? 450.f : -450.f;

			}
		}
	}
}
*/


bool pda = false;
bool pda2 = false;
bool pda3 = false;

void CMisc::StopFast(CUserCmd* pCmd) {

	if (Vars::Misc::CL_Move::Enabled.m_Var && Vars::Misc::CL_Move::Doubletap.m_Var && Vars::Misc::CL_Move::DoubletapKey.m_Var && (pCmd->buttons & IN_ATTACK) && !g_GlobalInfo.m_nShifted && !g_GlobalInfo.m_nWaitForShift)
	{
		g_GlobalInfo.m_bShouldShift = true;
	}


	if (const auto& pLocal = g_EntityCache.m_pLocal) {
		if (pLocal->IsOnGround()) {
			float speed = pLocal->GetVelocity().Lenght2D();

			if (g_GlobalInfo.fast_stop == true && GetAsyncKeyState(Vars::Misc::CL_Move::DoubletapKey.m_Var)) {
				if (speed > 1.f) {
					if (!pda) {
						g_Interfaces.Engine->ClientCmd_Unrestricted("cyoa_pda_open 1");
						pda = true;
					}
					if (pLocal->GetMaxSpeed() < 240)
					{
						pCmd->forwardmove = 0.f;

					}
					else {
						pCmd->forwardmove = -pCmd->forwardmove / 4;
					}
					pCmd->sidemove = 0.f;
					if (!pda2) {
						g_Interfaces.Engine->ClientCmd_Unrestricted("cyoa_pda_open 0");
						pda2 = true;
					}
				}
				else {
					if (!pda3) {
						g_Interfaces.Engine->ClientCmd_Unrestricted("cyoa_pda_open 0");
						pda3 = true;
					}
					pda = false;
					pda2 = false;
					g_GlobalInfo.fast_stop = false;
				}
			}
		}
	}
}

static float normalizeRad(float a) noexcept
{
	return std::isfinite(a) ? std::remainder(a, PI * 2) : 0.0f;
}

static float angleDiffRad(float a1, float a2) noexcept
{
	float delta;

	delta = normalizeRad(a1 - a2);
	if (a1 > a2)
	{
		if (delta >= PI)
			delta -= PI * 2;
	}
	else
	{
		if (delta <= -PI)
			delta += PI * 2;
	}
	return delta;
}


void CMisc::AutoStrafe(CUserCmd* pCmd)
{
	if (Vars::Misc::AutoStrafe.m_Var)
	{
		if (const auto& pLocal = g_EntityCache.m_pLocal)
		{
			if (!pLocal)
				return;
#
			static bool was_jumping = false;
			bool is_jumping = pCmd->buttons & IN_JUMP;


			if (!(pLocal->GetFlags() & (FL_ONGROUND | FL_INWATER)) && (!is_jumping || was_jumping))
			{
				if (!pLocal || !pLocal->IsAlive())
					return;

				if (pLocal->GetMoveType() & (MOVETYPE_LADDER | MOVETYPE_NOCLIP))
					return;

				const float speed = pLocal->GetVelocity().Lenght2D();
				auto vel = pLocal->GetVelocity();

				if (speed < 2.0f)
					return;

				constexpr auto perfectDelta = [](float speed) noexcept
				{
					if (const auto& pLocal = g_EntityCache.m_pLocal) {
						static auto speedVar = pLocal->GetMaxSpeed();
						static auto airVar = g_Interfaces.CVars->FindVar(_("sv_airaccelerate"));

						static auto wishSpeed = 30.0f;

						const auto term = wishSpeed / airVar->GetFloat() / speedVar * 100.f / speed;

						if (term < 1.0f && term > -1.0f)
							return acosf(term);
					}
					return 0.0f;
				};

				const float pDelta = perfectDelta(speed);
				if (pDelta)
				{
					const float yaw = DEG2RAD(pCmd->viewangles.y);
					const float velDir = atan2f(vel.y, vel.x) - yaw;
					const float wishAng = atan2f(-pCmd->sidemove, pCmd->forwardmove);
					const float delta = angleDiffRad(velDir, wishAng);

					g_Draw.String(FONT_MENU, g_ScreenSize.c, (g_ScreenSize.h / 2) - 50, { 255, 64, 64, 255 }, ALIGN_CENTERHORIZONTAL, _(L"Was jumping: %i"), (int)was_jumping);
					g_Draw.String(FONT_MENU, g_ScreenSize.c, (g_ScreenSize.h / 2) - 70, { 255, 64, 64, 255 }, ALIGN_CENTERHORIZONTAL, _(L"Is jumping: %i"), (int)is_jumping);

					float moveDir = delta < 0.0f ? velDir + pDelta : velDir - pDelta;

					pCmd->forwardmove = cosf(moveDir) * 450.f;
					pCmd->sidemove = -sinf(moveDir) * 450.f;
				}


			}
			was_jumping = is_jumping;
		}
	}
}

void CMisc::InitSpamKV(void* pKV)
{
	char chCommand[30] = "use_action_slot_item_server";
	typedef int(__cdecl* HashFunc_t)(const char*, bool);

	static DWORD dwHashFunctionLocation = g_Pattern.Find(_(L"client.dll"), _(L"FF 15 ? ? ? ? 83 C4 08 89 06 8B C6"));
	static HashFunc_t SymbForString = (HashFunc_t)* *(PDWORD*)(dwHashFunctionLocation + 0x2);

	int nAddr = 0;
	while (nAddr < 29)
	{
		switch (nAddr)
		{
			case 0:
				*(PDWORD)((DWORD)pKV + nAddr) = SymbForString(chCommand, true);
				break;
			case 16:
				*(PDWORD)((DWORD)pKV + nAddr) = 0x10000;
				break;
			default:
				*(PDWORD)((DWORD)pKV + nAddr) = 0;
				break;
		}

		nAddr += 4;
	}
}

void CMisc::NoiseMakerSpam()
{
	if (!Vars::Misc::NoisemakerSpam.m_Var)
		return;

	if (const auto& pLocal = g_EntityCache.m_pLocal)
	{
		if (pLocal->GetUsingActionSlot())
			return;

		if (pLocal->GetNextNoiseMakerTime() < g_Interfaces.GlobalVars->curtime)
		{
			if (const auto pKV = Utils::InitKeyValue())
			{
				InitSpamKV(pKV);
				g_Interfaces.Engine->ServerCmdKeyValues(pKV);
			}
		}
	}
}

void CMisc::BypassPure()
{
	if (Vars::Misc::BypassPure.m_Var)
	{
		static DWORD dwAddress = 0x0;

		while (!dwAddress)
			dwAddress = g_Pattern.Find(_(L"engine.dll"), _(L"A1 ? ? ? ? 56 33 F6 85 C0"));

		static DWORD* pPure = nullptr;

		while (!pPure)
		{
			if (reinterpret_cast<DWORD**>(dwAddress + 0x01))
				pPure = *reinterpret_cast<DWORD**>(dwAddress + 0x01);
		}

		if (*pPure)
			*pPure = 0x0;
	}
}

std::string GetSpam(const int nIndex) {
	std::string str;

	switch (nIndex)
	{
		case 0: str = XorStr("say CAM | Get g00d get CAM (ChadAlphaMales.club)").str(); break;
		case 1: str = XorStr("say SEOwned - Better than 20$ Darkstorm!").str(); break;
		case 2: str = XorStr("say SEOwned - Go get yours now from unknowncheats.me!").str(); break;
		case 3: str = XorStr("say SEOwned - Premium cheat by spook953 and Lak3, but it's free!").str(); break;
		default: str = XorStr("say SEOwned - See you @ unknowncheats.me!").str(); break;
	}

	return str;
}

void CMisc::ChatSpam()
{
	if (!Vars::Misc::ChatSpam.m_Var)
		return;

	float flCurTime = g_Interfaces.Engine->Time(); 
	static float flNextSend = 0.0f;

	if (flCurTime > flNextSend) {
		g_Interfaces.Engine->ClientCmd_Unrestricted(GetSpam(Utils::RandIntSimple(0, 5)).c_str());
		flNextSend = (flCurTime + 3.0f);
	}
}

void CMisc::AutoRocketJump(CUserCmd* pCmd)
{
	if (!Vars::Misc::AutoRocketJump.m_Var || !g_GlobalInfo.m_bWeaponCanAttack || !GetAsyncKeyState(VK_RBUTTON))
		return;

	if (g_Interfaces.EngineVGui->IsGameUIVisible() || g_Interfaces.Surface->IsCursorVisible())
		return;

	if (const auto& pLocal = g_EntityCache.m_pLocal)
	{
		if (pLocal->GetClassNum() != CLASS_SOLDIER || !pLocal->IsOnGround() || pLocal->IsDucking())
			return;

		if (const auto& pWeapon = g_EntityCache.m_pLocalWeapon)
		{
			if (pWeapon->IsInReload()) {
				pCmd->buttons |= IN_ATTACK;
				return;
			}

			else
			{
				if (pCmd->buttons & IN_ATTACK)
					pCmd->buttons &= ~IN_ATTACK;
			}

			if (g_GlobalInfo.m_nCurItemDefIndex == Soldier_m_TheBeggarsBazooka
				|| g_GlobalInfo.m_nCurItemDefIndex == Soldier_m_TheCowMangler5000
				|| pWeapon->GetSlot() != SLOT_PRIMARY)
				return;

			if (pLocal->GetViewOffset().z < 60.05f)
			{
				pCmd->buttons |= IN_ATTACK | IN_JUMP;

				Vec3 vVelocity = pLocal->GetVelocity();
				Vec3 vAngles = { vVelocity.IsZero() ? 89.0f : 45.0f, Math::VelocityToAngles(vVelocity).y - 180.0f, 0.0f };

				if (g_GlobalInfo.m_nCurItemDefIndex != Soldier_m_TheOriginal && !vVelocity.IsZero()) {
					Vec3 vForward = {}, vRight = {}, vUp = {};
					Math::AngleVectors(vAngles, &vForward, &vRight, &vUp);
					Math::VectorAngles((vForward * 23.5f) + (vRight * -5.6f) + (vUp * -3.0f), vAngles);
				}

				Math::ClampAngles(vAngles);
				pCmd->viewangles = vAngles;
				g_GlobalInfo.m_bSilentTime = true;
			}

			else pCmd->buttons |= IN_DUCK;
		}
	}
}

/*
void CMisc::CatIgnore() {
	auto CathookMessage = []() -> void
	{
		void* CathookMessage = nullptr;

		CathookMessage = Utils::CreateKeyVals({
				_("\"cl_drawline\"\
				\n{\
				\n\t\"panel\" \"1\"\
				\n\t\"line\" \"0\"\
				\n\t\"x\" \"0xCA7\"\
				\n\t\"y\" \"1234567.f\"\
				\n}\n")
			}
		);

		g_Interfaces.Engine->ServerCmdKeyValues(CathookMessage);

		CathookMessage = Utils::CreateKeyVals({
				_("\"cl_drawline\"\
				\n{\
				\n\t\"panel\" \"1\"\
				\n\t\"line\" \"0\"\
				\n\t\"x\" \"0xCA8\"\
				\n\t\"y\" \"1234567.f\"\
				\n}\n")
			}
		);

		g_Interfaces.Engine->ServerCmdKeyValues(CathookMessage);
	};
}
*/
void CMisc::nopush() {
	ConVar* noPush = g_Interfaces.CVars->FindVar(_("tf_avoidteammates_pushaway"));
	if (Vars::Misc::NoPush.m_Var) {
		if (noPush->GetInt() == 1) noPush->SetValue(0);
	}
	else {
		if (noPush->GetInt() == 0) noPush->SetValue(1);
	}
}