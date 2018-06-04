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

	class AimbotFeature : public Feature
	{
	protected:
		virtual auto chooseBone(const uint32_t bestTarget) const -> HitboxID = 0;

		virtual auto getAimStrength() const -> float = 0;
		virtual auto getFov() const -> float = 0;
		virtual auto rcsEnabled() const -> bool = 0;
		virtual auto friendlyFireEnabled() const -> bool = 0;
		
	public:
		AimbotFeature(FeatureSet* fs) : Feature(fs) {}

		auto execFeature() const -> void;
	};

	class LegitBot : public AimbotFeature
	{
	private:
		static string featureName;
		auto chooseBone(const uint32_t bestTarget) const -> HitboxID;

	public:
		LegitBot(FeatureSet* fs) : AimbotFeature(fs) {}

		auto getFeatureName() const -> string const& { return featureName; }
		auto getAimStrength() const -> float
		{
			return getContainerSet()->getSetting("Aim Strength")->f();
		}
		auto getFov() const -> float
		{
			return getContainerSet()->getSetting("Legit FOV")->f();
		}
		auto rcsEnabled() const -> bool
		{
			return static_cast<bool>(getContainerSet()->getSetting("Legit RCS")->i());
		}
		auto friendlyFireEnabled() const -> bool
		{
			return static_cast<bool>(getContainerSet()->getSetting("Legit Friendly Fire")->i());
		}
	};

	class RageBot : public AimbotFeature
	{
	private:
		static string featureName;
		static constexpr float aimStrength = 0.99999f;
		auto chooseBone(const uint32_t bestTarget) const -> HitboxID;

	public:
		RageBot(FeatureSet* fs) : AimbotFeature(fs) {}

		auto getFeatureName() const -> string const& { return featureName; }
		auto getAimStrength() const -> float
		{
			return aimStrength;
		}
		auto getFov() const -> float
		{
			return getContainerSet()->getSetting("Rage FOV")->f();
		}
		auto rcsEnabled() const -> bool
		{
			return static_cast<bool>(getContainerSet()->getSetting("Rage RCS")->i());
		}
		auto friendlyFireEnabled() const -> bool
		{
			return static_cast<bool>(getContainerSet()->getSetting("Rage Friendly Fire")->i());
		}
	};

	class AimbotFeatureSet : public FeatureSet
	{
	private:
		static string setName;
	public:
		AimbotFeatureSet()
		{
			setEnabled(true);

			const auto aimstrLookup = [](int32_t lookupVal) -> Setting::setting_value
			{
				constexpr std::array<float, 5> aimStrengths = { 0, 0.01f, 0.015f, 0.02f, 0.025f };
				Setting::setting_value sv;
				sv.fSetting = aimStrengths[lookupVal];
				return sv;
			};

			const auto fovLookup = [](int32_t lookupVal) -> Setting::setting_value
			{
				constexpr std::array<float, 10> aimFovs = { 0, 0.0033f, 0.0077f, 0.011f, 0.022f, 0.055f, 0.11f, 0.33f, 0.55f, 0.99f };
				Setting::setting_value sv;
				sv.fSetting = aimFovs[lookupVal];
				return sv;
			};

			featureSettings["Legit Aim Key"] = Setting(0);
			featureSettings["Legit Friendly Fire"] = Setting(0);
			featureSettings["Aim Strength"] = Setting(0, aimstrLookup);
			featureSettings["Legit FOV"] = Setting(0, fovLookup);
			featureSettings["Legit RCS"] = Setting(0);

			featureSettings["Rage Aim Key"] = Setting(0);
			featureSettings["Rage Friendly Fire"] = Setting(0);
			featureSettings["Rage FOV"] = Setting(0, fovLookup);
			featureSettings["Rage RCS"] = Setting(0);
			featureSettings["Bodyshots"] = Setting(0);


			features["Ragebot"] = new RageBot(this);
			features["Legitbot"] = new LegitBot(this);
		}

		auto getFeatureSetName() const -> string const& { return setName; }

		auto execAllFeatures() const -> void;
	};
}
