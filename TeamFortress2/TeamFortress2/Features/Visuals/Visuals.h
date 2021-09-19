#pragma once
#include "../../SDK/SDK.h"
#include "../../Utils/FNV/fnv1a.h"

struct EventLogging_t {
	EventLogging_t(std::string _string) {
		szText = _string;
		flTime = g_Interfaces.GlobalVars->realtime;
	}

	std::string szText;
	float flTime;
	int flAlpha = 255;
};

class CVisuals
{
private:
	int m_nHudZoom = 0;
	float flTextTime = 4.f;
	float flTextFadeInTime = 0.2f;
	float flTextFadeOutTime = 0.3f;
	int flIdealHeight = 5;
	int flIdealWidth = 5;
	int flSlideInDistance = 20;
	int flSlideOutDistance = 20;
	float flSlideOutSpeed = 0.3f;

public:
	std::vector<EventLogging_t> vecEventVector;
	void RunEventLogs();
	bool RemoveScope(int nPanel);
	void FOV(CViewSetup *pView);
	void Fullbright();
	void ARatio();
	void ViewmodelXYZ();
	void ThirdPerson();
	void ModulateWorld();
	void UpdateWorldModulation();
	void RestoreWorldModulation();
};

inline CVisuals g_Visuals;