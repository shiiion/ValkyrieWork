#pragma once
#include "res.h"

namespace valkyrie
{
	typedef std::pair<string, uint32_t> SigPair_t;
	typedef std::function<uint32_t(const uint32_t)> SigRead_t;
	typedef std::function<bool(const uint32_t, string const&)> SigCheck_t;

	static auto derefRead(const uint32_t address) -> uint32_t;
	static auto pointerCheck(const uint32_t address, string const& targetMod) -> bool;
	static auto noRead(const uint32_t address) -> uint32_t { return address; }
	static auto noCheck(const uint32_t address, string const& targetMod) -> bool { return true; }

	template<constexpr size_t T>
	struct Signature
	{
		std::array<std::pair<string, uint32_t>, T> signatures;
		//module our sig is in
		std::string modName;

		//module our sig value is in (eg: sig in client refers to data in engine)
		//empty string means targetModName = modName
		std::string targetModName = "";

		//read is called after sigscan success
		SigRead_t readFunction = derefRead;
		//check is called after read
		SigCheck_t checkFunction = noCheck;

		//checks if the first good signature is not badAddr
		inline auto sanityCheckSigs() const -> bool;
		auto getSigAddress(bool forceRescan = false) const -> uint32_t;
	private:
		uint32_t cachedSig = badAddr;
	};

	Signature<3> entityListSigs;
	Signature<2> clientStateSigs;
	Signature<2> isInGameSigs;
	Signature<1> localPlayerSigs;
	Signature<1> localPlayerOffsetSigs;
	Signature<2> cRenderSigs;
	Signature<2> viewAngleSigs;
	Signature<1> radarSigs;
	Signature<2> forceJumpSigs;
	Signature<2> inputSystemSigs;
	Signature<2> inputSystemOffsetSigs;
	Signature<1> CHLClientListVTableSigs;
	Signature<1> dormantSigs;
	Signature<2> boneMatrixSigs;
	Signature<2> modelHeaderSigs;
	Signature<1> viewAngleOffsetSigs;
	Signature<1> commandContextSigs;
	Signature<1> gameResourcesSigs;
	Signature<1> scaleformUISigs;
	Signature<1> chatOpenOffsetSigs;

	auto initializeSignatures() -> void;
	auto checkAllSignatures() -> bool;
}