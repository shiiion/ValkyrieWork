#pragma once

#include "offsets.h"

namespace valkyrie
{
	//Will block until base signatures have been read
	auto initializeGlobals() -> bool;

	struct Globals
	{
		Globals();

		uint32_t screenWidth, screenHeight;
		uint32_t entityList, clientState, localPlayer, cRender, viewAngles, radarBase, forceJump,
			inputField, dataTable, scaleFormUI, chatOpen, commandContext, commandContextMutex;

		EntityOffsets entOffs;
		MiscOffsets otherOffs;
	};

	extern Globals globals;
}