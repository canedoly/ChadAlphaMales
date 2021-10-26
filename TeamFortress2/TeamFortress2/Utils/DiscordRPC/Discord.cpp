#include "Discord.h"

std::time_t CurrentTime = std::time(0);

void Discord::init() {
	DiscordEventHandlers Handler;
	memset(&Handler, 0, sizeof(Handler));
	Discord_Initialize(_("901542866956976180"), &Handler, 1, NULL);
}

void Discord::update() {
	DiscordRichPresence discordPresence;
	memset(&discordPresence, 0, sizeof(discordPresence));
	//discordPresence.state = _("CAM.club");
	discordPresence.details = _("Promoting your house, if it's for sale!");
	discordPresence.startTimestamp = CurrentTime;
	//discordPresence.largeImageText = _("lol");
	//discordPresence.largeImageKey = _("icon");
	//discordPresence.smallImageKey = _("icon");
	//discordPresence.smallImageText = _("Example for UC.me");
	Discord_UpdatePresence(&discordPresence);
}

