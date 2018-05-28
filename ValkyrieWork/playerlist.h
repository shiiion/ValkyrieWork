#pragma once

#include "res.h"

namespace valkyrie
{
	
	template<typename EntityType>
	auto readEntityFromIndex(const uint32_t listBase, const uint32_t index, EntityType& entity) -> void;

	template<typename EntityType>
	auto readEntityFromHandle(const uint32_t listBase, const uint32_t handle, EntityType& entity) -> void;
	
	

	struct RadarPlayer
	{
		vec3 pos;
		vec3 viewAngles;
		std::array<float, 2> UNK1; // something to do with speed on radar? varies between 0 and 0.1 not sure
		std::array<uint8_t, 32> UNK2; // one section seems to count up, not sure what (too fast for curtime i think)
		uint8_t UNK3;
		uint8_t team;		  // seems that if this is equal to 0x40, it is a teammate, otherwise an enemy?
		uint8_t spottedFlag; // equal to 2 means spotted
		uint8_t UNK4;
		uint32_t index;		   // relative to 1 (first player)
		uint32_t entityListIndex; // relative to 0 (first entity)
		int32_t health;
		std::array<wchar_t, 32> name; // wide string for name, need to handle this our code is multibyte
		std::array<uint8_t, 348> UNK5;
	};
	constexpr uint32_t radarSize = sizeof(RadarPlayer);

	auto readRadarPlayer(const uint32_t index, RadarPlayer& radarPlayer) -> void;

	struct BaseCombatWeapon
	{
		uint32_t base;
		int32_t weaponID;

		auto readData() -> void;
	};

	//uh erm
	constexpr uint32_t expectedHitboxCount = 19;

	struct CSPlayer
	{
		uint32_t base;
		vec3 pos;
		vec3 vel;
		vec3 rot;
		vec3 viewOffset;
		vec3 viewPunch;
		vec3 aimPunch;
		vec3 bboxMin;
		vec3 bboxMax;
		std::array<vec3, expectedHitboxCount> bones;
		bool isDead;
		uint32_t team;
		bool dormant;
		int32_t health;
		uint8_t fFlags;
		uint8_t moveState;

		int32_t weaponID;
		std::array<char, 32> callout;
		float sensorTime;

		bool hitboxReadSuccess;

		auto readData() -> void;

		constexpr auto validTarget(CSPlayer const& other) const -> bool;
		constexpr auto validPlayer() const -> bool;
	};

	class PlayerList
	{
	private:
		vector<CSPlayer> playerList;
		CSPlayer localPlayer;
		//
		uint32_t maxPlayers;

	public:
		PlayerList();

		//really short??
		inline auto readLocalPlayer() -> void;
		inline auto readMaxPlayers() -> void;

		auto allocatePlayers() -> void;
		auto readPlayers() -> void;

		constexpr auto getLocalPlayer() const -> CSPlayer const&;
		constexpr auto getLocalPlayer() -> CSPlayer&;

		//cant constexpr,,,
		auto size() const -> size_t;

		//shorthanded + epic
		auto operator[](const uint32_t index) const -> CSPlayer const&;
		auto operator[](const uint32_t index) -> CSPlayer&;
	};

	extern PlayerList playerList;
}