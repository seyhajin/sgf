#pragma once

#include "entity.h"

namespace sgf {

class Light : public Entity {
public:
	SGF_OBJECT_TYPE(Light, Entity);

	Light(Scene* scene) : Entity(scene) {
	}

	Property<Vec3f> color{1};
	Property<float> intensity{1};
	Property<float> radius{1};
	Property<float> range{100};

private:
	void onEnable() override;

	void onDisable() override;
};

} // namespace sgf
