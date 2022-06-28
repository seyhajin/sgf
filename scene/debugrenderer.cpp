#include "debugrenderer.h"

#include "shaderloader.h"

//#include <core3d/core3d.hh>

namespace sgf {

namespace {

ShaderLoader g_shader("shaders/debugrenderer.glsl");

} // namespace

DebugRenderer::DebugRenderer() {
	clear();
}

void DebugRenderer::clear() {
	m_vertices.clear();
	m_renderOps.clear();
	fillColor = Vec4f(1, 0, 1, 1);
	lineColor = Vec4(0, 0, 0, 1);
	modelMatrix = AffineMat4f{};
	depthMode = m_currentOp.depthMode = DepthMode::enable;
	blendMode = m_currentOp.blendMode = BlendMode::disable;
	cullMode = m_currentOp.cullMode = CullMode::disable;
	m_currentOp.numVertices = 0;
	m_currentOp.order = 0;
}

void DebugRenderer::addTriangle(CVec3f v0, CVec3f v1, CVec3f v2) {
	// debug() << "### addTriangle" << v0 << v1 << v2;
	m_vertices.emplace_back(modelMatrix.value() * v0, fillColor);
	m_vertices.emplace_back(modelMatrix.value() * v1, fillColor);
	m_vertices.emplace_back(modelMatrix.value() * v2, fillColor);
	addPrimitive(3);
}

void DebugRenderer::addLine(CVec3f v0, CVec3f v1) {
	m_vertices.emplace_back(modelMatrix.value() * v0, lineColor);
	m_vertices.emplace_back(modelMatrix.value() * v1, lineColor);
	addPrimitive(2);
}

void DebugRenderer::addBox(CBoxf box) {
	Vec3f corners[8];
	for (uint i = 0; i < 8; ++i) corners[i] = box.corner(i);

	uint faces[] = {2, 3, 1, 0, 3, 7, 5, 1, 7, 6, 4, 5, //
					6, 2, 0, 4, 6, 7, 3, 2, 0, 1, 5, 4};

	for (uint i = 0; i < 24; i += 4) {
		addTriangle(corners[faces[i]], corners[faces[i + 1]], corners[faces[i + 2]]);
		addTriangle(corners[faces[i]], corners[faces[i + 2]], corners[faces[i + 3]]);
	}

	for (uint i = 0; i < 24; i += 4) {
		addLine(corners[faces[i + 0]], corners[faces[i + 1]]);
		addLine(corners[faces[i + 1]], corners[faces[i + 2]]);
		addLine(corners[faces[i + 2]], corners[faces[i + 3]]);
		addLine(corners[faces[i + 3]], corners[faces[i + 0]]);
	}
}

void DebugRenderer::flushPrimitives() {
	if (!m_currentOp.numVertices) return;
	m_renderOps.push_back(m_currentOp);
	m_currentOp.numVertices = 0;
}

void DebugRenderer::addPrimitive(uint order) {
	if (depthMode != m_currentOp.depthMode || blendMode != m_currentOp.blendMode || cullMode != m_currentOp.cullMode ||
		order != m_currentOp.order) {
		flushPrimitives();
		m_currentOp.depthMode = depthMode;
		m_currentOp.blendMode = blendMode;
		m_currentOp.cullMode = cullMode;
		m_currentOp.order = order;
	}
	m_currentOp.numVertices += order;
}

Vector<RenderPassType> DebugRenderer::renderPasses() const {
	return {RenderPassType::overlay};
}

void DebugRenderer::onRender(RenderContext& rc, RenderPassType pass) {

	if (m_vertices.empty()) return;

	flushPrimitives();

	auto gc = rc.graphicsContext();

	if (!m_vertexBuffer || m_vertices.size() > m_vertexBuffer->length) {
		VertexFormat vertexFormat{AttribFormat::float3, AttribFormat::float4};
		assert(bytesPerVertex(vertexFormat) == sizeof(Vertex));
		m_vertexBuffer = graphicsDevice()->createVertexBuffer(m_vertices.size(), vertexFormat, nullptr);
	}
	m_vertexBuffer->updateData(0, m_vertices.size(), m_vertices.data());

	gc->setVertexBuffer(m_vertexBuffer);
	gc->setShader(g_shader.open());

	uint firstVertex = 0;
	for (auto& rop : m_renderOps) {
		gc->setDepthMode(rop.depthMode);
		gc->setBlendMode(rop.blendMode);
		gc->setCullMode(rop.cullMode);
		gc->drawGeometry(rop.order, firstVertex, rop.numVertices, 1);
		firstVertex += rop.numVertices;
	}
}

} // namespace sgf
