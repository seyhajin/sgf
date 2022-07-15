#pragma once

#include <geom/geom.hh>

namespace sgf {

constexpr int maxLights = 16;

struct LightParams {
	Vec4f position;
	Vec4f color;
	float radius = 0;
	float range = 0;
	char _pad[8];
};

SGF_ASSERT_ALIGNED16(LightParams);

} // namespace sgf