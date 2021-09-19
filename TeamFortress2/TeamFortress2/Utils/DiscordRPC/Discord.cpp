#include "Discord.h"

std::time_t CurrentTime = std::time(0);

void Discord::init() {
	DiscordEventHandlers Handler;
	memset(&Handler, 0, sizeof(Handler));
	Discord_Initialize(_("885146235915354152"), &Handler, 1, NULL);
}

void Discord::update() {
	DiscordRichPresence discordPresence;
	memset(&discordPresence, 0, sizeof(discordPresence));
	//discordPresence.state = _("CAM.club");
	discordPresence.details = _("Currently Pwning with CAM.club");
	discordPresence.startTimestamp = CurrentTime;
	//discordPresence.largeImageText = _("lol");
	//discordPresence.largeImageKey = _("icon");
	//discordPresence.smallImageKey = _("icon");
	//discordPresence.smallImageText = _("Example for UC.me");
	Discord_UpdatePresence(&discordPresence);
}

