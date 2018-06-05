#include "espfeatures.h"
#include "playerlist.h"
#include "globals.h"
#include "RenderPayload.h"
#include "valkAPI.h"
#include "csgoutils.h"
#include "memory.h"

#undef max
#undef min

namespace valkyrie
{
	string BoxEsp::featureName = "Box ESP";
	string BoneEsp::featureName = "Bone ESP";
	string HealthEsp::featureName = "Health ESP";
	string NameEsp::featureName = "Name ESP";
	string DistanceEsp::featureName = "Distance ESP";
	string WeaponEsp::featureName = "Weapon ESP";
	string EspFeatureSet::setName = "ESP";

	static bool checkVisible(CSPlayer const& target, vec3 const& camera)
	{
		std::array<vec3, 8> positions;

	}

	static auto getVertices(vec3 const& maxes, vec3 const& mins, std::array<vec3, 8>& vOut)
	{
		const vec3 dims = maxes - mins;

		vOut[0] = mins;
		vOut[1] = vec3(mins.x + dims.x, mins.y, mins.z);
		vOut[2] = vec3(mins.x + dims.x, mins.y + dims.y, mins.z);
		vOut[3] = vec3(mins.x, mins.y + dims.y, mins.z);
		vOut[4] = vec3(mins.x, mins.y, mins.z + dims.z);
		vOut[5] = vec3(mins.x + dims.x, mins.y, mins.z + dims.z);
		vOut[6] = maxes;
		vOut[7] = vec3(mins.x, mins.y + dims.y, mins.z + dims.z);
	}

	static auto rotateZ(vec3& vec, const float angle)
	{
		vec2 temp = vec2(vec.x, vec.y).rotate(toRad(angle));
		vec.x = temp.x;
		vec.y = temp.y;
	}

	static auto getBoundingBox2D(vec3 const& mins, vec3 const& maxes,
		vec2& oMins, vec2& oMaxes, 
		matrix_t viewMat)
	{
		oMins.x = oMins.y = std::numeric_limits<float>::max();
		oMaxes.x = oMaxes.y = std::numeric_limits<float>::min();

		std::array<vec3, 8> vertices3;
		getVertices(maxes, mins, vertices3);

		std::array<vec2, 8> vertices2;
		for (auto a = 0; a < 8; a++)
		{
			worldToScreen(vertices3[a], vertices2[a], viewMat);
			oMaxes.x = std::max(oMaxes.x, vertices2[a].x);
			oMaxes.y = std::max(oMaxes.y, vertices2[a].y);
			oMins.x = std::min(oMins.x, vertices2[a].x);
			oMins.y = std::min(oMins.y, vertices2[a].y);
		}
	}

	static auto getBoundingBox3D(vec3 const& mins, vec3 const& maxes,
		vec3 const& rotationPoint, const float yaw,
		std::array<vec2, 8>& outVertices, matrix_t viewMat)
	{
		std::array<vec3, 8> vertices3;
		getVertices(maxes, mins, vertices3);

		for (uint32_t a = 0; a < 8; a++)
		{
			vertices3[a] = vertices3[a] - rotationPoint;
			rotateZ(vertices3[a], yaw);
			vertices3[a] = vertices3[a] + rotationPoint;
			worldToScreen(vertices3[a], outVertices[a], viewMat);
		}
	}

	static auto writeHeadHitbox(PlayerPayload& output, vec3 const& pos, matrix_t viewMatrix)
	{
		const vec3 headMax = pos + vec3(6, 6, 6);
		const vec3 headMin = pos + vec3(-6, -6, -6);
	}

	auto BoxEsp::execFeature() const -> void
	{
		ESPPayload& buffer = getPayloadBuffer();
		PlayerPayload& playerPayload = buffer.players[payloadIndex];

		vec3 absMin = player->pos + player->bboxMin;
		vec3 absMax = player->pos + player->bboxMax;

		buffer.boundingBoxMode = setting();

		getBoundingBox2D(absMin, absMax, playerPayload.bbox[0],
			playerPayload.bbox[1], getViewMatrix());

		if (setting() == setting3D)
		{
			getBoundingBox3D(absMin, absMax, player->pos, player->rot.y,
				playerPayload.bbox3d, getViewMatrix());
		}
	}

	auto BoneEsp::execFeature() const -> void
	{
		ESPPayload& buffer = getPayloadBuffer();
		PlayerPayload& playerPayload = buffer.players[payloadIndex];

		buffer.drawBones = static_cast<bool>(setting());

		if (setting() == settingEnabled)
		{
			vec3 headMax = player->bones[0] + vec3(6, 6, 6);
			vec3 headMin = player->bones[0] - vec3(6, 6, 6);

			getBoundingBox2D(headMin, headMax, playerPayload.headbbox[0],
				playerPayload.headbbox[1], getViewMatrix());

			for (auto a = 0u; a < expectedHitboxCount; a++)
			{
				worldToScreen(player->bones[a], playerPayload.bones[a], getViewMatrix());
			}
		}
	}

	auto HealthEsp::execFeature() const -> void
	{
		ESPPayload& buffer = getPayloadBuffer();
		PlayerPayload& playerPayload = buffer.players[payloadIndex];

		buffer.drawHealth = static_cast<bool>(setting());
		playerPayload.health = player->health;
	}

	auto NameEsp::execFeature() const -> void
	{
		ESPPayload& buffer = getPayloadBuffer();
		PlayerPayload& playerPayload = buffer.players[payloadIndex];

		buffer.drawName = static_cast<bool>(setting());

		if (setting() == settingEnabled)
		{
			RadarPlayer rPlayer;
			readRadarPlayer(index, rPlayer);

			if (rPlayer.entityListIndex > 0)
			{
				wcsncpy(playerPayload.name.data(), rPlayer.name.data(), playerPayload.name.size());
				playerPayload.name[playerPayload.name.size() - 1] = 0ui16;
			}
			else
			{
				buffer.drawName = false;
			}
		}
	}

	auto DistanceEsp::execFeature() const -> void
	{
		ESPPayload& buffer = getPayloadBuffer();
		PlayerPayload& playerPayload = buffer.players[payloadIndex];

		buffer.drawDistance = static_cast<bool>(setting());

		if (setting() == settingEnabled)
		{
			const float distanceUnits = player->pos.distance(playerList.getLocalPlayer().pos);
			playerPayload.distance = distanceUnits * unitsToMeters;
		}
	}

	auto WeaponEsp::execFeature() const -> void
	{
		ESPPayload& buffer = getPayloadBuffer();
		PlayerPayload& playerPayload = buffer.players[payloadIndex];

		buffer.drawWeaponName = static_cast<bool>(setting());

		if (setting() == settingEnabled)
		{
			string wepName;
			getWeaponName(player->weaponID, wepName);
			strncpy(playerPayload.weaponName.data(), wepName.c_str(),
				playerPayload.weaponName.size());
			playerPayload.weaponName[playerPayload.weaponName.size() - 1] = 0ui8;
		}
	}

	auto EspFeatureSet::execAllFeatures() const -> void
	{
		ESPPayload& buffer = getPayloadBuffer();
		const bool teamDrawEnabled = (getSetting("Team ESP")->i() != 0);

		if (enabled())
		{
			uint8_t counter = 0;

			CSPlayer const& local = playerList.getLocalPlayer();

			for (uint32_t a = 0; a < playerList.size(); a++)
			{
				auto& playerPayload = buffer.players[counter];
				CSPlayer const& player = playerList[a];
				bool shouldDraw = player.hitboxReadSuccess;

				playerPayload.isTeammate = (player.team == local.team);

				if (!player.validPlayer() ||
					player.base == local.base ||
					!shouldDraw ||
					(!teamDrawEnabled && playerPayload.isTeammate))
				{
					continue;
				}

				if (shouldDraw)
				{
					buffer.shouldDraw = true;

					for (auto const& kv : features)
					{
						EspFeature const* feature = dynamic_cast<EspFeature const*>(kv.second);
						feature->player = &player;
						feature->index = a;
						feature->payloadIndex = counter;
						kv.second->execFeature();
					}
					counter++;
				}
			}

			buffer.numPlayers = counter;
		}
		else
		{
			buffer.shouldDraw = false;
		}
	}
}