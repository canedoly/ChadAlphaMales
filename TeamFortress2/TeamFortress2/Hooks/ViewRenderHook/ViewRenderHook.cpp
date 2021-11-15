#include "ViewRenderHook.h"
#include "../../Features/Chams/Chams.h"
#include "../../Features/Visuals/Visuals.h"
#include "../../Features/RichPresence/RichPresence.h"
void __fastcall ViewRenderHook::LevelInit::Hook() {
	//g_Interfaces.CVars->ConsoleColorPrintf({ 255, 200, 0, 255 }, _("[!] Level init\n"));

	g_Visuals.DevTextures();
	g_Visuals.SkyboxChanger();
	g_Chams.Init();

	dt.Shifting = false;
	dt.Recharging = false;
	dt.Charged = 0;
	dt.ChargedReverse = dt.ToShift;
	dt.ToWait = 0;
	
	Table.Original<fn>(index)(g_Interfaces.ViewRender);
}

void __fastcall ViewRenderHook::LevelShutdown::Hook() {
	//g_Interfaces.CVars->ConsoleColorPrintf({ 255, 200, 0, 255 }, _("[!] Level shutdown\n"));
	g_Chams.Delete();
	Table.Original<fn>(index)(g_Interfaces.ViewRender);
}