#pragma once

#include "res.h"
#include "feature.h"
#include <list>

namespace valkyrie
{
	class ESPFeature : public Feature
	{
	public:
		static bool espEnabled;
		static bool drawTeam;
	};

	class BoxEsp : public ESPFeature
	{
	private:

		auto execBox2D() const -> void;
		auto execBox3D() const -> void;


	public:
		auto execFeature() const -> void;
		auto getFeatureName() const -> string const& { return "Player Box ESP"; }
	};

	class BoneEsp : public ESPFeature
	{
	private:
		auto execHeadBox() const -> void;
		auto execBoneLines() const -> void;

	public:
		auto execFeature() const -> void;
		auto getFeatureName() const -> string const& { return "Player Bone ESP"; }
	};

	class HealthEsp : public ESPFeature
	{
	private:
	public:
		auto execFeature() const -> void;
		auto getFeatureName() const -> string const& { return "Player Health ESP"; }
	};

	class NameEsp : public ESPFeature
	{
	private:
	public:
		auto execFeature() const -> void;
		auto getFeatureName() const -> string const& { return "Player Name ESP"; }
	};

	class DistanceEsp : public ESPFeature
	{
	private:
	public:
		auto execFeature() const -> void;
		auto getFeatureName() const -> string const& { return "Player Distance ESP"; }
	};

	class WeaponEsp : public ESPFeature
	{
	private:
	public:
		auto execFeature() const -> void;
		auto getFeatureName() const -> string const& { return "Player Weapon ESP"; }
	};

	class GlowEsp : public ESPFeature
	{
	private:
	public:
		auto execFeature() const -> void;
		auto getFeatureName() const -> string const& { return "Player Glow ESP"; }
	};
}