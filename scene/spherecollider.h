#pragma once

#include "collider.h"

namespace sgf {

class SphereCollider : public Collider {
public:
	SGF_OBJECT_TYPE(SphereCollider, Collider);

	Property<float> radius;

	SphereCollider(float radius=1):radius(radius){}

	bool intersectRay(CLinef ray, float margin, Contact& contact) const override;

private:

	void onUpdate() override;
};

Mat3f solidSphereInertiaTensor(float mass,float radius);

Mat3f hollowSphereInertiaTensor(float mass,float radius);

} // namespace sgf
