#pragma once
#include "../../SDK/SDK.h"

class CESP
{
private:
	bool ShouldRun();
	void AAIndicator();
	bool GetDrawBounds(CBaseEntity* pEntity, Vec3* vTrans, int& x, int& y, int& w, int& h);
	void DrawPaths();
	void HandleDormancy(CBaseEntity* entity, int index);
	std::wstring GetPlayerConds(CBaseEntity* pEntity);
	const wchar_t* GetPlayerClass(int nClassNum);

	void DrawPlayers(CBaseEntity *pLocal);
	void DrawBuildings(CBaseEntity *pLocal);
	void DrawWorld();
	void Draw3DBox(Vec3* vPoints, Color_t clr);
	void CreateDLight(int nIndex, Color_t DrawColor, const Vec3 &vOrigin, float flRadius);
	void DrawBones(CBaseEntity* pPlayer, const std::vector<int>& vecBones, Color_t clr);
	void OffScreen(Vec3& position, int alpha, Color_t color);
	float m_flEntityAlpha[4096];
	float m_flTimeSinceDormant[4096];
	bool  m_bTimeSet[4096];

public:
	void Run();
	void OffScreen(const Vec3& position, int alpha, Color_t teamColor);
};

inline CESP g_ESP;

class CPlayerArrows
{
private:
	bool ShouldRun(CBaseEntity* pLocal);
	//void DrawArrowTo(const Vec3& vecFromPos, const Vec3& vecToPos, Color_t color);

	std::vector<Vec3> m_vecPlayers = {};

public:
	void Run();
	bool upAlpha = true;
	int alpha = 0;
};

inline CPlayerArrows g_PlayerArrows;