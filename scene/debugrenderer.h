#pragma once

#include "material.h"
#include "renderer.h"
#include "mesh.h"

namespace sgf {

class DebugRenderer : public Renderer {
public:
	Property<Vec4f> fillColor;
	Property<Vec4f> lineColor;
	Property<AffineMat4f> modelMatrix;
	Property<DepthMode> depthMode;
	Property<BlendMode> blendMode;
	Property<CullMode> cullMode;

	DebugRenderer();

	void clear();

	void addTriangle(CVertex v0, CVertex v1, CVertex v2,Material* material=nullptr);

	void addMesh(const Mesh* mesh);

private:
	struct Vertex {
		Vec3f position;
		Vec3f normal;
		Vec2f texCoords0;
		Vec4f color;

		Vertex(CVec3f position, CVec3f normal, CVec2f texCoords0, CVec4f color) : position(position), normal(normal), texCoords0(texCoords0), color(color) {
		}
	};

	struct RenderOp {
		SharedPtr<Material> material;
		DepthMode depthMode;
		BlendMode blendMode;
		CullMode cullMode;
		uint order;
		uint numVertices;
	};
	Vector<Vertex> m_vertices;
	Vector<RenderOp> m_renderOps;
	RenderOp m_currentOp{};

	SharedPtr<GraphicsBuffer> m_vertexBuffer;
	SharedPtr<VertexState> m_vertexState;

	void addVertex(sgf::CVertex vertex);

	void addPrimitive(uint order,Material* material);

	void flushPrimitives();

	Vector<RenderPassType> renderPasses() const override;

	void onRender(RenderContext& rc, RenderPassType pass) override;
};

} // namespace sgf
