#pragma once

#include "collider.h"
#include "meshcolliderdata.h"

namespace sgf {

class MeshCollider : public Collider {
public:
	SGF_OBJECT_TYPE(MeshCollider, Collider);

	Property<SharedPtr<MeshColliderData>> colliderData;

	MeshCollider(Scene* scene) : Collider(scene) {
	}

	bool intersectRay(CLinef worldRay, float radius, Contact& contact) const override;
};

} // namespace sgf
