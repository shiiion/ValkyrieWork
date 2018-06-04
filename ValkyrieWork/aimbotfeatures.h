#pragma once

#include "res.h"
#include "feature.h"

namespace valkyrie
{
	enum HitboxID
	{
		head = 0,
		neck = 1,
		rightShoulder = 17,
		leftShoulder = 15,
		upperTorso = 6,
		midTorso = 5,
		lowTorso = 4,
		pelvis = 2,
		rightHip = 8,
		leftHip = 7,
	};

	class LegitBot : public Feature
	{
	private:
		float aimStrength;
		float aimFov;
		float aimRandomizeAmt;
		bool rcsEnabled = false;
		bool friendlyFire = false;

		auto chooseBone(const uint32_t bestTarget) const -> HitboxID;

	public:
		auto execFeature() const -> void;
		auto getFeatureName() const -> string const& { return "Legitbot"; }

		constexpr auto setAimStrength(uint32_t menuIndex) -> void;
		constexpr auto setAimFov(uint32_t menuIndex) -> void;
		constexpr auto setRcsMode(uint32_t menuIndex) -> void { rcsEnabled = menuIndex; }
		constexpr auto setFFMode(uint32_t menuIndex) -> void { friendlyFire = menuIndex; }
		//constexpr auto setAimRandomization(uint32_t menuIndex) -> void; unused for now

	};

	class RageBot : public Feature
	{
	private:
		const float aimStrength = 0.99999f;
		float aimFov;
		float aimRandomizeAmt;
		bool rcsEnabled = false;
		bool friendlyFire = false;
		bool bodyShots = false;

		auto chooseBone(bool bodyShots) const->HitboxID;

	public:
		auto execFeature() const -> void;
		auto getFeatureName() const -> string const& { return "Ragebot"; }

		constexpr auto setAimFov(uint32_t menuIndex) -> void;
		constexpr auto setRcsMode(uint32_t menuIndex) -> void { rcsEnabled = menuIndex; }
		constexpr auto setFFMode(uint32_t menuIndex) -> void { friendlyFire = menuIndex; }
		constexpr auto setBodyShotMode(uint32_t menuIndex) -> void { bodyShots = menuIndex; }
		//constexpr auto setAimRandomization(uint32_t menuIndex) -> void; unused for now
	};
}
