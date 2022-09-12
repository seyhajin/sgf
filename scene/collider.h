#pragma once

#include "contact.h"
#include "entity.h"
#include "physicsmaterial.h"

namespace sgf {

class Collider : public Entity {
public:
	SGF_OBJECT_TYPE(Collider, Entity);

	Property<PhysicsMaterial> material;

	//! Returns a point/normal on the margin
	virtual bool intersectRay(CLinef ray, float margin, Contact& contact) const = 0;

protected:
	void onEnable() override;

	void onDisable() override;

	void onUpdate() override;

private:
	AffineMat4f m_pivot;
};

} // namespace sgf
