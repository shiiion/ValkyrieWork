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
		mutable uint8_t payloadIndex;
	};

	class BoxEsp : public EspFeature
	{
	private:
		static string featureName;
		static constexpr uint8_t setting2D = 1ui8;
		static constexpr uint8_t setting3D = 2ui8;
	public:
		BoxEsp(FeatureSet* fs) : EspFeature(fs) {}
		auto execFeature() const -> void;
		auto getFeatureName() const -> string const& { return featureName; }
	};

	class BoneEsp : public EspFeature
	{
	private:
		static string featureName;
		static constexpr uint8_t settingEnabled = 1ui8;
	public:
		BoneEsp(FeatureSet* fs) : EspFeature(fs) {}
		auto execFeature() const -> void;
		auto getFeatureName() const -> string const& { return featureName; }
	};

	class HealthEsp : public EspFeature
	{
	private:
		static string featureName;
		static constexpr uint8_t settingEnabled = 1ui8;
	public:
		HealthEsp(FeatureSet* fs) : EspFeature(fs) {}
		auto execFeature() const -> void;
		auto getFeatureName() const -> string const& { return featureName; }
	};

	class NameEsp : public EspFeature
	{
	private:
		static string featureName;
		static constexpr uint8_t settingEnabled = 1ui8;
	public:
		NameEsp(FeatureSet* fs) : EspFeature(fs) {}
		auto execFeature() const -> void;
		auto getFeatureName() const -> string const& { return featureName; }
	};

	class DistanceEsp : public EspFeature
	{
	private:
		static string featureName;
		static constexpr uint8_t settingEnabled = 1ui8;
		//source units to meters (approx)
		static constexpr float unitsToMeters = (19.05f / 1000.f);
	public:
		DistanceEsp(FeatureSet* fs) : EspFeature(fs) {}
		auto execFeature() const -> void;
		auto getFeatureName() const -> string const& { return featureName; }
	};

	class WeaponEsp : public EspFeature
	{
	private:
		static string featureName;
		static constexpr uint8_t settingEnabled = 1ui8;
	public:
		WeaponEsp(FeatureSet* fs) : EspFeature(fs) {}
		auto execFeature() const -> void;
		auto getFeatureName() const -> string const& { return featureName; }
	};



	class EspFeatureSet : public FeatureSet
	{
	private:
		static string setName;
	public:
		EspFeatureSet()
		{
			//dummy value... remove if needed
			featureSettings["Color Scheme"] = Setting(0);
			featureSettings["Team ESP"] = Setting(0);

			features["Box ESP"] = new BoxEsp(this);
			features["Bone ESP"] = new BoneEsp(this);
			features["Health ESP"] = new HealthEsp(this);
			features["Name ESP"] = new NameEsp(this);
			features["Distance ESP"] = new DistanceEsp(this);
			features["Weapon ESP"] = new WeaponEsp(this);
		}

		auto getFeatureSetName() const -> string const& { return setName; }

		auto execAllFeatures() const -> void;
	};
}