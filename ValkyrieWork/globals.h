#pragma once

#include "res.h"

namespace valkyrie
{
	//returns true if sigs succeeded and base globals read
	auto initializeGlobals() -> bool;
	//initialize me ingame
	auto initializeDTOffsets() -> bool;
	
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
	};

	struct MiscOffsets
	{
		uint32_t gameState;
		uint32_t maxPlayers;
		uint32_t viewAngle;
	};

	struct Globals
	{
		Globals();

		//this gets initialized outside of initalizeGlobals
		uint32_t screenWidth, screenHeight;


		uint32_t entityList, clientState, localPlayer, cRender, viewAngles, radarBase, forceJump,
			inputField, dataTable, scaleFormUI, chatOpen, commandContext, commandContextMutex;

		EntityOffsets entOffs;
		MiscOffsets otherOffs;
	};
	//extra entity offsets
	constexpr uint32_t bboxMinOffset = 0x320;
	constexpr uint32_t bboxMaxOffset = bboxMinOffset + 0x0C;

	//ok
	constexpr uint32_t entListStride = 0x10;
	constexpr uint32_t matrixSize = 48;

	//model offsets
	constexpr uint32_t hitboxSetOffset = 0xB0;
	constexpr uint32_t bboxCountOffset = 0x04;
	constexpr uint32_t bboxOffset = 0x08;

	constexpr uint32_t hitvectorOffset = 0xBAA4;

	extern Globals globals;
}