#pragma once
#include <list>
#include "../Vars.h"

#define MAX_CACHE TIME_TO_TICKS(1)

struct HitboxData
{
	Vec3 vCenter;
	matrix3x4 matrix[128];
	int tick;
};

class CCache;
class PlayerCache
{
public:
	CBaseEntity* player = nullptr;
	int index;
	int guid;
	bool bHit = false;

	std::list<HitboxData> bones;
	Vec3 GetHitbox(int tick, int i);
	Vec3 GetHitbox(const HitboxData& Bone, int i);

	bool Full();
	HitboxData* FindTick(int tick);
	void Update(CBaseEntity* Player = nullptr);

	PlayerCache(CBaseEntity* Player);
};

class CCache
{
	int tick = 0;
public:
	int GetTick();
	int ApproxLostTicks();

	std::list<PlayerCache> players;
	PlayerCache* FindSlot();

	PlayerCache* FindIndex(int Index);
	PlayerCache* FindUID(int GUID);
	PlayerCache* FindPlayer(CBaseEntity* Player);

	void Update(CBaseEntity* You, CUserCmd* cmd);
};

inline CCache g_Cache;