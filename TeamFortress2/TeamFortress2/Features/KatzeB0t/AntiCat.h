#pragma once
#include "../../Hooks/Hooks.h"
#include "../../SDK/SDK.h"

namespace Exploits {
	class CH {
	public:
		void sendDrawlineKv(float x_value, float y_value);
		void run_auth();
	};
	inline CH cathook;
}