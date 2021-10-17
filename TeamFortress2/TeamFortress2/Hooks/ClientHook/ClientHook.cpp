#include "ClientHook.h"

#include "../../Features/Misc/Misc.h"
#include "../../Features/Visuals/Visuals.h"
#include "../../Features/Menu/Menu.h"
#include "../../Features/AttributeChanger/AttributeChanger.h"
#include "../../Features/RichPresence/RichPresence.h"
#include "../../SDK/Includes/bitbuf.h"
void __stdcall ClientHook::PreEntity::Hook(char const* szMapName)
{
	Table.Original<fn>(index)(g_Interfaces.Client, szMapName);
}

void __stdcall ClientHook::PostEntity::Hook()
{
	Table.Original<fn>(index)(g_Interfaces.Client);

	g_Interfaces.Engine->ClientCmd_Unrestricted(_("r_maxdlights 69420"));
	g_Interfaces.Engine->ClientCmd_Unrestricted(_("r_dynamic 1"));
	g_Visuals.ModulateWorld();
}

void __stdcall ClientHook::ShutDown::Hook()
{
	Table.Original<fn>(index)(g_Interfaces.Client);
	g_EntityCache.Clear();
}

void __stdcall ClientHook::FrameStageNotify::Hook(EClientFrameStage FrameStage)
{
	switch (FrameStage)
	{
	case EClientFrameStage::FRAME_RENDER_START:
	{
		g_GlobalInfo.m_vPunchAngles = Vec3();

		if (Vars::Visuals::RemovePunch.m_Var) {
			if (const auto& pLocal = g_EntityCache.m_pLocal) {
				g_GlobalInfo.m_vPunchAngles = pLocal->GetPunchAngles();	//Store punch angles to be compesnsated for in aim
				pLocal->ClearPunchAngle();								//Clear punch angles for visual no-recoil
			}
		}
		g_Visuals.ViewmodelXYZ();
		g_Visuals.ARatio();
		g_Visuals.ThirdPerson();
		break;
	}

	default: break;
	}

	Table.Original<fn>(index)(g_Interfaces.Client, FrameStage);

	if (Vars::Misc::SteamRPC.m_Var)
		source::features::steamrichpresence.Update();

	switch (FrameStage)
	{
	case EClientFrameStage::FRAME_NET_UPDATE_START: {
		g_EntityCache.Clear();
		break;
	}

												  //#ifdef DEVELOPER_BUILD
	case EClientFrameStage::FRAME_NET_UPDATE_POSTDATAUPDATE_START: {
		g_AttributeChanger.Run();
		break;
	}
																 //#endif

	case EClientFrameStage::FRAME_NET_UPDATE_END:
	{
		g_EntityCache.Fill();

		g_GlobalInfo.m_bLocalSpectated = false;

		if (const auto& pLocal = g_EntityCache.m_pLocal)
		{
			for (const auto& Teammate : g_EntityCache.GetGroup(EGroupType::PLAYERS_TEAMMATES))
			{
				if (Teammate->IsAlive() || g_EntityCache.Friends[Teammate->GetIndex()])
					continue;

				CBaseEntity* pObservedPlayer = g_Interfaces.EntityList->GetClientEntityFromHandle(Teammate->GetObserverTarget());

				if (pObservedPlayer == pLocal)
				{
					switch (Teammate->GetObserverMode()) {
					case OBS_MODE_FIRSTPERSON: break;
					case OBS_MODE_THIRDPERSON: break;
					default: continue;
					}

					g_GlobalInfo.m_bLocalSpectated = true;
					break;
				}
			}
		}

		break;
	}

	case EClientFrameStage::FRAME_RENDER_START: {
		g_Visuals.UpdateWorldModulation();
		break;
	}

	default: break;
	}
}

bool __stdcall ClientHook::DispatchUserMessage::Hook(int msg_type, bf_read& msg_data)
{
	/*bf_read msg_copy = msg_data;
	msg_copy.Seek(0);
	switch (msg_type) {
	case 3:
	case 4:
		{
			char msg_name[50], playerName[128] params1 , msg[127] params2 ;


			int ent_idx = msg_copy.ReadByte();
			BOOL chat = msg_copy.ReadShort();
			msg_copy.ReadString(msg_name, 50);
			msg_copy.ReadString(playerName, 128);
			msg_copy.ReadString(msg, 127);
			BOOL textallchat = msg_copy.ReadShort();

			//std::cout << "msg_name: " << msg_name << std::endl;
			//std::cout << "player_name: " << playerName << std::endl;
			//std::cout << "message: " << msg << std::endl;
			g_Interfaces.CVars->ConsoleColorPrintf({ 0, 255, 12, 255 }, _("msg_type: %d\n"), msg_type);
			g_Interfaces.CVars->ConsoleColorPrintf({ 0, 255, 12, 255 }, _("msg_type: %d\n"), ent_idx);
			g_Interfaces.CVars->ConsoleColorPrintf({ 0, 255, 12, 255 }, _("msg_type: %d\n"), chat);
			g_Interfaces.CVars->ConsoleColorPrintf({ 0, 255, 12, 255 }, _("msg_name: %s\n"), msg_name);
			g_Interfaces.CVars->ConsoleColorPrintf({ 0, 255, 12, 255 }, _("playerName: %s\n"), playerName);
			g_Interfaces.CVars->ConsoleColorPrintf({ 0, 255, 12, 255 }, _("msg: %s\n"), msg);
			g_Interfaces.CVars->ConsoleColorPrintf({ 0, 255, 12, 255 }, _("textallchat: %d\n"), textallchat);
			break;
		}
	}*/
	return Table.Original<fn>(index)(g_Interfaces.Client, msg_type, msg_data);
}

using WriteUserCmd_t = void(__fastcall*)(void*, CUserCmd*, CUserCmd*);

const int MAX_USERCMD_LOSS = 10;
const int MAX_USERCMDS_SEND = 62;

uintptr_t WriteUserCmd_offset = g_Pattern.Find(L"client.dll", L"55 8B EC 83 E4 F8 51 53 56 8B D9 8B 0D");

void WriteUserCmd(bf_write* buffer, CUserCmd* to, CUserCmd* from) {
	static auto WriteUserCmdFn = reinterpret_cast<WriteUserCmd_t>(WriteUserCmd_offset);
	__asm
	{
		mov ecx, buffer;
		mov edx, to;
		push from;
		call WriteUserCmdFn;
		add esp, 4h;
	}
}


bool __stdcall ClientHook::WriteUserCmdDeltaToBuffer::Hook(bf_write* buffer, int from, int to, bool isNewCmd) {
	/*std::cout << from << std::endl;
	std::cout << buffer << std::endl;
	std::cout << to << std::endl;
	std::cout << isNewCmd << std::endl;
	static auto oWriteUserCmdDeltaToBuffer = Table.Original<fn>(index);
	if (g_GlobalInfo.m_nShifted >= 0) {
		return oWriteUserCmdDeltaToBuffer(g_Interfaces.Client, buffer, from, to, isNewCmd);
	}

	if (from != -1) {
		return true;
	}

	int tickbase = g_GlobalInfo.m_nShifted;

	g_GlobalInfo.m_nShifted = 0;
	//auto state = g_Interfaces.ClientState;
	int* pNewCmds			= (int*)((uintptr_t)buffer - 0x2C);
	int* pBackupCmds		= (int*)((uintptr_t)buffer - 0x30);
	auto newCmds			= pNewCmds;

	*pBackupCmds = 0;

	int iNewCmds			= *pNewCmds;
	int iNextCmds			= g_Interfaces.ClientState->chokedcommands + g_Interfaces.ClientState->lastoutgoingcommand + 1;
	int iTotalNewCmds =		std::min(iNewCmds + abs(tickbase), 22);

	*pNewCmds = iTotalNewCmds;
	for (to = iNextCmds - iNewCmds + 1; to <= iNextCmds; to++)
	{
		if (!oWriteUserCmdDeltaToBuffer(g_Interfaces.Client, buffer, from, to, true)) {
			return false;
		}
		from = to;
	}
	CUserCmd* pCmd = g_Interfaces.Input->GetUserCmd(from);
	if (!pCmd) {
		return true;
	}
	CUserCmd toCmd = *pCmd, fromCmd = *pCmd;
	toCmd.command_number++;
	toCmd.tick_count += 3 * g_Interfaces.GlobalVars->tickcount;
	for (int i = 0; i <= abs(tickbase); i++) {
		WriteUserCmd(buffer, &toCmd, &fromCmd);
		fromCmd = toCmd;
		toCmd.command_number++;
		toCmd.tick_count++;
	}

	return true;*/
	return Table.Original<fn>(index)(g_Interfaces.Client, buffer, from, to, isNewCmd);
}