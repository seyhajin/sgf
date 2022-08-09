#pragma once

#include "collisionspace.h"

namespace sgf {

class LinearSpace : public CollisionSpace {
public:
	void addCollider(Collider* collider) override;

	void removeCollider(Collider* collider) override;

	void updateCollider(Collider* collider) override;

	Collider* intersectRay(CLinef ray, float radius, Contact& contact) override;

private:
	Vector<Collider*> m_colliders;
};

}
