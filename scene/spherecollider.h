#pragma once

#include "collider.h"

namespace sgf {

class SphereCollider : public Collider {
public:
	SGF_OBJECT_TYPE(SphereCollider, Collider);

	Property<float> radius{1};

	SphereCollider(Scene* scene) : Collider(scene) {
	}

	bool intersectRay(CLinef ray, float radius, Contact& contact) const override;
};

} // namespace sgf
