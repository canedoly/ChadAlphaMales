#include "GameMovementHook.h"

void __stdcall GameMovement::ProcessMovement::Hook(CBaseEntity* pPlayer, CMoveData* pMove)
{
	fn OriginalFn = Table.Original<fn>(index);

	if (pPlayer == g_EntityCache.m_pLocal && dt.Shifting) {
		return;
	}
	else {
		OriginalFn(g_Interfaces.GameMovement, pPlayer, pMove);
	}
}
