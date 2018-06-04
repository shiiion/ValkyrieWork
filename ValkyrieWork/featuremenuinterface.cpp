#include "featuremenuinterface.h"

#include "res.h"

#include "feature.h"

#include "Menu.h"

namespace valkyrie
{
	auto writeMenuToFeatureList() -> void
	{
		using menu::Menu;

		if (Menu.recentChange)
		{
			for (auto i = 0; i < Menu.ItemMax; i++)
			{
				string const& featureStr = Menu.Item[i].ImplicitName;
				int32_t menuVal = static_cast<int32_t>(*Menu.Item[i].Variable);

				FeatureSet* set;
				Setting* setting;
				Feature* feature;
				if ((set = featureList.getFeatureSet(featureStr)) != nullptr)
				{
					set->setEnabled(menuVal != 0);
				}
				else if ((setting = featureList.getSetting(featureStr)) != nullptr)
				{
					setting->set(menuVal, Setting::setting_value(menuVal));
				}
				else if ((feature = featureList.getFeature(featureStr)) != nullptr)
				{
					feature->changeSetting(menuVal);
				}
			}
			Menu.recentChange = false;
		}
	}
}