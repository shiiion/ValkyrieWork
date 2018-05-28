#include "valkAPI.h"
#include <cstdio>
#include <random>

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
		const auto rowDot = [](auto r, vec3 const& v) -> float
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
		float yaw, pitch, temp;

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

	template<constexpr uint32_t attempts, constexpr uint32_t timeoutSeconds, typename callback, typename ...args>
	auto tryFn(std::atomic_bool const& exitMgr, callback& cb, args&& ... _args) -> bool
	{
		for (uint32_t a = 0; (a < attempts) && exitMgr.load(); ++a)
		{
			if (cb(std::forward(_args)...))
			{
				return true;
			}

			std::this_thread::sleep_for(std::chrono::seconds(timeoutSeconds));
		}
		return false;
	}

	template<constexpr uint32_t timeoutSeconds, typename callback, typename ...args>
	auto waitForFn(std::atomic_bool const& exitMgr, callback& cb, args&& ... _args) -> bool
	{
		while (exitMgr.load())
		{
			if (cb(std::forward(_args)...))
			{
				return true;
			}

			std::this_thread::sleep_for(std::chrono::seconds(timeoutSeconds));
		}
		return false;
	}

	template<typename T>
	auto random(T min, T max) -> T
	{
		static_assert(std::is_integral<T>::value || std::is_floating_point<T>::value,
			"random types must be integral or floating point");

		std::default_random_engine rnGenerator;
		if constexpr(std::is_integral<T>::value)
		{
			std::uniform_int_distribution<T> distribution(min, max);
			return distribution(rnGenerator);
		}
		if constexpr(std::is_floating_point<T>::value)
		{
			std::uniform_real_distribution<T> distribution(min, max);
			return distribution(rnGenerator);
		}
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