#pragma once

#include "renderer.h"
#include "terraindata.h"

namespace sgf {

class TerrainRenderer : public Renderer {
public:
	TerrainRenderer(TerrainData* data);

	void validate();

private:
	enum struct Dirty { none = 0, textures = 1, params = 2, mesh = 4, all = mesh * 2 - 1 };

	SharedPtr<TerrainData> m_data;

	Dirty m_dirty = Dirty::all;

	SharedPtr<Texture> m_heightTexture;
	SharedPtr<Texture> m_normalTexture;
	SharedPtr<GraphicsBuffer> m_paramsBuffer;
	SharedPtr<VertexState> m_vertexState;
	uint m_numIndices{};

	void updateParams();

	void updateMesh();

	void bindShaderParams(GraphicsContext* gc);

	Vector<RenderPassType> renderPasses() const override;

	void onUpdate() override;

	void onRender(RenderContext& rc, RenderPassType pass) override;
};

} // namespace sgf
