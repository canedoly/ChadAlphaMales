#pragma once

#include "../../SDK/SDK.h"

namespace ClientHook
{
	inline SEOHook::VTable Table;

	namespace PreEntity
	{
		const int index = 5;
		using fn = void(__thiscall *)(CBaseClientDLL *, char const *);
		void __stdcall Hook(char const *szMapName);
	}

	namespace PostEntity
	{
		const int index = 6;
		using fn = void(__thiscall *)(CBaseClientDLL *);
		void __stdcall Hook();
	}

	namespace ShutDown
	{
		const int index = 7;
		using fn = void(__thiscall *)(CBaseClientDLL *);
		void __stdcall Hook();
	}

	namespace FrameStageNotify
	{
		const int index = 35;
		using fn = void(__thiscall *)(CBaseClientDLL *, EClientFrameStage);
		void __stdcall Hook(EClientFrameStage FrameStage);
	}

	namespace DispatchUserMessage
	{
		const int index = 36;
		using fn = bool(__thiscall*)(CBaseClientDLL*, int, bf_read&);
		bool __stdcall Hook(int msg_type, bf_read& msg_data);
	}

	//23 WriteUsercmdDeltaToBuffer(int, bf_write*, int, int, bool)
	namespace WriteUserCmdDeltaToBuffer
	{
		const int index = 23;
		using fn = bool(__thiscall*)(CBaseClientDLL*, bf_write*, int, int, bool);
		bool __stdcall Hook(bf_write* buffer, int from, int to, bool isNewCmd);
	}
}