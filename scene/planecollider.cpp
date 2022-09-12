#include "planecollider.h"

#include <imgui/imgui.hh>

namespace sgf {

bool PlaneCollider::intersectRay(CLinef ray, float radius, Contact& contact) const {

	auto tplane = matrix() * plane;
	if (ray.d.dot(tplane.n) >= 0) return false;

	tplane.d -= radius;

	float t = tplane.t_intersect(ray);
	if (t < 0 || t >= contact.time) return false;

	contact.normal = tplane.n;
	contact.point = ray * t - tplane.n * radius;
	contact.time = t;

	return true;
}

} // namespace sgf
