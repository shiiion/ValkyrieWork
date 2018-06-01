#include "miscfeatures.h"
#include "csgoutils.h"
#include "valkAPI.h"
#include "playerlist.h"

#include "memory.h"
#include "globals.h"

#include "RenderPayload.h"

#include <sstream>

namespace valkyrie
{
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
					readRadarPlayer(index + 1, extraInfo);

					std::array<char, 32> name;
					std::wcstombs(name.data, extraInfo.name.data, 32);

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
						h.creationTime = static_cast<uint32_t>(GetTickCount());
						csgoProc.read(vectorBase + (hitPosSize * a), 
							reinterpret_cast<uintptr_t>(&h), 
							sizeof(vec3));
						hitList.emplace_back(h);
					}

					hitCounter = count;
					newHit = true;
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

				for (; it1 != hitList.end() && it2 != buffer.hitPoints.end(); it1 = it1++, it2++)
				{
					worldToScreen(it1->loc, *it2, viewMatrix);
				}

				if (newHit)
				{
					//TODO: make sound here
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
}