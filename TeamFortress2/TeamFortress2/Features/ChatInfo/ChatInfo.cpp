#include "ChatInfo.h"

#include "../Vars.h"
#include "../Visuals/Visuals.h"

#define GET_PLAYER_USERID(userid) g_Interfaces.EntityList->GetClientEntity(g_Interfaces.Engine->GetPlayerForUserID(userid))
#define GET_INDEX_USERID(userid) g_Interfaces.Engine->GetPlayerForUserID(userid)

std::string hitgroup_to_name(int hitgroup) {
	switch (hitgroup) {
	case HITBOX_HEAD:
		return _("head");
	default:
		return _("body");
	}
}

void CChatInfo::AddListeners()
{
	//Client
	g_Interfaces.GameEvent->AddListener(this, _("player_changeclass"), false);
	g_Interfaces.GameEvent->AddListener(this, _("flagstatus_update"), false);
	g_Interfaces.GameEvent->AddListener(this, _("player_hurt"), false);
	g_Interfaces.GameEvent->AddListener(this, _("cl_drawline"), false);
	//Server
	g_Interfaces.GameEvent->AddListener(this, _("vote_cast"), true);
	g_Interfaces.GameEvent->AddListener(this, _("player_activate"), true);
	g_Interfaces.GameEvent->AddListener(this, _("player_disconnect"), true);
	g_Interfaces.GameEvent->AddListener(this, _("player_connect"), true);
}

void CChatInfo::RemoveListeners()
{
	g_Interfaces.GameEvent->RemoveListener(this);
}

void CChatInfo::FireGameEvent(CGameEvent* pEvent)
{
	const auto& pLocal = g_EntityCache.m_pLocal;

	if (pEvent)
	{
		const int nLocal = g_Interfaces.Engine->GetLocalPlayer();
		const std::string_view szEvent(pEvent->GetName());

		if (pLocal && Vars::Visuals::ChatInfo.m_Var)
		{
			int nLocalTeam = pLocal->GetTeamNum();

			if (!szEvent.compare(_("player_changeclass")))
			{
				if (const auto& pEntity = GET_PLAYER_USERID(pEvent->GetInt(_("userid"))))
				{
					if (pEntity->GetTeamNum() != nLocalTeam)
					{
						int nIndex = pEntity->GetIndex();

						PlayerInfo_t pi;
						if (g_Interfaces.Engine->GetPlayerInfo(nIndex, &pi))
						{
							char szBuff[85];
							sprintf(szBuff, _("%s is now %s"), pi.name, Utils::GetClassByIndex(pEvent->GetInt(XorStr("class").c_str())));
							g_Visuals.vecEventVector.push_back(EventLogging_t{ szBuff });

							//sprintf(szBuff, _("[CAM] \x3%s is now %s\n"), pi.name, Utils::GetClassByIndex(pEvent->GetInt(XorStr("class").c_str())));
							//g_Interfaces.ClientMode->m_pChatElement->ChatPrintf(nIndex, szBuff);
						}
					}
				}

				return;
			}


			if (!szEvent.compare(_("player_hurt")))
			{
				const auto pAttacker = (CBaseEntity*)g_Interfaces.EntityList->GetClientEntity(g_Interfaces.Engine->GetPlayerForUserID(pEvent->GetInt(_("attacker"))));
				if (!pAttacker)
					return;

				const auto pVictim = (CBaseEntity*)g_Interfaces.EntityList->GetClientEntity(g_Interfaces.Engine->GetPlayerForUserID(pEvent->GetInt(_("userid"))));
				if (!pVictim)
					return;

				if (pAttacker == pLocal && pVictim != pLocal)
				{
					PlayerInfo_t player_info;
					if (!g_Interfaces.Engine->GetPlayerInfo(pVictim->GetIndex(), &player_info))
						return;
					
					char buffer[256];

					sprintf_s(buffer, _("Hurt %s for %i (%i health remaining)\n"), player_info.name, pEvent->GetInt(_("damageamount")), pEvent->GetInt(_("health")));

					g_Visuals.vecEventVector.push_back(EventLogging_t{ buffer });
				}
				return;
			}

			// :(
			if (!szEvent.compare(_("player_connect")))
			{
				char szBuff[255];
				sprintf(szBuff, _("%s connected. (%s)"), pEvent->GetString(_("name")), pEvent->GetString(_("address")));
				g_Interfaces.ClientMode->m_pChatElement->ChatPrintf(GET_INDEX_USERID(pEvent->GetInt(_("userid"))), szBuff);
				g_Visuals.vecEventVector.push_back(EventLogging_t{ szBuff });
				return;
			}

			if (!szEvent.compare(_("player_disconnect")))
			{
				//char szBuff[255];
				//sprintf(szBuff, _("%s disconnected cuh. (%s)"), pEvent->GetString(_("name")), pEvent->GetString(_("address")));
				//g_Interfaces.ClientMode->m_pChatElement->ChatPrintf(GET_INDEX_USERID(pEvent->GetInt(_("userid"))), szBuff);

				int uid = pEvent->GetInt(_("userid"));
				for (int i : g_GlobalInfo.storedmis) {
					if (uid = i) {
						g_GlobalInfo.storedmis.erase(std::remove(g_GlobalInfo.storedmis.begin(), g_GlobalInfo.storedmis.end(), uid), g_GlobalInfo.storedmis.end());
					}
				}
				return;
			}
			/*
			if (!szEvent.compare(_("cl_drawline"))) {
				const char* CathookName = pEvent->GetName();
				int player_idx = pEvent->GetInt(_("player"), 0xDEAD);

				int CathookLine = pEvent->GetInt(_("line"), -1);
				int CathookPanel = pEvent->GetInt(_("panel"), -1);
				float CathookX = pEvent->GetFloat(_("x"), -1.f);
				float CathookY = pEvent->GetFloat(_("y"), -1.f);
				if (player_idx != 0xDEAD  && CathookLine == 0 && CathookPanel == 2) {
					if ((CathookX == 0xCA8 || CathookX == 0xCA7) && CathookY == 1234567.f) {
						PlayerInfo_t pi;
						if (g_Interfaces.Engine->GetPlayerInfo(player_idx, &pi)) {
							//for (int i : g_GlobalInfo.storedmis) {
								//if (pi.userID != i) {
									if (player_idx != pLocal->GetIndex()) {
										char szBuff[255];
										sprintf(szBuff, _("CAT detected: %s"), pi.name);
										g_Visuals.vecEventVector.push_back(EventLogging_t{ szBuff });

										sprintf(szBuff, _("\x4[CAM]\x1 CAT detected: \x3%s"), pi.name);
										g_Interfaces.ClientMode->m_pChatElement->ChatPrintf(GET_INDEX_USERID(pEvent->GetInt(_("userid"))), szBuff);
										g_GlobalInfo.storedmis.push_back(pi.userID);
									}
								//}
							//}
						}
					}
				}
				return;
			}
			*/

			
			// https://github.com/mastercomfig/team-comtress-2/blob/a4ae53293537dd3dbd07c3365f07af84e164d809/game/client/c_vote_controller.cpp
			// Left off here.
			if (!szEvent.compare(_("vote_cast"))) {
				int iClient = pEvent->GetInt(_("entityid"));
				int iTeam = pEvent->GetInt(_("team"));

				PlayerInfo_t sInfo;
				if (iClient == g_Interfaces.Engine->GetLocalPlayer() || iTeam != pLocal->GetTeamNum() || g_Interfaces.Engine->GetPlayerInfo(iClient, &sInfo))
					return;

				int iVoteOption = pEvent->GetInt(_("vote_option")) ? 1 : 0; //vote_option => 0 = yes, 1 = no

				char szBuff[255];
				sprintf(szBuff, _("%s voted for %s"), sInfo.name, !iVoteOption ? _("yes") : _("no"));
				g_Visuals.vecEventVector.push_back(EventLogging_t{ szBuff });
				sprintf(szBuff, _("[CAM] \x4%s voted for \x3%s"), sInfo.name, !iVoteOption ? _("yes") : _("no"));
				g_Interfaces.ClientMode->m_pChatElement->ChatPrintf(GET_INDEX_USERID(pEvent->GetInt(_("userid"))), szBuff);
				return;
			}
			
		}
	}
}