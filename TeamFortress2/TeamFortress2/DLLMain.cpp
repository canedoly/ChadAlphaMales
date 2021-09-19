#include "Hooks/Hooks.h"

#include "Features/Glow/Glow.h"
#include "Features/Chams/Chams.h"
#include "Features/Chams/DMEChams.h"
#include "Features/ChatInfo/ChatInfo.h"
#include "Features/Visuals/Visuals.h"
#include "Features/RichPresence/RichPresence.h"
#include "Features/Vars.h"
#include "Utils/DiscordRPC/Discord.h"

#include "Features/Menu/Menu.h"
#include "Features/NewMenu/NewMenu.h"

Discord* g_DiscordRPC;

using namespace std::chrono; //lol??? 

DWORD WINAPI MainThread(LPVOID lpParam)
{
	//"mss32.dll" being one of the last modules to be loaded
	//So wait for that before proceeding, after it's up everything else should be too
	//Allows us to correctly use autoinject and just start the game.
	while (!WinAPI::GetModuleHandleW(_(L"mss32.dll")))
		std::this_thread::sleep_for(std::chrono::seconds(5));
	
	try {
		g_SteamInterfaces.Init();
		g_Interfaces.Init();
		g_Interfaces.CVars->ConsoleColorPrintf({ 0, 155, 255, 255 }, _("[!] Initializing stuff...\n"));
		g_Interfaces.Init();
		g_NetVars.Init();
		g_Glow.Init();
		g_Chams.Init();
		g_DMEChams.Init();
		g_dwDirectXDevice = **reinterpret_cast<DWORD**>(g_Pattern.Find(L"shaderapidx9.dll", L"A1 ? ? ? ? 50 8B 08 FF 51 0C") + 0x1);
		g_Hooks.Init();
		g_ConVars.Init();
		g_ChatInfo.AddListeners();
		g_Draw.InitFonts
		({
			//FONT_ESP
			{ 0x0, _("Verdana"), 12, 0, FONTFLAG_NONE },
			//FONT_ESP_OUTLINED
			{ 0x0, _("Verdana"), 12, 0, FONTFLAG_OUTLINE },
			//{ 0x0, _("Segoe UI"), 12, 0, FONTFLAG_DROPSHADOW | FONTFLAG_ANTIALIAS},

			//FONT_ESP_NAME
			{ 0x0, _("Verdana"), 12, 0, FONTFLAG_DROPSHADOW },
			//FONT_ESP_NAME_OUTLINED
			//{ 0x0, _("Verdana"), 12, 0, FONTFLAG_OUTLINE | FONTFLAG_DROPSHADOW},
			{ 0x0, _("Segoe UI"), 13, 100, FONTFLAG_DROPSHADOW | FONTFLAG_ANTIALIAS},

			//FONT_ESP_COND
			{ 0x0, _("Consolas"), 10, 0, FONTFLAG_NONE },
			//FONT_ESP_COND_OUTLINED
			{ 0x0, _("Consolas"), 10, 0, FONTFLAG_OUTLINE },

			//FONT_ESP_PICKUPS
			{ 0x0, _("Consolas"), 13, 0, FONTFLAG_NONE },
			//FONT_ESP_PICKUPS_OUTLINED
			{ 0x0, _("Consolas"), 13, 0, FONTFLAG_OUTLINE },

			//FONT_MENU
			{ 0x0, _("Verdana"), 12, 0, FONTFLAG_NONE | FONTFLAG_DROPSHADOW },
			//FONT_MENU_OUTLINED
			{ 0x0, _("Verdana"), 12, 0, FONTFLAG_OUTLINE },

			//FONT_DEBUG
			{ 0x0, _("Arial"), 16, 0, FONTFLAG_OUTLINE }
		});
	}
	catch (const std::runtime_error& error)
	{
		MessageBoxA(nullptr, error.what(), _("Oopsie woopsie!! CAM FUCKING CRASHED"), MB_OK | MB_ICONERROR);
		WinAPI::FreeLibraryAndExitThread(static_cast<HMODULE>(lpParam), 0);
	}

	if (!source::features::steamrichpresence.Update())
		return false;

	g_DiscordRPC->init();
	g_DiscordRPC->update();

	char buffer[256];
	sprintf_s(buffer, _("Cheat injected successfully!"));
	g_Visuals.vecEventVector.push_back(EventLogging_t{ buffer });
	sprintf_s(buffer, _("Press \"Insert\" to open menu!"));
	g_Visuals.vecEventVector.push_back(EventLogging_t{ buffer });

	g_Interfaces.CVars->ConsoleColorPrintf({ 0, 255, 0, 255 }, _("[!] CAM Loaded!\n"));
	g_Interfaces.Surface->PlaySound(_("vo//items//wheatley_sapper//wheatley_sapper_attached14.mp3"));

	while (!GetAsyncKeyState(VK_F11))
		std::this_thread::sleep_for(420ms);

	g_Menu.m_bOpen = false;

	g_Interfaces.CVars->ConsoleColorPrintf({ 255, 200, 0, 255 }, _("[-] Stopping Discord RPC\n"));
	Discord_Shutdown();

	g_Interfaces.CVars->ConsoleColorPrintf({ 255, 200, 0, 255 }, _("[-] Stopping Steam RPC\n"));
	source::features::steamrichpresence.Destroy();

	g_ChatInfo.RemoveListeners();

	g_Interfaces.Surface->SetCursorAlwaysVisible(false);
	std::this_thread::sleep_for(100ms);
	
	g_Hooks.Release();
	g_Visuals.RestoreWorldModulation(); //needs to do this after hooks are released cuz UpdateWorldMod in FSN will override it
	g_Interfaces.CVars->ConsoleColorPrintf({ 255, 200, 0, 255 }, _("[-] Unloading CAM...\n"));
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
