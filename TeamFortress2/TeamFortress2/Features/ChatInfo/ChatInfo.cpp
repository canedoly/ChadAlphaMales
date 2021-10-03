#include "ChatInfo.h"

#include "../Vars.h"
#include "../Misc/Misc.h"
#include "../Visuals/Visuals.h"
#include "../../Utils/TFM/tfm.h"
#include "../KatzeB0t/AntiCat.h"
#define GET_PLAYER_USERID(userid) g_Interfaces.EntityList->GetClientEntity(g_Interfaces.Engine->GetPlayerForUserID(userid))
#define GET_INDEX_USERID(userid) g_Interfaces.Engine->GetPlayerForUserID(userid)

constexpr int CAT_IDENTIFY = 0xCA7;
constexpr int CAT_REPLY = 0xCA8;

using namespace std::chrono; //lol??? 

void CChatInfo::Event(CGameEvent* pEvent, const FNV1A_t uNameHash) {
	if (!g_Interfaces.Engine->IsConnected() || !g_Interfaces.Engine->IsInGame())
		return;

	if (const auto pLocal = g_EntityCache.m_pLocal) {
		if (Vars::Visuals::ChatInfo.m_Var) {
			if (Vars::Misc::VoteRevealer.m_Var && uNameHash == FNV1A::HashConst(_("vote_cast"))) {
				const auto pEntity = g_Interfaces.EntityList->GetClientEntity(pEvent->GetInt(_("entityid")));
				if (pEntity && pEntity->IsPlayer()) {
					const bool bVotedYes = pEvent->GetInt(_("vote_option")) == 0;
					PlayerInfo_t pi;
					g_Interfaces.Engine->GetPlayerInfo(pEntity->GetIndex(), &pi);
					g_Interfaces.ClientMode->m_pChatElement->ChatPrintf(0, tfm::format(_("\x4[CAM] \x3%s \x1voted \x3%s"), pi.name, bVotedYes ? "YES" : "NO").c_str());
				}
			}
			
			if (uNameHash == FNV1A::HashConst(_("localplayer_respawn"))) {
				Exploits::cathook.run_auth();
			}
			
			if (uNameHash == FNV1A::HashConst("achievement_earned")) { // We do a little pasting :troll:
				const int player = pEvent->GetInt("player", 0xDEAD);
				const int achievement = pEvent->GetInt("achievement", 0xDEAD);

				// 0xCA7 is an identify and mark request.
				// 0xCA8 is a mark request.

				PlayerInfo_t info;
				if (g_Interfaces.Engine->GetPlayerInfo(player, &info) && (achievement == 0xCA7 || achievement == 0xCA8) && pLocal->GetIndex() != player) {
					if (m_known_bots.find(info.friendsID) == m_known_bots.end()) {
						char szBuff[255];
						g_Visuals.AddToEventLog(_("CAT detected: %s"), info.name);

						sprintf(szBuff, _("\x4[CAM]\x1 CAT detected: \x3%s"), info.name);
						g_Interfaces.ClientMode->m_pChatElement->ChatPrintf(GET_INDEX_USERID(pEvent->GetInt(_("userid"))), szBuff);
						{ // marked by other bots. r.i.p cl_drawline :(
							// this will be detected by fedoraware and lmaobox easily.
							// use 0xCA7 if you want to make more bots do the thing,
							// most only care about being marked.
							KeyValues* kv = new KeyValues(_("AchievementEarned"));
							kv->SetInt(_("achievementID"), 0xCA8);
							g_Interfaces.Engine->ServerCmdKeyValues(kv);
						}

						m_known_bots[info.friendsID] = true;
					}
				}
			}

			/*
			if (uNameHash == FNV1A::HashConst(_("achievement_earned"))) {
				int nPlayer = pEvent->GetInt(_("player"), 0xDEAD);
				int achievement = pEvent->GetInt(_("achievement"), 0xDEAD);
				PlayerInfo_t info;
				if (nPlayer != 0xDEAD && (achievement == CAT_IDENTIFY || achievement == CAT_REPLY))
				{
					if (g_Interfaces.Engine->GetPlayerInfo(nPlayer, &info)) {
						if (m_known_bots.find(info.friendsID) == m_known_bots.end()) {
							char szBuff[255];
							g_Visuals.AddToEventLog(_("CAT detected: %s"), info.name);

							sprintf(szBuff, _("\x4[CAM]\x1 CAT detected: \x3%s"), info.name);
							g_Interfaces.ClientMode->m_pChatElement->ChatPrintf(GET_INDEX_USERID(pEvent->GetInt(_("userid"))), szBuff);
							g_GlobalInfo.storedmis.push_back(info.userID);
							m_known_bots[info.friendsID] = true;
						}
					}
				}
			}
			*/

			if (uNameHash == FNV1A::HashConst(_("player_changeclass"))) {
				if (const auto& pEntity = g_Interfaces.EntityList->GetClientEntity(g_Interfaces.Engine->GetPlayerForUserID(pEvent->GetInt(_("userid"))))) {
					int nIndex = pEntity->GetIndex();

					PlayerInfo_t pi;
					g_Interfaces.Engine->GetPlayerInfo(nIndex, &pi);

					g_Visuals.AddToEventLog(_("%s is now %s"), pi.name, Utils::GetClassByIndex(pEvent->GetInt(XorStr("class").c_str())));
					//g_Interfaces.ClientMode->m_pChatElement->ChatPrintf(nIndex, tfm::format("\x4[CAM] \x1%s is now %s", pi.name, Utils::GetClassByIndex(pEvent->GetInt("class"))).c_str());
				}
			}
			if (uNameHash == FNV1A::HashConst(_("player_connect"))) {
				Exploits::cathook.run_auth();
				g_Interfaces.ClientMode->m_pChatElement->ChatPrintf(GET_INDEX_USERID(pEvent->GetInt(_("userid"))), tfm::format(_("\x3%s\x1 connected. (%s)"), pEvent->GetString(_("name")), pEvent->GetString(_("address"))).c_str());
			}
			/*
			if (uNameHash == FNV1A::HashConst("player_disconnect")) {
				const int nPlayer = pEvent->GetInt("userid");
				PlayerInfo_t info;
				char szBuff[255];
				sprintf(szBuff, _("Detected player leaving"));
				g_Interfaces.ClientMode->m_pChatElement->ChatPrintf(GET_INDEX_USERID(pEvent->GetInt(_("userid"))), szBuff);
				g_GlobalInfo.storedmis.push_back(info.userID);
				if (g_Interfaces.Engine->GetPlayerInfo(nPlayer, &info)) {
					if (m_known_bots.find(info.friendsID) == m_known_bots.end()) {
						g_Visuals.AddToEventLog(_("Hurt %s for %i (%i health remaining)\n"), player_info.name, pEvent->GetInt(_("damageamount")), pEvent->GetInt(_("health")));
						sprintf(szBuff, _("\x4[CAM]\x1 CAT left: \x3%s"), info.name);
						g_Interfaces.ClientMode->m_pChatElement->ChatPrintf(GET_INDEX_USERID(pEvent->GetInt(_("userid"))), szBuff);
						g_GlobalInfo.storedmis.push_back(info.userID);
						m_known_bots.erase(info.friendsID);
					}
				}
			}
			*/
		}

		if (uNameHash == FNV1A::HashConst(_("player_hurt")))
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
				const auto crit = pEvent->GetBool(_("crit"));

				if (crit) {
					g_Visuals.AddToEventLog(_("Hurt %s for %i (%i health remaining) (crit)\n"), player_info.name, pEvent->GetInt(_("damageamount")), pEvent->GetInt(_("health")));
					//sprintf_s(buffer, _("Hurt %s for %i (%i health remaining) (crit)\n"), player_info.name, pEvent->GetInt(_("damageamount")), pEvent->GetInt(_("health")));
				}
				else {
					g_Visuals.AddToEventLog(_("Hurt %s for %i (%i health remaining)\n"), player_info.name, pEvent->GetInt(_("damageamount")), pEvent->GetInt(_("health")));
					//sprintf_s(buffer, _("Hurt %s for %i (%i health remaining)\n"), player_info.name, pEvent->GetInt(_("damageamount")), pEvent->GetInt(_("health")));
				}
				//g_Visuals.vecEventVector.push_back(EventLogging_t{ buffer });
			}
			return;
		}
	}
}