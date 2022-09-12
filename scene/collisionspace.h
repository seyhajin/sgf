#pragma once

#include "collider.h"

namespace sgf {

class CollisionSpace : public Object {
public:
	SGF_OBJECT_TYPE(CollisionSpace, Object);

	virtual void addCollider(Collider* collider) = 0;

	virtual void removeCollider(Collider* collider) = 0;

	virtual void updateCollider(Collider* collider) = 0;

	virtual Collider* intersectRay(CLinef ray, float radius, Contact& contact, const Collider* ignore) = 0;
};

} // namespace sgf
