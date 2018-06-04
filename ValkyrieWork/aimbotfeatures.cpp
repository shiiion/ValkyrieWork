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
	//menu options for both aimbots
	static constexpr uint32_t aimStrengthOptions = 5;
	static constexpr float aimStrengths[5] = { 0, 0.01f, 0.015f, 0.02f, 0.025f };
	static constexpr uint32_t aimFovOptions = 10;
	static constexpr float aimFovs[10] = { 0, 0.0033f, 0.0077f, 0.011f, 0.022f, 0.055f, 0.11f, 0.33f, 0.55f, 0.99f };

	//shared helper functions for both aimbots
	static auto chooseEnemy(float& distanceToCenter, bool friendlyFire) -> uint32_t
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
			//checks if w2s fails, sets playerscreenposition
			//bone 5 is middle torso. this may be changed in the future.
			if (!currentPlayer.validPlayer() || !worldToScreen(currentPlayer.bones[5], playerScreenPosition, getViewMatrix()))
			{
				//did not succeed
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

	static auto aimAtEnemy(const vec3& enemyPos, const vec3& localPos, float enemyDistance, bool rcsEnabled, float FOV, float aimStrength) -> void
	{
		vec2 screenCenter = vec2(globals.screenWidth / 2.f, globals.screenHeight / 2.f);
		const float normalizedDist = enemyDistance / sqrt(screenCenter.dot(screenCenter));
		if (normalizedDist > FOV)
		{
			//enemy is not within field of view; do nothing else
			return;
		}

		vec3 viewAngles;
		csgoProc.read<vec3>(globals.viewAngles + globals.otherOffs.viewAngle, &viewAngles, 1u);

		vec3 viewPunchCalc;

		//randomize , percent subject to change
		const float randPercent = 1.f / 8.f;

		const float lowerAimpunchX = -abs(playerList.getLocalPlayer().aimPunch.x * randPercent);
		const float upperAimpunchX = abs(playerList.getLocalPlayer().aimPunch.x * randPercent);
		const float lowerAimpunchY = -abs(playerList.getLocalPlayer().aimPunch.y * randPercent);
		const float upperAimpunchY = abs(playerList.getLocalPlayer().aimPunch.y * randPercent);

		const vec3 randomRecoil = vec3(random<float>(lowerAimpunchX, upperAimpunchX), random<float>(lowerAimpunchY, upperAimpunchY), 0);

		//compensate for random rcs
		viewPunchCalc = viewPunchCalc + (((playerList.getLocalPlayer().aimPunch * 2.f) + randomRecoil) + playerList.getLocalPlayer().viewPunch);

		vec3 angles;
		vectorAngles(enemyPos - localPos, angles);
		normalizeAngles(angles);
		//Aimbot changes where it aims based on whether RCS enabled or not.
		vec3 deltaAngles = rcsEnabled ? vec3(angles.x - viewPunchCalc.x, angles.y - viewPunchCalc.y, 0)
			: vec3(angles.x - viewAngles.x, angles.y - viewAngles.y, 0);

		const vec3 originalDeltaAngles = deltaAngles;

		normalizeAngles(deltaAngles);
		clampAngles(deltaAngles);
		//make it a directional vector
		deltaAngles = deltaAngles / sqrt(deltaAngles.dot(deltaAngles));

		float dPitch = aimStrength * deltaAngles.x;
		float dYaw = aimStrength * deltaAngles.y;

		//reduce jitter
		dPitch = (abs(dPitch) < abs(originalDeltaAngles.x) ? dPitch : originalDeltaAngles.x);
		dYaw = (abs(dYaw) < abs(originalDeltaAngles.y) ? dYaw : originalDeltaAngles.y);

		viewAngles.x += dPitch;
		viewAngles.y += dYaw;

		normalizeAngles(viewAngles);
		clampAngles(viewAngles);

		csgoProc.write<vec3>(globals.viewAngles + globals.otherOffs.viewAngle, &viewAngles, 1u);
	}


	//legitbot implementation ****************************************************************************|
	constexpr auto LegitBot::setAimStrength(uint32_t menuIndex) -> void
	{
		if (menuIndex >= aimStrengthOptions)
		{
			aimStrength = aimStrengths[0];
		}
		else
		{
			aimStrength = aimStrengths[menuIndex];
		}
	}

	constexpr auto LegitBot::setAimFov(uint32_t menuIndex) -> void
	{
		if (menuIndex >= aimFovOptions)
		{
			aimFov = aimFovs[0];
		}
		else
		{
			aimFov = aimFovs[menuIndex];
		}
	}

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
		//localplayer needs to be alive to take shots haha
		if (!localPlayer.isDead)
		{
			aimAtEnemy(targetBone, localPlayer.pos + localPlayer.viewOffset, bestEnemyDistance, this->rcsEnabled, this->aimFov, this->aimStrength);
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
	//legitbot implementation ended **********************************************************************|


	
	//ragebot implementation *****************************************************************************|
	constexpr auto RageBot::setAimFov(uint32_t menuIndex) -> void
	{
		if (menuIndex >= aimFovOptions)
		{
			aimFov = aimFovs[0];
		}
		else
		{
			aimFov = aimFovs[menuIndex];
		}
	}

	auto RageBot::execFeature() const -> void
	{
		CSPlayer& localPlayer = playerList.getLocalPlayer();

		float bestEnemyDistance;
		const uint32_t targetIndex = chooseEnemy(bestEnemyDistance, this->friendlyFire);

		const HitboxID boneID = chooseBone(this->bodyShots);

		//double check the target index values aren't bs
		if (targetIndex < 0 || unsigned(targetIndex) >= playerList.size())
		{
			return;
		}
		const CSPlayer& targetPlayer = playerList[targetIndex];
		const vec3 targetBone = targetPlayer.bones[boneID];
		//localplayer needs to be alive to take shots haha
		if (!localPlayer.isDead)
		{
			aimAtEnemy(targetBone, localPlayer.pos + localPlayer.viewOffset, bestEnemyDistance, this->rcsEnabled, this->aimFov, this->aimStrength);
		}
	}
	auto RageBot::chooseBone(bool bodyShots) const -> HitboxID
	{
		return bodyShots ? midTorso : head;
	}

	//ragebot implementation ended ***********************************************************************|
	
	
}
