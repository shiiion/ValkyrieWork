#pragma once

#include "res.h"

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
}