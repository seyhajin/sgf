#pragma once

#include "entity.h"
#include "modelrenderer.h"

namespace sgf {

class Model : public Entity {
public:
	SGF_OBJECT_TYPE(Model, Entity);

	Property<SharedPtr<ModelRenderData>> renderData;
	Property<Vec4f> color{1};
	Property<float> morph;

	Model(Scene* scene);

protected:
	void onEnable() override;

	void onDisable() override;

private:
	ModelRenderer* m_renderer{};
};

} // namespace sgf
