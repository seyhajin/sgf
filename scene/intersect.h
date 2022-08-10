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
bool intersectRayTriangle(CLinef ray, CVec3f v0, CVec3f v1, CVec3f v2, float radius, Contact& contact);

//! ray must be normalized
bool intersectRayEdge(CLinef ray, CVec3f v0, CVec3f v1, float radius, Contact& contact);

} // namespace sgf
