#include "res.h"
#include "globals.h"

#include "valkAPI.h"
#include "playerlist.h"

#include "esprendering.h"

#include "Renderer.h"

namespace renderer
{
	using std::vector;
	using valkyrie::vec2;
	using TextLine = std::tuple<void const*, bool, color>;


	static const color black		= color(0x00ui8, 0x00ui8, 0x00ui8, 0xFFui8);
	static const color white		= color(0xFFui8, 0xFFui8, 0xFFui8, 0xFFui8);
	static const color red			= color(0xFFui8, 0x00ui8, 0x00ui8, 0xFFui8);
	static const color blue			= color(0x00ui8, 0x00ui8, 0xFFui8, 0xFFui8);
	static const color green		= color(0x00ui8, 0xFFui8, 0x00ui8, 0xFFui8);
	static const color yellow		= color(0xFFui8, 0xFFui8, 0x00ui8, 0xFFui8);
	static const color orange		= color(0xFFui8, 0xA5ui8, 0x00ui8, 0xFFui8);
	static const color purple		= color(0x80ui8, 0xA5ui8, 0x80ui8, 0xFFui8);

	static const color lightBlue	= color(0xFFui8, 0x00ui8, 0x96ui8, 0xFFui8);
	static const color darkBlue		= color(0xFFui8, 0x00ui8, 0x3Cui8, 0xFFui8);
	static const color cyan			= color(0x00ui8, 0xFFui8, 0xFFui8, 0xFFui8);
	static const color violet		= color(0xFFui8, 0x00ui8, 0xFFui8, 0xFFui8);

	struct ColorConfig
	{
		color boxColors[2];
		color headBoxColors[2];
		color skeletonColors[2];

		ColorConfig(const color enemyBox, const color teamBox, const color enemyHead, 
			const color teamHead, const color enemySkeleton, const color teamSkeleton)
		{
			boxColors[0] = enemyBox;
			boxColors[1] = teamBox;
			headBoxColors[0] = enemyHead;
			headBoxColors[1] = teamHead;
			skeletonColors[0] = enemySkeleton;
			skeletonColors[1] = teamSkeleton;
		}
	};

	static ColorConfig colorConfigs[] = { ColorConfig(yellow, cyan, red, purple, white, white), ColorConfig(yellow, violet, red, cyan, orange, blue) };

	struct TextStack
	{
		//t1: text ptr
		//t2: isWide (false if MB)
		//t3: text color
		vector<TextLine> lines;

		void operator()(vec2 const& startLoc)
		{
			auto textStackHeight = 0.f;
			constexpr auto textStackDistance = 12.f;

			for (auto const& p : lines)
			{
				if (std::get<1>(p))
				{
					wchar_t const* wtextPtr = reinterpret_cast<wchar_t const*>(std::get<0>(p));

					pRenderer->DrawString(wtextPtr, startLoc.x, startLoc.y + textStackHeight,
						centered, true, static_cast<DWORD>(std::get<2>(p).argb), static_cast<DWORD>(black.argb));
				}
				else
				{
					char const* textPtr = reinterpret_cast<char const*>(std::get<0>(p));

					pRenderer->DrawString(textPtr, startLoc.x, startLoc.y + textStackHeight,
						centered, true, static_cast<DWORD>(std::get<2>(p).argb), static_cast<DWORD>(black.argb));
				}
				textStackHeight += textStackDistance;
			}
		}
	};


	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


	static auto drawLine(vec2 const& start, vec2 const& end, const color color, const float thickness) -> void
	{
		pRenderer->DrawLine(start.x, start.y, end.x, end.y, static_cast<DWORD>(color.argb), thickness);
	}

	//static auto drawOutlinedLine(vec2 const& start, vec2 const& end, const color inner, const color outer, const float thickness) -> void
	//{
	//	const auto drawBox = [](vec2 const& mins, vec2 const& maxes, const color c) -> void
	//	{
	//		const float w = maxes.x - mins.x;
	//		const float h = maxes.y - mins.y;
	//
	//		const vec2 v2 = vec2(mins.x + w, mins.y);
	//		const vec2 v4 = vec2(mins.x, mins.y + h);
	//
	//		drawLine(mins, v2, c, 1);
	//		drawLine(v2, maxes, c, 1);
	//		drawLine(maxes, v4, c, 1);
	//		drawLine(v4, mins, c, 1);
	//	};
	//	pRenderer->DrawLine(start.x, start.y, end.x, end.y, static_cast<DWORD>(inner.argb), thickness);
	//	drawBox(start - vec2(thickness - 1, thickness), end + vec2(thickness + 1, thickness), outer);
	//}

	static constexpr auto boundsCheck(vec2 const& vec) -> bool
	{
		const uint32_t x = static_cast<uint32_t>(vec.x);
		const uint32_t y = static_cast<uint32_t>(vec.y);
		return valkyrie::range<uint32_t>(x, 0, valkyrie::globals.screenWidth) &&
			valkyrie::range<uint32_t>(y, 0, valkyrie::globals.screenHeight);
	}

	static auto drawBox2d(vec2 const& mins, vec2 const& maxes, const color outer, const color inner) -> void
	{
		const auto drawBoxInternal = [](vec2 const& mins, vec2 const& maxes, const color c, const float thickness, const bool hExtend) -> void
		{
			const float w = maxes.x - mins.x;
			const float h = maxes.y - mins.y;

			const vec2 v2 = vec2(mins.x + w, mins.y);
			const vec2 v4 = vec2(mins.x, mins.y + h);

			const bool boundv1 = boundsCheck(mins);
			const bool boundv2 = boundsCheck(v2);
			const bool boundv3 = boundsCheck(maxes);
			const bool boundv4 = boundsCheck(v4);

			float extension = (hExtend ? 1.f : 0.f);

			if (boundv1 || boundv2)
			{
				drawLine(mins + vec2(-extension, 0), v2 + vec2(extension, 0), c, thickness);
			}
			if (boundv2 || boundv3)
			{
				drawLine(v2, maxes, c, thickness);
			}
			if (boundv3 || boundv4)
			{
				drawLine(maxes + vec2(extension, 0), v4 + vec2(-extension, 0), c, thickness);
			}
			if (boundv4 || boundv1)
			{
				drawLine(v4, mins, c, thickness);
			}
		};

		drawBoxInternal(mins, maxes, outer, 3.f, true);
		drawBoxInternal(mins, maxes, inner, 1.f, false);
	}

	static auto drawBox3d(std::array<vec2, 8> const& vertices, const color outer, const color inner) -> void
	{
		const auto drawBoxInternal = [](std::array<vec2, 8> const& v, const color c, const float thickness) -> void
		{
			//there is literally an internal compiler error if i make this an std::array<bool, 8>, no joke
			bool boundsArr[8];
			for (auto a = 0u; a < v.size(); a++)
			{
				boundsArr[a] = boundsCheck(v[a]);
			}

			if (boundsArr[0] || boundsArr[1])
				drawLine(v[0], v[1], c, thickness);
			if (boundsArr[1] || boundsArr[2])
				drawLine(v[1], v[2], c, thickness);
			if (boundsArr[2] || boundsArr[3])
				drawLine(v[2], v[3], c, thickness);
			if (boundsArr[3] || boundsArr[0])
				drawLine(v[3], v[0], c, thickness);

			if (boundsArr[0] || boundsArr[4])
				drawLine(v[0], v[4], c, thickness);
			if (boundsArr[1] || boundsArr[5])
				drawLine(v[1], v[5], c, thickness);
			if (boundsArr[2] || boundsArr[6])
				drawLine(v[2], v[6], c, thickness);
			if (boundsArr[3] || boundsArr[7])
				drawLine(v[3], v[7], c, thickness);

			if (boundsArr[4] || boundsArr[5])
				drawLine(v[4], v[5], c, thickness);
			if (boundsArr[5] || boundsArr[6])
				drawLine(v[5], v[6], c, thickness);
			if (boundsArr[6] || boundsArr[7])
				drawLine(v[6], v[7], c, thickness);
			if (boundsArr[7] || boundsArr[4])
				drawLine(v[7], v[4], c, thickness);
		};

		drawBoxInternal(vertices, outer, 3.f);
		drawBoxInternal(vertices, inner, 1.f);
	}

	static auto drawBones(std::array<vec2, valkyrie::expectedHitboxCount> const& bones, const color c)
	{
		//head to neck
		drawLine(bones[0], bones[1], c, 1);
		drawLine(bones[1], bones[6], c, 1);

		//spine
		drawLine(bones[2], bones[3], c, 1);
		drawLine(bones[3], bones[4], c, 1);
		drawLine(bones[4], bones[5], c, 1);
		drawLine(bones[5], bones[6], c, 1);

		//pelvis to legs
		drawLine(bones[2], bones[8], c, 1);
		drawLine(bones[2], bones[7], c, 1);

		//left leg
		drawLine(bones[8], bones[10], c, 1);
		drawLine(bones[10], bones[12], c, 1);

		//right leg
		drawLine(bones[7], bones[9], c, 1);
		drawLine(bones[9], bones[11], c, 1);

		//spine to arms
		drawLine(bones[1], bones[17], c, 1);
		drawLine(bones[1], bones[15], c, 1);

		//left arm
		drawLine(bones[17], bones[18], c, 1);
		drawLine(bones[18], bones[14], c, 1);

		//right arm
		drawLine(bones[15], bones[16], c, 1);
		drawLine(bones[16], bones[13], c, 1);
	}
	
	static auto drawHealthBar(vec2 const& boxBase, const float size, const color outer, const color inner) -> void
	{
		const vec2 lineStart = boxBase + vec2(6, 0);
		const vec2 lineEnd = boxBase + vec2(6, -size);
		drawLine(lineStart, lineEnd, outer, 3.f);
		drawLine(lineStart, lineEnd, inner, 1.f);
	}

	static auto getHealthColor(const float healthScalar) -> color
	{
		const float healthHue = 135.f * healthScalar;
		
		const hsv healthHsv = { healthHue, 0.75f, 1.f };
		const rgb healthRgb = pRenderer->hsv2rgb(healthHsv);
		return color(static_cast<uint8_t>(healthRgb.r * 255.f),
			static_cast<uint8_t>(healthRgb.r * 255.f),
			static_cast<uint8_t>(healthRgb.r * 255.f),
			0xFFui8);
	}

	auto drawESP(valkyrie::ESPPayload const& payload) -> void
	{
		//EDITME
		ColorConfig const& ccfg = colorConfigs[0];
		TextStack textStackRender;
		textStackRender.lines.reserve(3u);

		for (auto a = 0u; a < payload.numPlayers; a++)
		{
			auto const& player = payload.players[a];

			//force index to be 0 or 1
			uint8_t ccfgIndex = static_cast<uint8_t>(!!player.isTeammate);

			std::array<char, 64> distString;
			snprintf(distString.data(), 64, "%.01f Meters", player.distance);

			const float healthScalar = static_cast<float>(player.health) / 100.f;
			const color healthColor = getHealthColor(healthScalar);


			if (payload.boundingBoxMode == 1ui8)
			{
				drawBox2d(player.bbox[0], player.bbox[1], black, ccfg.boxColors[ccfgIndex]);
			}
			else if (payload.boundingBoxMode == 2ui8)
			{
				drawBox3d(player.bbox3d, black, ccfg.boxColors[ccfgIndex]);
			}
			if (payload.drawBones)
			{
				//draw head with bones
				drawBox2d(player.headbbox[0], player.headbbox[1], black, ccfg.headBoxColors[ccfgIndex]);
				drawBones(player.bones, ccfg.skeletonColors[ccfgIndex]);
			}
			if (payload.drawHealth)
			{
				const auto healthBarSize = (player.bbox[1].y - player.bbox[0].y) * healthScalar;
				drawHealthBar(player.bbox[1], healthBarSize, black, healthColor);
			}
			if (payload.drawName)
			{
				TextLine line = { player.name.data(), true, healthColor };
				textStackRender.lines.emplace_back(line);
			}
			if (payload.drawDistance)
			{
				TextLine line = { distString.data(), false, healthColor };
				textStackRender.lines.emplace_back(line);
			}
			if (payload.drawWeaponName)
			{
				TextLine line = { player.weaponName.data(), false, healthColor };
				textStackRender.lines.emplace_back(line);
			}

			//
			{
				const float xLoc = player.bbox[0].x + (player.bbox[1].x - player.bbox[0].x) / 2.f;
				const float yLoc = player.bbox[1].y + 6;
				textStackRender(vec2(xLoc, yLoc));
				textStackRender.lines.clear();
			}
		}
	}

	auto drawHitmarkers(valkyrie::ESPPayload const& data) -> void
	{
		for (auto a = 0u; a < data.numHitPoints; a++)
		{
			pRenderer->DrawCross(data.hitPoints[a].x, data.hitPoints[a].y, 10.f, 10.f, static_cast<D3DCOLOR>(white.argb));
		}
	}
}