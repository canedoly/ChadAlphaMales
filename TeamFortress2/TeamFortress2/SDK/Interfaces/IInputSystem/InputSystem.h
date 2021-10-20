#pragma once
#include "../../Includes/Includes.h"

class IInputSystem
{
public:
	virtual void AttachToWindow(void* hWnd) = 0;
	virtual void DetachFromWindow() = 0;

	virtual void EnableInput(bool bEnable) = 0;
	void ResetInputState()
	{
		typedef void(__thiscall* OrgFn)(PVOID);
		GetVFunc<OrgFn>(this, 25)(this);
	}
};

#define VINPUTSYSTEM_INTERFACE "InputSystemVersion001"