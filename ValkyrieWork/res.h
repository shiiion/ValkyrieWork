#pragma once

#include <Windows.h>

#include <iostream>
#include <string>
#include <map>
#include <unordered_map>
#include <vector>
#include <memory>
#include <functional>
#include <algorithm>
#include <thread>
#include <mutex>
#include <atomic>
#include <array>

//also includes <cmath>
#include "vector.h"

namespace valkyrie
{
	constexpr auto pi = 3.1415926f;
	constexpr auto badAddr = 0xFFFFFFFFu;

	//float only (csgo)
	inline auto toRad(float deg) -> float
	{
		constexpr auto piOver180 = pi / 180.0f;
		return deg * piOver180; 
	};

	inline auto toDeg(float rad) -> float
	{
		constexpr auto invPiOver180 = 180.0f / pi;
		return rad * invPiOver180;
	};

	template<typename T>
	constexpr auto range(const T val, const T low, const T high) -> bool
	{
		return (val <= high) && (val >= low);
	}

	using std::cout;
	using std::endl;
	using std::string;
	using std::vector;
	using std::map;
	using std::unordered_map;
	using std::unique_ptr;
	using std::shared_ptr;
	using std::function;
	using std::lock_guard;
	using std::mutex;
	using std::thread;


	//source-like matrix4x4
	typedef float matrix_t[16];
	typedef float* pmatrix_t;
}