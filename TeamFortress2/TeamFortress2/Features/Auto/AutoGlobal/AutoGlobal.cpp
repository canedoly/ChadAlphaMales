#include "AutoGlobal.h"

#include "../../Vars.h"

bool CAutoGlobal::IsKeyDown()
{
	switch (Vars::Triggerbot::Global::TriggerKey)
	{
		case 0x0: return true;
		default: return (GetAsyncKeyState(Vars::Triggerbot::Global::TriggerKey) & 0x8000);
	}
}