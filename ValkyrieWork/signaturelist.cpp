#include "signaturelist.h"
#include "memory.h"

namespace valkyrie
{
	Signature<3> entityListSigs;
	Signature<2> clientStateSigs;
	Signature<2> isInGameOffsetSigs;
	Signature<1> maxPlayerOffsetSigs;
	Signature<1> localPlayerSigs;
	Signature<1> localPlayerOffsetSigs;
	Signature<2> cRenderSigs;
	Signature<2> viewAngleSigs;
	Signature<1> radarSigs;
	Signature<2> forceJumpSigs;
	Signature<2> inputSystemSigs;
	Signature<2> inputSystemOffsetSigs;
	Signature<1> dataTable;
	Signature<1> dormantSigs;
	Signature<2> boneMatrixSigs;
	Signature<2> modelHeaderSigs;
	Signature<1> viewAngleOffsetSigs;
	Signature<1> commandContextSigs;
	Signature<1> gameResourcesSigs;
	Signature<1> scaleformUISigs;
	Signature<1> chatOpenOffsetSigs;

	auto derefRead(const uint32_t address, const uint32_t offset) -> uint32_t
	{
		uint32_t ret = badAddr;
		csgoProc.read(address + offset, &ret, 1);
		return ret;
	}

	//check if our "pointer" is actually in the target module
	auto pointerCheck(const uint32_t address, string const& targetMod) -> bool
	{
		Module const& sigModule = csgoProc.getModule(targetMod);
		const auto rangeCheckSig = [&sigModule](const uint32_t sigAddress) -> bool
		{
			return (sigAddress >= sigModule.baseAddress) &&
				(sigAddress <= sigModule.baseAddress + static_cast<uint32_t>(sigModule.moduleSize));
		};

		return (sigModule.name != "$modulenotfound") && rangeCheckSig(address);
	}

	auto noRead(const uint32_t address, const uint32_t offset) -> uint32_t
	{
		return address;
	}

	auto noCheck(const uint32_t address, string const& targetMod) -> bool
	{
		return true;
	}

	template<size_t T>
	auto Signature<T>::sanityCheckSigs() const -> bool
	{
		return getSigAddress() != badAddr;
	}

	template<size_t T>
	auto Signature<T>::getSigAddress(bool forceRescan) const -> uint32_t
	{
		if (cachedSig != badAddr && !forceRescan)
		{
			return cachedSig;
		}
		const string targetModName_s = (targetModName == "" ? modName : targetModName);
		
		for (SigPair_t const& pair : signatures)
		{
			uint32_t scanResult = csgoProc.sigScan(pair.first, modName);
			if (scanResult != badAddr)
			{
				uint32_t readResult = readFunction(scanResult, pair.second);
				if (checkFunction(readResult, targetModName_s))
				{
					return cachedSig = readResult;
				}
			}
		}
		return badAddr;
	}

	auto initializeSignatures() -> void
	{
		const auto byteRead = [](const uint32_t address, const uint32_t offset) -> uint32_t
		{
			uint8_t val;
			csgoProc.read(address + offset, &val, 1);
			return static_cast<uint32_t>(val);
		};

		const auto doubleDerefRead = [](const uint32_t address, const uint32_t offset) -> uint32_t
		{
			uint32_t ret = badAddr;
			csgoProc.read(address + offset, &ret, 1);
			csgoProc.read(ret, &ret, 1);
			return ret;
		};

		//some munted shit
		const auto radarRead = [](const uint32_t address, const uint32_t offset) -> uint32_t
		{
			uint32_t ret = badAddr;
			csgoProc.read(address + offset, &ret, 1);
			csgoProc.read(ret, &ret, 1);
			csgoProc.read(ret + 0x20, &ret, 1);
			return ret + 0x01D8;
		};

		//more munted shit
		const auto clientTableRead = [](const uint32_t address, const uint32_t offset) -> uint32_t
		{
			uint32_t ret = badAddr;
			csgoProc.read(address + offset, &ret, 1);
			csgoProc.read(ret + 32, &ret, 1);
			csgoProc.read(ret + 1, &ret, 1);
			csgoProc.read(ret, &ret, 1);
			return ret;
		};

		//radar is a bit... slow
		const auto retardedCheck = [](const uint32_t address, const string& targetMod) -> bool
		{
			return address >= csgoProc.getModule("csgo.exe").baseAddress;
		};

		const char* const client = "client.dll";
		const char* const engine = "engine.dll";
		const char* const inputSystem = "inputsystem.dll";
		const char* const scaleformUI = "scaleformui.dll";

		entityListSigs.modName = client;
		entityListSigs.checkFunction = pointerCheck;
		entityListSigs.signatures[0] = SigPair_t("83 C4 14 8B 0D ?? ?? ?? ?? 89", 0x05);
		entityListSigs.signatures[1] = SigPair_t("8B B5 ?? ?? ?? ?? 8B 0D", 0x08);
		entityListSigs.signatures[2] = SigPair_t("05 ?? ?? ?? ?? C1 E9 ?? 39 48 04", 0x01);

		clientStateSigs.modName = engine;
		clientStateSigs.checkFunction = pointerCheck;
		clientStateSigs.signatures[0] = SigPair_t("7C ?? A1 ?? ?? ?? ?? 83 B8", 0x03);
		clientStateSigs.signatures[1] = SigPair_t("A1 ?? ?? ?? ?? 83 B8 ?? ?? ?? ?? 06 0F 94 C0 C3", 0x01);

		isInGameOffsetSigs.modName = engine;
		isInGameOffsetSigs.signatures[0] = SigPair_t("7C ?? A1 ?? ?? ?? ?? 83 B8", 0x09);
		isInGameOffsetSigs.signatures[1] = SigPair_t("A1 ?? ?? ?? ?? 83 B8 ?? ?? ?? ?? 06 0F 94 C0 C3", 0x07);

		maxPlayerOffsetSigs.modName = engine;
		maxPlayerOffsetSigs.signatures[0] = SigPair_t("89 81 ?? ?? ?? ?? 8B 57 2C", 0x02);

		localPlayerSigs.modName = client;
		localPlayerSigs.checkFunction = pointerCheck;
		localPlayerSigs.signatures[0] = SigPair_t("8D 34 85 ?? ?? ?? ?? 89 15 ?? ?? ?? ?? 8B 41 08 8B 48", 0x03);

		localPlayerOffsetSigs.modName = client;
		localPlayerOffsetSigs.readFunction = byteRead;
		localPlayerOffsetSigs.signatures[0] = SigPair_t("8D 34 85 ?? ?? ?? ?? 89 15 ?? ?? ?? ?? 8B 41 08 8B 48", 0x12);

		cRenderSigs.modName = engine;
		cRenderSigs.checkFunction = pointerCheck;
		cRenderSigs.signatures[0] = SigPair_t("B9 ?? ?? ?? ?? A1 ?? ?? ?? ?? FF 60 38", 0x01);
		cRenderSigs.signatures[1] = SigPair_t("A1 ?? ?? ?? ?? B9 ?? ?? ?? ?? FF 60 10", 0x01);

		viewAngleSigs.modName = engine;
		viewAngleSigs.checkFunction = pointerCheck;
		viewAngleSigs.signatures[0] = SigPair_t("55 8B EC A1 ?? ?? ?? ?? B9 ?? ?? ?? ?? FF 50 14 8B 4D 08", 0x16);
		viewAngleSigs.signatures[1] = SigPair_t("A1 ?? ?? ?? ?? 8D 48 08 E9", 0x01);

		radarSigs.modName = client;
		radarSigs.readFunction = radarRead;
		radarSigs.checkFunction = retardedCheck;
		radarSigs.signatures[0] = SigPair_t("A1 ?? ?? ?? ?? 8B 0C B0 8B 01 FF 50 ?? 46 3B 35 ?? ?? ?? ?? 7C EA 8B 0D", 0x01);

		forceJumpSigs.modName = client;
		forceJumpSigs.checkFunction = pointerCheck;
		forceJumpSigs.signatures[0] = SigPair_t("55 8B EC 51 53 8A", 0xCB);
		forceJumpSigs.signatures[1] = SigPair_t("89 0D ?? ?? ?? ?? 8B 0D ?? ?? ?? ?? 8B D6 8B C1 83 CA 02", 0x08);

		inputSystemSigs.modName = inputSystem;
		inputSystemSigs.checkFunction = pointerCheck;
		inputSystemSigs.signatures[0] = SigPair_t("B9 ?? ?? ?? ?? 89 45 14 6A", 0x01);
		inputSystemSigs.signatures[1] = SigPair_t("C6 05 ?? ?? ?? ?? 00 B8", 0x08);

		inputSystemOffsetSigs.modName = inputSystem;
		inputSystemOffsetSigs.readFunction = noRead;
		inputSystemOffsetSigs.signatures[0] = SigPair_t("0F B6 87 ?? ?? ?? ?? 8D", 0x00);

		dataTable.modName = client;
		dataTable.readFunction = clientTableRead;
		dataTable.checkFunction = pointerCheck;
		dataTable.signatures[0] = SigPair_t("8B 08 56 68 C0", -0x68);

		dormantSigs.modName = client;
		dormantSigs.signatures[0] = SigPair_t("83 79 ?? FF 74 ?? 8A 81 ?? ?? ?? ?? C3", 0x08);

		boneMatrixSigs.modName = client;
		boneMatrixSigs.signatures[0] = SigPair_t("FF B7 ?? ?? ?? ?? 8D 84", 0x02);
		boneMatrixSigs.signatures[1] = SigPair_t("8B 8F ?? ?? ?? ?? 8B 74", 0x02);

		modelHeaderSigs.modName = client;
		modelHeaderSigs.signatures[0] = SigPair_t("F3 0F 11 45 FC 83 BE ?? ?? ?? ?? 00 75", 0x07);
		modelHeaderSigs.signatures[1] = SigPair_t("74 ?? 8B CE E8 ?? ?? ?? ?? 8B 8E ?? ?? ?? ?? 85 C9 0F", 0x0B);

		viewAngleOffsetSigs.modName = engine;
		viewAngleOffsetSigs.signatures[0] = SigPair_t("8B 14 85 ?? ?? ?? ?? 8B 82", 0x09);

		commandContextSigs.modName = engine;
		commandContextSigs.readFunction = noRead;
		commandContextSigs.signatures[0] = SigPair_t("FF 75 08 53 81 C1 ?? ?? ?? ?? E8", 0x00);

		gameResourcesSigs.modName = client;
		gameResourcesSigs.checkFunction = pointerCheck;
		gameResourcesSigs.signatures[0] = SigPair_t("83 F8 01 0F 85 ?? ?? ?? ?? 8B 3D", 0x0B);

		scaleformUISigs.modName = client;
		scaleformUISigs.readFunction = doubleDerefRead;
		scaleformUISigs.checkFunction = pointerCheck;
		scaleformUISigs.targetModName = scaleformUI;
		scaleformUISigs.signatures[0] = SigPair_t("6A 02 FF 50 ?? 8B 0D ?? ?? ?? ?? 6A 01 8B", 0x07);

		chatOpenOffsetSigs.modName = scaleformUI;
		chatOpenOffsetSigs.signatures[0] = SigPair_t("88 9E ?? ?? ?? ?? 5E 5B 5D", 0x02);
	}

	auto checkAllSignatures() -> bool
	{
		//unghetto me:
		return entityListSigs.sanityCheckSigs() &&
			clientStateSigs.sanityCheckSigs() &&
			isInGameOffsetSigs.sanityCheckSigs() &&
			maxPlayerOffsetSigs.sanityCheckSigs() &&
			localPlayerSigs.sanityCheckSigs() &&
			localPlayerOffsetSigs.sanityCheckSigs() &&
			cRenderSigs.sanityCheckSigs() &&
			viewAngleSigs.sanityCheckSigs() &&
			radarSigs.sanityCheckSigs() &&
			forceJumpSigs.sanityCheckSigs() &&
			inputSystemSigs.sanityCheckSigs() &&
			inputSystemOffsetSigs.sanityCheckSigs() &&
			dataTable.sanityCheckSigs() &&
			dormantSigs.sanityCheckSigs() &&
			boneMatrixSigs.sanityCheckSigs() &&
			modelHeaderSigs.sanityCheckSigs() &&
			viewAngleOffsetSigs.sanityCheckSigs() &&
			commandContextSigs.sanityCheckSigs() &&
			gameResourcesSigs.sanityCheckSigs() &&
			scaleformUISigs.sanityCheckSigs() &&
			chatOpenOffsetSigs.sanityCheckSigs();
	}
}