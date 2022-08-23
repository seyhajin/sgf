#pragma once

#include "contact.h"

namespace sgf {

//! ray must be normalized
bool intersectRaySphere(CLinef ray, CVec3f v0, float radius, Contact& contact);

//! ray and dir must be normalized
bool intersectRayCapsule(CLinef ray, CVec3f v0, CVec3f dir, float length, float radius, Contact& contact);

//! ray and dir must be normalized
bool intersectRayCylinder(CLinef ray, CVec3f v0, CVec3f dir, float length, Contact& contact);

//! ray must be normalized
bool intersectRayEdge(CLinef ray, float radius, CVec3f v0, CVec3f v1, Contact& contact);

//! ray must be normalized
bool intersectRayTriangle(CLinef ray, float radius, CVec3f v0, CVec3f v1, CVec3f v2, Contact& contact);

inline bool isUnit(float n) {
	return std::abs(std::abs(n) - 1.0f) <= unitLengthEpsilon;
}

inline bool isUnit(CVec3f v) {
	return std::abs(v.length() - 1.0f) <= unitLengthEpsilon;
}

inline bool isUnit(CLinef r) {
	return isUnit(r.d);
}

} // namespace sgf
