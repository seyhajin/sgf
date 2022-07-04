#pragma once

#include "entity.h"
#include "model.h"

namespace sgf {

class ModelInstance : public Entity {
public:
	SGF_OBJECT_TYPE(ModelInstance, Entity);

	Property<Vec4f> color{1};
	Property<float> morph;
};

} // namespace sgf
