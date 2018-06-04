#pragma once

#include "res.h"
#include <functional>

namespace valkyrie
{
	class FeatureSet;

	//use fSetting for FOV geno
	struct Setting
	{
		union setting_value
		{
			setting_value(float fs) : fSetting(fs) {};
			setting_value(int32_t is) : iSetting(is) {};
			setting_value() : iSetting(0) {};

			float fSetting;
			int32_t iSetting;
		}setting;

		using SettingLookup = function<setting_value(int32_t)>;
		SettingLookup lookupFn = nullptr;

		Setting(const int32_t sv = 0, SettingLookup lookup = nullptr)
		{
			setting.iSetting = sv;
			lookupFn = lookup;
		}

		Setting(const float sv, SettingLookup lookup = nullptr)
		{
			setting.fSetting = sv;
			lookupFn = lookup;
		}

		constexpr auto i() const -> int32_t
		{
			return setting.iSetting;
		}
		constexpr auto f() const -> float
		{
			return setting.fSetting;
		}
		
		auto set(int32_t menuLookup, setting_value onFail) -> void
		{
			if (lookupFn != nullptr)
			{
				setting = lookupFn(menuLookup);
			}
			else
			{
				setting = onFail;
			}
		}

	};

	typedef map<string, Setting> FeatureConfig_t;

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

		auto getFeature(string const& name) -> Feature*;
		auto getFeature(string const& name) const -> Feature const*;

		auto getSetting(string const& name) -> Setting*;
		auto getSetting(string const& name) const -> Setting const*;

		~FeatureSet()
		{
			for (auto& kv : features)
			{
				if (kv.second)
				{
					delete kv.second;
				}
			}
		}
	};

	class FeatureList
	{
	private:
		vector<FeatureSet*> sets;
	public:
		auto initFeatures() -> void;
		auto execAllFeatures() const -> void;

		auto getFeature(string const& name) -> Feature*;
		auto getFeature(string const& name) const -> Feature const*;

		auto getSetting(string const& name) const -> Setting const*;
		auto getSetting(string const& name) -> Setting*;


		auto getFeatureSet(string const& name) const-> FeatureSet const*;
		auto getFeatureSet(string const& name) -> FeatureSet*;

		~FeatureList()
		{
			for (FeatureSet* f : sets)
			{
				if (f)
				{
					delete f;
				}
			}
		}
	};

	extern FeatureList featureList;
}