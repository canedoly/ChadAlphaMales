#pragma once
#include "../../SDK/SDK.h"

class CDMEChams
{
private:
	IMaterial *m_pMatShaded;
	IMaterial *m_pMatShiny;
	IMaterial *m_pMatFlat;
	IMaterial *m_pMatWFShaded;
	IMaterial *m_pMatWFShiny;
	IMaterial *m_pMatWFFlat;
	IMaterial *m_pMatFresnelHDR0;
	IMaterial *m_pMatFresnelHDR1;
	IMaterial *m_pMatFresnel;

private:
	bool ShouldRun();

public:
	void Init();
	bool Render(const DrawModelState_t &pState, const ModelRenderInfo_t &pInfo, matrix3x4 *pBoneToWorld);
	bool m_bRendering;
};

inline CDMEChams g_DMEChams;