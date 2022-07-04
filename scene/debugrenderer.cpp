#include "debugrenderer.h"

#include "mesh.h"
#include "shaderloader.h"

namespace sgf {

namespace {

// clang-format off
VertexLayout vertexLayout {{
{AttribFormat::float3, 0, 0, 0,   sizeof(Vertex)},
{AttribFormat::float3, 0, 1, 12,  sizeof(Vertex)},
{AttribFormat::float2, 0, 2, 24,  sizeof(Vertex)},
{AttribFormat::float4, 0, 3, 32,  sizeof(Vertex)}
}};
// clang-format on

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

void DebugRenderer::addTriangle(CVertex v0, CVertex v1, CVertex v2, Material* material) {
	addVertex(v0);
	addVertex(v1);
	addVertex(v2);
	addPrimitive(3, material);
}

void DebugRenderer::addVertex(sgf::CVertex& v) {
	m_vertices.emplace_back(modelMatrix.value() * v.position, modelMatrix.value().m * v.normal, v.texCoords0, v.color);
}

void DebugRenderer::addMesh(const Mesh* mesh) {
	auto& vertices = mesh->vertices();
	auto& materials = mesh->materials();
	for (auto& tri : mesh->triangles()) {
		addVertex(vertices[tri.v0]);
		addVertex(vertices[tri.v1]);
		addVertex(vertices[tri.v2]);
		addPrimitive(3, materials[tri.materialId]);
	}
}

void DebugRenderer::flushPrimitives() {
	if (!m_currentOp.numVertices) return;
	m_renderOps.push_back(m_currentOp);
	m_currentOp.numVertices = 0;
}

void DebugRenderer::addPrimitive(uint order, Material* material) {
	if (depthMode != m_currentOp.depthMode || blendMode != m_currentOp.blendMode || cullMode != m_currentOp.cullMode ||
		order != m_currentOp.order || m_currentOp.material.value() != material) {
		flushPrimitives();
		m_currentOp.material = material;
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

	if (!m_vertexBuffer || m_vertices.size() * sizeof(Vertex) > m_vertexBuffer->size) {
		m_vertexBuffer = graphicsDevice()->createGraphicsBuffer(BufferType::vertex, m_vertices.size() * sizeof(Vertex), nullptr);
		m_vertexState = graphicsDevice()->createVertexState({m_vertexBuffer},nullptr,vertexLayout);
	}
	m_vertexBuffer->updateData(0, m_vertices.size() * sizeof(Vertex), m_vertices.data());

	gc->setVertexState(m_vertexState);
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
