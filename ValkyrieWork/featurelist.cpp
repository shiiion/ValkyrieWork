#include "feature.h"
#include "aimbotfeatures.h"
#include "espfeatures.h"
#include "miscfeatures.h"

namespace valkyrie
{
	FeatureList featureList;

	auto FeatureList::initFeatures() -> void
	{
		sets.emplace_back(new EspFeatureSet());
		sets.emplace_back(new AimbotFeatureSet());
		sets.emplace_back(new MiscFeatureSet());
	}

	auto FeatureList::execAllFeatures() const -> void
	{
		for (auto const& featureSet : sets)
		{
			if (featureSet->enabled())
			{
				featureSet->execAllFeatures();
			}
		}
	}

	auto FeatureList::getFeature(string const& name) -> Feature*
	{
		return const_cast<Feature*>(const_cast<const FeatureList*>(this)->getFeature(name));
	}

	auto FeatureList::getFeature(string const& name) const -> Feature const*
	{
		for (auto const& featureSet : sets)
		{
			Feature const* f = featureSet->getFeature(name);
			if (f != nullptr)
			{
				return f;
			}
		}
		return static_cast<Feature const*>(nullptr);
	}

	auto FeatureList::getSetting(string const& name) const -> Setting const*
	{
		for (auto const& featureSet : sets)
		{
			Setting const* s = featureSet->getSetting(name);
			if (s != nullptr)
			{
				return s;
			}
		}
		return nullptr;
	}

	auto FeatureList::getSetting(string const& name) -> Setting*
	{
		return const_cast<Setting*>(const_cast<const FeatureList*>(this)->getSetting(name));
	}

	auto FeatureList::getFeatureSet(string const& name) const -> FeatureSet const*
	{
		for (auto const& featureSet : sets)
		{
			if (featureSet->getFeatureSetName() == name)
			{
				return featureSet;
			}
		}

		return static_cast<FeatureSet const*>(nullptr);
	}

	auto FeatureList::getFeatureSet(string const& name) -> FeatureSet*
	{
		return const_cast<FeatureSet*>(const_cast<const FeatureList*>(this)->getFeatureSet(name));
	}

	//~~~~~~~~~~~~~~~~~~~~~~

	auto FeatureSet::execAllFeatures() const -> void
	{
		for (auto const& f : features)
		{
			f.second->execFeature();
		}
	}

	auto FeatureSet::getFeature(string const& name) const -> Feature const*
	{
		auto const& it = features.find(name);

		if (it == features.end())
		{
			return static_cast<Feature const*>(nullptr);
		}
		return it->second;
	}

	auto FeatureSet::getFeature(string const& name) -> Feature*
	{
		return const_cast<Feature*>(const_cast<const FeatureSet*>(this)->getFeature(name));
	}

	auto FeatureSet::getSetting(string const& name) const -> Setting const*
	{
		auto const& it = featureSettings.find(name);

		if (it == featureSettings.end())
		{
			return static_cast<Setting const*>(nullptr);
		}
		return &it->second;
	}

	auto FeatureSet::getSetting(string const& name) -> Setting*
	{
		return const_cast<Setting*>(static_cast<const FeatureSet*>(this)->getSetting(name));
	}
}