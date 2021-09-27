#include "GetDrawPositionHook.h"

#include "../../Features/Vars.h"

void __cdecl GetDrawPositionHook::Hook(float *pX, float *pY, bool *pbBehindCamera, Vec3 angleCrosshairOffset)
{
	if (Vars::Visuals::CrosshairAimPos.m_Var && !g_GlobalInfo.m_vAimPos.IsZero())
	{
		Vec3 vScreen;
		if (Utils::W2S(g_GlobalInfo.m_vAimPos, vScreen)) {
			if (pX) { *pX = vScreen.x; }
			if (pY) { *pY = vScreen.y; }
			if (pbBehindCamera) { *pbBehindCamera = false; }
		}
	}
	else if (Vars::Visuals::ThirdpersonCrosshair.m_Var && Vars::Visuals::ThirdpersonOffsetX.m_Var != 0 && Vars::Visuals::ThirdpersonOffsetY.m_Var != 0 && Vars::Visuals::ThirdpersonOffsetZ.m_Var != 0 && g_Interfaces.Input->CAM_IsThirdPerson()) {
		if (auto& pLocal = g_EntityCache.m_pLocal) {
			Vec3 pViewangles = g_Interfaces.Engine->GetViewAngles();
			Vec3 vForward{}, vRight{}, vUp{};
			Math::AngleVectors(pViewangles, &vForward, &vRight, &vUp);
			Vec3 vTraceStart = pLocal->GetShootPos();
			Vec3 vTraceEnd = (vTraceStart + vForward * 8192);

			CGameTrace trace = { };
			CTraceFilterHitscan filter = { };
			filter.pSkip = pLocal;

			Utils::Trace(vTraceStart, vTraceEnd, (MASK_SHOT /* | CONTENTS_GRATE | MASK_VISIBLE*/), &filter, &trace);



			Vec3 vScreen;
			if (Utils::W2S(trace.vEndPos, vScreen)) {
				if (pX) { *pX = vScreen.x; }
				if (pY) { *pY = vScreen.y; }
				if (pbBehindCamera) { *pbBehindCamera = false; }
			}
		}
	}

	else Func.Original<fn>()(pX, pY, pbBehindCamera, angleCrosshairOffset);
}