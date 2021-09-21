#pragma once
#include "../NewMenu/NewMenu.h"
#include "../Menu/ConfigManager/ConfigManager.h"
#include "../Vars.h"
#include <set>

// I want a playerlist that will show
// Name - Kick - Checkbox with ignore - Profile?

// File will consist of pi.friendsID on each newline
// If not checkbox the friendsID is in the file - they are ignored
// If checkbox && friendsID is in file - remove it from file

// Kick = callvote kick pi.userID

// Profile = g_SteamInterfaces.Friends015->ActivateGameOverlayToUser("steamid", CSteamID((UINT64)(0x0110000100000000ULL + pi.friendsID)));

class CPlayerlist {
private:
	//The location of all ignored users
	std::wstring ignorePath = (g_CFG.m_sConfigPath + _(L"\\CAMCore\\IGNORE.cam"));
	//A set of friendsIDs
	std::set<uint32_t> ignoredPlayers;

public:

	//Saves the file
	void SaveSet() {
		std::wofstream IgnoreWrite(ignorePath, std::ios::out);
		if (IgnoreWrite.is_open()) {
			for (uint32_t y : ignoredPlayers) {
				IgnoreWrite << y << "\n";
			}
		}
		IgnoreWrite.close();
	}

	// Run this when injecting
	void GetIgnoredPlayers() {
		//Get
		std::wifstream IgnoreRead(ignorePath);

		uint32_t x;
		while (IgnoreRead >> x)
		{
			ignoredPlayers.insert(x);
		}
	}

	// Pass in the users friendsID
	void AddIgnore(uint32_t friendsID) {
		ignoredPlayers.insert(friendsID);
		SaveSet();
	}

	void RemoveIgnore(uint32_t friendsID) {
		ignoredPlayers.erase(friendsID);
		SaveSet();
	}

	// Use this for checkbox?
	bool IsIgnored(uint32_t friendsID) {
		return ignoredPlayers.find(friendsID) != ignoredPlayers.end();
	}
};

inline CPlayerlist g_Playerlist;