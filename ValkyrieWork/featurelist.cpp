#include "feature.h"

namespace valkyrie
{
	auto FeatureList::initFeatures() -> void
	{
		//initialize stuff here
	}

	auto FeatureList::execAllFeatures() const -> void
	{
		for (auto const& featureSet : sets)
		{
			if (featureSet.enabled())
			{
				featureSet.execAllFeatures();
			}
		}
	}

	auto FeatureList::readFeatureConfig(FeatureConfig_t const& config) -> void
	{
		for (auto const& kv : config)
		{
			Feature* f = getFeatureByName<Feature>(kv.first);
			if (f != nullptr)
			{
				f->changeSetting(kv.second);
			}

		}
	}

	template<typename T>
	auto FeatureList::getFeatureByName(string const& name) -> T*
	{
		return const_cast<T*>(const_cast<const FeatureList*>(this)->getFeatureByName<T>(name));
	}

	template<typename T>
	auto FeatureList::getFeatureByName(string const& name) const -> T const*
	{
		for (auto const& featureSet : sets)
		{
			T* f = featureSet.getFeatureByName<T>(name);
			if (f != nullptr)
			{
				return f;
			}
		}
		return reinterpret_cast<T const*>(nullptr);
	}

	template<typename T>
	auto FeatureList::getFeatureSetByName(string const& name) const -> T const*
	{
		for (auto const& featureSet : sets)
		{
			if (featureSet.getFeatureSetName() == name)
			{
				return dynamic_cast<T const*>(&featureSet);
			}
		}

		return static_cast<T const*>(nullptr);
	}

	template<typename T>
	auto FeatureList::getFeatureSetByName(string const& name) -> T*
	{
		return const_cast<T*>(const_cast<const FeatureList*>(this)->getFeatureSetByName<T>(name));
	}

	//~~~~~~~~~~~~~~~~~~~~~~

	auto FeatureSet::execAllFeatures() const -> void
	{
		for (auto const& f : features)
		{
			f.second->execFeature();
		}
	}

	template<typename T>
	auto FeatureSet::getFeatureByName(string const& name) const -> T const*
	{
		auto const& it = features.find(name);

		if (it == features.end())
		{
			return static_cast<T const*>(nullptr);
		}
		return dynamic_cast<T const*>(it->second);
	}

	template<typename T>
	auto FeatureSet::getFeatureByName(string const& name) -> T*
	{
		return const_cast<T*>(const_cast<const FeatureSet*>(this)->getFeatureByName<T>(name));
	}

	auto FeatureSet::getSettingByName(string const& name) const -> Setting const*
	{
		auto const& it = featureSettings.find(name);

		if (it == featureSettings.end())
		{
			return static_cast<Setting const*>(nullptr);
		}
		return &it->second;
	}

	auto FeatureSet::getSettingByName(string const& name) -> Setting*
	{
		return const_cast<Setting*>(static_cast<const FeatureSet*>(this)->getSettingByName(name));
	}
}