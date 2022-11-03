#include "EngineHook.h"

#include "../../Features/Vars.h"

#include "../../Features/Misc/Misc.h"


void __cdecl EngineHook::CL_Move::Hook(float accumulated_extra_samples, bool bFinalTick)
{
	static auto oClMove = Func.Original<fn>();
	const auto& pLocal = g_EntityCache.m_pLocal;
	// this is being worked on
	// there are stuff in clientmodehook i need to change
	// i also plan to make this way more customizable

	if (!Vars::Misc::CL_Move::Doubletap.m_Var)
	{
		DT.currentTicks = 0;
		return oClMove(accumulated_extra_samples, bFinalTick);
	}

	if (Vars::Misc::CL_Move::TeleportKey.m_Var && GetAsyncKeyState(Vars::Misc::CL_Move::TeleportKey.m_Var))
	{
		switch (Vars::Misc::CL_Move::WarpMode.m_Var)
		{
			case 0:	// plain
			{
				while (DT.currentTicks > 0)
				{
					oClMove(0, DT.currentTicks == 1);
					DT.currentTicks--;
				}
				break;
			}
			case 1:	// smooth/boost
			{
				if (DT.currentTicks)
				{
					oClMove(0, DT.currentTicks == 1);
					DT.currentTicks--;
				}
				break;
			}
			case 2:	//2nd method of smooth
			{
				if (DT.currentTicks)
				{
					oClMove(0, false);
					DT.currentTicks--;
				}
				break;
			}
			// rn i have 2 methods of using rijin smooth warp/boost whichever works better gets officialy added
		}
		return;
	}

	// i should make a fail safe if were fakelagging but not rn cuz fakelag doesn't even work l0l
	if (GetAsyncKeyState(Vars::Misc::CL_Move::RechargeKey.m_Var))
	{
		DT.isRecharging = true;
		DT.shouldShift = false;
	}

	// recharge
	if (DT.isRecharging && DT.currentTicks < Vars::Misc::CL_Move::DTTicks.m_Var - 1)	// (- 1) because 1 tick/command is reserved for cl_move call. So 22 ticks is actually 21
	{
		g_GlobalInfo.ForceSendPacket = true;
		DT.barAlpha = 255;
		DT.shouldStop = false;
		DT.currentTicks++;
		DT.WaitTicks = 26;	// turn this into a var (amount of ticks we want to wait + latency should be smart)
		return;
	}
	else	// if we aren't recharging set it to false
	{
		DT.isRecharging = false;
	}

	oClMove(accumulated_extra_samples, (DT.isShifting && !DT.WaitTicks) ? true : bFinalTick);

	if (Vars::Misc::CL_Move::NotInAir.m_Var)
	{
		if (!pLocal->IsOnGround())	// the player isn't on ground, and the var is enabled. so we shouldn't use dt
		{
			DT.shouldShift = false;
		}
	}
	
	if (DT.WaitTicks && Vars::Misc::CL_Move::WaitForDT.m_Var)	// we have wait ticks and var is enabled, so we shouldn't use dt
	{ 
		DT.shouldShift = false;
	}

	if (DT.WaitTicks)
	{
		DT.WaitTicks--;
	}

	if (DT.shouldShift)
	{
		const auto& pLocal = g_EntityCache.m_pLocal;
		if (!pLocal) { return; }

		if (Vars::Misc::CL_Move::DTMode.m_Var == 0 && GetAsyncKeyState(Vars::Misc::CL_Move::DoubletapKey.m_Var) 
		|| Vars::Misc::CL_Move::DTMode.m_Var == 1)
		{
			DT.isShifting = true;
			DT.shouldStop = true;

			while (DT.currentTicks > 0)
			{
				oClMove(0, (DT.currentTicks == 1));
				DT.currentTicks--;
			}

			g_Interfaces.Engine->FireEvents();
			DT.WaitTicks = 26;
		}
		DT.shouldShift = false;
		DT.shouldStop = false;
	}
}

// todo: add proper cl_sendmove
// and try to fix tickbase

// the commit with updated cl_sendmove - https://github.com/canedoly/Fedoraware/commit/824166b82ea1e3258b1ded320fa722e590e8fc6c

// fedorawre history - https://github.com/canedoly/Fedoraware/commits/afb1b82c0d630f63991462cb01b186037e711260

// hook - https://github.com/canedoly/Fedoraware/commit/9b00165051b22bf6a1f7f23208b20262ccd9a3ff

void __cdecl EngineHook::CL_SendMove::Hook(void* ecx, void* edx)
{
byte data[4000];

	const int nextcommandnr = g_Interfaces.ClientState->lastoutgoingcommand + g_Interfaces.ClientState->chokedcommands + 1;

	CLC_Move moveMsg;
	moveMsg.m_DataOut.StartWriting(data, sizeof(data));
	moveMsg.m_nNewCommands = std::clamp(1 + g_Interfaces.ClientState->chokedcommands, 0, 15);
	const int extraCommands = g_Interfaces.ClientState->chokedcommands + 1 - moveMsg.m_nNewCommands;
	const int backupCommands = std::max(2, extraCommands);
	moveMsg.m_nBackupCommands = std::clamp(backupCommands, 0, 7);

	const int numcmds = moveMsg.m_nNewCommands + moveMsg.m_nBackupCommands;

	int from = -1;
	bool bOK = true;
	for (int to = nextcommandnr - numcmds + 1; to <= nextcommandnr; to++) {
		const bool isnewcmd = to >= nextcommandnr - moveMsg.m_nNewCommands + 1;
		bOK = bOK && g_Interfaces.Client->WriteUsercmdDeltaToBuffer(&moveMsg.m_DataOut, from, to, isnewcmd);
		from = to;
	}

	if (bOK) {
		if (extraCommands)
			g_Interfaces.ClientState->m_NetChannel->m_nChokedPackets -= extraCommands;
		GetVFunc<bool(__thiscall*)(PVOID, INetMessage* msg, bool, bool)>(g_Interfaces.ClientState->m_NetChannel, 37)(g_Interfaces.ClientState->m_NetChannel, &moveMsg, false, false);
	}
	//static auto originalFn = Func.Original<fn>();
	//return originalFn(ecx, edx);
}













/*
	if (Vars::Misc::CL_Move::Doubletap.m_Var)
	{
		dt.FastStop = false;
		// if (Vars::Misc::CL_Move::TeleportKey.m_Var && (GetAsyncKeyState(Vars::Misc::CL_Move::TeleportKey.m_Var)) && !g_GlobalInfo.m_nShifted) //teleport
		// {
		// 	g_GlobalInfo.fast_stop = false;
		// 	while (g_GlobalInfo.m_nShifted < MAX_NEW_COMMANDS_HEAVY)87
		// 	{
		// 		g_GlobalInfo.fast_stop = false;
		// 		g_GlobalInfo.m_nShifted++;
		// 		oClMove(accumulated_extra_samples, (g_GlobalInfo.m_nShifted == (MAX_NEW_COMMANDS_HEAVY - 1))); //this teleports you
		// 		//g_GlobalInfo.m_nShifted++;
		// 	}
		// 	return;
		// }
		if (GetAsyncKeyState(Vars::Misc::CL_Move::RechargeKey.m_Var)) {//recharge key
			dt.FastStop = false;
			dt.Recharging = true;
		}
	}

	if (dt.Recharging && dt.Charged < Vars::Misc::CL_Move::DTTicks.m_Var) //recharge
	{
		dt.barAlpha = 255;
		dt.FastStop = false;
		dt.Charged++;
		dt.ChargedReverse--;
		dt.ToWait = DT_WAIT_CALLS;
		return;
	}
	else {
		dt.FastStop = false;
		dt.Recharging = false;
	}

	oClMove(accumulated_extra_samples, (dt.Shifting && !dt.ToWait) ? true : bFinalTick);

	if (dt.ToWait)
	{
		dt.FastStop = false;
		dt.ToWait--;
		return;
	}
	if (dt.Shifting)
	{
		const auto& pLocal = g_EntityCache.m_pLocal;
		if (!pLocal) // lol.
			return;

		if (GetAsyncKeyState(Vars::Misc::CL_Move::DoubletapKey.m_Var)) {
			dt.FastStop = true;
			while (dt.Charged != 0)
			{
				if (!dt.Shifting) {
					return;
				}
				if (!Vars::Misc::CL_Move::NotInAir.m_Var) {
					oClMove(accumulated_extra_samples, (dt.Charged == 1)); //this doubletaps
					dt.ChargedReverse++;
					dt.Charged--;
				}
				if (Vars::Misc::CL_Move::NotInAir.m_Var) {

					if (pLocal->IsOnGround()) {
						oClMove(accumulated_extra_samples, (dt.Charged == 1)); //this doubletaps
						dt.ChargedReverse++;
						dt.Charged--;
					}
					else {
						dt.FastStop = false;
						return;
					}
				}
			}
			dt.FastStop = false;
		}
		dt.FastStop = false;

		dt.Shifting = false;
	}
*/