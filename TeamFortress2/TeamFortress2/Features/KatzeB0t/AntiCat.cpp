#include "AntiCat.h"

bool send_drawline_reply = true;
constexpr int CAT_IDENTIFY = 0xCA7;
constexpr int CAT_REPLY = 0xCA8;
//constexpr float AUTH_MESSAGE = 1234567.0f;

// Welcome back Achievement based identify.
void sendAchievementKv()
{
    KeyValues* identify = new KeyValues(_("AchievementEarned"));
    KeyValues* mark = new KeyValues(_("AchievementEarned"));

    identify->SetInt("achievementID", CAT_REPLY);
    mark->SetInt("achievementID", CAT_IDENTIFY);

    g_Interfaces.Engine->ServerCmdKeyValues(identify);
    g_Interfaces.Engine->ServerCmdKeyValues(mark);
}

void Exploits::CH::run_auth() {
    if (send_drawline_reply)
        sendAchievementKv();
    
        send_drawline_reply = !send_drawline_reply;
}