#pragma once

#include "entity.h"
#include "model.h"

namespace sgf {

class ModelRenderer;

class ModelInstance : public Entity {
public:
	SGF_OBJECT_TYPE(ModelInstance, Entity);

	Property<SharedPtr<Model>> model;
	Property<Vec4f> color{1};
	Property<float> morph;

	ModelInstance(Scene* scene);

protected:
	void onEnable() override;

	void onDisable() override;

private:
	ModelRenderer* m_renderer{};
};

} // namespace sgf
