#pragma once

#include <geom/geom.hh>

namespace sgf {

struct PhysicsMaterial {
	float bounciness=.25f;
	float friction=.75f;

	bool operator==(const PhysicsMaterial& that) const {
		return bounciness == that.bounciness;
	}
	bool operator!=(const PhysicsMaterial& that) const {
		return bounciness != that.bounciness;
	}
};

using CPhysicsMaterial = const PhysicsMaterial&;

} // namespace sgf
