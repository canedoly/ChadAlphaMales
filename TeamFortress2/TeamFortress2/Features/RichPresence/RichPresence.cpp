#include "RichPresence.h"
#include "../Vars.h"

namespace source
{
	namespace features
	{
		bool SteamRichPresence::Update()
		{
			if (!g_SteamInterfaces.Friends015)
				return false;
			
			g_SteamInterfaces.Friends015->ClearRichPresence(); // Clear the previous RPC
			g_SteamInterfaces.Friends015->SetRichPresence(_("steam_display"), _("#TF_RichPresence_State_PlayingGeneric"));
			g_SteamInterfaces.Friends015->SetRichPresence(_("state"), _("PlayingMatchGroup"));
			g_SteamInterfaces.Friends015->SetRichPresence(_("currentmap"), Vars::Misc::SteamRPCText.c_str());
			//g_SteamInterfaces.Friends015->SetRichPresence(_("currentmap"), _("Currently Pwning with CAM"));

			return true;
		}

		void SteamRichPresence::Destroy()
		{
			g_SteamInterfaces.Friends015->ClearRichPresence();
		}
	}
}