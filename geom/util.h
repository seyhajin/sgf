#pragma once

#include "vec3.h"
#include "line.h"

#include <core/core.hh>

namespace sgf{

inline bool isUnit(float n) {
	return std::abs(std::abs(n) - 1.0f) <= unitLengthEpsilon;
}

inline bool isUnit(CVec3f v) {
	return std::abs(v.length() - 1.0f) <= unitLengthEpsilon;
}

inline bool isUnit(CLinef r) {
	return isUnit(r.d);
}

}
