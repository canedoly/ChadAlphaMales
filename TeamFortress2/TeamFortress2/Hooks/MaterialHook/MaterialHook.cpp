#include "MaterialHook.h"

void __stdcall Material::Uncache::Hook(bool bPreserveVars)
{
	fn OriginalFn = Table.Original<fn>(fartcumballs69sexweed);
	return;
	OriginalFn(g_Interfaces.MatSystem, bPreserveVars);
}
