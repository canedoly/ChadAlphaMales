#include "WndProcHook.h"

extern IMGUI_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
extern IMGUI_API LRESULT imguiwndproc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);


LRESULT CALLBACK WndProcHook::Hook(HWND window, UINT msg, WPARAM wParam, LPARAM lParam)
{   
    if (g_GlobalInfo.Unload) {
        if (g_NewMenu.menuOpen && ImGui_ImplWin32_WndProcHandler(window, msg, wParam, lParam))
            return true;
    }
    else {
        if (g_NewMenu.menuOpen && imguiwndproc(window, msg, wParam, lParam))
            return true;
    }

    return CallWindowProcA(pOldWindowProc, window, msg, wParam, lParam);
}
