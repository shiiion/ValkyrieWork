#include "miscfeatures.h"
#include "csgoutils.h"
#include "valkAPI.h"
#include "playerlist.h"

#include "memory.h"
#include "globals.h"
#include "resource.h"

#include "RenderPayload.h"

#include <sstream>

namespace valkyrie
{
	string BunnyhopFeature::featureName = "Bunnyhop";
	string SpamChatFeature::featureName = "Chat Spam";
	string HitMarkerFeature::featureName = "Hitmarkers";
	string GlowEsp::featureName = "Glow";
	string MiscFeatureSet::setName = "Miscellaneous";

	static auto playHitmarkerSound() -> void
	{
		PlaySound(MAKEINTRESOURCE(IDR_WAVE1), nullptr, SND_RESOURCE | SND_ASYNC);
	}

	auto SpamChatFeature::execFeature() const -> void
	{
		const uint32_t tickTime = static_cast<uint32_t>(GetTickCount());
		if (setting() == 1u && (tickTime - lastTickMessage) > spamTimerTicks)
		{
			CSPlayer const& local = playerList.getLocalPlayer();
			bool foundTarget = false;

			uint32_t index = lastPlayerIndex;
			for (; index < playerList.size(); ++index)
			{
				CSPlayer const& target = playerList[index];
				if (target.validPlayer() &&
					target.team == local.team &&
					!target.isDead &&
					target.callout[0]) //check for bad callout names (on some maps like shipped or cache)
				{
					foundTarget = true;
					lastPlayerIndex = index + 1u;

					RadarPlayer extraInfo;
					readRadarPlayer(index, extraInfo);

					std::array<char, 32> name;
					std::wcstombs(name.data(), extraInfo.name.data(), 32);

					string weaponName;
					getWeaponName(target.weaponID, weaponName);

					std::ostringstream outString;

					outString << "say \""
						<< name.data()
						<< " | HP:"
						<< target.health
						<< " | "
						<< weaponName
						<< " | "
						<< target.callout.data()
						<< "\"";

					sendUserCmd(outString.str());
					break;
				}
			}

			lastPlayerIndex = (foundTarget ? lastPlayerIndex : 0);
			lastTickMessage = tickTime;
		}
	}

	auto HitMarkerFeature::execFeature() const -> void
	{
		CSPlayer const& local = playerList.getLocalPlayer();
		ESPPayload& buffer = getPayloadBuffer();

		if (setting() == 1u && !local.isDead)
		{
			const float startTime = timeSeconds();
			uint32_t count;
			csgoProc.read(local.base + hitvectorOffset, &count, 1);
			if (count - hitCounter > maxHitsPerExec)
			{
				hitCounter = count;
			}
			else
			{
				bool newHit = false;

				if (!count)
				{
					hitList = {};
					hitCounter = 0;
				}
				else
				{
					uint32_t vectorBase;
					csgoProc.read(local.base + hitvectorOffset + 4u, &vectorBase, 1);
					constexpr uint32_t hitPosSize = 20u;

					for (auto a = hitCounter; a < count; a++)
					{
						Hit h = {};
						h.creationTime = static_cast<float>(GetTickCount());
						csgoProc.read<vec3>(vectorBase + (hitPosSize * a), 
							&h.loc, 
							1);
						h.creationTime = startTime;
						hitList.emplace_back(h);
						newHit = true;
					}

					hitCounter = count;
				}

				while (hitList.size() &&
					hitList.front().creationTime + hitFadeTime < startTime)
				{
					hitList.pop_front();
				}

				buffer.numHitPoints = static_cast<uint32_t>(hitList.size());
				buffer.drawHitpoints = true;

				auto it1 = hitList.begin();
				auto it2 = buffer.hitPoints.begin();

				const pmatrix_t viewMatrix = getViewMatrix();

				for (; it1 != hitList.end() && it2 != buffer.hitPoints.end();it1++, it2++)
				{
					worldToScreen(it1->loc, *it2, viewMatrix);
				}

				if (newHit)
				{
					playHitmarkerSound();
				}
			}
		}
		else //constantly update hitcount
		{
			hitList = {};
			csgoProc.read(local.base + hitvectorOffset, &hitCounter, 1);
			buffer.drawHitpoints = false;
		}
	}

	auto BunnyhopFeature::execFeature() const -> void
	{
		CSPlayer const& local = playerList.getLocalPlayer();
		if (setting() == 1u && 
			isChatClosed() &&
			checkKeyState(8, 1) &&
			!local.isDead)
		{
			constexpr uint32_t jump = 5u;
			constexpr uint32_t noJump = 4u;
			if (local.fFlags == 1ui8)
			{
				csgoProc.write(globals.forceJump, &jump, 1);
			}
			else
			{
				csgoProc.write(globals.forceJump, &noJump, 1);
			}
		}
	}

	//alternative, use hook
	auto GlowEsp::execFeature() const -> void
	{
		if (setting() == settingEnabled)
		{
			for (auto a = 0u; a < playerList.size(); a++)
			{
				CSPlayer const& player = playerList[a];

				if (player.validTarget(playerList.getLocalPlayer()) &&
					player.health > 0 &&
					player.health <= 100 &&
					player.sensorTime != newSensorTime)
				{
					csgoProc.write(player.base + globals.entOffs.detectedBySensor, &newSensorTime, 1);
				}
			}
		}
		else
		{
			for (auto a = 0u; a < playerList.size(); a++)
			{
				CSPlayer const& player = playerList[a];

				if (player.validTarget(playerList.getLocalPlayer()) &&
					player.health > 0 &&
					player.health <= 100 &&
					player.sensorTime == newSensorTime)
				{
					constexpr float zero = 0;
					csgoProc.write(player.base + globals.entOffs.detectedBySensor, &zero, 1);
				}
			}
		}
	}
}