#pragma once
#include "Include/discord_register.h"
#include "Include/discord_rpc.h"
#include "../../SDK/SDK.h"
#include <vcruntime_string.h>
#include <ctime>

class Discord
{
public:
	void init();
	void update();

};
inline Discord disc;