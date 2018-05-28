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
		//last tick we sent a message for timing
		mutable uint32_t lastTickMessage;
		//last player we displayed
		mutable uint32_t lastPlayerIndex = 0u;

		static constexpr uint32_t spamTimerTicks = 1100u;
	public:
		auto execFeature() const -> void;
		auto getFeatureName() const -> string const& { return "Chat Spam"; }

		~SpamChatFeature() {}
	};

	class HitMarkerFeature : public Feature
	{
	private:

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
		auto execFeature() const -> void;
		auto getFeatureName() const -> string const& { return "Hitmarkers"; }

		~HitMarkerFeature() {}
	};

	class BunnyhopFeature : public Feature
	{
	private:
	public:
		auto execFeature() const -> void;
		auto getFeatureName() const -> string const& { return "Bunnyhop"; }

		~BunnyhopFeature() {}
	};
}