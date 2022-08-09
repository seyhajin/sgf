#pragma once

#include "contact.h"
#include "entity.h"

namespace sgf {

class Collider : public Entity {
public:
	SGF_OBJECT_TYPE(Collider, Entity);

	Collider(Scene* scene) : Entity(scene) {
	}

	virtual bool intersectRay(CLinef ray, float radius, Contact& contact) = 0;

private:
	void onEnable() override;

	void onDisable() override;

	void onUpdate() override;
};

} // namespace sgf
