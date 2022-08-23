#include "meshcollider.h"
#include <imgui/imgui.hh>

#include "intersect.h"

namespace sgf {

bool MeshCollider::intersectRay(CLinef worldRay, float rayRadius, Contact& contact) const {

	if (!colliderData.value()) return false;

	auto localRay = worldMatrix().inverse() * worldRay;

	if (!colliderData.value()->intersectRay(localRay, rayRadius, contact)) return false;

	contact.normal = worldMatrix().m * contact.normal;
	contact.point = worldMatrix() * contact.point - contact.normal * rayRadius;

	return true;
}

} // namespace sgf
