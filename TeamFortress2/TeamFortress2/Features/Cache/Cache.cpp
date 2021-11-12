#include "Cache.h"

Vector PlayerCache::GetHitbox(int tick, int i)
{
	if (i < 0 || i > 17) i = 0; // Quick sanity check

	const HitboxData* bone = FindTick(tick);
	if (!bone)
		return Vector();

	return GetHitbox(*bone, i);
}

Vector PlayerCache::GetHitbox(const HitboxData& Bone, int i)
{
	if (i < 0 || i > 17) i = 0; // Quick sanity check

	model_t* model = player->GetModel();
	if (!model)
		return Vector();

	studiohdr_t* hdr = g_Interfaces.ModelInfo->GetStudioModel(model);
	if (!hdr)
		return Vector();

	int HitboxSetIndex = *(int*)((DWORD)hdr + 0xB0);
	if (!HitboxSetIndex)
		return Vector();

	mstudiohitboxset_t* pSet = (mstudiohitboxset_t*)(((PBYTE)hdr) + HitboxSetIndex);
	mstudiobbox_t* box = pSet->hitbox(i);
	if (!box)
		return Vector();

	Vector vCenter = (box->bbmin + box->bbmax) * 0.5f;

	Vector vHitbox;
	Math::VectorTransform(vCenter, Bone.matrix[box->bone], vHitbox);

	return vHitbox;
}

HitboxData* PlayerCache::FindTick(int tick)
{
	for (auto& bone : bones)
		if (bone.tick == tick)
			return &bone;
	return nullptr;
}

void PlayerCache::Update(CBaseEntity* Player)
{
	if (Player != nullptr)
		player = Player;
	else if (player == nullptr)
		return;

	if (player->GetLifeState() != LIFE_ALIVE)
		bones.clear();

	index = player->GetIndex();
	guid = player->GetInfo().userID;

	bones.push_front(HitboxData());
	if (bones.size() > MAX_CACHE)
		bones.pop_back();

	HitboxData* bone = &bones.front();
	player->SetupBones(bone[0].matrix, 128, 0x100, g_Interfaces.GlobalVars->curtime);
	bone[0].vCenter = player->GetWorldSpaceCenter(), bone[0].tick = g_Cache.GetTick();
}

PlayerCache::PlayerCache(CBaseEntity* Player)
{
	if (Player == nullptr)
		return;

	Update(Player);
}

bool PlayerCache::Full()
{
	return bones.size() >= MAX_CACHE;
}

PlayerCache* CCache::FindIndex(int Index)
{
	for (auto& player : players)
		if (player.player && player.index == Index)
			return &player;
	return nullptr;
}

PlayerCache* CCache::FindUID(int GUID)
{
	for (auto& player : players)
		if (player.player && player.guid == GUID)
			return &player;
	return nullptr;
}

PlayerCache* CCache::FindPlayer(CBaseEntity* Player)
{
	for (auto& player : players)
		if (player.player && player.player == Player)
			return &player;
	return nullptr;
}

PlayerCache* CCache::FindSlot()
{
	for (auto& player : players)
		if (player.player == nullptr)
			return &player;
	return nullptr;
}

int CCache::GetTick()
{
	return g_GlobalInfo.lastChlTick;
}

void CCache::Update(CBaseEntity* You, CUserCmd* cmd)
{
	tick = cmd->tick_count;

	for (auto& player : players)
	{
		CBaseEntity* pPlayer;
		if (!g_Interfaces.EntityList->GetClientEntity(player.index)->ToPlayer(pPlayer))
			player.player = nullptr;
	}

	for (int i = 1; i <= g_Interfaces.Engine->GetMaxClients(); i++)
	{
		//if (i == me)
			//continue;
		CBaseEntity* pPlayer;
		if (!g_Interfaces.EntityList->GetClientEntity(i)->ToPlayer(pPlayer))
			continue;
		if (pPlayer->GetTeamNum() == You->GetTeamNum() && i != g_Interfaces.Engine->GetLocalPlayer())
			continue;

		bool found = false;
		for (auto& player : players)
		{
			if (pPlayer->GetInfo().userID == player.guid)
			{
				player.Update(pPlayer);
				found = true;
				break;
			}
		}
		if (found)
			continue;

		PlayerCache cache(pPlayer), * pCache = FindSlot();
		if (pCache)
			*pCache = cache;
		else
			players.push_back(cache);
	}
}

int CCache::ApproxLostTicks()
{
	INetChannel* netchan = g_Interfaces.Engine->GetNetChannelInfo();
	float lost_time;

	lost_time = netchan->GetLatency(FLOW_INCOMING) + netchan->GetLatency(FLOW_OUTGOING);

	return TIME_TO_TICKS(ceil(lost_time * 10));
}