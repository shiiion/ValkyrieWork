#pragma once

#include "res.h"
#include <random>

//logging cut out in release
#ifdef Development
#define PRINTLN_LOG(str) cout << (str) << endl
#define PRINT_LOG(str) cout << (str)
#else
#define PRINTLN_LOG(str)
#define PRINT_LOG(str)
#endif

//putting globally referenced helper functions here
namespace valkyrie
{
	auto worldToScreen(vec3 const& origin, vec2& screenOut, const matrix_t transformMatrix) -> bool;
	auto normalizeAngles(vec3& angles) -> void;
	auto clampAngles(vec3& angles) -> void;
	auto vectorAngles(vec3 const& direction, vec3& angles) -> void;

	//opens console and redirects stdout
	auto openConsole() -> void;

	//Generic fail-safes
	template<uint32_t attempts, uint32_t timeoutSeconds, typename callback, typename ... args>
	auto tryFn(std::atomic_bool const& exitMgr, callback& cb, args&& ... _args) -> bool
	{
		for (uint32_t a = 0; (a < attempts) && exitMgr.load(); ++a)
		{
			if (cb(std::forward<args>(_args)...))
			{
				return true;
			}

			std::this_thread::sleep_for(std::chrono::seconds(timeoutSeconds));
		}
		return false;
	}

	template<uint32_t timeoutSeconds, typename callback, typename ... args>
	auto waitForFn(std::atomic_bool const& exitMgr, callback& cb, args&& ... _args) -> bool
	{
		while (exitMgr.load())
		{
			if (cb(std::forward<args>(_args)...))
			{
				return true;
			}

			std::this_thread::sleep_for(std::chrono::seconds(timeoutSeconds));
		}
		return false;
	}

	//uniform distribution random
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

	//time since start of program
	auto timeSeconds() -> float;
}