#pragma once

#include "collider.h"

namespace sgf {

class PlaneCollider : public Collider{
public:
	SGF_OBJECT_TYPE(PlaneCollider, Collider);

	Property<Planef> plane{{{0,1,0},0}};

	bool intersectRay(CLinef ray, float radius, Contact& contact) const override;

private:
	Planef m_plane;
};

}
