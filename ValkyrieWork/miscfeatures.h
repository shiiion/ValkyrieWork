#pragma once

#include "res.h"
#include "feature.h"
#include <list>

namespace valkyrie
{
	//assume this is called very often
	class SpamChatFeature : public Feature
	{
	private:
		static string featureName;
		//last tick we sent a message for timing
		mutable uint32_t lastTickMessage;
		//last player we displayed
		mutable uint32_t lastPlayerIndex = 0u;

		static constexpr uint32_t spamTimerTicks = 1100u;
	public:
		SpamChatFeature(FeatureSet* fs) : Feature(fs) {}

		auto execFeature() const -> void;
		auto getFeatureName() const -> string const& { return featureName; }

		~SpamChatFeature() {}
	};

	class HitMarkerFeature : public Feature
	{
	private:
		static string featureName;

		struct Hit
		{
			vec3 loc;
			float creationTime;
		};

		mutable std::list<Hit> hitList;
		mutable uint32_t hitCounter;

		static constexpr float hitFadeTime = 0.3f;
		static constexpr uint32_t maxHitsPerExec = 64u;

	public:
		HitMarkerFeature(FeatureSet* fs) : Feature(fs) {}

		auto execFeature() const -> void;
		auto getFeatureName() const -> string const& { return featureName; }

		~HitMarkerFeature() {}
	};

	class BunnyhopFeature : public Feature
	{
	private:
		static string featureName;
	public:
		BunnyhopFeature(FeatureSet* fs) : Feature(fs) {}
		auto execFeature() const -> void;
		auto getFeatureName() const -> string const& { return featureName; }

		~BunnyhopFeature() {}
	};

	class MiscFeatureSet : public FeatureSet
	{
	private:
		static string setName;
	public:
		MiscFeatureSet()
		{
			features["Chat Spam"] = new SpamChatFeature(this);
			features["Hitmarkers"] = new HitMarkerFeature(this);
			features["Bunnyhop"] = new BunnyhopFeature(this);
			//default enabled, should not be disabled
			setEnabled(true);
		}

		auto getFeatureSetName() const -> string const& { return setName; }
	};
}