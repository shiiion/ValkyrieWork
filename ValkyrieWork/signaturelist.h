#pragma once
#include "res.h"

namespace valkyrie
{
	typedef std::pair<string, uint32_t> SigPair_t;
	typedef std::function<uint32_t(const uint32_t, const uint32_t)> SigRead_t;
	typedef std::function<bool(const uint32_t, string const&)> SigCheck_t;

	auto derefRead(const uint32_t address, const uint32_t offset) -> uint32_t;
	auto pointerCheck(const uint32_t address, string const& targetMod) -> bool;
	auto noRead(const uint32_t address, const uint32_t offset) -> uint32_t;
	auto noCheck(const uint32_t address, string const& targetMod) -> bool;

	template<size_t T>
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
		mutable uint32_t cachedSig = badAddr;
	};

	extern Signature<3> entityListSigs;
	extern Signature<2> clientStateSigs;
	extern Signature<2> isInGameOffsetSigs;
	extern Signature<1> maxPlayerOffsetSigs;
	extern Signature<1> localPlayerSigs;
	extern Signature<1> localPlayerOffsetSigs;
	extern Signature<2> cRenderSigs;
	extern Signature<2> viewAngleSigs;
	extern Signature<1> radarSigs;
	extern Signature<2> forceJumpSigs;
	extern Signature<2> inputSystemSigs;
	extern Signature<2> inputSystemOffsetSigs;
	extern Signature<1> dataTable;
	extern Signature<1> dormantSigs;
	extern Signature<2> boneMatrixSigs;
	extern Signature<2> modelHeaderSigs;
	extern Signature<1> viewAngleOffsetSigs;
	extern Signature<1> commandContextSigs;
	extern Signature<1> gameResourcesSigs;
	extern Signature<1> scaleformUISigs;
	extern Signature<1> chatOpenOffsetSigs;

	auto initializeSignatures() -> void;
	auto checkAllSignatures() -> bool;
}