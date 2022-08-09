#include "linearspace.h"

namespace sgf {

void LinearSpace::addCollider(Collider* collider) {
	assert(!contains(m_colliders, collider));
	m_colliders.push_back(collider);
}

void LinearSpace::removeCollider(Collider* collider) {
	assert(contains(m_colliders, collider));
	remove(m_colliders, collider);
}

void LinearSpace::updateCollider(Collider* collider) {
}

Collider* LinearSpace::intersectRay(CLinef ray, float radius, Contact& contact) {
	Collider* hit{};
	for (auto c : m_colliders) {
		if (c->intersectRay(ray, 0, contact)) hit = c;
	}
	return hit;
}

} // namespace sgf
