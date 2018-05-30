#include "aimbotfeatures.h"
#include "valkAPI.h"
#include "csgoutils.h"
#include "playerlist.h"
#include "memory.h"
#include "globals.h"
#include <limits>
#undef max
#undef min

namespace valkyrie
{
	auto LegitBot::execFeature() const -> void
	{
		CSPlayer& localPlayer = playerList.getLocalPlayer();

		float bestEnemyDistance;
		uint32_t targetIndex = chooseEnemy(bestEnemyDistance);

	}

	//shared helper functions for both aimbots
	uint32_t chooseEnemy(float& distanceToCenter, bool friendlyFire)
	{
		CSPlayer& localPlayer = playerList.getLocalPlayer();
		uint32_t bestIndex;
		vec2 screenCenter = vec2(globals.screenWidth / 2.f, globals.screenHeight / 2.f);
		float minDist = std::numeric_limits<float>::max();
		float dist = 0;
		for (auto i = 0; i < playerList.size(); i++)
		{
			vec2 playerScreenPosition;
			//if failed to read adequate number of hitboxes
			if (!playerList[i].hitboxReadSuccess)
			{
				continue;
			}
			//if trying to read your own shit, obviously not gonna aim at urself LOL!!! :D XD
			if (playerList[i].base == localPlayer.base)
			{
				continue;
			}
			//skip teammates that cannot be aimed at if friendly fire is on
			if (!friendlyFire && !playerList[i].validTarget)
			{
				continue;
			}
			//what the fuck is this?
			//checks if w2s fails, sets playerscreenposition, lol what a shit
			//bone 5 is middle torso. this may be changed in the future.
			if (!playerList[i].validPlayer() || !worldToScreen(playerList[i].bones[5], playerScreenPosition, getViewMatrix()))
			{
				//did not succeed. if it did succeed, this block will skip and code will continue to execute using playerScreenPosition
				continue;
			}

			float distToCenter = playerScreenPosition.distance(screenCenter);
			//update minDist if a closer player is found
			if (distToCenter < minDist)
			{
				minDist = distToCenter;
				bestIndex = i;
			}
			dist = minDist;
		}
		distanceToCenter = dist;
		return bestIndex;
	}
}
