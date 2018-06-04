#pragma once

#include "renderpayload.h"

namespace renderer
{
	//simpler colors...
	union color
	{
		color(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a) : a(a), r(r), g(g), b(b) {}
		color(const uint32_t argb) : argb(argb) {}
		color() : argb(0) {}

		struct
		{
			uint8_t b;
			uint8_t g;
			uint8_t r;
			uint8_t a;
		};
		uint32_t argb;
	};

	auto drawESP(valkyrie::ESPPayload const& payload) -> void;

	auto drawHitmarkers(valkyrie::ESPPayload const& payload) -> void;
}