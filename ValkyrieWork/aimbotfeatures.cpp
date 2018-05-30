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
	//legitbot implementation
	auto LegitBot::execFeature() const -> void
	{
		CSPlayer& localPlayer = playerList.getLocalPlayer();
		
		float bestEnemyDistance;
		const uint32_t targetIndex = chooseEnemy(bestEnemyDistance, this->friendlyFire);

		const HitboxID boneID = chooseBone(targetIndex);

		//double check the target index values aren't bs
		if (targetIndex < 0 || unsigned(targetIndex) >= playerList.size())
		{
			return;
		}
		const CSPlayer& targetPlayer = playerList[targetIndex];
		const vec3 targetBone = targetPlayer.bones[boneID];

		if (!localPlayer.isDead)
		{
			
		}



	}

	auto LegitBot::chooseBone(const uint32_t bestTarget) const -> HitboxID
	{
		HitboxID bestBone = midTorso;
		const vec2 screenCenter = vec2(globals.screenWidth / 2.f, globals.screenHeight / 2.f);
		float minDist = std::numeric_limits<float>::max();
		if (bestTarget >= 0)
		{
			const vector<HitboxID> hitboxes = { head,neck,rightShoulder,leftShoulder,upperTorso,
				midTorso,lowTorso,pelvis,rightHip,leftHip };

			for (auto aimpoint : hitboxes)
			{
				vec2 playerScreenPosition;
				//checks if w2s fails on bone or that this player is still a valid target
				//sets playerscreenposition to current bone if succeed
				if (!playerList[bestTarget].validPlayer() || !worldToScreen(playerList[bestTarget].bones[aimpoint], playerScreenPosition, getViewMatrix()))
				{
					//failed
					continue;
				}
				const float distToCenter = playerScreenPosition.distance(screenCenter);
				if (distToCenter < minDist)
				{
					minDist = distToCenter;
					bestBone = aimpoint;
				}
			}
		}
		return bestBone;
	}

	
	//shared helper functions for both aimbots
	auto chooseEnemy(float& distanceToCenter, bool friendlyFire) -> uint32_t
	{
		CSPlayer& localPlayer = playerList.getLocalPlayer();
		uint32_t bestIndex;
		const vec2 screenCenter = vec2(globals.screenWidth / 2.f, globals.screenHeight / 2.f);
		float minDist = std::numeric_limits<float>::max();
		float dist = 0;
		for (auto i = 0u; i < playerList.size(); i++)
		{
			vec2 playerScreenPosition;
			const CSPlayer& currentPlayer = playerList[i];
			//if failed to read adequate number of hitboxes
			if (!currentPlayer.hitboxReadSuccess)
			{
				continue;
			}
			//if trying to read your own shit, obviously not gonna aim at urself LOL!!! :D XD
			if (currentPlayer.base == localPlayer.base)
			{
				continue;
			}
			//skip teammates that cannot be aimed at if friendly fire is off
			if (!friendlyFire && !currentPlayer.validTarget(localPlayer))
			{
				continue;
			}
			//what the fuck is this?
			//checks if w2s fails, sets playerscreenposition, lol what a shit
			//bone 5 is middle torso. this may be changed in the future.
			if (!currentPlayer.validPlayer() || !worldToScreen(currentPlayer.bones[5], playerScreenPosition, getViewMatrix()))
			{
				//did not succeed. if it did succeed, this block will skip and code will continue to execute using playerScreenPosition
				continue;
			}

			const float distToCenter = playerScreenPosition.distance(screenCenter);
			//update minDist if a closer player is found
			if (distToCenter < minDist)
			{
				minDist = distToCenter;
				bestIndex = i;
			}
			dist = minDist;
		}
		//set reference var
		distanceToCenter = dist;
		return bestIndex;
	}

	auto aimAtEnemy(const vec3& enemyPos, const vec3& localPos, float enemyDistance) -> void
	{
		//tbd
	}
}
