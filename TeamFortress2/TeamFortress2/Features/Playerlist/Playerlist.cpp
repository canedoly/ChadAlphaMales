#include "Playerlist.h"

bool sortByTeam(const plistPlayer& a, const plistPlayer& b)
{
	return a.teamNum < b.teamNum;
}

bool findPlayer(int id, int teamNum)
{
	for (const auto& player : g_Playerlist.players)
		if (player.info.userID == id && player.teamNum == teamNum)
			return true;
	return false;
}

void CPlayerlist::GetPlayers()
{
	if (g_Interfaces.Engine->IsInGame() && !g_Interfaces.Engine->IsDrawingLoadingImage() && g_Interfaces.Engine->IsConnected())
	{
		CBaseEntity* local = g_Interfaces.EntityList->GetClientEntity(g_Interfaces.Engine->GetLocalPlayer());
		for (int i = 1; i <= g_Interfaces.Engine->GetMaxClients(); i++)
		{
			CBaseEntity* ent = g_Interfaces.EntityList->GetClientEntity(i);

			if (!ent || !ent->IsPlayer() || ent == g_EntityCache.m_pLocal)
			{
				if (!players.empty())
				{
					for (auto it = players.begin(); it != players.end(); it++)
					{
						if (it->index == i)
						{
							it = players.erase(it);
							break;
						}
					}
				}
				continue;
			}

			plistPlayer player;

			if (g_Interfaces.Engine->GetPlayerInfo(i, &player.info))
			{
				int teamNum = ent->GetTeamNum();
				player.teamNum = teamNum;
				//player.color = Utils::GetTeamColor(teamNum, Vars::ESP::Main::EnableTeamEnemyColors.m_Var);
				player.index = i;

				if (players.empty())
					players.push_back(player);
				else if (!players.empty() && !findPlayer(player.info.userID, teamNum))
				{
					for (auto i = players.begin(); i != players.end(); i++)
					{
						if (i->info.userID == player.info.userID)
						{
							i = players.erase(i);
							break;
						}
					}
					players.push_back(player);
				}
			}
		}
		if (!players.empty()) {
			std::sort(players.begin(), players.end(), sortByTeam);
		}
	}
}