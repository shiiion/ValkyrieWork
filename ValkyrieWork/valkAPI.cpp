#include "valkAPI.h"
#include <cstdio>

namespace valkyrie
{
	auto worldToScreen(vec3 const& origin, vec2& screenOut, const matrix_t tMat) -> bool
	{
		const float width = static_cast<float>(1920);
		const float height = static_cast<float>(1080);//get from sysmetrics
		const float halfWidth = width * 0.5f;
		const float halfHeight = height * 0.5f;

		const auto matIdx = [tMat](auto r, auto c) -> float
		{
			return tMat[(r * 4) + c];
		};
		const auto rowDot = [matIdx](auto r, vec3 const& v) -> float
		{
			return matIdx(r, 0) * v.x + matIdx(r, 1) * v.y + matIdx(r, 2) * v.z + matIdx(r, 3);
		};

		const float w = rowDot(3, origin);

		if (w > 0.001f)
		{
			const float halfWInv = (1.f / w) * 0.5f;

			screenOut.x = halfWidth + (halfWInv * rowDot(0, origin) * width + 0.5f);
			screenOut.y = halfHeight - (halfWInv * rowDot(1, origin) * height + 0.5f);

			return range(screenOut.x, 0.f, width) && range(screenOut.y, 0.f, height);
		}

		return false;
	}

	auto normalizeAngles(vec3& angles) -> void
	{
		if (std::isfinite(angles.x) && std::isfinite(angles.y) && std::isfinite(angles.z))
		{
			if (angles.y > 180.f)
			{
				angles.y -= 360.f;
			}
			else if (angles.y < -180.f)
			{
				angles.y += 360.f;
			}
			angles.z = 0;
		}
		else
		{
			angles = vec3();
		}
	}

	auto clampAngles(vec3& angles) -> void
	{
		angles.x = min(max(angles.x, -89.f), 89.f);
		angles.z = 0;
	}

	auto vectorAngles(vec3 const& direction, vec3& angles) -> void
	{
		float yaw, pitch = 0, temp;

		if (direction.x == 0 && direction.y == 0)
		{
			pitch = 0.f;
			if (direction.z > 0.f)
			{
				yaw = 270.f;
			}
			else
			{
				yaw = 90.f;
			}
		}
		else
		{
			yaw = toDeg(atan2(direction.y, direction.x));
			if (yaw < 0.f)
				yaw += 360.f;

			temp = sqrt(direction.x * direction.x + direction.y * direction.y);
			temp = toDeg(atan2(-direction.z, temp));
			if (pitch < 0.f)
			{
				pitch += 360.f;
			}
		}

		angles = vec3(pitch, yaw, 0);
	}

	auto openConsole() -> void
	{
		AllocConsole();
		freopen("CONOUT$", "w", stdout);
	}

	auto timeSeconds() -> float
	{
		typedef std::chrono::duration<float> sec_t;
		typedef std::chrono::high_resolution_clock time_t;

		static const auto startTime = time_t::now();

		const auto curTime = time_t::now();

		return static_cast<sec_t>(curTime - startTime).count();
	}
}