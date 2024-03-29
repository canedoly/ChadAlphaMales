#pragma once
#include "../../SDK/SDK.h"

class CChams
{
private:
	IMaterial* m_pMatShaded, * m_pMatFlat, * m_pMatShiny, * m_pMatBrick, * m_pMatFresnelHDR0, * m_pMatFresnelHDR1, * m_pMatFresnel, * m_pMatAA;

	std::map<CBaseEntity*, bool> m_DrawnEntities;

private:
	bool ShouldRun();

private:
	void RenderBT(CBaseEntity* Player, IMatRenderContext* pRenderContext);
	void RenderPlayers(CBaseEntity* pLocal, IMatRenderContext* pRenderContext);
	void RenderBuildings(CBaseEntity* pLocal, IMatRenderContext* pRenderContext);
	void RenderWorld(CBaseEntity* pLocal, IMatRenderContext* pRenderContext);

public:
	void DrawModel(CBaseEntity* pEntity);
	void Init();
	void Delete();
	void Render();

	inline bool HasDrawn(CBaseEntity* pEntity) {
		return m_DrawnEntities.find(pEntity) != m_DrawnEntities.end();
	}

	inline bool IsChamsMaterial(IMaterial* pMat) {
		return (pMat == m_pMatShaded || pMat == m_pMatFlat ||
			pMat == m_pMatShiny || pMat == m_pMatBrick);
	}

public:
	bool m_bHasSetStencil;
	bool m_bRendering;
};

inline CChams g_Chams;