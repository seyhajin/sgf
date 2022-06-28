#pragma once

#include <geom/geom.hh>

namespace sgf {

constexpr int maxLights = 16;

struct LightParams {
	Vec4f position;
	Vec4f color;
	float radius = 0;
	float range = 0;
} SGF_ALIGN16;

} // namespace sgf