#pragma once
#include "../../SDK/SDK.h"

struct backtracking_record {
	CBaseEntity* entity;
	Vec3 head_position;
	Vec3 absorigin;
};

struct backtrack_data {
	int tick_count;
	std::vector<backtracking_record> records;
};

struct tick_record {
	tick_record(float simulationtime, Vector headposition, Vector absorigin) {
		simulation_time = simulationtime;
		head_position = headposition;
		absorigin = absorigin;
	}

	float simulation_time = -1;
	Vector head_position;
	Vector absorigin;
	matrix3x4 bone_matrix[128];
};

class CBacktrack {
public:
	std::vector<tick_record> ticks[64];
	bool GoodTick(int tick);
	void Start(CUserCmd* pCmd);
	void Calculate(CUserCmd* pCmd);
	void DoBacktrack(CUserCmd* pCmd);
};

inline CBacktrack g_Backtrack;