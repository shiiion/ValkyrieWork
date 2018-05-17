#pragma once

#include "res.h"

namespace valkyrie
{
	class NetvarReader;

	struct EntityOffsets
	{
		uint32_t team;
		uint32_t dormant;
		uint32_t health;
		uint32_t pos;
		uint32_t view;
		uint32_t deadFlag;
		uint32_t modelHeader;
		uint32_t boneMatrix;
		uint32_t velocity;
		uint32_t rotation;
		uint32_t fFlags;
		uint32_t aimPunch;
		uint32_t viewPunch;
		uint32_t moveState;
		uint32_t hWeapon;
		uint32_t weaponID;
		uint32_t lastPlaceName;
		uint32_t detectedBySensor;

		bool hasBeenRead;

		void loadFromRecvTable(NetvarReader const& recv);
	};

	struct MiscOffsets
	{
		uint32_t gameState;
		uint32_t viewAngle;
	};
}