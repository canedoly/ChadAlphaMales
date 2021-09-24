#pragma once

#include "../SDK/SDK.h"
#include "../Features/Playerlist/Playerlist.h"
/*
template <class T>
class CVar
{
public:
	T m_Var;
	const wchar_t* m_szDisplayName;
;*/

namespace Vars
{
	namespace Sekret {
		inline HMODULE dll;
	}
	namespace Menu
	{
		inline bool Opened = false;
		inline Rect_t Position = { 100, 100, 870, 480 };
		inline const auto Title = XorStr(L"Team Fortress 2");
		inline int TitleBarH = 16;

		inline int SpacingX = 8;
		inline int SpacingY = 4;
		inline int SpacingText = 4;

		inline int CheckBoxW = 10;
		inline int CheckBoxH = 10;
		inline int CheckBoxFillSize = 4;

		inline int ButtonTabW = 198;
		inline int ButtonTabH = 25;

		inline int ButtonW = 90;
		inline int ButtonH = 20;

		inline int ButtonWSmall = 70;
		inline int ButtonHSmall = 16;

		inline int ComboBoxW = 90;
		inline int ComboBoxH = 14;

		inline int InputBoxW = 90;
		inline int InputBoxH = 14;
		inline int InputColorBoxW = 30;

		namespace Colors
		{
			//inline Color_t WindowBackground = = 30, 30, 30, 255 ;
			inline Color_t TitleBar = { 60, 60, 60, 255 };
			inline Color_t Text = {255, 255, 255, 190};
			inline Color_t Widget = { 20, 20, 20, 255 };
			inline Color_t WidgetActive = { 150, 0, 255, 255 };
			inline Color_t OutlineMenu = { 50, 50, 50, 255 };
		}
	}

	namespace Aimbot
	{
		namespace CritHack
		{
			inline bool Active = true;
			inline int CritKey = VK_LSHIFT;
			inline bool SaveBucket = false;
			inline bool MeeleCrits = false;
			inline bool CritBar = false;
		}

		namespace Global
		{
			inline bool Active = true;
			inline int AimKey = VK_LSHIFT;
			inline bool AutoShoot = true;
			inline bool AimPlayers = true;
			inline bool AimBuildings = true;
			inline bool IgnoreInvlunerable = true;
			inline bool IgnoreCloaked = false;
			inline bool IgnoreFriends = true;
			inline bool IgnoreTaunting = true;
		}

		namespace Hitscan
		{
			inline bool Active = true;
			inline int SortMethod = 0;
			inline int AimMethod = 1;
			inline int AimHitbox = 2;
			inline float AimFOV = 15.0f;
			inline float SmoothingAmount = 4.0f;
			inline int TapFire = 1;
			inline bool ScanHitboxes = true;
			inline bool ScanHead = true;
			inline bool ScanBuildings = true;
			inline bool WaitForHeadshot = true;
			inline bool WaitForCharge = true;
			inline bool SpectatedSmooth = false;
			inline bool ScopedOnly = false;
			inline bool AutoScope = false;
			inline bool AutoRev = false;
		}

		namespace Projectile
		{
			inline bool Active = true;
			inline bool PerformanceMode = true;
			inline int SortMethod = 0;
			inline int AimMethod = 1;
			inline int AimPosition = 2;
			inline float AimFOV = 25.0f;
			inline bool FeetAimIfOnGround = false;
		}

		namespace Melee
		{
			inline bool Active = true;
			inline int SortMethod = 1;
			inline int AimMethod = 1;
			inline float AimFOV = 45.0f;
			inline float SmoothingAmount = 8.0f;
			inline bool RangeCheck = true;
			inline bool PredictSwing = true;
			inline bool WhipTeam = true;
		}
	}

	namespace Triggerbot
	{
		namespace Global
		{
			inline bool Active = false;
			inline int TriggerKey = VK_LSHIFT;
			inline bool IgnoreInvlunerable = true;
			inline bool IgnoreCloaked = false;
			inline bool IgnoreFriends = false;
		}

		namespace Shoot
		{
			inline bool Active = false;
			inline bool TriggerPlayers = true;
			inline bool TriggerBuildings = true;
			inline bool HeadOnly = false;
			inline bool WaitForCharge = true;
			inline float HeadScale = 0.7f;
		}

		namespace Stab
		{
			inline bool Active = true;
			inline bool RageMode = false;
			inline bool Silent = false;
			inline bool Disguise = false;
			inline bool IgnRazor = false;
			inline float Range = 0.9f;
		}

		namespace Detonate
		{
			inline bool Active = true;
			inline bool Stickies = true;
			inline bool Flares = true;
			inline float RadiusScale = 1.0f;
		}

		namespace Blast
		{
			inline bool Active = false;
			inline bool Rage = false;
			inline bool Silent = false;
			inline int Fov = 55;
		}

		namespace Uber
		{
			inline bool Active = false;
			inline bool OnlyFriends = false;
			inline bool PopLocal = false;
			inline float HealthLeft = 35.0f;
		}
	}

	namespace ESP
	{

		namespace Main
		{
			//inline bool Active	= true;
			inline int Outline = 2;
		}

		namespace Players
		{
			inline bool Active = true;
			inline bool ShowLocal = true;
			inline int IgnoreTeammates = 2;
			inline int IgnoreCloaked = 0;
			inline bool Name = true;
			inline int Uber = 2;
			inline int Class = 1;
			inline bool Health = false;
			inline bool Cond = true;
			inline bool HealthBar = true;
			inline int Box = 0;
			inline int Bones = 0;
			inline bool GUID = false;
			inline bool Lines = false;
			inline bool Dlights = false;
			inline float DlightRadius = 200.0f;
			inline float Alpha = 1.0f;
		}

		namespace Buildings
		{
			inline bool Active = true;
			inline bool IgnoreTeammates = true;
			inline bool Name = true;
			inline bool Health = false;
			inline bool Owner = false;
			inline bool Level = true;
			inline bool Cond = true;
			inline bool HealthBar = true;
			inline bool Lines = false;
			inline int Box = 0;
			inline bool Dlights = false;
			inline float DlightRadius = 200.0f;
			inline float Alpha = 1.0f;
		}

		namespace World
		{
			inline bool Active = true;
			inline bool HealthText = true;
			inline bool AmmoText = true;
			inline float Alpha = 1.0f;
		}
	}

	namespace Chams
	{
		/*
		namespace Main
		{
			inline bool Active= false ;
		}
		*/

		namespace Players
		{
			inline bool Active = true;
			inline bool ShowLocal = true;
			inline int IgnoreTeammates = 2;
			inline bool Wearables = true;
			inline bool Weapons = true;
			inline int Material = 1;
			inline bool IgnoreZ = false;
			inline float Alpha = 1.0f;
		}

		namespace Buildings
		{
			inline bool Active = true;
			inline bool IgnoreTeammates = true;
			inline int Material = 1;
			inline bool IgnoreZ = false;
			inline float Alpha = 1.0f;
		}

		namespace World
		{
			inline bool Active = false;
			inline bool Health = true;
			inline bool Ammo = true;
			inline int Projectiles = 1;
			inline int Material = 1;
			inline bool IgnoreZ = false;
			inline float Alpha = 1.0f;
		}

		namespace DME
		{
			inline bool Active = false;
			inline bool SeeThru = false;

			inline int Hands = 0;
			inline float HandsAlpha = 1.0f;

			inline int Weapon = 0;
			inline float WeaponAlpha = 1.0f;
		}
	}

	namespace Glow
	{
		namespace Main
		{
			//inline bool Active= false;
			inline int Scale = 5;
		}

		namespace Players
		{
			inline bool Active = true;
			inline bool ShowLocal = true;
			inline int IgnoreTeammates = 2;
			inline bool Wearables = true;
			inline bool Weapons = true;
			inline float Alpha = 1.0f;
			inline int Color = 0;
		}

		namespace Buildings
		{
			inline bool Active = true;
			inline bool IgnoreTeammates = true;
			inline float Alpha = 1.0f;
			inline int Color = 0;
		}

		namespace World
		{
			inline bool Active = false;
			inline bool Health = true;
			inline bool Ammo = true;
			inline int Projectiles = 1;
			inline float Alpha = 1.0f;
		}
	}

	namespace Radar
	{
		namespace Main
		{
			inline bool Active = false;
			inline int  BackAlpha = 200;
			inline int  Size = 100;
			inline int  Range = 1500;
		}

		namespace Players
		{
			inline bool Active = true;
			inline int  IconType = 1;
			//inline int  BackGroundType	= 2;
			//inline bool Outline			= true;
			inline int  IgnoreTeam = 2;
			//inline int  IgnoreCloaked		= 0;
			inline bool Health = false;
			inline int  IconSize = 24;
		}

		namespace Buildings
		{
			inline bool Active = true;
			//inline bool Outline		= false;
			inline bool IgnoreTeam = true;
			inline bool Health = false;
			inline int  IconSize = 18;
		}

		namespace World
		{
			//inline bool Active     = false;
			inline bool Health = true;
			inline bool Ammo = true;
			inline int  IconSize = 14;
		}
	}

	namespace Visuals
	{
		inline bool RemoveDisguises = false;
		inline bool RemoveTaunts = false;
		inline int Fullbright = 0;
		inline int FieldOfView = 110;
		inline float AspectRatioValue = 0;
		inline int AimFOVAlpha = 10;

		inline int ViewModelX = 0;
		inline int ViewModelY = 0;
		inline int ViewModelZ = 0;

		inline bool RemoveScope = true;
		inline bool RemoveZoom = true;
		inline bool RemovePunch = false;
		inline bool CrosshairAimPos = true;
		inline bool ChatInfo = true;

		inline bool SpectatorList = true;
		//inline int SpectatorList = 3;

		inline bool SpyWarning = true;
		inline bool SpyWarningAnnounce = true;
		inline int SpyWarningStyle = 0;
		inline bool SpyWarningVisibleOnly = true;
		inline bool SpyWarningIgnoreFriends = true;

		inline bool Snow = false;
		inline bool ToolTips = false;

		inline bool ThirdPerson = false;
		inline int ThirdPersonKey = 0x0;
		inline bool ThirdPersonSilentAngles = true;
		inline bool ThirdPersonInstantYaw = true;

		inline bool WorldModulation = false;
		inline bool SkyboxChanger = true;

		inline bool PlayerList = false;

		namespace Skins
		{
			inline bool Enabled = false;
			inline int Sheen = 0;
			inline int Effect = 0;
			inline int Particle = 0;
			inline bool Acient = false;
			inline bool Override = false;
		}
	}

	namespace Skybox
	{
		inline int skyboxnum = 0;
		inline std::string SkyboxName = "";
	}

	namespace Misc
	{
		inline bool AutoJump = true;
		inline bool AutoStrafe = true;
		inline bool TauntSlide = false;
		inline bool TauntControl = false;
		inline bool BypassPure = false;
		inline bool NoisemakerSpam = false;
		inline bool DisableInterpolation = true;
		inline bool MedalFlip = true;
		inline bool AutoRocketJump = false;
		inline bool ChatSpam = false;
		inline bool NoPush = false;
		inline bool InstantRespawn = false;
		inline bool EdgeJump = false;
		inline bool AntiAFK = false;
		inline bool VoteRevealer = false;
		inline bool VotesInChat = false;
		namespace CL_Move
		{
			inline bool Enabled = true;
			inline bool Doubletap = true;
			inline bool NotInAir = true;
			//inline int TeleportKey= 0x46;
			inline int RechargeKey = 0x52;
			inline int DoubletapKey = 0x46;

			inline bool WaitForDT = true;

			inline bool Fakelag = true;
			inline bool FakelagOnKey = true;
			inline int FakelagKey = 0x54;
			inline int FakelagValue = 1;
		}
	}

	namespace AntiHack
	{
		namespace AntiAim
		{
			inline bool Active = false;
			inline int Pitch = 0;
			inline int SpinSpeed = 0;
			inline int YawReal = 0;
			inline int YawFake = 0;
		}
	}
}