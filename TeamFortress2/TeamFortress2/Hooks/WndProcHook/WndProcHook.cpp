//#include "WndProcHook.h"
//
//extern IMGUI_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
//extern IMGUI_API LRESULT imguiwndproc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
//
//
//LRESULT CALLBACK WndProcHook::Hook(HWND window, UINT msg, WPARAM wParam, LPARAM lParam)
//{   
//    if (g_GlobalInfo.Unload) {
//        if (g_Menu.menuOpen && ImGui_ImplWin32_WndProcHandler(window, msg, wParam, lParam))
//            return true;
//    }
//    else {
//        if (g_Menu.menuOpen && imguiwndproc(window, msg, wParam, lParam))
//            return true;
//    }
//
//    return CallWindowProcA(pOldWindowProc, window, msg, wParam, lParam);
//}
#include "WndProcHook.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LONG __stdcall WndProcHook::Hook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (g_Menu.menuOpen) {
		g_Interfaces.InputSystem->ResetInputState();
		g_Interfaces.InputSystem->EnableInput(false);
		ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
		return 1;
	}

	return CallWindowProcW(WndProc, hWnd, uMsg, wParam, lParam);
}