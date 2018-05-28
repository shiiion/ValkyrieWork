#include "feature.h"

namespace valkyrie
{
	auto FeatureList::initFeatures() -> void
	{
		//initialize stuff here
	}

	auto FeatureList::execAllFeatures() const -> void
	{
		for (auto const& feature : features)
		{
			if (feature.second.get() && feature.second->setting())
			{
				feature.second->execFeature();
			}
		}
	}

	auto FeatureList::readFeatureConfig(FeatureConfig_t const& config) -> void
	{
		for (auto const& kv : config)
		{
			auto const& it = features.find(kv.first);
			if (it != features.end())
			{
				it->second->changeSetting(kv.second);
			}
		}
	}

	template<typename T>
	auto FeatureList::getFeatureByName(string const& name) -> T&
	{
		return dynamic_cast<T>(features[name]);
	}

	template<typename T>
	auto FeatureList::getFeatureByName(string const& name) const -> T const&
	{
		return dynamic_cast<T>(features.at(name));
	}
}