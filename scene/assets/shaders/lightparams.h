#pragma once

#include <geom/geom.hh>

namespace sgf {

constexpr int maxLights = 16;

struct alignas(16) LightParams {
	Vec4f position;
	Vec4f color;
	float radius = 0;
	float range = 0;
};

} // namespace sgf
