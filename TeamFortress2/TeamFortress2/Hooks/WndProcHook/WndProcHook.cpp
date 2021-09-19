#include "WndProcHook.h"

IMGUI_API LRESULT WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui::GetCurrentContext() == nullptr)
        return 0;

    ImGuiIO& io = ImGui::GetIO();
    switch (msg)
    {
    case WM_LBUTTONDOWN: case WM_LBUTTONDBLCLK:
    case WM_RBUTTONDOWN: case WM_RBUTTONDBLCLK:
    case WM_MBUTTONDOWN: case WM_MBUTTONDBLCLK:
    {
        int button = 0;
        if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONDBLCLK) button = 0;
        if (msg == WM_RBUTTONDOWN || msg == WM_RBUTTONDBLCLK) button = 1;
        if (msg == WM_MBUTTONDOWN || msg == WM_MBUTTONDBLCLK) button = 2;
        if (!ImGui::IsAnyMouseDown() && ::GetCapture() == nullptr)
            ::SetCapture(hwnd);
        io.MouseDown[button] = true;
        return 1;
    }
    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
    case WM_MBUTTONUP:
    {
        int button = 0;
        if (msg == WM_LBUTTONUP) button = 0;
        if (msg == WM_RBUTTONUP) button = 1;
        if (msg == WM_MBUTTONUP) button = 2;
        io.MouseDown[button] = false;
        if (!ImGui::IsAnyMouseDown() && ::GetCapture() == hwnd)
            ::ReleaseCapture();
        return 1;
    }
    case WM_MOUSEWHEEL:
        io.MouseWheel += GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? +1.0f : -1.0f;
        return 1;
    case WM_MOUSEHWHEEL:
        io.MouseWheelH += GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? +1.0f : -1.0f;
        return 1;
    case WM_MOUSEMOVE:
        io.MousePos.x = (signed short)(lParam);
        io.MousePos.y = (signed short)(lParam >> 16);
        return 1;
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
        if (wParam < 256)
            io.KeysDown[wParam] = 1;
        return 1;
    case WM_KEYUP:
    case WM_SYSKEYUP:
        if (wParam < 256)
            io.KeysDown[wParam] = 0;
        return 1;
    case WM_CHAR:
        // You can also use ToAscii()+GetKeyboardState() to retrieve characters.
        if (wParam > 0 && wParam < 0x10000)
            io.AddInputCharacter((unsigned short)wParam);
        return 1;
    }
    return 0;
}

LRESULT __stdcall WndProcHook::Hook(HWND window, UINT msg, WPARAM wParam, LPARAM lParam) {
    //LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    
    if (g_NewMenu.menuOpen && WndProcHandler(window, msg, wParam, lParam))
        return true;

    //g_Interfaces.Surface->SetCursorAlwaysVisible(!g_NewMenu.menuOpen);
    //g::interfaces::input->enableInput(!g_NewMenu.menuOpen);
    return (WndProcHandler(window, msg, wParam, lParam) && g_NewMenu.menuOpen) ? 1 : CallWindowProc(WndProc, window, msg, wParam, lParam);
    //return CallWindowProc(WndProc, window, msg, wParam, lParam);
}
