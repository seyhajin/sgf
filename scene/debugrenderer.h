#pragma once

#include "material.h"
#include "renderer.h"

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

	void addTriangle(CVec3f v0, CVec3f v1, CVec3f v2);
	void addLine(CVec3f v0, CVec3f v1);
	void addBox(CBoxf box);

private:
	struct Vertex {
		Vec3f position;
		Vec4f color;

		Vertex(CVec3f position, CVec4f color) : position(position), color(color) {
		}
	};

	struct RenderOp {
		DepthMode depthMode;
		BlendMode blendMode;
		CullMode cullMode;
		uint order;
		uint numVertices;
	};
	Vector<Vertex> m_vertices;
	Vector<RenderOp> m_renderOps;
	RenderOp m_currentOp{};

	SharedPtr<VertexBuffer> m_vertexBuffer;

	void addPrimitive(uint order);

	void flushPrimitives();

	Vector<RenderPassType> renderPasses() const override;

	void onRender(RenderContext& rc, RenderPassType pass) override;
};

} // namespace sgf
