#pragma once

#include "res.h"

namespace valkyrie
{
	class FeatureSet;
	typedef map<string, uint8_t> FeatureConfig_t;

	//use fSetting for FOV geno
	struct Setting
	{
		float fSetting;
		uint32_t iSetting;
	};

	class Feature
	{
	private:
		uint32_t _setting;
		FeatureSet* container;
	public:
		Feature(FeatureSet* container)
		{
			this->container = container;
		}

		//features will not "modify themselves" to an outsider perspective
		//hence constness
		virtual auto execFeature() const -> void = 0;
		virtual auto getFeatureName() const -> string const& = 0;

		constexpr auto setting() const -> uint32_t
		{
			return _setting;
		}

		constexpr auto changeSetting(const uint32_t setting) -> void
		{
			_setting = setting;
		}

		constexpr auto getContainerSet() const -> FeatureSet const*
		{
			return container;
		}

		//save settings?
		virtual ~Feature() {};
	};

	class FeatureSet
	{
	private:
		bool _enabled;
	protected:
		map<string, Feature*> features;
		map<string, Setting> featureSettings;
	public:

		constexpr auto enabled() const -> bool
		{
			return _enabled;
		}

		constexpr auto setEnabled(bool enabled) -> void
		{
			_enabled = enabled;
		}

		virtual auto execAllFeatures() const -> void;
		virtual auto getFeatureSetName() const -> string const& = 0;

		template<typename T>
		auto getFeatureByName(string const& name) -> T*;
		template<typename T>
		auto getFeatureByName(string const& name) const -> T const*;

		auto getSettingByName(string const& name) -> Setting*;
		auto getSettingByName(string const& name) const -> Setting const*;
	};

	class FeatureList
	{
	private:
		vector<FeatureSet> sets;
	public:
		auto initFeatures() -> void;
		auto execAllFeatures() const -> void;

		auto readFeatureConfig(FeatureConfig_t const& config) -> void;

		template<typename T>
		auto getFeatureByName(string const& name) -> T*;

		template<typename T>
		auto getFeatureByName(string const& name) const -> T const*;

		template<typename T>
		auto getFeatureSetByName(string const& name) const-> T const*;

		template<typename T>
		auto getFeatureSetByName(string const& name) -> T*;
	};

	extern FeatureList featureList;
}