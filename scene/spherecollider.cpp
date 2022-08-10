#include "spherecollider.h"

#include "intersect.h"

namespace sgf {

bool SphereCollider::intersectRay(CLinef ray, float r, Contact& contact) {

	if (!intersectRaySphere(ray, worldPosition(), radius + r, contact)) return false;

	contact.point -= contact.normal * r;

	return true;
}

} // namespace sgf
