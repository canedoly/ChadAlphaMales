#include "InstantRespawn.h"
#include "../Vars.h"

//Credits to Cathook Devs
void CInstantRespawn::InstantRespawn()
{
	if (!Vars::Misc::InstantRespawn.m_Var || !g_EntityCache.m_pLocal)
		return;

	auto* KV = new KeyValues("MVM_Revive_Response");
	KV->SetInt("accepted", 1);
	g_Interfaces.Engine->ServerCmdKeyValues(KV);
}