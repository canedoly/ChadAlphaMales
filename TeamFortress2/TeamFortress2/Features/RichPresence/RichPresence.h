#pragma once

#include "../../SDK/SDK.h"

namespace source
{
	namespace features
	{
		class SteamRichPresence
		{
		public:

			bool Update();

			void Destroy();

		};

		inline SteamRichPresence steamrichpresence;

	}
}