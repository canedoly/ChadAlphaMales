#include "BulletTracer.h"
#include "../../Features/Vars.h"
constexpr int TEAM_RED = 2;



void __fastcall BulletTracers::FireBullet::Hook(void* ecx, void* edx, CBaseCombatWeapon* pWeapon, const FireBulletsInfo_t& info, bool bDoEffects, int nDamageType, int nCustomDamageType) {
	static auto original = Func.Original<fn>();
	Vec3 vecStart = info.m_vecSrc;
	Vec3 vecnd = info.m_vecSrc + info.m_vecDirShooting * info.m_flDistance;
	CGameTrace trace;
	CTraceFilterHitscan filter = { };
	auto pLocal = g_EntityCache.m_pLocal;
	if (!pLocal)
		return;
	filter.pSkip = pLocal;
	//CTraceFilterHitscan* filter = nullptr;
	//filter->pSkip = pWeapon;
	Ray_t ray;
	ray.Init(vecStart, vecnd);

	g_Interfaces.EngineTrace->TraceRay(ray, MASK_SHOT, &filter, &trace);

	if (auto& pLocal = g_EntityCache.m_pLocal) {

		if (pWeapon) {
			static bool doTracer = true;
			int iAttachment = pWeapon->LookupAttachment(_("muzzle"));
			pWeapon->GetAttachment(iAttachment, trace.vStartPos);
			int teamNum = pLocal->GetTeamNum();
			std::string fart;
			switch (Vars::Visuals::TracerEffect.m_Var) {
				case 0:
					doTracer = false;
					break;
				case 1:
					if (teamNum == TEAM_RED) {
						fart = _("dxhr_sniper_rail_red");
					}
					else {
						fart = _("dxhr_sniper_rail_blue");
					}
					break;
				case 2:
					if (g_EntityCache.m_pLocal->IsCritBoosted()) {
						if (teamNum == TEAM_RED)
							fart = _("bullet_tracer_raygun_red_crit");
						else
							fart = _("bullet_tracer_raygun_blue_crit");
					}
					else
					{
						if (teamNum == TEAM_RED)
							fart = _("bullet_tracer_raygun_red");
						else
							fart = _("bullet_tracer_raygun_blue");
					}
					break;
				case 3:
					//Merasmus Vortex
					fart = _("merasmus_zap_beam02");
					break;
				case 4:
					//Merasmus Zap
					fart = _("merasmus_zap");
					break;
			}

			Vec3 shootPos = pLocal->GetShootPos();
			shootPos.z -= 5.0f;

			if (doTracer) {
				UTIL_ParticleTracer(fart.c_str(), trace.vStartPos, trace.vEndPos, pLocal->GetIndex(), iAttachment, true);
			}
			doTracer = true;
		}
	}

	if (!Vars::Visuals::TracerEffect.m_Var) {
		original(ecx, edx, pWeapon, info, bDoEffects, nDamageType, nCustomDamageType);
	}
}