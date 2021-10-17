#pragma once
#include "../../Includes/Includes.h"

class IInputSystem
{
public:
	void ResetInputState()
	{
		typedef void(__thiscall* OrgFn)(PVOID);
		GetVFunc<OrgFn>(this, 25)(this);
	}
};

#define VINPUTSYSTEM_INTERFACE "InputSystemVersion001"