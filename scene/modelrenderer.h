#pragma once

#include "model.h"
#include "renderer.h"
#include "modelinstance.h"

namespace sgf {

class ModelRenderer : public Renderer {
public:
	SGF_OBJECT_TYPE(ModelRenderer,Renderer);

	ModelRenderer(Model* model);

	void addInstance(ModelInstance* instance);
	void removeInstance(ModelInstance* instance);

private:
	struct Instance {
		Mat4f matrix;
		Vec4f color;
		float morph;

//		Instance(CMat4f matrix, CVec4f color, float morph) : matrix(matrix), color(color), morph(morph) {
//		}
	};

	SharedPtr<Model> m_model;
	VertexLayout m_vertexLayout;
	Vector<ModelInstance*> m_instances;
	SharedPtr<GraphicsBuffer> m_instanceBuffer;
	SharedPtr<VertexState> m_vertexState;

	Vector<RenderPassType> renderPasses() const override;

	void sortInstances(CVec3f eyePos);

	void updateInstanceBuffer();

	void onUpdate() override;

	void onRender(RenderContext& rc, RenderPassType pass) override;

	void renderDebug(RenderContext& rc);
};

} // namespace sgf
