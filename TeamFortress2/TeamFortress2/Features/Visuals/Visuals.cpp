#include "Visuals.h"
#include "../Vars.h"
#include "../Menu/Menu.h"
#include <format>
void CVisuals::RunEventLogs()
{
	for (auto i = 0; i < vecEventVector.size(); i++) {
		auto log = vecEventVector[i];
		auto time_delta = fabs(g_Interfaces.GlobalVars->realtime - log.flTime);

		if (vecEventVector.size() > 12 ||  time_delta > flTextTime) {
			vecEventVector.erase(vecEventVector.begin() + i);
			continue;
		}

		auto height = flIdealHeight + (14 * i),
			width = flIdealWidth;

		if (time_delta < flTextFadeInTime) {
			log.flAlpha = ((time_delta / flTextFadeInTime) * 255.f);
			width = (time_delta / flTextFadeInTime) * (float)(flSlideInDistance)+(flIdealWidth - flSlideInDistance);
		}

		if (time_delta > flTextTime - flTextFadeOutTime) {
			log.flAlpha = (255 - (((time_delta - (flTextTime - flTextFadeOutTime)) / flTextFadeOutTime) * 255.f));
			width = flIdealWidth + (((time_delta - (flTextTime - flTextFadeOutTime)) / flTextFadeOutTime) * (float)(flSlideOutDistance));
		}
		// Idk i put flAlpha on everything, looks cooler lol
		g_Draw.String(FONT_MENU_OUTLINED, width, height, Color_t(255,255,255, log.flAlpha), ALIGN_DEFAULT, Utils::ConvertUtf8ToWide(log.szText.c_str()).data());
	}
}

bool CVisuals::RemoveScope(int nPanel)
{
	if (!m_nHudZoom && Hash::IsHudScope(g_Interfaces.Panel->GetName(nPanel)))
		m_nHudZoom = nPanel;

	return (Vars::Visuals::RemoveScope.m_Var && nPanel == m_nHudZoom);
}

void CVisuals::FOV(CViewSetup *pView)
{
	CBaseEntity *pLocal = g_EntityCache.m_pLocal;

	if (pLocal && pView)
	{
		if (pLocal->GetObserverMode() == OBS_MODE_FIRSTPERSON || (pLocal->IsScoped() && !Vars::Visuals::RemoveZoom.m_Var))
			return;

		pView->fov = Vars::Visuals::FieldOfView.m_Var;

		if (pLocal->IsAlive())
			pLocal->SetFov(Vars::Visuals::FieldOfView.m_Var);
	}
}


void CVisuals::Fullbright() {
	static ConVar* fullbright = g_Interfaces.CVars->FindVar(_("mat_fullbright"));
	fullbright->SetValue(Vars::Visuals::Fullbright.m_Var); // Probably isn't the best idea to set it like 500x a second :woozy_face:
}


void CVisuals::ARatio() {
	float ratio = (Vars::Visuals::AspectRatioValue.m_Var * 0.1) / 2;
	static ConVar* RatioVar = g_Interfaces.CVars->FindVar(_("r_aspectratio"));
	if (ratio > 0.001)
		RatioVar->SetValue(ratio);
	else
		RatioVar->SetValue((35 * 0.1f) / 2);
}

void CVisuals::ViewmodelXYZ() {
	static char buff[12];
	snprintf(buff, sizeof(buff), _("%d %d %d"), (int)Vars::Visuals::ViewModelX.m_Var, (int)Vars::Visuals::ViewModelY.m_Var, (int)Vars::Visuals::ViewModelZ.m_Var);

	static auto customview = g_Interfaces.CVars->FindVar(_("tf_viewmodels_offset_override"));
	customview->m_fnChangeCallback = 0;
	customview->SetValue(buff);
}

void CVisuals::ThirdPerson()
{
	if (const auto &pLocal = g_EntityCache.m_pLocal)
	{
		if (Vars::Visuals::ThirdPersonKey.m_Var)
		{
			if (!g_Interfaces.EngineVGui->IsGameUIVisible() && !g_Interfaces.Surface->IsCursorVisible() && !g_Menu.m_bTyping)
			{
				static float flPressedTime = g_Interfaces.Engine->Time();
				float flElapsed = g_Interfaces.Engine->Time() - flPressedTime;

				if ((GetAsyncKeyState(Vars::Visuals::ThirdPersonKey.m_Var) & 0x8000) && flElapsed > 0.2f) {
					Vars::Visuals::ThirdPerson.m_Var = !Vars::Visuals::ThirdPerson.m_Var;
					flPressedTime = g_Interfaces.Engine->Time();
				}
			}
		}
		
		bool bIsInThirdPerson = g_Interfaces.Input->CAM_IsThirdPerson();

		if (!Vars::Visuals::ThirdPerson.m_Var
			|| ((!Vars::Visuals::RemoveScope.m_Var || !Vars::Visuals::RemoveZoom.m_Var) && pLocal->IsScoped()))
		{
			if (bIsInThirdPerson)
				pLocal->ForceTauntCam(0);

			return;
		}

		if (!bIsInThirdPerson)
			pLocal->ForceTauntCam(1);

		if (bIsInThirdPerson && Vars::Visuals::ThirdPersonSilentAngles.m_Var)
		{
			g_Interfaces.Prediction->SetLocalViewAngles(g_GlobalInfo.m_vRealViewAngles);

			if (Vars::Visuals::ThirdPersonInstantYaw.m_Var)
			{
				if (const auto &pAnimState = pLocal->GetAnimState())
					pAnimState->m_flCurrentFeetYaw = g_GlobalInfo.m_vRealViewAngles.y;
			}
		}
	}
}

bool bWorldIsModulated = false;

void ApplyModulation(const Color_t &clr)
{
	for (MaterialHandle_t h = g_Interfaces.MatSystem->First(); h != g_Interfaces.MatSystem->Invalid(); h = g_Interfaces.MatSystem->Next(h))
	{
		if (const auto &pMaterial = g_Interfaces.MatSystem->Get(h))
		{
			if (pMaterial->IsErrorMaterial() || !pMaterial->IsPrecached())
				continue;

			std::string_view group(pMaterial->GetTextureGroupName());

			if (group.find(_(TEXTURE_GROUP_WORLD)) != group.npos || group.find(_(TEXTURE_GROUP_SKYBOX)) != group.npos)
			{
				bool bFound = false;
				IMaterialVar *pVar = pMaterial->FindVar(_("$color2"), &bFound);

				if (bFound && pVar)
					pVar->SetVecValue(Color::TOFLOAT(clr.r), Color::TOFLOAT(clr.g), Color::TOFLOAT(clr.b));

				else pMaterial->ColorModulate(Color::TOFLOAT(clr.r), Color::TOFLOAT(clr.g), Color::TOFLOAT(clr.b));
			}
		}
	}

	bWorldIsModulated = true;
}

void CVisuals::ModulateWorld()
{
	if (!Vars::Visuals::WorldModulation.m_Var)
		return;

	ApplyModulation(Colors::WorldModulation);
}

void CVisuals::UpdateWorldModulation()
{
	if (!Vars::Visuals::WorldModulation.m_Var) {
		RestoreWorldModulation();
		return;
	}

	auto ColorChanged = [&]() -> bool
	{
		static Color_t old = Colors::WorldModulation;
		Color_t cur = Colors::WorldModulation;

		if (cur.r != old.r || cur.g != old.g || cur.b != old.b) {
			old = cur;
			return true;
		}

		return false;
	};

	if (!bWorldIsModulated || ColorChanged())
		ApplyModulation(Colors::WorldModulation);
}

void CVisuals::RestoreWorldModulation()
{
	if (!bWorldIsModulated)
		return;

	ApplyModulation({ 255, 255, 255, 255 });
	bWorldIsModulated = false;
}