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

namespace CBacktrack {
	extern std::vector<tick_record> ticks[64];
	extern bool GoodTick(int tick);
	extern void Start(CUserCmd* pCmd);
	extern void Calculate(CUserCmd* pCmd);
	extern void DoBacktrack(CUserCmd* pCmd);
};
