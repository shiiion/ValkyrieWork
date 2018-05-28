#pragma once

#include "res.h"

namespace valkyrie
{
	//checks if signonstate is ingame
	auto signonStateInGame() -> bool;

	//checks csgo's input bitfield for our input
	//NOTE: bit will be used as (1ui8 << bit)
	auto checkKeyState(const uint32_t byte, const uint8_t bit) -> bool;

	//checks if ingame chatbox is closed
	auto isChatClosed() -> bool;

	//get weapon's name
	auto getWeaponName(const int32_t wepID, string& wepNameOut) -> void;

	//sends a command string akin to ExecuteClientCMD, returns false on failure
	auto sendUserCmd(string const& command) -> bool;

	auto updateViewMatrix() -> void;
	auto getViewMatrix() -> const pmatrix_t;
}