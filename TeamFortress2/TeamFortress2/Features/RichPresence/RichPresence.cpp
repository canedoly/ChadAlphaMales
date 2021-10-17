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
			g_SteamInterfaces.Friends015->SetRichPresence(_("steam_display"), _("#TF_RichPresence_Display"));
			g_SteamInterfaces.Friends015->SetRichPresence(_("state"), _("PlayingMatchGroup"));
			g_SteamInterfaces.Friends015->SetRichPresence(_("matchgrouploc"), _("SpecialEvent"));
			
			// Let's do both of these just incase
			g_SteamInterfaces.Friends015->SetRichPresence(_("currentmap"), Vars::Misc::SteamRPCText.c_str());
			
			g_SteamInterfaces.Friends015->SetRichPresence(_("steam_player_group"), _("ChadAlphaMales.club")); 
			g_SteamInterfaces.Friends015->SetRichPresence(_("steam_player_group_size"), _("1338")); // smh... You have to do + 1 for some reaso

			return true;
		}

		void SteamRichPresence::Destroy()
		{
			g_SteamInterfaces.Friends015->ClearRichPresence();
		}
	}
}