#include "spherecollider.h"

#include "intersect.h"
#include "rigidbody.h"
#include "scene.h"

#include <imgui/imgui.hh>

namespace sgf {

bool SphereCollider::intersectRay(CLinef ray, float rayRadius, Contact& contact) const {

	if (!intersectRaySphere(ray, worldPosition(), radius + rayRadius, contact)) return false;

	contact.point -= contact.normal * rayRadius;

	contact.material = material;

	return true;
}

void SphereCollider::onUpdate() {

	auto body = parent() ? parent()->cast<RigidBody>() : nullptr;
	if (!body) return;

	// Ok, we want to move this collider from its current position to rididbody's current position
	auto src = this->worldPosition();
	auto dst = body->worldPosition();

	constexpr float eps = .0001f;

	uint hits = 0;

	for (;;) {

		Contact contact;
		contact.time = src.distance(dst);
		if (contact.time <= eps) {
			body->warpToWorldPosition(src);
			dst = src;
			break;
		}

		Linef ray(src, (dst - src) / contact.time);
		auto collider = scene->intersectRay(ray, radius, contact, this);
		if (!collider) {
			body->warpToWorldPosition(dst);
			break;
		}

		PhysicsMaterial physmat{std::max(material.value().bounciness, contact.material.bounciness),
								std::min(material.value().friction, contact.material.friction)};

		auto body2 = collider->parent() ? collider->parent()->cast<RigidBody>() : nullptr;

		src = ray * contact.time;

		body->warpToWorldPosition(src);
		if (++hits == 3) {
			dst = src;
			break;
		}
		if (body2) {
			body2->warpToWorldPosition(collider->worldPosition());
			//			debug() <<"### d, pt1, pt2, time"<<collider->worldPosition().distance(body2->worldPosition()) <<
			//(contact.point-body->worldPosition()).length() << (contact.point-body2->worldPosition()).length() <<
			//contact.time; 			debug() <<"### pt1, pt2"<<(contact.point-body->worldPosition()).length() <<
			//(contact.point-body2->worldPosition()).length();
		}

		body->addCollisionForces(body2, contact.point, contact.normal, physmat);

		Planef plane(src, contact.normal);
		plane.d -= eps;
		dst = plane.nearest(dst);
	}

	updateWorldMatrix({body->worldMatrix().m, dst});
}

Mat3f solidSphereInertiaTensor(float mass, float radius) {
	return {mass * radius * radius * 2.0f / 5.0f};
}

Mat3f hollowSphereInertiaTensor(float mass, float radius) {
	return {mass * radius * radius * 2.0f / 3.0f};
}

} // namespace sgf
