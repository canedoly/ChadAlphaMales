#include "SurfaceHook.h"

void __stdcall SurfaceHook::OnScreenSizeChanged::Hook(int OldWidht, int OldHeight)
{
	Table.Original<fn>(index)(g_Interfaces.Surface, OldWidht, OldHeight);

	g_ScreenSize.Update();
	g_Draw.ReloadFonts();
}

void __stdcall SurfaceHook::LockCursor::Hook()
{
	Table.Original<fn>(index)(g_Interfaces.Surface);
}