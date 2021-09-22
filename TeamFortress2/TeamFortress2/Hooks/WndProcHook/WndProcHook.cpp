#include "WndProcHook.h"

extern IMGUI_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WndProcHook::Hook(HWND window, UINT msg, WPARAM wParam, LPARAM lParam)
{
    
    if (g_NewMenu.menuOpen && ImGui_ImplWin32_WndProcHandler(window, msg, wParam, lParam))
        return true;
    return CallWindowProcW(WndProc, window, msg, wParam, lParam);
}
