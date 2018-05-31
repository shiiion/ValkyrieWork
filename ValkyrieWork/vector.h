#pragma once

#include <cmath>

namespace valkyrie
{

	struct vec2
	{
		float x, y;

		vec2() : vec2(0, 0) {}
		vec2(float x, float y) : x(x), y(y) {}

		inline auto operator+(vec2 const& other) const -> vec2
		{
			return vec2(x + other.x, y + other.y);
		}

		inline auto operator-(vec2 const& other) const -> vec2
		{
			return vec2(x - other.x, y - other.y);
		}

		inline auto operator-() const -> vec2
		{
			return vec2(-x, -y);
		}

		inline auto operator*(float scalar) const -> vec2
		{
			return vec2(x * scalar, y * scalar);
		}

		inline auto operator/(float scalar) const -> vec2
		{
			return vec2(x / scalar, y / scalar);
		}

		inline auto dot(vec2 const& other) const -> float
		{
			return x * other.x + y * other.y;
		}

		inline auto distance(vec2 const& other) const -> float
		{
			return sqrt(dot(*this - other));
		}

		inline auto rotate(const float angle) const -> vec2
		{
			const float cos = cosf(angle), sin = sinf(angle);
			return vec2(y * cos - x * sin, x * cos + y * sin);
		}
	};

	struct vec3
	{
		float x, y, z;

		vec3() : vec3(0, 0, 0) {}
		vec3(float x, float y, float z) : x(x), y(y), z(z) {}

		inline auto operator+(vec3 const& other) const -> vec3
		{
			return vec3(x + other.x, y + other.y, z + other.z);
		}

		inline auto operator-(vec3 const& other) const -> vec3
		{
			return vec3(x - other.x, y - other.y, z - other.z);
		}

		inline auto operator-() const -> vec3
		{
			return vec3(-x, -y, -z);
		}

		inline auto operator*(float scalar) const -> vec3
		{
			return vec3(x * scalar, y * scalar, z * scalar);
		}

		inline auto operator/(float scalar) const -> vec3
		{
			return vec3(x / scalar, y / scalar, z / scalar);
		}

		inline auto dot(vec3 const& other) const -> float
		{
			return x * other.x + y * other.y + z * other.z;
		}

		inline auto mag() const -> float
		{
			return sqrt(dot(*this));
		}

		inline auto distance(vec3 const& other) const -> float
		{
			return (*this - other).mag();
		}

		inline auto xyRotation() const -> float
		{
			return atan2f(y, x);
		}
	};
}