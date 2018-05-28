#pragma once

#include "res.h"

namespace valkyrie
{

	typedef map<string, uint8_t> FeatureConfig_t;

	class Feature
	{
	private:
		uint8_t _setting;
	public:
		//features will not "modify themselves" to an outsider perspective
		//hence constness
		virtual auto execFeature() const -> void = 0;
		virtual auto getFeatureName() const -> string const& = 0;

		constexpr auto setting() const -> uint8_t
		{
			return _setting;
		}

		constexpr auto changeSetting(const uint8_t setting) -> void
		{
			_setting = setting;
		}

		//save settings?
		virtual ~Feature() {};
	};

	class FeatureList
	{
		using FeaturePointer = unique_ptr<Feature>;
	private:
		map<string, FeaturePointer> features;

	public:
		auto initFeatures() -> void;
		auto execAllFeatures() const -> void;

		auto readFeatureConfig(FeatureConfig_t const& config) -> void;

		template<typename T>
		auto getFeatureByName(string const& name) -> T&;

		template<typename T>
		auto getFeatureByName(string const& name) const -> T const&;
	};

	extern FeatureList featureList;
}