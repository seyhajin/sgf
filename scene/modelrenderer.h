#pragma once

#include "renderer.h"
#include "modelrenderdata.h"

namespace sgf {

class Model;

class ModelRenderer : public Renderer {
public:
	SGF_OBJECT_TYPE(ModelRenderer,Renderer);

	ModelRenderer(ModelRenderData* renderData);

	void addInstance(Model* instance);
	void removeInstance(Model* instance);

private:
	struct Instance {
		Mat4f matrix;
		Vec4f color;
		float morph{};
	};

	SharedPtr<ModelRenderData> m_renderData;
	VertexLayout m_vertexLayout;
	Vector<Model*> m_instances;
	SharedPtr<GraphicsBuffer> m_instanceBuffer;
	SharedPtr<VertexState> m_vertexState;

	Vector<RenderPassType> renderPasses() const override;

	void sortInstances(CVec3f eyePos);

	void updateInstanceBuffer();

	void onUpdate() override;

	void onRender(RenderContext& rc, RenderPassType pass) override;
};

} // namespace sgf
