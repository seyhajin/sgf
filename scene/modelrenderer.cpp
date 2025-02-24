﻿#include "modelrenderer.h"

#include "modelrenderdata.h"
#include "model.h"

#include "shaderasset.h"

#include <imgui/imgui.hh>

namespace sgf {

namespace {

ShaderAsset g_shader("shaders/modelrenderer.glsl");

} // namespace

ModelRenderer::ModelRenderer(ModelRenderData* renderData) : m_renderData(renderData), m_vertexLayout(renderData->vertexLayout) {
	m_vertexLayout.addAttribs(
		{
			AttribFormat::float4, // location=8 : vec4 matrix[0]
			AttribFormat::float4, // matrix[1]
			AttribFormat::float4, // matrix[2]
			AttribFormat::float4, // matrix[3]
			AttribFormat::float4, // color
			AttribFormat::float1  // morph
		},
		1, 8, 0, 1);
}

Vector<RenderPassType> ModelRenderer::renderPasses() const {
	if (m_renderData->hasOpaqueSurfaces()) {
		if (m_renderData->hasBlendedSurfaces()) return {RenderPassType::opaque, RenderPassType::blended};
		return {RenderPassType::opaque};
	}
	if (m_renderData->hasBlendedSurfaces()) return {RenderPassType::blended};
	return {};
}

void ModelRenderer::addInstance(Model* instance) {
	assert(!contains(m_instances, instance));
	m_instances.push_back(instance);
}

void ModelRenderer::removeInstance(Model* instance) {
	assert(contains(m_instances, instance));
	remove(m_instances, instance);
}

void ModelRenderer::sortInstances(CVec3f eyePos) {
	auto cmpFunc = [eyePos](Model* x, Model* y) {
		return eyePos.distanceSquared(x->worldPosition()) > eyePos.distanceSquared(y->worldPosition());
	};
	std::sort(m_instances.begin(), m_instances.end(), cmpFunc);
}

void ModelRenderer::updateInstanceBuffer() {

	if (!m_instanceBuffer || m_instances.size() * sizeof(Instance) > m_instanceBuffer->size) {
		// Resize instance buffer
		m_instanceBuffer = graphicsDevice()->createGraphicsBuffer(BufferType::vertex,
																  m_instances.size() * sizeof(Instance) * 8, nullptr);

		m_vertexState = graphicsDevice()->createVertexState({m_renderData->vertexBuffer, m_instanceBuffer},
															m_renderData->indexBuffer, m_vertexLayout);
	}

	// Update instance buffer
	Instance* ptr = static_cast<Instance*>(m_instanceBuffer->lockData(0, m_instances.size() * sizeof(Instance)));
	for (auto model : m_instances) *ptr++ = {model->worldMatrix(), model->color, model->morph};
	m_instanceBuffer->unlockData();
}

void ModelRenderer::onUpdate() {

	//	if (!m_instances.size()) return;

	//  updateInstanceBuffer();
}

void ModelRenderer::onRender(RenderContext& rc, RenderPassType pass) {

	if (m_instances.empty()) return;

	if (m_renderData->hasBlendedSurfaces()) sortInstances(rc.renderParams()->camera.cameraMatrix.t.xyz());

	updateInstanceBuffer();

	auto gc = rc.graphicsContext();

	gc->setShader(g_shader.open());

	gc->setVertexState(m_vertexState);

	auto& surfaces = (pass == RenderPassType::blended ? m_renderData->blendedSurfaces : m_renderData->opaqueSurfaces);

	for (auto& surf : surfaces) {

		surf.material->bindShaderParams(gc);

		gc->drawIndexedGeometry(3, surf.firstIndex, surf.numIndices, m_instances.size());
	}
}

} // namespace sgf
