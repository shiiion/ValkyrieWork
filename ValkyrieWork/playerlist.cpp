#include "playerlist.h"
#include "memory.h"
#include "globals.h"

#include "valkAPI.h"

namespace valkyrie
{
	PlayerList playerList;

	static auto getHitboxSetBase(const uint32_t entBase) -> uint32_t
	{
		uint32_t modelHeader, hitboxSetOffsetVal;
		{
			uint32_t temp;
			csgoProc.read(entBase + globals.entOffs.modelHeader, &temp, 1);
			csgoProc.read(temp, &modelHeader, 1);
		}
		csgoProc.read(modelHeader + hitboxSetOffset, &hitboxSetOffsetVal, 1);
		return modelHeader + hitboxSetOffsetVal;
	}

	template<typename EntityType>
	auto readEntityFromIndex(const uint32_t listBase, const uint32_t index, EntityType& entity) -> void
	{
		static_assert(std::is_same<EntityType, BaseCombatWeapon>::value ||
			std::is_same<EntityType, CSPlayer>::value, "readEntityFromIndex only accepts CSPlayer or BaseCombatWeapon types");
		
		csgoProc.read(listBase + (entListStride * index), &entity.base, 1);
		entity.readData();
	}

	template<typename EntityType>
	auto readEntityFromHandle(const uint32_t listBase, const uint32_t handle, EntityType& entity) -> void
	{
		static_assert(std::is_same<EntityType, BaseCombatWeapon>::value ||
			std::is_same<EntityType, CSPlayer>::value, "readEntityFromHandle only accepts CSPlayer or BaseCombatWeapon types");
		
		readEntityFromIndex(listBase, handle & 0xFFF, entity);
	}

	auto readRadarPlayer(const uint32_t index, RadarPlayer& radarPlayer) -> void
	{
		csgoProc.read(globals.radarBase + (index * radarSize), &radarPlayer, 1);
	}

	auto BaseCombatWeapon::readData() -> void
	{
		if (!base)
		{
			return;
		}

		csgoProc.read(base + globals.entOffs.weaponID, &weaponID, 1);
	}

	auto CSPlayer::readData() -> void
	{
		hitboxReadSuccess = false;

		if (!base)
		{
			return;
		}

		csgoProc.read(base + globals.entOffs.dormant, &dormant, 1);
		if (dormant)
		{
			return;
		}

		csgoProc.read(base + bboxMinOffset, &bboxMin, 1);
		csgoProc.read(base + bboxMaxOffset, &bboxMax, 1);

		csgoProc.read(base + globals.entOffs.pos, &pos, 1);
		csgoProc.read(base + globals.entOffs.view, &viewOffset, 1);
		csgoProc.read(base + globals.entOffs.velocity, &vel, 1);
		csgoProc.read(base + globals.entOffs.rotation, &rot, 1);
		normalizeAngles(rot);
		clampAngles(rot);
		csgoProc.read(base + globals.entOffs.deadFlag, &isDead, 1);
		csgoProc.read(base + globals.entOffs.team, &team, 1);
		csgoProc.read(base + globals.entOffs.health, &health, 1);
		csgoProc.read(base + globals.entOffs.fFlags, &fFlags, 1);
		csgoProc.read(base + globals.entOffs.moveState, &moveState, 1);
		csgoProc.read(base + globals.entOffs.viewPunch, &viewPunch, 1);
		csgoProc.read(base + globals.entOffs.aimPunch, &aimPunch, 1);
		csgoProc.read(base + globals.entOffs.lastPlaceName, &callout[0], 1);
		csgoProc.read(base + globals.entOffs.detectedBySensor, &sensorTime, 1);

		{
			uint32_t hWeapon;
			BaseCombatWeapon weapon;
			csgoProc.read(base + globals.entOffs.hWeapon, &hWeapon, 1);
			readEntityFromHandle(globals.entityList, hWeapon, weapon);
			weaponID = weapon.weaponID;
		}

		const uint32_t hitboxSetBase = getHitboxSetBase(base);
		uint32_t hitboxCount, hitboxSetList;
		csgoProc.read(hitboxSetBase + bboxCountOffset, &hitboxCount, 1);
		if (hitboxCount != expectedHitboxCount)
		{
			return;
		}
		hitboxReadSuccess = true;

		csgoProc.read(hitboxSetBase + bboxOffset, &hitboxSetList, 1);
		hitboxSetList += hitboxSetBase;

		uint32_t bonematrixBase;
		csgoProc.read(base + globals.entOffs.boneMatrix, &bonematrixBase, 1);

		for (uint32_t a = 0; a < expectedHitboxCount; a++)
		{
			matrix_t bone;
			int32_t boneID;
			csgoProc.read(hitboxSetList + (0x44 * a), &boneID, 1);

			const uint32_t hitboxMatrix = bonematrixBase + (matrixSize * boneID);
			csgoProc.read(hitboxMatrix, bone, 16);

			bones[a] = vec3(bone[3], bone[7], bone[11]);
		}
	}

	//~~~~~~~~~~~~~~~~~~~~~

	PlayerList::PlayerList() : maxPlayers(0)
	{
		//i dont think servers can manage over 128
		playerList.reserve(128);
	};

	auto PlayerList::readLocalPlayer() -> void
	{
		csgoProc.read(globals.localPlayer, &localPlayer.base, 1);
		localPlayer.readData();
	}

	auto PlayerList::readMaxPlayers() -> void
	{
		const uint32_t address = globals.clientState + globals.otherOffs.maxPlayers;
		csgoProc.read(address, &maxPlayers, 1);
	}

	auto PlayerList::allocatePlayers() -> void
	{
		playerList.resize(static_cast<size_t>(maxPlayers));
	}

	auto PlayerList::readPlayers() -> void
	{
		readLocalPlayer();
		readMaxPlayers();

		if (maxPlayers != static_cast<uint32_t>(playerList.size()))
		{
			allocatePlayers();
		}

		uint32_t index = 1;
		for (CSPlayer& player : playerList)
		{
			readEntityFromIndex(globals.entityList, index++, player);
		}
	}

	auto PlayerList::size() const -> size_t
	{
		return playerList.size();
	}

	auto PlayerList::operator[](const uint32_t index) const -> CSPlayer const&
	{
		return playerList[index];
	}

	auto PlayerList::operator[](const uint32_t index) -> CSPlayer&
	{
		return playerList[index];
	}
}