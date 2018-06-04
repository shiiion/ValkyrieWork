#include "globals.h"
#include "valkAPI.h"
#include "signaturelist.h"
#include "memory.h"
#include "netvarreader.h"

namespace valkyrie
{
	Globals globals;

	auto initializeGlobals() -> bool
	{
		initializeSignatures();
		if (!checkAllSignatures())
		{
			PRINTLN_LOG("WHY");
			return false;
		}

		globals.entityList = entityListSigs.getSigAddress();
		csgoProc.read(clientStateSigs.getSigAddress(), &globals.clientState, 1);
		//globals.clientState = clientStateSigs.getSigAddress();

		globals.localPlayer = localPlayerSigs.getSigAddress() + localPlayerOffsetSigs.getSigAddress();
		
		globals.cRender = cRenderSigs.getSigAddress();
		csgoProc.read(viewAngleSigs.getSigAddress(), &globals.viewAngles, 1);
		globals.radarBase = radarSigs.getSigAddress();
		globals.forceJump = forceJumpSigs.getSigAddress();
		//now THIs is munted
		{
			const uint32_t inputFieldOffsets = inputSystemOffsetSigs.getSigAddress();
			uint32_t off1, off2, off3, inputFieldOffset;
			csgoProc.read(inputFieldOffsets + 0x03, &off1, 1);
			csgoProc.read(inputFieldOffsets + 0x09, &off2, 1);
			csgoProc.read(inputFieldOffsets + 0x0F, &off3, 1);

			globals.inputField = inputSystemSigs.getSigAddress();
			csgoProc.read(globals.inputField + off1, &inputFieldOffset, 1);
			
			globals.inputField += (off3 * inputFieldOffset) + off2;
		}

		globals.dataTable = dataTable.getSigAddress();
		//FIX THESE YAY!!
		csgoProc.read(commandContextSigs.getSigAddress() + 0x06, &globals.commandContext, 1);
		csgoProc.read(commandContextSigs.getSigAddress() - 0x35, &globals.commandContextMutex, 1);

		//FIX THIS TOO? ? 
		globals.scaleFormUI = scaleformUISigs.getSigAddress();
		globals.chatOpen = chatOpenOffsetSigs.getSigAddress();

		//~~~~~~~~~~
		
		globals.entOffs.dormant = dormantSigs.getSigAddress() + 0x08;
		globals.entOffs.boneMatrix = boneMatrixSigs.getSigAddress() + 0x04;
		globals.entOffs.modelHeader = modelHeaderSigs.getSigAddress();
		globals.otherOffs.gameState = isInGameOffsetSigs.getSigAddress();
		//FIX THIS!?!?
		globals.otherOffs.maxPlayers = maxPlayerOffsetSigs.getSigAddress() + 0x08;
		globals.otherOffs.viewAngle = viewAngleOffsetSigs.getSigAddress();

		return true;
	}

	auto initializeDTOffsets() -> bool
	{
		if (globals.entOffs.hasBeenRead)
		{
			return true;
		}

		NetvarReader netvarReader;
		netvarReader.readNetvarTable();

		if (!netvarReader.tablesRead())
		{
			return false;
		}

		auto const& baseEntityTable = netvarReader.getTable("DT_BaseEntity");
		auto const& basePlayerTable = netvarReader.getTable("DT_BasePlayer");
		auto const& csPlayerTable = netvarReader.getTable("DT_CSPlayer");
		auto const& lpExclusiveTable = netvarReader.getTable("DT_LocalPlayerExclusive");
		auto const& cslpExclusiveTable = netvarReader.getTable("DT_CSLocalPlayerExclusive");
		auto const& localTable = netvarReader.getTable("DT_Local");
		auto const& econEntityTable = netvarReader.getTable("DT_EconEntity");
		auto const& attributeContainerTable = netvarReader.getTable("DT_AttributeContainer");
		auto const& scriptCreatedItemTable = netvarReader.getTable("DT_ScriptCreatedItem");

		globals.entOffs.team = baseEntityTable.getPropOffset("m_iTeamNum");

		globals.entOffs.health = basePlayerTable.getPropOffset("m_iHealth");
		globals.entOffs.deadFlag = basePlayerTable.getPropOffset("m_lifestate");
		globals.entOffs.fFlags = basePlayerTable.getPropOffset("m_fFlags");
		globals.entOffs.lastPlaceName = basePlayerTable.getPropOffset("m_szLastPlaceName");

		globals.entOffs.rotation = csPlayerTable.getPropOffset("m_angEyeAngles[0]");
		globals.entOffs.moveState = csPlayerTable.getPropOffset("m_iMoveState");
		globals.entOffs.detectedBySensor = csPlayerTable.getPropOffset("m_flDetectedByEnemySensorTime");

		globals.entOffs.view = lpExclusiveTable.getPropOffset("m_vecViewOffset[0]");
		globals.entOffs.velocity = lpExclusiveTable.getPropOffset("m_vecVelocity[0]");
		globals.entOffs.aimPunch = lpExclusiveTable.getPropOffset("m_Local") +
			localTable.getPropOffset("m_aimPunchAngle");
		globals.entOffs.viewPunch = lpExclusiveTable.getPropOffset("m_Local") +
			localTable.getPropOffset("m_viewPunchAngle");

		globals.entOffs.pos = cslpExclusiveTable.getPropOffset("m_vecOrigin");

		globals.entOffs.weaponID = econEntityTable.getPropOffset("m_AttributeManager") +
			attributeContainerTable.getPropOffset("m_Item") +
			scriptCreatedItemTable.getPropOffset("m_iItemDefinitionIndex");

		return globals.entOffs.hasBeenRead = true;
	}

	Globals::Globals()
	{
		ZeroMemory(this, sizeof(Globals));
	}
}