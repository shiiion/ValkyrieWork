#include "csgoutils.h"
#include "globals.h"
#include "memory.h"

namespace valkyrie
{
	static matrix_t viewMatrix;

	static const char* gunNames[] = 
		{ "",		 "Deagle",	"Dual Berettas",
		"Five Seven","Glock",	 "",
		"",			 "AK 47",	 "AUG",
		"AWP",		 "FAMAS",	 "G3SG1",
		"",			 "Galil",	 "M249",
		"",			 "M4A1",	  "MAC 10",
		"",			 "P90",		  "",
		"",			 "",		  "",
		"UMP 45",	 "XM1014",	"Bizon",
		"MAG7",		 "Negev",	 "Sawed Off",
		"Tec 9",	 "Zeus",	  "P2000",
		"MP7",		 "MP9",		  "Nova",
		"P250",		 "",		  "SCAR 20",
		"SG 556",	 "SSG 08",	"",
		"Knife",	 "Flashbang", "Grenade",
		"Smoke",	 "Molotov",   "Decoy",
		"Incendiary","C4",		  "",
		"",			 "",		  "",
		"",			 "",		  "",
		"",			 "",		  "Knife",
		"M4A1",		 "USP",		  "",
		"CZ75 Auto",  "Revolver" };

	static const char* knifeNames[] = 
		{  "Bayonet Knife",
			"",
			"",
			"",
			"",
			"Flip Knife",
			"Gut Knife",
			"Karambit Knife",
			"M9 Bayonet Knife",
			"Huntsman Knife",
			"",
			"",
			"Falchion Knife",
			"",
			"",
			"Butterfly Knife",
			"Shadow Daggers" };

	auto signonStateInGame() -> bool
	{
		uint8_t signonVal;
		csgoProc.read(globals.clientState + globals.otherOffs.gameState, &signonVal, 1);
		return signonVal == 6ui8;
	}

	auto checkKeyState(const uint32_t byte, const uint8_t bit) -> bool
	{
		uint8_t keystate;
		csgoProc.read(globals.inputField + byte, &keystate, 1);
		return keystate & (1ui8 << bit);
	}

	auto isChatClosed() -> bool
	{
		uint32_t state, state2;
		csgoProc.read(globals.scaleFormUI + globals.chatOpen, &state, 1);
		csgoProc.read(globals.scaleFormUI + globals.chatOpen - 4u, &state2, 1);
		return (state != 1u) && (state2 != 2u);
	}

	auto getWeaponName(const int32_t wepID, string& wepNameOut) -> void
	{
		if (range(wepID, 500, 516))
		{
			wepNameOut = knifeNames[wepID - 500];
		}
		else if (range(wepID, 1, 64))
		{
			wepNameOut = gunNames[wepID];
		}
	}

	auto sendUserCmd(string const& command) -> bool
	{
		//christ
		constexpr uint32_t zero = 0;
		constexpr uint32_t two = 2;
		const auto tryLockGame = []() -> bool
		{
			uint32_t threadOwner;
			csgoProc.read(globals.commandContextMutex, &threadOwner, 1);
			if (threadOwner)
			{
				return false;
			}
			threadOwner = 0xFFFFFFFFu;
			csgoProc.write(globals.commandContextMutex, &threadOwner, 1);
			return true;
		};
		const auto unlockGame = []() -> void
		{
			constexpr uint32_t noOwner = 0u;
			csgoProc.write(globals.commandContextMutex, &noOwner, 1);
		};

		const uint32_t newCmdLength = static_cast<const uint32_t>(command.length());

		bool isProcessing;
		csgoProc.read(globals.commandContext + 0x2048u, &isProcessing, 1);

		uint32_t curCmdLength;
		csgoProc.read(globals.commandContext + 0x2004u, &curCmdLength, 1);

		uint32_t listCount, listAlloc;
		csgoProc.read(globals.commandContext + 0x2020u, &listCount, 1);
		csgoProc.read(globals.commandContext + 0x2024u, &listAlloc, 1);

		uint32_t freeNode;
		csgoProc.read(globals.commandContext + 0x201Cu, &freeNode, 1);

		if ((newCmdLength >= 512u) ||
			isProcessing || 
			(curCmdLength + newCmdLength + 1u > 8192u) ||
			(listCount >= (listAlloc - 1u)) ||
			!freeNode ||
			!tryLockGame())
		{
			return false;
		}
		
		uint32_t curTick;
		csgoProc.read(globals.commandContext + 0x2034, &curTick, 1);

		listCount++;
		const uint32_t resultLen = curCmdLength + newCmdLength + 1;
		
		uint32_t headNode, tailNode;
		csgoProc.read(globals.commandContext + 0x2014u, &headNode, 1);
		csgoProc.read(globals.commandContext + 0x2018u, &tailNode, 1);

		uint32_t nextFreeNode;
		csgoProc.read(freeNode + 0x14u, &nextFreeNode, 1);
		if (!nextFreeNode || freeNode == nextFreeNode)
		{
			unlockGame();
			return false;
		}

		csgoProc.write(globals.commandContext + curCmdLength, command.c_str(), command.length() + 1);
		csgoProc.write(globals.commandContext + 0x201Cu, &nextFreeNode, 1);
		csgoProc.write(globals.commandContext + 0x2004u, &resultLen, 1);

		if (headNode)
		{
			csgoProc.write(tailNode + 0x14u, &freeNode, 1);
			csgoProc.write(freeNode + 0x10u, &tailNode, 1);
			csgoProc.write(freeNode + 0x14u, &zero, 1);
		}
		else
		{
			csgoProc.write(globals.commandContext + 0x2014u, &freeNode, 1);
			csgoProc.write(freeNode + 0x10u, &zero, 1);
			csgoProc.write(freeNode + 0x14u, &zero, 1);
		}

		csgoProc.write(globals.commandContext + 0x2018u, &freeNode, 1);
		csgoProc.write(globals.commandContext + 0x2020u, &listCount, 1);

		csgoProc.write(freeNode, &curTick, 1);
		csgoProc.write(freeNode + 0x04u, &curCmdLength, 1);
		csgoProc.write(freeNode + 0x08u, &newCmdLength, 1);
		csgoProc.write(freeNode + 0x0Cu, &two, 1);

		unlockGame();
		return true;
	}

	auto updateViewMatrix() -> void
	{
		uint32_t state;
		csgoProc.read(globals.cRender + 0xE8u, &state, 1u);
		
		if (state <= 1u)
		{
			csgoProc.read(globals.cRender + 0x9Cu, 
				reinterpret_cast<uintptr_t>(viewMatrix), 
				sizeof(matrix_t));
		}
		else
		{
			uint32_t off2 = 0;
			csgoProc.read(globals.cRender + 0xDCu, &off2, 1u);

			const uint32_t finalAddr = (0x210u * 2u) + off2 - 0x44u;
			csgoProc.read(globals.cRender + 0x9Cu,
				reinterpret_cast<uintptr_t>(viewMatrix),
				sizeof(matrix_t));
		}
	}

	auto getViewMatrix() -> const pmatrix_t
	{
		return static_cast<const pmatrix_t>(viewMatrix);
	}
}