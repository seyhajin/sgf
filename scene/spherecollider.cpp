#include "spherecollider.h"

#include "intersect.h"

#include <imgui/imgui.hh>

namespace sgf {

bool SphereCollider::intersectRay(CLinef ray, float rayRadius, Contact& contact) const {

	ImGuiEx::Debug("SphereCollider ray") << worldMatrix().inverse() * ray;

	if (!intersectRaySphere(ray, worldPosition(), radius + rayRadius, contact)) return false;

	contact.point -= contact.normal * rayRadius;

	return true;
}

} // namespace sgf
