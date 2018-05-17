#include "globals.h"
#include "valkAPI.h"
#include "signaturelist.h"
#include "memory.h"

namespace valkyrie
{
	auto initializeGlobals() -> bool
	{
		initializeSignatures();
		if (!checkAllSignatures())
		{
			return false;
		}

		csgoProc.read(entityListSigs.getSigAddress(), &globals.entityList, 1);
		csgoProc.read(clientStateSigs.getSigAddress(), &globals.clientState, 1);
		globals.clientState = clientStateSigs.getSigAddress();
		globals.localPlayer = localPlayerSigs.getSigAddress();
		globals.cRender = cRenderSigs.getSigAddress();
		globals.viewAngles = viewAngleSigs.getSigAddress();
		globals.radarBase = radarSigs.getSigAddress();
		globals.forceJump = forceJumpSigs.getSigAddress();
		globals.inputField = inputSystemSigs
	}

	Globals::Globals()
	{
		ZeroMemory(this, sizeof(Globals));


	}
}