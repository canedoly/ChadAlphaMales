#include "AntiCat.h"

bool send_drawline_reply = true;
constexpr int CAT_IDENTIFY = 0xCA7;
constexpr int CAT_REPLY = 0xCA8;
constexpr float AUTH_MESSAGE = 1234567.0f;

void Exploits::Cathook::sendDrawlineKv(float x_value, float y_value) {
    KeyValues* kv = new KeyValues(_("cl_drawline"));
    kv->SetInt(_("panel"), 2);
    kv->SetInt(_("line"), 0);
    kv->SetFloat(_("x"), x_value);
    kv->SetFloat(_("y"), y_value);
    g_Interfaces.Engine->ServerCmdKeyValues(kv);
}

void Exploits::Cathook::run_auth() {
    if (send_drawline_reply)
        sendDrawlineKv(0xCA8, 1234567.0f);

    send_drawline_reply = !send_drawline_reply;
}