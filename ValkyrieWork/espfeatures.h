#pragma once

#include "res.h"
#include "feature.h"
#include <list>

namespace valkyrie
{
	struct CSPlayer;

	struct EspFeature : Feature
	{
		EspFeature(FeatureSet* fs) : Feature(fs) {}
		mutable CSPlayer const* player;
		mutable uint8_t index;
	};

	class BoxEsp : public EspFeature
	{
	private:
		static constexpr uint8_t setting2D = 1ui8;
		static constexpr uint8_t setting3D = 2ui8;
	public:
		BoxEsp(FeatureSet* fs) : EspFeature(fs) {}
		auto execFeature() const -> void;
		auto getFeatureName() const -> string const& { return "Player Box ESP"; }
	};

	class BoneEsp : public EspFeature
	{
	private:
		static constexpr uint8_t settingEnabled = 1ui8;
	public:
		BoneEsp(FeatureSet* fs) : EspFeature(fs) {}
		auto execFeature() const -> void;
		auto getFeatureName() const -> string const& { return "Player Bone ESP"; }
	};

	class HealthEsp : public EspFeature
	{
	private:
		static constexpr uint8_t settingEnabled = 1ui8;
	public:
		HealthEsp(FeatureSet* fs) : EspFeature(fs) {}
		auto execFeature() const -> void;
		auto getFeatureName() const -> string const& { return "Player Health ESP"; }
	};

	class NameEsp : public EspFeature
	{
	private:
		static constexpr uint8_t settingEnabled = 1ui8;
	public:
		NameEsp(FeatureSet* fs) : EspFeature(fs) {}
		auto execFeature() const -> void;
		auto getFeatureName() const -> string const& { return "Player Name ESP"; }
	};

	class DistanceEsp : public EspFeature
	{
	private:
		static constexpr uint8_t settingEnabled = 1ui8;
		//source units to meters (approx)
		static constexpr float unitsToMeters = (19.05f / 1000.f);
	public:
		DistanceEsp(FeatureSet* fs) : EspFeature(fs) {}
		auto execFeature() const -> void;
		auto getFeatureName() const -> string const& { return "Player Distance ESP"; }
	};

	class WeaponEsp : public EspFeature
	{
	private:
		static constexpr uint8_t settingEnabled = 1ui8;
	public:
		WeaponEsp(FeatureSet* fs) : EspFeature(fs) {}
		auto execFeature() const -> void;
		auto getFeatureName() const -> string const& { return "Player Weapon ESP"; }
	};

	class GlowEsp : public EspFeature
	{
	private:
		static constexpr float newSensorTime = 50000000.f;
		static constexpr uint8_t settingEnabled = 1ui8;
	public:
		GlowEsp(FeatureSet* fs) : EspFeature(fs) {}
		auto execFeature() const -> void;
		auto getFeatureName() const -> string const& { return "Player Glow ESP"; }
	};



	class EspFeatureSet : public FeatureSet
	{
	public:
		EspFeatureSet()
		{
			//dummy value... remove if needed
			featureSettings["Color Scheme"] = { 0, 0 };
			featureSettings["Draw Team"] = { 0, 0 };

			features["Player Box ESP"] = new BoxEsp(this);
			features["Player Bone ESP"] = new BoneEsp(this);
			features["Player Health ESP"] = new HealthEsp(this);
			features["Player Name ESP"] = new NameEsp(this);
			features["Player Distance ESP"] = new DistanceEsp(this);
			features["Player Weapon ESP"] = new WeaponEsp(this);
			features["Player Glow ESP"] = new GlowEsp(this);
		}

		auto getFeatureSetName() const -> string const& { return "ESP"; }

		auto execAllFeatures() const -> void;
	};
}