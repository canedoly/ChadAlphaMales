#include "ModelRenderHook.h"

#include "../../Features/Chams/Chams.h"
#include "../../Features/Chams/DMEChams.h"
#include "../../Features/Glow/Glow.h"
#include "../../Features/Vars.h"

void __stdcall ModelRenderHook::DrawModelExecute::Hook(const DrawModelState_t &pState, const ModelRenderInfo_t &pInfo, matrix3x4 *pBoneToWorld)
{
	CBaseEntity *pEntity = g_Interfaces.EntityList->GetClientEntity(pInfo.m_nEntIndex);
	const char* szModel = g_Interfaces.ModelInfo->GetModelName(pInfo.m_pModel);

	if ((g_Chams.HasDrawn(pEntity) || g_Glow.HasDrawn(pEntity)) && !g_Glow.m_bDrawingGlow)
		return;

	if (g_DMEChams.Render(pState, pInfo, pBoneToWorld))
		return;

	if (Vars::Visuals::RemoveHats.m_Var && strstr(szModel, "player/items"))
		return;

	Table.Original<fn>(index)(g_Interfaces.ModelRender, pState, pInfo, pBoneToWorld);
}

void __stdcall ModelRenderHook::ForcedMaterialOverride::Hook(IMaterial *mat, EOverrideType type)
{
	if (!g_DMEChams.m_bRendering)
	{
		if ((g_Glow.m_bRendering && !g_Glow.IsGlowMaterial(mat)) || (g_Chams.m_bRendering && !g_Chams.IsChamsMaterial(mat)))
			return;
	}

	Table.Original<fn>(index)(g_Interfaces.ModelRender, mat, type);
}