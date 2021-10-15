#include "EngineHook.h"

#include "../../Features/Vars.h"

#include "../../Features/Misc/Misc.h"

// By not sending a command this tick (returning), we charge 1 tick to use when doubletapping
// By calling original multiple times, we can batch all the charged ticks at once

void __cdecl EngineHook::CL_Move::Hook(float accumulated_extra_samples, bool bFinalTick)
{
	if (Vars::Misc::CL_Move::Doubletap.m_Var) {
		auto& pLocal = g_EntityCache.m_pLocal;
		if (!pLocal) {
			return Func.Original<fn>()(accumulated_extra_samples, bFinalTick);
		}
		/* Recharge */

		if (GetAsyncKeyState(Vars::Misc::CL_Move::RechargeKey.m_Var)) {
			dt.FastStop = false;
			dt.Shifting = false;
			dt.Recharging = true;
		}

		if (dt.Recharging && dt.Charged < dt.ToShift) {
			dt.barAlpha = 255;
			dt.Charged++;
			dt.ToWait = DT_WAIT_CALLS;
			return;
		}
		else {
			pLocal->SetTickBase(pLocal->GetTickBase() + dt.ToShift + 1);
			dt.Recharging = false;
		}

		Func.Original<fn>()(accumulated_extra_samples, (dt.Shifting && dt.ToWait == 0) ? true : bFinalTick);

		/* Doubletap */

		if (dt.ToWait > 0 && dt.Charged > 0) {
			dt.ToWait--;
			return;
		}

		if (dt.Charged == 0) {
			dt.Shifting = false;
			dt.FastStop = false;
		}

		if (dt.Shifting) {
			if (const auto& pLocal = g_EntityCache.m_pLocal) {
				while (dt.Charged < dt.ToShift + 1)
				{
					dt.FastStop = true;
					if (!dt.Shifting) {
						return;
					}
					if (dt.Charged <= dt.ToWait) {
						return;
					}
					Func.Original<fn>()(accumulated_extra_samples, (dt.Charged == 1));
					dt.Charged--;
				}
				dt.Shifting = false;
				dt.FastStop = false;
			}
		}
	}
	else {
		Func.Original<fn>()(accumulated_extra_samples, bFinalTick);
	}
}

//void __cdecl EngineHook::CL_Move::Hook(float accumulated_extra_samples, bool bFinalTick)
//{
//	if (Vars::Misc::CL_Move::Doubletap.m_Var)
//	{
//		g_GlobalInfo.fast_stop = false;
//		/*
//		if (Vars::Misc::CL_Move::TeleportKey.m_Var && (GetAsyncKeyState(Vars::Misc::CL_Move::TeleportKey.m_Var)) && !g_GlobalInfo.m_nShifted) //teleport
//		{
//			g_GlobalInfo.fast_stop = false;
//			while (g_GlobalInfo.m_nShifted < MAX_NEW_COMMANDS_HEAVY)87
//			{
//				g_GlobalInfo.fast_stop = false;
//				g_GlobalInfo.m_nShifted++;
//				Func.Original<fn>()(accumulated_extra_samples, (g_GlobalInfo.m_nShifted == (MAX_NEW_COMMANDS_HEAVY - 1))); //this teleports you
//				//g_GlobalInfo.m_nShifted++;
//			}
//
//			return;
//		}
//		*/
//		if (GetAsyncKeyState(Vars::Misc::CL_Move::RechargeKey.m_Var)) {//recharge key
//			g_GlobalInfo.fast_stop = false;
//			g_GlobalInfo.m_bRecharging = true;
//		}
//	}
//	
//	if (g_GlobalInfo.m_bRecharging && g_GlobalInfo.m_nShifted) //recharge
//	{
//		g_GlobalInfo.fast_stop = false;
//		g_GlobalInfo.m_nShifted--; //goes from 15 to 0
//		g_GlobalInfo.m_nWaitForShift = DT_WAIT_CALLS;
//		return;
//	}
//	else {
//		g_GlobalInfo.fast_stop = false;
//		g_GlobalInfo.m_bRecharging = false;
//	}
//
//
//
//
//	Func.Original<fn>()(accumulated_extra_samples, (g_GlobalInfo.m_bShouldShift && !g_GlobalInfo.m_nWaitForShift) ? true : bFinalTick);
//
//	if (g_GlobalInfo.m_nWaitForShift)
//	{
//		g_GlobalInfo.fast_stop = false;
//		g_GlobalInfo.m_nWaitForShift--;
//		return;
//	}
//	/*
//	if (g_GlobalInfo.m_bShouldShift) {
//		const auto& pLocal = g_EntityCache.m_pLocal;
//		int nClass = pLocal->GetClassNum();
//		int dtTicks;
//		if (nClass == CLASS_HEAVY) {
//			dtTicks = MAX_NEW_COMMANDS_HEAVY;
//		}
//		else {
//			dtTicks = MAX_NEW_COMMANDS;
//		}
//
//		CUserCmd* pCmd;
//
//		if (GetAsyncKeyState(Vars::Misc::CL_Move::DoubletapKey.m_Var)) {
//			if (g_GlobalInfo.m_nShifted < dtTicks) { // do not try shifting without being charged
//				for (int i = pCmd->command_number + 1; i < pCmd->command_number + 22 + 1; i++) {
//					CUserCmd* future_cmd = g_Interfaces.Input->GetUserCmd(i % MULTIPLAYER_BACKUP);
//					if (future_cmd)
//						memcpy(future_cmd, pCmd, sizeof(CUserCmd)); // copy current command as default data
//
//					// set up the command
//					future_cmd->command_number = i;
//					future_cmd->hasbeenpredicted = true;
//
//					// verify the command
//					CVerifiedUserCmd* verified_cmd = GetVerifiedCmds(i % MULTIPLAYER_BACKUP);
//					if (verified_cmd)
//						memcpy(&verified_cmd->m_cmd, future_cmd, sizeof(CUserCmd));
//
//					// CNetChan::SetChoked, magic netchannel values hit p
//					//INetChannelInfo* nci = g_tf2.m_engine_client->GetNetChannelInfo();
//					INetChannelInfo* nci = g_Interfaces.Engine->GetNetChannelInfo();
//					if (nci) {
//						//Func.Original<fn>()(accumulated_extra_samples, (g_GlobalInfo.m_nShifted == (dtTicks - 1))); //this doubletaps
//						GetVFunc<void(__thiscall*)(void*)>(nci, 45)(nci);
//					}
//					// manual offset for cl.chokedcommands, can be found reversing CL_SendMove
//					++* (int*)((int)L"engine.dll" + 0x469438);
//
//					verified_cmd->m_crc = future_cmd->GetChecksum();
//				}
//			}
//			g_GlobalInfo.fast_stop = false;
//
//			g_GlobalInfo.m_bShouldShift = false;
//		}
//	}
//	*/
//	if (g_GlobalInfo.m_bShouldShift)
//	{
//		const auto& pLocal = g_EntityCache.m_pLocal;
//		if (!pLocal) // lol.
//			return;
//
//		int nClass = pLocal->GetClassNum();
//		int dtTicks;
//		if (nClass == CLASS_HEAVY) {
//			dtTicks = MAX_NEW_COMMANDS_HEAVY;
//		}
//		else {
//			dtTicks = g_GlobalInfo.MaxNewCommands;
//		}
//		
//		if (GetAsyncKeyState(Vars::Misc::CL_Move::DoubletapKey.m_Var)) {
//			g_GlobalInfo.fast_stop = true;
//			while (g_GlobalInfo.m_nShifted < dtTicks)
//			{
//				if (!g_GlobalInfo.m_bShouldShift) {
//					return;
//				}
//				if (!Vars::Misc::CL_Move::NotInAir.m_Var) {
//					Func.Original<fn>()(accumulated_extra_samples, (g_GlobalInfo.m_nShifted == (dtTicks - 1))); //this doubletaps
//					g_GlobalInfo.m_nShifted++;
//				}
//				if (Vars::Misc::CL_Move::NotInAir.m_Var) {
//
//					if (pLocal->IsOnGround()) {
//						Func.Original<fn>()(accumulated_extra_samples, (g_GlobalInfo.m_nShifted == (dtTicks - 1))); //this doubletaps
//						g_GlobalInfo.m_nShifted++;
//					}
//					else {
//						g_GlobalInfo.fast_stop = false;
//						return;
//					}
//				}
//			}
//			g_GlobalInfo.fast_stop = false;
//		}
//		g_GlobalInfo.fast_stop = false;
//
//		g_GlobalInfo.m_bShouldShift = false;
//	}
//	
//}