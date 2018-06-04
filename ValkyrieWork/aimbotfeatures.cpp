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
	string RageBot::featureName = "Ragebot";
	string LegitBot::featureName = "Legitbot";
	string AimbotFeatureSet::setName = "Aimbot";

	static auto checkKeyByMenuIndex(int32_t menuIdx) -> bool
	{
		switch (menuIdx)
		{
		case 0:
			return checkKeyState(0x0D, 3);
		case 1:
			return checkKeyState(0x0D, 4);
		case 2:
			return checkKeyState(0x0D, 5);
		case 3:
			return checkKeyState(0x0D, 6);
		case 4:
			return checkKeyState(0x0D, 7);
		default:
			return false;
		}
	}


	//shared helper functions for both aimbots
	static auto chooseEnemy(float& distanceToCenter, const bool friendlyFire) -> int32_t
	{
		CSPlayer const& localPlayer = playerList.getLocalPlayer();
		int32_t bestIndex = -1;
		const vec2 screenCenter = vec2(globals.screenWidth / 2.f, globals.screenHeight / 2.f);
		float minDist = std::numeric_limits<float>::max();
		float dist = 0;
		vec2 playerScreenPosition;

		for (auto i = 0u; i < playerList.size(); i++)
		{
			const CSPlayer& currentPlayer = playerList[i];
			//if failed to read adequate number of hitboxes
			//if trying to read your own shit, obviously not gonna aim at urself LOL!!! :D XD shut up geno
			//skip teammates that cannot be aimed at if friendly fire is off
			if (!currentPlayer.hitboxReadSuccess ||
				!currentPlayer.validPlayer() ||
				currentPlayer.base == localPlayer.base ||
				(!friendlyFire && (currentPlayer.team == localPlayer.team)))
			{
				continue;
			}
			//what the fuck is this?
			//checks if w2s fails, sets playerscreenposition
			//bone 5 is middle torso. this may be changed in the future.
			//uh dont do this? w2s returns false if off-screen...
			//i'll leave it here FN
			if (!worldToScreen(currentPlayer.bones[5], playerScreenPosition, getViewMatrix()))
			{
				//did not succeed
				continue;
			}

			const float distToCenter = playerScreenPosition.distance(screenCenter);
			//update minDist if a closer player is found
			if (distToCenter < minDist)
			{
				minDist = distToCenter;
				bestIndex = static_cast<int32_t>(i);
			}
			dist = minDist;
		}
		//set reference var
		distanceToCenter = dist;
		return bestIndex;
	}

	//i dont want to touch this...
	static auto aimAtEnemy(const vec3& enemyPos, const vec3& localPos, const float enemyDistance, 
		const bool rcsEnabled, const float FOV, const float aimStrength) -> void
	{
		const vec2 screenCenter = vec2(globals.screenWidth / 2.f, globals.screenHeight / 2.f);
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
		//what??
		//can we not
		constexpr float randPercent = 1.f / 8.f;

		const float lowerAimpunchX = -abs(playerList.getLocalPlayer().aimPunch.x * randPercent);
		const float upperAimpunchX = abs(playerList.getLocalPlayer().aimPunch.x * randPercent);
		const float lowerAimpunchY = -abs(playerList.getLocalPlayer().aimPunch.y * randPercent);
		const float upperAimpunchY = abs(playerList.getLocalPlayer().aimPunch.y * randPercent);

		//const vec3 randomRecoil = vec3(random<float>(lowerAimpunchX, upperAimpunchX), random<float>(lowerAimpunchY, upperAimpunchY), 0);

		//compensate for random rcs
		viewPunchCalc = viewPunchCalc + (((playerList.getLocalPlayer().aimPunch * 2.f)/* + randomRecoil*/) + playerList.getLocalPlayer().viewPunch);

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

	auto AimbotFeature::execFeature() const -> void
	{
		const CSPlayer& localPlayer = playerList.getLocalPlayer();

		if (localPlayer.isDead) return;

		float bestEnemyDistance;
		const int32_t targetIndex = chooseEnemy(bestEnemyDistance, friendlyFireEnabled());

		//double check the target index values aren't bs
		if (targetIndex == -1 || targetIndex >= static_cast<int32_t>(playerList.size()))
		{
			return;
		}

		const HitboxID boneID = chooseBone(targetIndex);

		const CSPlayer& targetPlayer = playerList[targetIndex];
		const vec3 targetBone = targetPlayer.bones[boneID];

		aimAtEnemy(targetBone, localPlayer.pos + localPlayer.viewOffset, 
			bestEnemyDistance, rcsEnabled(), getFov(), getAimStrength());
	}

	//legitbot implementation ****************************************************************************|
	auto LegitBot::chooseBone(const uint32_t bestTarget) const -> HitboxID
	{
		HitboxID bestBone = midTorso;
		const vec2 screenCenter = vec2(globals.screenWidth / 2.f, globals.screenHeight / 2.f);
		float minDist = std::numeric_limits<float>::max();
		if (bestTarget >= 0)
		{
			//performance bump from using vectors
			const std::array<HitboxID, 10>  hitboxes = { head,neck,rightShoulder,leftShoulder,upperTorso,
				midTorso,lowTorso,pelvis,rightHip,leftHip };

			for (auto const aimpoint : hitboxes)
			{
				vec2 boneScreenPos;
				//checks if w2s fails on bone or that this player is still a valid target
				//sets playerscreenposition to current bone if succeed
				if (!playerList[bestTarget].validPlayer() || !worldToScreen(playerList[bestTarget].bones[aimpoint], boneScreenPos, getViewMatrix()))
				{
					//failed
					continue;
				}
				const float distToCenter = boneScreenPos.distance(screenCenter);
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

	//LOL
	auto RageBot::chooseBone(const uint32_t bestTarget) const -> HitboxID
	{
		return getContainerSet()->getSetting("Rage Bodyshots")->i() ? midTorso : head;
	}

	//ragebot implementation ended ***********************************************************************|
	
	auto AimbotFeatureSet::execAllFeatures() const -> void
	{
		Feature const* f = nullptr;
		//this will "technically" select legit over rage if both enabled
		//ensure it is not...
		if ((f = features.at("Legitbot"))->setting() == 1ui32)
		{
			if (checkKeyByMenuIndex(getSetting("Legit Aim Key")->i()))
			{
				f->execFeature();
			}
		}
		else if ((f = features.at("Ragebot"))->setting() == 1ui32)
		{
			if (checkKeyByMenuIndex(getSetting("Rage Aim Key")->i()))
			{
				f->execFeature();
			}
		}
	}
}
