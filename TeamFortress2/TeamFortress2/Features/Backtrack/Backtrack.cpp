#include "Backtrack.h"
#include "../Visuals/Visuals.h"

float LerpTime()
{

	static ConVar* updaterate = g_Interfaces.CVars->FindVar("cl_updaterate");
	static ConVar* minupdate = g_Interfaces.CVars->FindVar("sv_minupdaterate");
	static ConVar* maxupdate = g_Interfaces.CVars->FindVar("sv_maxupdaterate");
	static ConVar* lerp = g_Interfaces.CVars->FindVar("cl_interp");
	static ConVar* cmin = g_Interfaces.CVars->FindVar("sv_client_min_interp_ratio");
	static ConVar* cmax = g_Interfaces.CVars->FindVar("sv_client_max_interp_ratio");
	static ConVar* ratio = g_Interfaces.CVars->FindVar("cl_interp_ratio");

	float lerpurmom = lerp->GetFloat();
	float maxupdateurmom = maxupdate->GetFloat();
	int updaterateurmom = updaterate->GetInt();
	float ratiourmom = ratio->GetFloat();
	int sv_maxupdaterate = maxupdate->GetInt();
	int sv_minupdaterate = minupdate->GetInt();
	float cminurmom = cmin->GetFloat();
	float cmaxurmom = cmax->GetFloat();

	if (sv_maxupdaterate && sv_minupdaterate)
		updaterateurmom = maxupdateurmom;

	if (ratiourmom == 0)
		ratiourmom = 1.0f;

	if (cmin && cmax && cmin->GetFloat() != 1)
		ratiourmom = std::clamp(ratiourmom, cminurmom, cmaxurmom);

	return std::max(lerpurmom, ratiourmom / updaterateurmom);
}

//std::vector<tick_record> CBacktrack::ticks[64];

bool CBacktrack::GoodTick(int tick)
{
	auto nci = g_Interfaces.Engine->GetNetChannelInfo();

	if (!nci) {
		return false;
	}

	float correct = std::clamp(nci->GetLatency(FLOW_OUTGOING) + LerpTime(), 0.f, 1.f);

	float delta_time = correct - (g_Interfaces.GlobalVars->curtime - TICKS_TO_TIME(tick));

	return fabsf(delta_time) < 0.2f;
}

void CBacktrack::Start(CUserCmd* pCmd) {
	auto pLocal = GLOCAL;
	if (pLocal == nullptr || !pLocal || pCmd == nullptr || !pCmd) {
		return;
	}

	CBaseCombatWeapon* pWeapon = pLocal->GetActiveWeapon();
	if (!pWeapon) {
		return;
	}

	for (int i = 0; i < g_Interfaces.Engine->GetMaxClients(); i++) {
		CBaseEntity* pEntity = g_Interfaces.EntityList->GetClientEntity(i);
		if (!pEntity || pEntity->GetDormant() || pEntity->GetLifeState() != LIFE_ALIVE) {
			continue;
		}

		if (pEntity->GetTeamNum() == pLocal->GetTeamNum()) {
			continue;
		}

		int hitbox = 0;

		ticks[i].insert(ticks[i].begin(), tick_record{ pEntity->GetSimulationTime(), pEntity->GetHitboxPos(hitbox), pEntity->GetAbsOrigin() });

		if (ticks[i].size() > 12) {
			ticks[i].pop_back();
		}
	}
}

void CBacktrack::Calculate(CUserCmd* pCmd) {
	auto pLocal = GLOCAL;
	if (pLocal == nullptr || !pLocal || pCmd == nullptr || !pCmd) {
		return;
	}

	Vec3 viewDirection, newViewDirection;
	g_Interfaces.Engine->GetViewAngles(viewDirection);
	Math::AngleVectors(viewDirection, &newViewDirection);

	CBaseCombatWeapon* pWeapon = pLocal->GetActiveWeapon();
	if (!pWeapon) {
		return;
	}

	int best_target_index = -1; float best_field_of_view = FLT_MAX;
	for (int i = 0; i < g_Interfaces.Engine->GetMaxClients(); i++) {
		CBaseEntity* pEntity = g_Interfaces.EntityList->GetClientEntity(i);
		if (!pEntity || pEntity->GetDormant() || pEntity->GetLifeState() != LIFE_ALIVE) {
			continue;
		}

		if (pEntity->GetTeamNum() == pLocal->GetTeamNum()) {
			continue;
		}

		if (ticks[i].empty()) {
			continue;
		}

		float fovDistance = Math::DistPointToLine(pEntity->GetEyePosition(), pLocal->GetEyePosition(), newViewDirection);
		if (fovDistance < best_field_of_view) {
			best_field_of_view = fovDistance;
			best_target_index = i;
		}

		for (unsigned int t = 0; t < ticks[i].size(); t++) {
			if (!pEntity->SetupBones(ticks[i].at(t).bone_matrix, 128, 256, 0)) {
				continue;
			}
		}
	}

	float final_target_index = -1;
	if (best_target_index != -1) {
		for (unsigned int i = 0; i < ticks[best_target_index].size(); i++) {
			float field_of_view_distance = Math::DistPointToLine(ticks[best_target_index].at(i).head_position, pLocal->GetEyePosition(), newViewDirection);
			if (field_of_view_distance < best_field_of_view) {
				best_field_of_view = field_of_view_distance;
				final_target_index = ticks[best_target_index].at(i).simulation_time;
			}
		}

		if (final_target_index != -1) {
			if (pCmd->buttons & IN_ATTACK || g_GlobalInfo.m_bAttacking) {
				pCmd->tick_count = TIME_TO_TICKS(final_target_index);
			}
		}
	}
}

void CBacktrack::DoBacktrack(CUserCmd* pCmd) {
	auto pLocal = GLOCAL;
	if (pLocal == nullptr || !pLocal || pCmd == nullptr || !pCmd) {
		return;
	}

	Start(pCmd);
	Calculate(pCmd);
}