#include "WndProcHook.h"

#include <algorithm>
bool IsAnyMouseButtonDown() {
    auto &io = ImGui::GetIO();
    for (int i = 0; i < sizeof(io.MouseDown); i++) {
        if (io.MouseDown[i]) {
            return true;
        }
    }
    return false;
}

extern IMGUI_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
extern IMGUI_API LRESULT imguiwndproc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);


LRESULT CALLBACK WndProcHook::Hook(HWND window, UINT msg, WPARAM wParam, LPARAM lParam)
{    
    if (g_NewMenu.menuOpen && imguiwndproc(window, msg, wParam, lParam))
        return true;

    return CallWindowProcA(pOldWindowProc, window, msg, wParam, lParam);
}
