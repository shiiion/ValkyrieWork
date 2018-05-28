#pragma once

#include "res.h"

namespace valkyrie
{
	struct PlayerPayload
	{
		std::array<wchar_t, 32> name;
		std::array<char, 32> weaponName;
		float distance;
		std::array<vec2, 19> bones;
		std::array<vec2, 2> bbox;
		std::array<vec2, 2> headbbox;
		std::array<vec2, 8> bbox3d;
		int32_t health;
		bool isTeammate;
	};

	struct ESPPayload
	{
		std::array<PlayerPayload, 128> players;
		std::array<vec2, 64> hitPoints;

		bool shouldDraw;
		bool drawName;
		bool drawWeaponName;
		bool drawDistance;
		bool drawBones;
		//0 = none, 1 = 2d, 2 = 3d
		uint8_t boundingBoxMode;
		bool drawHealth;
		bool drawHitpoints;

		uint8_t numPlayers;
		uint8_t numHitPoints;
	};

	//copies the static buffer payload to the global one
	auto copyBufferToPayload() -> void;
	//when we for sure dont want to draw
	auto disablePayload() -> void;
	//copies out the global payload
	auto copyOutPayload(ESPPayload& copyOut) -> void;

	auto getPayloadBuffer() -> ESPPayload&;
}