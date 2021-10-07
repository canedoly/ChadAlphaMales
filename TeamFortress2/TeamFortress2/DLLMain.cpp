#include "Hooks/Hooks.h"

#include "Features/Glow/Glow.h"
#include "Features/Chams/Chams.h"
#include "Features/Chams/DMEChams.h"
#include "Features/Visuals/Visuals.h"
#include "Features/RichPresence/RichPresence.h"
#include "Utils/DiscordRPC/Discord.h"
#include "Utils/Events/Events.h"
#include "Features/Menu/Menu.h"
#include "Features/NewMenu/NewMenu.h"
#include "Features/Playerlist/Playerlist.h"

Discord* g_DiscordRPC;

using namespace std::chrono; //lol??? 

int StringToWString(std::wstring& ws, const std::string& s)
{
	std::wstring wsTmp(s.begin(), s.end());

	ws = wsTmp;

	return 0;
}

DWORD WINAPI MainThread(LPVOID lpParam)
{
	/*
		"mss32.dll" being one of the last modules to be loaded
		So wait for that before proceeding, after it's up everything else should be too
		Allows us to correctly use autoinject and just start the game.
		Adding "ntdll.dll" seems to fix SOME of the crashing issues which is kind of funny
	*/

	while (!WinAPI::GetModuleHandleW(_(L"mss32.dll")) || !WinAPI::GetModuleHandleW(_(L"ntdll.dll")))
		std::this_thread::sleep_for(std::chrono::seconds(5));
	
	g_SteamInterfaces.Init();
	g_Interfaces.Init();
	g_Interfaces.Engine->ClientCmd_Unrestricted("unbind f7"); // for legacy only lmao
	g_Interfaces.Engine->ClientCmd_Unrestricted("cl_vote_ui_active_after_voting 1");
	g_Interfaces.Engine->ClientCmd_Unrestricted("cl_timeout 99999");
	g_Interfaces.Engine->ClientCmd_Unrestricted("clear");
	g_Interfaces.CVars->ConsoleColorPrintf({ 0, 155, 255, 255 }, _("[!] Initializing stuff...\n"));
	g_NetVars.Init();
	g_Glow.Init();
	g_Chams.Init();
	g_DMEChams.Init();
	g_dwDirectXDevice = **reinterpret_cast<DWORD**>(g_Pattern.Find(L"shaderapidx9.dll", L"A1 ? ? ? ? 50 8B 08 FF 51 0C") + 0x1);
	g_Hooks.Init();
	g_ConVars.Init();
	//EFonts
	g_Draw.InitFonts
	({
		//FONT_ESP
		{ 0x0, _("Segoe UI"), 12, 0, FONTFLAG_DROPSHADOW | FONTFLAG_ANTIALIAS },
		//FONT_ESP_OUTLINED
		{ 0x0, _("Segoe UI"), 12, 0, FONTFLAG_DROPSHADOW | FONTFLAG_ANTIALIAS },

		//FONT_ESP_NAME
		{ 0x0, _("Verdana"), 12, 0, FONTFLAG_DROPSHADOW },
		//FONT_ESP_NAME_OUTLINED
		{ 0x0, _("Segoe UI"), 13, 100, FONTFLAG_DROPSHADOW | FONTFLAG_ANTIALIAS},

		//FONT_ESP_COND
		{ 0x0, _("Segoe UI"), 12, 100, FONTFLAG_DROPSHADOW | FONTFLAG_ANTIALIAS },
		//FONT_ESP_COND_OUTLINED
		{ 0x0, _("Consolas"), 10, 0, FONTFLAG_OUTLINE },

		//FONT_ESP_PICKUPS
		{ 0x0, _("Consolas"), 13, 0, FONTFLAG_NONE },
		//FONT_ESP_PICKUPS_OUTLINED
		{ 0x0, _("Segoe UI"), 13, 100, FONTFLAG_DROPSHADOW | FONTFLAG_ANTIALIAS},

		//FONT_MENU
		{ 0x0, _("Verdana"), 12, 0, FONTFLAG_NONE | FONTFLAG_DROPSHADOW },
		//FONT_MENU_OUTLINED
		{ 0x0, _("Verdana"), 12, 0, FONTFLAG_OUTLINE },

		/*FONT_ICONS*/					
		{ 0x0, _("Tf2weaponicons Regular"), 20, 0, FONTFLAG_NONE},
	});

	//Initialize ignored set
	g_Playerlist.GetIgnoredPlayers();
	g_Events.Setup({ "vote_cast", "player_changeclass", "player_connect", "player_hurt", "localplayer_respawn", "achievement_earned" });
	if (!source::features::steamrichpresence.Update())
		return false;

	//g_DiscordRPC->init();
	//g_DiscordRPC->update();

	g_Visuals.AddToEventLog(_("Cheat injected successfully!"));
	g_Visuals.AddToEventLog(_("Press \"Insert\" to open menu!"));

	g_Interfaces.CVars->ConsoleColorPrintf({ 0, 255, 0, 255 }, _("[!] CAM Loaded!\n"));
	g_Interfaces.Surface->PlaySound(_("vo//items//wheatley_sapper//wheatley_sapper_attached14.mp3"));
	
	std::wstring defaultConfig = L"Default"; // Thank you Mr.Fedora lmao
	if (!std::filesystem::exists(g_CFG.m_sConfigPath + L"\\" + defaultConfig)) {

		std::wstring s;
		StringToWString(s, "Default");
		g_CFG.Load(s.c_str());
		g_Visuals.AddToEventLog(_("Default config loaded!"));
	}

	while (!GetAsyncKeyState(VK_F11))
		std::this_thread::sleep_for(420ms);

	g_Interfaces.CVars->ConsoleColorPrintf({ 255, 200, 0, 255 }, _("[-] Unloading CAM...\n"));
	g_Interfaces.CVars->ConsoleColorPrintf({ 255, 200, 0, 255 }, _("[-] Stopping Discord RPC\n"));
	//Discord_Shutdown();

	g_Interfaces.CVars->ConsoleColorPrintf({ 255, 200, 0, 255 }, _("[-] Stopping Steam RPC\n"));
	source::features::steamrichpresence.Destroy();

	g_Events.Destroy();

	g_Interfaces.Surface->SetCursorAlwaysVisible(false);
	std::this_thread::sleep_for(100ms);
	
	g_Hooks.Release();
	g_Visuals.RestoreWorldModulation(); //needs to do this after hooks are released cuz UpdateWorldMod in FSN will override it
	g_Interfaces.CVars->ConsoleColorPrintf({ 255, 255, 0, 255 }, _("[!] CAM Unloaded!\n"));
	WinAPI::FreeLibraryAndExitThread(static_cast<HMODULE>(lpParam), EXIT_SUCCESS);
	return EXIT_SUCCESS;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		Utils::RemovePEH(hinstDLL);

		if (auto hMainThread = WinAPI::CreateThread(0, 0, MainThread, hinstDLL, 0, 0))
			WinAPI::CloseHandle(hMainThread);
	}

	return TRUE;
}
