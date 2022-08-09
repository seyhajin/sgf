#pragma once

#include <geom/geom.hh>

namespace sgf {

struct Contact {
	Vec3f point;
	Vec3f normal;
	float time{1};
};

} // namespace sgf
